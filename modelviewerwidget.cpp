#include "modelviewerwidget.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QRegularExpression>
#include <QSurfaceFormat>
#include <QCoreApplication>
#include <cstdio>

// ═══════════════════════════════════════════════════════════════════════════
// Shader sources
// ═══════════════════════════════════════════════════════════════════════════

// Portable GLSL shaders — no version directive, no layout qualifiers.
// Qt's QOpenGLShaderProgram prepends the correct #version automatically.
// Attribute locations are bound explicitly via bindAttributeLocation().
static const char *vertexShaderSource =
    "attribute highp vec3 aPosition;\n"
    "attribute highp vec2 aTexCoord;\n"
    "attribute highp vec3 aNormal;\n"
    "uniform highp mat4 uModel;\n"
    "uniform highp mat4 uView;\n"
    "uniform highp mat4 uProjection;\n"
    "uniform highp mat3 uNormalMatrix;\n"
    "varying highp vec2 vTexCoord;\n"
    "varying highp vec3 vNormal;\n"
    "varying highp vec3 vFragPos;\n"
    "void main() {\n"
    "    highp vec4 worldPos = uModel * vec4(aPosition, 1.0);\n"
    "    vFragPos = worldPos.xyz;\n"
    "    vNormal = uNormalMatrix * aNormal;\n"
    "    vTexCoord = aTexCoord;\n"
    "    gl_Position = uProjection * uView * worldPos;\n"
    "}\n";

static const char *fragmentShaderSource =
    "uniform sampler2D uTexture;\n"
    "uniform bool uHasTexture;\n"
    "uniform highp vec3 uLightPos;\n"
    "uniform highp vec3 uViewPos;\n"
    "uniform highp vec3 uLightColor;\n"
    "uniform highp vec3 uObjectColor;\n"
    "varying highp vec2 vTexCoord;\n"
    "varying highp vec3 vNormal;\n"
    "varying highp vec3 vFragPos;\n"
    "void main() {\n"
    "    highp vec3 baseColor = uHasTexture ? texture2D(uTexture, vTexCoord).rgb : uObjectColor;\n"
    "    highp vec3 norm = normalize(vNormal);\n"
    "    highp vec3 lightDir = normalize(uLightPos - vFragPos);\n"
    "    highp vec3 viewDir = normalize(uViewPos - vFragPos);\n"
    // Two-sided lighting: flip the normal if it faces away from camera
    // This fixes TripoSR meshes with inconsistent face winding/normals
    "    if (dot(norm, viewDir) < 0.0) norm = -norm;\n"
    "    highp float ambientStr = 0.4;\n"
    "    highp vec3 ambient = ambientStr * uLightColor * baseColor;\n"
    "    highp float diff = max(dot(norm, lightDir), 0.0);\n"
    "    highp vec3 diffuse = diff * uLightColor * baseColor;\n"
    "    highp vec3 halfDir = normalize(lightDir + viewDir);\n"
    "    highp float spec = pow(max(dot(norm, halfDir), 0.0), 64.0);\n"
    "    highp vec3 specular = 0.4 * spec * uLightColor;\n"
    "    highp vec3 lightDir2 = normalize(vec3(-uLightPos.x, uLightPos.y * 0.5, -uLightPos.z) - vFragPos);\n"
    "    highp float diff2 = max(dot(norm, lightDir2), 0.0);\n"
    "    highp vec3 fill = 0.15 * diff2 * uLightColor * baseColor;\n"
    "    gl_FragColor = vec4(ambient + diffuse + specular + fill, 1.0);\n"
    "}\n";

// ═══════════════════════════════════════════════════════════════════════════
// Constructor / Destructor
// ═══════════════════════════════════════════════════════════════════════════

