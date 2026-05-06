#ifndef BACSTATUSDIALOG_H
#define BACSTATUSDIALOG_H

#include <QDialog>
#include <QString>
#include <QVector>
#include <QPointer>

class QComboBox;
class QLabel;
class QFrame;
class QProgressBar;
class QVBoxLayout;
class QPushButton;
class QTimer;
class QNetworkAccessManager;
class QNetworkReply;
class MainWindow;

// Visual status panel for a smart bin (BAC_INTEL).
// - Lets the user pick a bac from a combobox.
// - Shows the global status read from BAC_INTEL.STATUT_BAC
//   (EN_ATTENTE / EN_SERVICE / A_VIDER / EN_PANNE). The badge updates live
//   when MainWindow emits bacMotionStateChanged() (PIR sensor edges flip
//   EN_ATTENTE <-> EN_SERVICE in the DB).
// - Shows a card per compartment (4 compartments) with a provisional fill
//   percentage. Real per-compartment values will be wired once the schema
//   is extended.
class BacStatusDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BacStatusDialog(QWidget *parent = nullptr);
    ~BacStatusDialog() override;

private slots:
    void onBacChanged(int index);
    void onRefreshClicked();
    void onMotionStateChanged(bool active);
    void onCameraPollTick();
    void onCameraReplyFinished();
    void onTriggerClassifyClicked();
    void onClassificationUpdate(const QString &phase, const QString &material);
    void onBinFillUpdated(int binIndex, int percent);
    void onMachineStateChanged(bool on);

private:
    struct CompartmentCard {
        QFrame       *card        = nullptr;
        QLabel       *titleLabel  = nullptr;
        QLabel       *iconLabel   = nullptr;
        QLabel       *fillLabel   = nullptr;
        QLabel       *statusLabel = nullptr;
        QProgressBar *progress    = nullptr;
    };

    void buildUi();
    void buildCameraPanel(QVBoxLayout *root);
    void loadBacs();
    void refreshCurrentBac();
    void renderGlobalStatus(const QString &statut);
    void renderCompartments(int idBac, int globalRemplissage);
    QString resolveBacStatus(int idBac) const;
    static QVector<int> provisionalCompartmentFills(int idBac, int globalRemplissage);
    void setCameraStatus(const QString &text, const QString &color);
    void highlightTargetCompartment(const QString &material);

    bool m_motionActive = false;
    MainWindow *m_mw = nullptr;

    QComboBox    *m_cbBac        = nullptr;
    QPushButton  *m_btnRefresh   = nullptr;
    QPushButton  *m_btnClose     = nullptr;

    // Header / global status
    QLabel       *m_lblTitle     = nullptr;
    QLabel       *m_lblBacInfo   = nullptr;
    QFrame       *m_statusBadge  = nullptr;
    QLabel       *m_statusIcon   = nullptr;
    QLabel       *m_statusText   = nullptr;
    QLabel       *m_statusHint   = nullptr;

    // Compartment cards (4)
    QVector<CompartmentCard> m_compartments;

    // ----- Live camera panel (ESP32-CAM RGB565 RAW) -----
    QFrame   *m_camCard       = nullptr;
    QLabel   *m_camView       = nullptr;   // live preview frame
    QLabel   *m_camStatus     = nullptr;   // "Connecting...", "Classifying...", etc.
    QLabel   *m_camMaterial   = nullptr;   // last detected material
    QPushButton *m_btnTrigger = nullptr;   // in-app mirror of physical pin-5
    QTimer   *m_camPollTimer  = nullptr;
    QNetworkAccessManager *m_camNet = nullptr;
    QPointer<QNetworkReply> m_camReply;
    QString   m_camUrl        = QStringLiteral("http://10.161.104.88/capture.raw");
    int       m_camWidth      = 160;
    int       m_camHeight     = 120;
    bool      m_camOnline     = false;
    int       m_camErrorCount = 0;
};

#endif // BACSTATUSDIALOG_H
