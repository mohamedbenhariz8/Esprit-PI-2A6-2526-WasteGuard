#ifndef MODELVIEWERWIDGET_H
#define MODELVIEWERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColor>
#include <QImage>
#include <QPainterPath>
#include <QVector>

class ModelViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    // Interaction modes
    enum InteractionMode {
        ModeRotate,
        ModeDraw,
        ModeErase
    };

    explicit ModelViewerWidget(QWidget *parent = nullptr);
    ~ModelViewerWidget();

    /// Load an OBJ file (with optional MTL/texture)
    bool loadOBJ(const QString &objPath);

    /// Set interaction mode
    void setInteractionMode(InteractionMode mode);
    InteractionMode interactionMode() const { return m_interactionMode; }

    /// Set drawing color
    void setDrawColor(const QColor &color);
    QColor drawColor() const { return m_drawColor; }

    /// Set drawing pen width
    void setDrawWidth(int width);

    /// Clear all drawings
    void clearDrawings();

    /// Reset camera to default
    void resetView();

    /// Apply external transform from hand tracking
    void applyGestureRotation(float dx, float dy);
    void applyGesturePan(float dx, float dy);
    void applyGestureZoom(float delta);
    
    /// Apply external drawing from hand tracking
    void applyGestureDraw(float nx, float ny);
    void applyGestureDrawEnd();
    void applyGestureErase(float nx, float ny);

    /// Set webcam frame preview
    void setWebcamFrame(const QImage &frame);
    void setShowWebcam(bool show);

signals:
    void modelLoaded(bool success, int vertexCount, int faceCount);
    void interactionModeChanged(InteractionMode mode);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    // ── OBJ Parsing ──
    struct RawVertex {
        QVector3D position;
        QVector2D texCoord;
        QVector3D normal;
    };

    struct DrawStroke {
        QVector<QPointF> points;
        QColor color;
        int width;
    };

    bool parseOBJ(const QString &path);
    bool parseMTL(const QString &path, const QString &objDir);
    void computeBoundingBox();
    void buildGLBuffers();
    void loadTextureIfAvailable();

    // ── OpenGL resources ──
    QOpenGLShaderProgram *m_shaderProgram = nullptr;
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLTexture *m_texture = nullptr;

    // ── Mesh data ──
    QVector<float> m_vertexData;   // interleaved: pos3, uv2, normal3
    int m_vertexCount = 0;
    int m_faceCount = 0;
    bool m_hasTexCoords = false;
    bool m_hasNormals = false;
    bool m_modelLoaded = false;
    bool m_glInitialized = false;

    // ── Texture path ──
    QString m_texturePath;

    // ── Bounding box & centering ──
    QVector3D m_bbMin, m_bbMax, m_bbCenter;
    float m_bbRadius = 1.0f;

    // ── Camera ──
    float m_rotationX = 20.0f;
    float m_rotationY = 0.0f;
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    float m_zoom = 1.0f;

    // ── Mouse tracking ──
    QPoint m_lastMousePos;
    bool m_mousePressed = false;
    Qt::MouseButton m_pressedButton = Qt::NoButton;

    // ── Interaction ──
    InteractionMode m_interactionMode = ModeRotate;

    // ── Drawing overlay ──
    QVector<DrawStroke> m_strokes;
    DrawStroke m_currentStroke;
    QColor m_drawColor = QColor(255, 50, 50);
    int m_drawWidth = 3;
    bool m_drawing = false;

    // ── Webcam preview ──
    QImage m_webcamFrame;
    bool m_showWebcam = false;

    // ── Diagnostic info ──
    QString m_diagnosticMsg;
};

#endif // MODELVIEWERWIDGET_H