ModelViewerWidget::ModelViewerWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_vbo(QOpenGLBuffer::VertexBuffer)
{
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    // Use CompatibilityProfile for maximum GPU support (Intel, AMD, NVIDIA)
    fmt.setVersion(2, 1);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(fmt);

    setAttribute(Qt::WA_TranslucentBackground, false);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(true);

    setMinimumSize(400, 300);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

ModelViewerWidget::~ModelViewerWidget()
{
    if (m_glInitialized && context()) {
        makeCurrent();
        m_vao.destroy();
        m_vbo.destroy();
        delete m_texture;
        m_texture = nullptr;
        delete m_shaderProgram;
        m_shaderProgram = nullptr;
        doneCurrent();
    } else {
        delete m_texture;
        m_texture = nullptr;
        delete m_shaderProgram;
        m_shaderProgram = nullptr;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// OBJ Loading
// ═══════════════════════════════════════════════════════════════════════════

bool ModelViewerWidget::loadOBJ(const QString &objPath)
{
    qDebug() << "[ModelViewerWidget] loadOBJ start:" << objPath;

    if (!parseOBJ(objPath)) {
        qWarning() << "[ModelViewerWidget] parseOBJ failed for:" << objPath;
        emit modelLoaded(false, 0, 0);
        return false;
    }

    qDebug() << "[ModelViewerWidget] parseOBJ done, computing bounding box...";
    QCoreApplication::processEvents();

    computeBoundingBox();

    // If OpenGL is already initialized, rebuild buffers
    if (m_glInitialized && context()) {
        qDebug() << "[ModelViewerWidget] Building GL buffers...";
        makeCurrent();
        if (!context()->isValid()) {
            qWarning() << "[ModelViewerWidget] makeCurrent failed, context invalid";
            doneCurrent();
            emit modelLoaded(false, 0, 0);
            return false;
        }
        buildGLBuffers();
        QCoreApplication::processEvents();
        loadTextureIfAvailable();
        doneCurrent();
    } else {
        qDebug() << "[ModelViewerWidget] GL not yet initialized, buffers will be built in initializeGL";
    }

    m_modelLoaded = true;
    resetView();
    update();
    qDebug() << "[ModelViewerWidget] Model ready:" << m_vertexCount << "vertices," << m_faceCount << "faces";
    emit modelLoaded(true, m_vertexCount, m_faceCount);
    return true;
}

bool ModelViewerWidget::parseOBJ(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open OBJ:" << path;
        return false;
    }

    QVector<QVector3D> positions;
    QVector<QVector2D> texCoords;
    QVector<QVector3D> normals;

    struct FaceVertex { int pi, ti, ni; };
    QVector<FaceVertex> faceVertices;

    // Pre-allocate for large meshes (TripoSR typically outputs ~200K vertices)
    positions.reserve(100000);
    texCoords.reserve(100000);
    normals.reserve(100000);
    faceVertices.reserve(300000);

    m_faceCount = 0;
    m_hasTexCoords = false;
    m_hasNormals = false;
    m_texturePath.clear();

    const QString objDir = QFileInfo(path).absolutePath();
    int lineCount = 0;

    // Use fast byte-level reading instead of QTextStream + QRegularExpression
    while (!file.atEnd()) {
        const QByteArray rawLine = file.readLine(4096);
        lineCount++;

        // Keep UI responsive every 5000 lines
        if ((lineCount % 5000) == 0) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        // Skip empty lines and comments
        if (rawLine.isEmpty() || rawLine[0] == '#' || rawLine[0] == '\n' || rawLine[0] == '\r')
            continue;

        const char *line = rawLine.constData();

        if (line[0] == 'v' && line[1] == ' ') {
            // Vertex position: "v x y z"
            float x, y, z;
            if (sscanf(line + 2, "%f %f %f", &x, &y, &z) == 3) {
                positions.append(QVector3D(x, y, z));
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            // Texture coordinate: "vt u v"
            float u, v;
            if (sscanf(line + 3, "%f %f", &u, &v) == 2) {
                texCoords.append(QVector2D(u, v));
                m_hasTexCoords = true;
            }
        }
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            // Normal: "vn x y z"
            float x, y, z;
            if (sscanf(line + 3, "%f %f %f", &x, &y, &z) == 3) {
                normals.append(QVector3D(x, y, z));
                m_hasNormals = true;
            }
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            // Face: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ..."
            QVector<FaceVertex> polyVerts;
            const char *ptr = line + 2;
            while (*ptr) {
                // Skip whitespace
                while (*ptr == ' ' || *ptr == '\t') ptr++;
                if (*ptr == '\0' || *ptr == '\n' || *ptr == '\r') break;

                FaceVertex fv = {-1, -1, -1};
                int pi = 0, ti = 0, ni = 0;
                int consumed = 0;

                // Try v/vt/vn
                if (sscanf(ptr, "%d/%d/%d%n", &pi, &ti, &ni, &consumed) >= 3 && consumed > 0) {
                    fv.pi = pi - 1; fv.ti = ti - 1; fv.ni = ni - 1;
                    ptr += consumed;
                }
                // Try v//vn
                else if (sscanf(ptr, "%d//%d%n", &pi, &ni, &consumed) >= 2 && consumed > 0) {
                    fv.pi = pi - 1; fv.ni = ni - 1;
                    ptr += consumed;
                }
                // Try v/vt
                else if (sscanf(ptr, "%d/%d%n", &pi, &ti, &consumed) >= 2 && consumed > 0) {
                    fv.pi = pi - 1; fv.ti = ti - 1;
                    ptr += consumed;
                }
                // Try v only
                else if (sscanf(ptr, "%d%n", &pi, &consumed) >= 1 && consumed > 0) {
                    fv.pi = pi - 1;
                    ptr += consumed;
                }
                else {
                    break; // malformed
                }
                polyVerts.append(fv);
            }

            // Fan triangulation
            for (int i = 1; i < polyVerts.size() - 1; ++i) {
                faceVertices.append(polyVerts[0]);
                faceVertices.append(polyVerts[i]);
                faceVertices.append(polyVerts[i + 1]);
                m_faceCount++;
            }
        }
        else if (rawLine.startsWith("mtllib ")) {
            QString mtlFile = QDir(objDir).absoluteFilePath(
                QString::fromUtf8(rawLine.mid(7).trimmed()));
            parseMTL(mtlFile, objDir);
        }
    }
    file.close();

    qDebug() << "[ModelViewerWidget] OBJ parsed:" << lineCount << "lines,"
             << positions.size() << "positions,"
             << texCoords.size() << "texcoords,"
             << normals.size() << "normals,"
             << m_faceCount << "faces";

    if (positions.isEmpty() || faceVertices.isEmpty()) {
        qWarning() << "OBJ file empty or no faces found";
        return false;
    }

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // Generate flat normals if none provided
    if (!m_hasNormals) {
        normals.resize(positions.size(), QVector3D(0, 1, 0));
        for (int i = 0; i < faceVertices.size(); i += 3) {
            int i0 = faceVertices[i].pi;
            int i1 = faceVertices[i+1].pi;
            int i2 = faceVertices[i+2].pi;
            if (i0 < 0 || i1 < 0 || i2 < 0) continue;
            if (i0 >= positions.size() || i1 >= positions.size() || i2 >= positions.size()) continue;
            QVector3D e1 = positions[i1] - positions[i0];
            QVector3D e2 = positions[i2] - positions[i0];
            QVector3D fn = QVector3D::crossProduct(e1, e2).normalized();
            if (i0 < normals.size()) normals[i0] += fn;
            if (i1 < normals.size()) normals[i1] += fn;
            if (i2 < normals.size()) normals[i2] += fn;
            faceVertices[i].ni = i0;
            faceVertices[i+1].ni = i1;
            faceVertices[i+2].ni = i2;
        }
        for (auto &n : normals) n.normalize();
        m_hasNormals = true;
    }

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // Build interleaved vertex data: [pos.x, pos.y, pos.z, u, v, nx, ny, nz]
    m_vertexData.clear();
    m_vertexData.reserve(faceVertices.size() * 8);

    for (const auto &fv : faceVertices) {
        if (fv.pi >= 0 && fv.pi < positions.size()) {
            const auto &p = positions[fv.pi];
            m_vertexData << p.x() << p.y() << p.z();
        } else {
            m_vertexData << 0.0f << 0.0f << 0.0f;
        }
        if (fv.ti >= 0 && fv.ti < texCoords.size()) {
            const auto &t = texCoords[fv.ti];
            m_vertexData << t.x() << t.y();
        } else {
            m_vertexData << 0.0f << 0.0f;
        }
        if (fv.ni >= 0 && fv.ni < normals.size()) {
            const auto &n = normals[fv.ni];
            m_vertexData << n.x() << n.y() << n.z();
        } else {
            m_vertexData << 0.0f << 1.0f << 0.0f;
        }
    }

    m_vertexCount = faceVertices.size();

    qDebug() << "[ModelViewerWidget] Vertex buffer built:" << m_vertexCount << "vertices,"
             << m_faceCount << "faces,"
             << positions.size() << "unique positions";

    return true;
}

bool ModelViewerWidget::parseMTL(const QString &path, const QString &objDir)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("map_Kd") || line.startsWith("map_kd")) {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                QString texFile = parts.last();
                m_texturePath = QDir(objDir).absoluteFilePath(texFile);
                qDebug() << "Texture found:" << m_texturePath;
            }
        }
    }
    file.close();
    return true;
}

