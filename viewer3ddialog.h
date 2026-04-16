#ifndef VIEWER3DDIALOG_H
#define VIEWER3DDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QJsonObject>
#include "modelviewerwidget.h"

class Viewer3DDialog : public QDialog
{
    Q_OBJECT
public:
    explicit Viewer3DDialog(const QString &meshPath,
                            const QString &productName = QString(),
                            QWidget *parent = nullptr);
    ~Viewer3DDialog();

private slots:
    void onColorClicked();
    void onDrawToggled();
    void onEraseToggled();
    void onRotateToggled();
    void onClearDrawings();
    void onResetView();
    void onToggleWebcam();
    void onBrushSizeChanged(int val);

    // Hand tracking
    void onTrackerOutput();
    void onTrackerError();
    void onTrackerFinished(int exitCode, QProcess::ExitStatus status);

    void onModelLoaded(bool success, int vertexCount, int faceCount);

private:
    void setupUi();
    void setupToolPanel();
    void startHandTracker();
    void stopHandTracker();
    void processGestureJson(const QJsonObject &json);
    QPushButton *makeColorButton(const QColor &color, int size = 32);
    QPushButton *makeToolButton(const QString &text, const QString &tooltip);
    void updateToolButtonStates();

    // Viewer
    ModelViewerWidget *m_viewer;
    QString m_meshPath;
    QString m_productName;

    // Tool panel widgets
    QPushButton *m_btnRotate;
    QPushButton *m_btnDraw;
    QPushButton *m_btnErase;
    QPushButton *m_btnClear;
    QPushButton *m_btnReset;
    QPushButton *m_btnWebcam;
    QLabel *m_lblStatus;
    QLabel *m_lblGesture;
    QSlider *m_sliderBrush;
    QLabel *m_lblBrushSize;
    QLabel *m_webcamPreview = nullptr;

    // Current selected color button
    QPushButton *m_selectedColorBtn = nullptr;

    // Hand tracker process
    QProcess *m_trackerProcess = nullptr;
    bool m_webcamActive = false;

    // Gesture smoothing
    float m_gestureSmoothing = 0.6f;
};

#endif // VIEWER3DDIALOG_H