void ModelViewerWidget::computeBoundingBox()
{
    if (m_vertexData.isEmpty()) return;

    m_bbMin = QVector3D(1e9, 1e9, 1e9);
    m_bbMax = QVector3D(-1e9, -1e9, -1e9);

    for (int i = 0; i < m_vertexData.size(); i += 8) {
        float x = m_vertexData[i];
        float y = m_vertexData[i + 1];
        float z = m_vertexData[i + 2];
        m_bbMin.setX(qMin(m_bbMin.x(), x));
        m_bbMin.setY(qMin(m_bbMin.y(), y));
        m_bbMin.setZ(qMin(m_bbMin.z(), z));
        m_bbMax.setX(qMax(m_bbMax.x(), x));
        m_bbMax.setY(qMax(m_bbMax.y(), y));
        m_bbMax.setZ(qMax(m_bbMax.z(), z));
    }

    m_bbCenter = (m_bbMin + m_bbMax) * 0.5f;
    m_bbRadius = (m_bbMax - m_bbMin).length() * 0.5f;
    if (m_bbRadius < 0.001f) m_bbRadius = 1.0f;
}

void ModelViewerWidget::buildGLBuffers()
{
    if (m_vertexData.isEmpty()) return;

    // Try to create VAO (optional - not all drivers support it)
    if (!m_vao.isCreated()) {
        m_vao.create(); // May fail silently on older drivers, that's OK
    }
    if (m_vao.isCreated()) m_vao.bind();

    if (!m_vbo.isCreated()) m_vbo.create();
    if (!m_vbo.isCreated()) {
        qWarning() << "[ModelViewerWidget] CRITICAL: Failed to create VBO!";
        return;
    }
    m_vbo.bind();
    m_vbo.allocate(m_vertexData.constData(),
                   m_vertexData.size() * sizeof(float));

    // Stride: 8 floats (pos3 + uv2 + normal3)
    const int stride = 8 * sizeof(float);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);

    // TexCoord attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));

    // Normal attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(5 * sizeof(float)));

    m_vbo.release();
    if (m_vao.isCreated()) m_vao.release();

    qDebug() << "[ModelViewerWidget] GL buffers built: VBO=" << m_vbo.isCreated()
             << "VAO=" << m_vao.isCreated()
             << "vertexCount=" << m_vertexCount;
}

void ModelViewerWidget::loadTextureIfAvailable()
{
    delete m_texture;
    m_texture = nullptr;

    if (m_texturePath.isEmpty() || !QFile::exists(m_texturePath) || !m_hasTexCoords) {
        qDebug() << "[ModelViewerWidget] No texture to load (path empty or no UVs)";
        return;
    }

    qDebug() << "[ModelViewerWidget] Loading texture:" << m_texturePath;

    QImage texImg(m_texturePath);
    if (texImg.isNull()) {
        qWarning() << "Cannot load texture image:" << m_texturePath;
        return;
    }

    qDebug() << "[ModelViewerWidget] Texture original size:" << texImg.width() << "x" << texImg.height();

    // Cap texture size for fast loading. 1024 is plenty for a real-time viewer.
    // A 4096x4096 RGBA image is ~64MB, 1024x1024 is ~4MB.
    static constexpr int kMaxTextureDim = 1024;
    if (texImg.width() > kMaxTextureDim || texImg.height() > kMaxTextureDim) {
        texImg = texImg.scaled(kMaxTextureDim, kMaxTextureDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "[ModelViewerWidget] Texture scaled to:" << texImg.width() << "x" << texImg.height();
    }

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    texImg = texImg.convertToFormat(QImage::Format_RGBA8888).mirrored();

    QOpenGLTexture *tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
    if (!tex->create()) {
        qWarning() << "[ModelViewerWidget] Failed to create OpenGL texture object";
        delete tex;
        return;
    }

    tex->setFormat(QOpenGLTexture::RGBA8_UNorm);
    tex->setSize(texImg.width(), texImg.height());
    tex->allocateStorage(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    tex->setData(texImg, QOpenGLTexture::DontGenerateMipMaps);
    tex->setMinificationFilter(QOpenGLTexture::Linear);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    m_texture = tex;
    qDebug() << "[ModelViewerWidget] Texture loaded successfully";
}

// ═══════════════════════════════════════════════════════════════════════════
// OpenGL lifecycle
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewerWidget::initializeGL()
{
    // Validate OpenGL context before proceeding
    if (!context() || !context()->isValid()) {
        qWarning() << "[ModelViewerWidget] OpenGL context is invalid!";
        m_glInitialized = false;
        return;
    }

    initializeOpenGLFunctions();
    m_glInitialized = true;

    const char* glVer = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* glRen = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    qDebug() << "[ModelViewerWidget] OpenGL initialized:"
             << "Version:" << (glVer ? glVer : "unknown")
             << "Renderer:" << (glRen ? glRen : "unknown");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Back-face culling eliminates the "transparent layered shell" z-fighting
    // that TripoSR meshes exhibit when both front and back faces render.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.06f, 0.09f, 0.14f, 1.0f);

    // Compile shaders with explicit attribute binding (no layout qualifiers)
    m_shaderProgram = new QOpenGLShaderProgram(this);

    // Bind attribute locations BEFORE linking
    m_shaderProgram->bindAttributeLocation("aPosition", 0);
    m_shaderProgram->bindAttributeLocation("aTexCoord", 1);
    m_shaderProgram->bindAttributeLocation("aNormal",   2);

    bool shaderOk = true;
    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qWarning() << "Vertex shader error:" << m_shaderProgram->log();
        shaderOk = false;
    }
    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qWarning() << "Fragment shader error:" << m_shaderProgram->log();
        shaderOk = false;
    }
    if (shaderOk && !m_shaderProgram->link()) {
        qWarning() << "Shader link error:" << m_shaderProgram->log();
        shaderOk = false;
    }
    if (!shaderOk) {
        qWarning() << "[ModelViewerWidget] Shader compilation failed!";
        m_diagnosticMsg = "Shader compilation failed: " + m_shaderProgram->log();
        delete m_shaderProgram;
        m_shaderProgram = nullptr;
        return;
    }

    qDebug() << "[ModelViewerWidget] Shaders compiled and linked OK";

    // Build GL buffers if model was loaded before GL init
    if (!m_vertexData.isEmpty()) {
        buildGLBuffers();
        loadTextureIfAvailable();
    }
}

void ModelViewerWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ModelViewerWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Check if we can render: need model + shader + VBO (VAO optional)
    if (!m_modelLoaded || !m_shaderProgram || !m_vbo.isCreated()) {
        goto draw_overlay;
    }

    {
        m_shaderProgram->bind();

        // If no VAO, bind VBO and set up attributes manually each frame
        if (!m_vao.isCreated()) {
            m_vbo.bind();
            const int stride = 8 * sizeof(float);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                                  reinterpret_cast<void*>(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                                  reinterpret_cast<void*>(5 * sizeof(float)));
        }

        // ── Matrices ──
        float aspect = float(width()) / float(qMax(1, height()));

        QMatrix4x4 projection;
        // Tight near/far ratio maximises depth buffer precision.
        // TripoSR meshes have overlapping faces that need precise z-sorting.
        const float nearPlane = qMax(0.1f, m_bbRadius * 0.1f);
        const float farPlane  = qMax(nearPlane + 1.0f, m_bbRadius * 6.0f);
        projection.perspective(45.0f, aspect, nearPlane, farPlane);

        float camDist = m_bbRadius * 2.8f / m_zoom;
        QVector3D eye(0, 0, camDist);

        QMatrix4x4 view;
        view.lookAt(eye, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

        QMatrix4x4 model;
        model.translate(m_panX * m_bbRadius * 0.01f, m_panY * m_bbRadius * 0.01f, 0);
        model.rotate(m_rotationX, 1, 0, 0);
        model.rotate(m_rotationY, 0, 1, 0);
        model.translate(-m_bbCenter);

        QMatrix3x3 normalMatrix = model.normalMatrix();

        m_shaderProgram->setUniformValue("uModel", model);
        m_shaderProgram->setUniformValue("uView", view);
        m_shaderProgram->setUniformValue("uProjection", projection);
        m_shaderProgram->setUniformValue("uNormalMatrix", normalMatrix);

        // ── Lighting ──
        QVector3D lightPos(m_bbRadius * 2.0f, m_bbRadius * 3.0f, m_bbRadius * 2.5f);
        m_shaderProgram->setUniformValue("uLightPos", lightPos);
        m_shaderProgram->setUniformValue("uViewPos", eye);
        m_shaderProgram->setUniformValue("uLightColor", QVector3D(1.0f, 0.98f, 0.95f));
        m_shaderProgram->setUniformValue("uObjectColor", QVector3D(0.7f, 0.7f, 0.75f));

        // ── Texture ──
        bool hasTex = m_texture && m_texture->isCreated() && m_hasTexCoords;
        m_shaderProgram->setUniformValue("uHasTexture", hasTex);
        if (hasTex) {
            m_texture->bind(0);
            m_shaderProgram->setUniformValue("uTexture", 0);
        }

        // ── Draw ──
        if (m_vao.isCreated()) m_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
        if (m_vao.isCreated()) m_vao.release();
        if (!m_vao.isCreated()) m_vbo.release();

        if (hasTex) m_texture->release();
        m_shaderProgram->release();
    }

draw_overlay:
    // ═══ 2D Overlay with QPainter ═══
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ── Draw "no model" or diagnostic message ──
    if (!m_modelLoaded || !m_shaderProgram || !m_vbo.isCreated()) {
        if (!m_modelLoaded) {
            QLinearGradient bg(rect().topLeft(), rect().bottomLeft());
            bg.setColorAt(0.0, QColor(17, 24, 39));
            bg.setColorAt(1.0, QColor(8, 15, 27));
            painter.fillRect(rect(), bg);
            painter.setPen(QColor(130, 130, 160));
            QFont f("Segoe UI", 16);
            f.setBold(true);
            painter.setFont(f);
            painter.drawText(rect(), Qt::AlignCenter,
                QString::fromUtf8("Glissez un fichier .obj ici\nou chargez un modele"));
        } else {
            // Model loaded but can't render — show diagnostic info
            painter.setPen(QColor(239, 68, 68));
            QFont f("Segoe UI", 12);
            f.setBold(true);
            painter.setFont(f);
            QString msg = "Erreur de rendu 3D:\n";
            if (!m_shaderProgram) msg += "- Shader non compile\n";
            if (!m_vbo.isCreated()) msg += "- VBO non cree\n";
            if (!m_diagnosticMsg.isEmpty()) msg += m_diagnosticMsg;
            painter.drawText(rect().adjusted(20, 20, -20, -20), Qt::AlignCenter | Qt::TextWordWrap, msg);
        }
    }

    // ── Draw strokes ──
    for (const auto &stroke : m_strokes) {
        if (stroke.points.size() < 2) continue;
        QPen pen(stroke.color, stroke.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        for (int i = 1; i < stroke.points.size(); ++i) {
            painter.drawLine(stroke.points[i - 1], stroke.points[i]);
        }
    }

    // ── Draw current stroke ──
    if (m_drawing && m_currentStroke.points.size() >= 2) {
        QPen pen(m_currentStroke.color, m_currentStroke.width,
                 Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        for (int i = 1; i < m_currentStroke.points.size(); ++i) {
            painter.drawLine(m_currentStroke.points[i - 1], m_currentStroke.points[i]);
        }
    }

    // ── Webcam preview ──
    if (m_showWebcam) {
        int pw = 200, ph = 150;
        QRect webcamRect(width() - pw - 10, 10, pw, ph);
        painter.setPen(QPen(QColor(99, 102, 241), 2));
        painter.setBrush(Qt::black);
        painter.drawRoundedRect(webcamRect, 8, 8);
        if (!m_webcamFrame.isNull()) {
            painter.drawImage(webcamRect.adjusted(2, 2, -2, -2),
                              m_webcamFrame.scaled(pw - 4, ph - 4, Qt::KeepAspectRatio));
        } else {
            painter.setPen(QColor(148, 163, 184));
            painter.drawText(webcamRect, Qt::AlignCenter, "Camera...");
        }
    }

    // ── Mode indicator ──
    {
        QString modeText;
        QColor modeColor;
        switch (m_interactionMode) {
        case ModeRotate:
            modeText = QString::fromUtf8("\xF0\x9F\x94\x84 Rotation");
            modeColor = QColor(99, 102, 241);
            break;
        case ModeDraw:
            modeText = QString::fromUtf8("\xe2\x9c\x8f\xef\xb8\x8f Dessin");
            modeColor = m_drawColor;
            break;
        case ModeErase:
            modeText = QString::fromUtf8("\xF0\x9F\xA7\xB9 Gomme");
            modeColor = QColor(239, 68, 68);
            break;
        }

        QFont f("Segoe UI", 11);
        f.setBold(true);
        painter.setFont(f);
        QFontMetrics fm(f);
        int tw = fm.horizontalAdvance(modeText) + 24;
        QRect badge(10, height() - 40, tw, 30);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 160));
        painter.drawRoundedRect(badge, 8, 8);
        painter.setPen(modeColor);
        painter.drawText(badge, Qt::AlignCenter, modeText);
    }

    painter.end();
}

// ═══════════════════════════════════════════════════════════════════════════
// Mouse interaction
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewerWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastMousePos = event->pos();
    m_mousePressed = true;
    m_pressedButton = event->button();

    if (m_interactionMode == ModeDraw && event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_currentStroke.points.clear();
        m_currentStroke.color = m_drawColor;
        m_currentStroke.width = m_drawWidth;
        m_currentStroke.points.append(event->position());
        update();
    }
    else if (m_interactionMode == ModeErase && event->button() == Qt::LeftButton) {
        // Erase strokes near click position
        QPointF pos = event->position();
        for (int i = m_strokes.size() - 1; i >= 0; --i) {
            for (const auto &pt : m_strokes[i].points) {
                if ((pt - pos).manhattanLength() < 20.0) {
                    m_strokes.removeAt(i);
                    update();
                    break;
                }
            }
        }
    }

    event->accept();
}

void ModelViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_mousePressed) return;

    QPoint delta = event->pos() - m_lastMousePos;
    m_lastMousePos = event->pos();

    if (m_interactionMode == ModeDraw && m_drawing) {
        m_currentStroke.points.append(event->position());
        update();
        return;
    }

    if (m_interactionMode == ModeErase && m_pressedButton == Qt::LeftButton) {
        QPointF pos = event->position();
        for (int i = m_strokes.size() - 1; i >= 0; --i) {
            for (const auto &pt : m_strokes[i].points) {
                if ((pt - pos).manhattanLength() < 20.0) {
                    m_strokes.removeAt(i);
                    update();
                    break;
                }
            }
        }
        return;
    }

    if (m_interactionMode == ModeRotate) {
        if (m_pressedButton == Qt::LeftButton) {
            // Rotate
            m_rotationY += delta.x() * 0.5f;
            m_rotationX += delta.y() * 0.5f;
            m_rotationX = qBound(-89.0f, m_rotationX, 89.0f);
        }
        else if (m_pressedButton == Qt::RightButton) {
            // Pan
            m_panX += delta.x() * 0.15f;
            m_panY -= delta.y() * 0.15f;
        }
        else if (m_pressedButton == Qt::MiddleButton) {
            // Zoom via drag
            m_zoom *= (1.0f + delta.y() * 0.005f);
            m_zoom = qBound(0.1f, m_zoom, 20.0f);
        }
        update();
    }

    event->accept();
}

void ModelViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_interactionMode == ModeDraw && m_drawing) {
        m_drawing = false;
        if (m_currentStroke.points.size() >= 2) {
            m_strokes.append(m_currentStroke);
        }
        m_currentStroke.points.clear();
        update();
    }

    m_mousePressed = false;
    m_pressedButton = Qt::NoButton;
    event->accept();
}

void ModelViewerWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f;
    m_zoom *= (1.0f + delta * 0.1f);
    m_zoom = qBound(0.1f, m_zoom, 20.0f);
    update();
    event->accept();
}

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

void ModelViewerWidget::setInteractionMode(InteractionMode mode)
{
    m_interactionMode = mode;
    switch (mode) {
    case ModeRotate: setCursor(Qt::ArrowCursor); break;
    case ModeDraw:   setCursor(Qt::CrossCursor); break;
    case ModeErase:  setCursor(Qt::PointingHandCursor); break;
    }
    emit interactionModeChanged(mode);
    update();
}

void ModelViewerWidget::setDrawColor(const QColor &color)
{
    m_drawColor = color;
    update();
}

void ModelViewerWidget::setDrawWidth(int width)
{
    m_drawWidth = qBound(1, width, 20);
}

void ModelViewerWidget::clearDrawings()
{
    m_strokes.clear();
    update();
}

void ModelViewerWidget::resetView()
{
    m_rotationX = 20.0f;
    m_rotationY = 0.0f;
    m_panX = 0.0f;
    m_panY = 0.0f;
    m_zoom = 1.0f;
    update();
}

void ModelViewerWidget::applyGestureRotation(float dx, float dy)
{
    m_rotationY += dx;
    m_rotationX += dy;
    m_rotationX = qBound(-89.0f, m_rotationX, 89.0f);
    update();
}

void ModelViewerWidget::applyGesturePan(float dx, float dy)
{
    m_panX += dx;
    m_panY += dy;
    update();
}

void ModelViewerWidget::applyGestureZoom(float delta)
{
    m_zoom += delta * 0.01f;
    if (m_zoom < 0.1f) m_zoom = 0.1f;
    if (m_zoom > 10.0f) m_zoom = 10.0f;
    update();
}

void ModelViewerWidget::applyGestureDraw(float nx, float ny)
{
    float px = nx * width();
    float py = ny * height();

    if (!m_drawing) {
        m_drawing = true;
        m_currentStroke.points.clear();
        m_currentStroke.color = m_drawColor;
        m_currentStroke.width = m_drawWidth;
    }
    m_currentStroke.points.append(QPointF(px, py));
    update();
}

void ModelViewerWidget::applyGestureDrawEnd()
{
    if (m_drawing) {
        m_drawing = false;
        if (m_currentStroke.points.size() >= 2) {
            m_strokes.append(m_currentStroke);
        }
        m_currentStroke.points.clear();
        update();
    }
}

void ModelViewerWidget::applyGestureErase(float nx, float ny)
{
    float px = nx * width();
    float py = ny * height();
    QPointF pos(px, py);

    bool erased = false;
    for (int i = m_strokes.size() - 1; i >= 0; --i) {
        for (const auto &pt : m_strokes[i].points) {
            if ((pt - pos).manhattanLength() < 40.0) { // Large radius for hand erase
                m_strokes.removeAt(i);
                erased = true;
                break;
            }
        }
    }
    if (erased) update();
}

void ModelViewerWidget::setWebcamFrame(const QImage &frame)
{
    m_webcamFrame = frame;
    update();
}

void ModelViewerWidget::setShowWebcam(bool show)
{
    m_showWebcam = show;
    update();
}
