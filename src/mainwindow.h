#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QByteArray>
#include <QString>
#include <QMap>
#include <QGridLayout> 
#include <QtCharts>
#include <QChartView>
#include <QPieSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QFileDialog>
#include <QPdfWriter>
#include <QTextDocument>
#include <QDesktopServices>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QInputDialog>
#include <QProgressDialog>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>
#include <QCalendarWidget>
#include <QPainter>
#include <QProcess>

class DotCalendar : public QCalendarWidget {
    Q_OBJECT
public:
    explicit DotCalendar(QWidget *parent = nullptr) : QCalendarWidget(parent) {}
    void setDayDots(const QDate &date, const QList<QColor> &dots) {
        m_dayData[date] = dots;
        update();
    }
    void clearDayDots() { m_dayData.clear(); update(); }
protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override {
        QCalendarWidget::paintCell(painter, rect, date);
        if (m_dayData.contains(date)) {
            const auto &dots = m_dayData[date];
            int n = qMin(dots.size(), 8);
            if (n > 0) {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                int dotSize = 5;
                int spacing = 2;
                int totalWidth = n * dotSize + (n - 1) * spacing;
                int startX = rect.center().x() - totalWidth / 2;
                int y = rect.bottom() - dotSize - 4;
                for (int i = 0; i < n; ++i) {
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(dots[i]);
                    painter->drawEllipse(startX + i * (dotSize + spacing), y, dotSize, dotSize);
                }
                painter->restore();
            }
        }
    }
private:
    QMap<QDate, QList<QColor>> m_dayData;
};

#include "employe.h"
#include "produit.h"
#include "stock.h"
#include "commande.h"
#include "client.h"
#include "intervention.h"
#include "stockmapwidget.h"
#include "triposr3ddialog.h"
#include "accessibilityhelper.h"
#include "voiceassistant.h"
#include "labibassistant.h"
#include "emailnotificationmanager.h"
#include "maintenancestatusdelegate.h"
#include "arduino.h"
#include "repairdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QStackedWidget;
class QTableWidget;
class QWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QDockWidget;
class QResizeEvent;
class QMoveEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setSessionContext(bool isAdmin, const QString &email);
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void logoutRequested();

public slots:
    Q_INVOKABLE void fetchDeviceLocation(QObject* mapObject);
    Q_INVOKABLE void fetchMyPosition();

private slots:
    // Employe
    void on_btnNouveau_clicked();
    void on_btnAjouter_clicked();
    void on_btnAnnuler_Ajout_clicked();
    void on_btnModifier_clicked();
    void on_btnSave_clicked();
    void on_btnAnalyser_clicked();
    void on_btnSimulerBadge_clicked();
    void onRfidSerialDataReady();
    void on_btnSupprimer_clicked();
    void on_cbProjetStats_currentIndexChanged(const QString &arg1);
    void on_btnFichePaie_clicked();
    void on_btnCommsSend_clicked();
    void on_btnLogout_clicked();

   
    // Client module slots (from mainwindowcl)
    void on_btnClient_clicked();
    void on_btn_ajouter_client_clicked();
    void on_btn_modifier_client_clicked();
    void on_btn_annuler_client_clicked();
    void on_btnNouveau_client_clicked();
    void onClientAdded(QString matricule, QString nom, QString email, QString type_contrat, QString date_creation, QString date_expiration, QString telephone);
    void onClientModified(int row, QString matricule, QString nom, QString email, QString type_contrat, QString date_creation, QString date_expiration, QString telephone);

    // Stock
    void on_btnSave_add_clicked();
    void on_btnSave_mod_clicked();
    void exportStockExcel();
    void on_btnPrediction_clicked();

    // Maintenance
    void on_btnSave_Add_clicked();
    void on_btnSave_Mod_clicked();
    void handleMaintEditClicked();
    void handleMaintDeleteClicked();

    // Produit
    void goAffichage();
    void goAjout();
    void goModification();
    void goStatistiques();
    void on_prod_btnSave_Add_clicked();
    void on_prod_btnSave_Mod_clicked();
    void on_prod_btnUpload_Add_clicked();
    void on_prod_btnUpload_Mod_clicked();
    void applyProduitFilterAndSort();
    void on_prod_btnPdf_clicked();
    void on_prod_btnBacStatus_clicked();
    void on_prod_btnMap3D_clicked();
    void onGeminiPdfReply(QNetworkReply *reply);
    void onProductImageDownloaded(QNetworkReply *reply, const QString &numSerie);
    void openStockMap3D();
    void on_prod_btnVideo3D_clicked();
    void on_prod_btn3DModel_clicked();

    void handleEditClicked();
    void handleDeleteClicked();

    // Card View Toggle (Produit)
    void slot_toggleView();

    // Pagination slots (Produit)
    void on_pagination_cbSize_currentIndexChanged(int index);
    void on_pagination_btnPrev_clicked();
    void on_pagination_btnNext_clicked();

    // Employee Card View
    void slot_toggleEmpView();
    void on_emp_pagination_cbSize_currentIndexChanged(int index);
    void on_emp_pagination_btnPrev_clicked();
    void on_emp_pagination_btnNext_clicked();

    // Stock Card View
    void slot_toggleStockView();
    void on_stock_pagination_cbSize_currentIndexChanged(int index);
    void on_stock_pagination_btnPrev_clicked();
    void on_stock_pagination_btnNext_clicked();

    // Client Card View
    void slot_toggleClientView();
    void on_client_pagination_cbSize_currentIndexChanged(int index);
    void on_client_pagination_btnPrev_clicked();
    void on_client_pagination_btnNext_clicked();

    // Maintenance Card View
    void slot_toggleMaintView();
    void on_maint_pagination_cbSize_currentIndexChanged(int index);
    void on_maint_pagination_btnPrev_clicked();
    void on_maint_pagination_btnNext_clicked();

    // Commande Card View
    void slot_toggleCmdView();
    void on_cmd_pagination_cbSize_currentIndexChanged(int index);
    void on_cmd_pagination_btnPrev_clicked();
    void on_cmd_pagination_btnNext_clicked();
    void loadCmdFromTableToForm(QTableWidget* t, int row);

    // Commande CRUD Slots
    void on_btnSave_Mod_3_clicked(); // Add Commande
    void on_btnSave_CmdMod_clicked();  // Modify Commande
    void on_btnCancel_Mod_3_clicked(); 
    void on_btnCancel_CmdMod_clicked(); 

    // ============ ACCESSIBILITÉ ET VOIX (SLOTS) ============
    void on_btnMicrophone_clicked();
    void handleHighContrastToggle();
    void handleZoomInClicked();
    void handleZoomOutClicked();

    // Voice Assistant Slots
    void onVoiceListeningStarted();
    void onVoiceListeningFinished();
    void onVoiceRecognized(const QString &text);
    void onVoiceCommandRecognized(const QString &command);
    void onVoiceError(const QString &errorMsg);
    void onVoiceSpeechFinished();

    // Commandes vocales pour maintenance
    void handleVoiceAddIntervention();
    void handleVoiceModifyIntervention();
    void handleVoiceDeleteIntervention();
    void handleVoiceSearchIntervention(const QString &searchTerm);
    void handleVoiceShowList();
    void handleVoiceExport();
    void handleVoiceHelp();

private:
    // Employe
    void setupStatistics();
    void setupAccueilDashboard();
    void setupDashboardCharts();
    void applyUnifiedTopBarStyle();
    void showDashboardHome();
    void showEmployesPage();
    void updateTaskChart(const QString &projectName);
    void installEmployeActionButtonsForRow(int row);
    void refreshEmployeActionButtons();
    Employe Etmp;
    Produit Ptmp;
    Stock Stmp;
    Commande Ctmp;
    Client CLtmp;
    Intervention INTtmp;
    void refreshEmployes();
    void applyEmployeSortAndFilter();
    void updateEmployeSidebarStats();
    // Produit
    void setupProduitModule();
    void applyStyleFix();
    void refreshActionButtons();
    void buildStatsCharts();
    void refreshProduitDashboardSummary();
    void refreshProduitTable();
    void addExampleRow();
    void verifyFournisseurEmail(QLineEdit *emailField);
    void loadProduitToModificationForm(int row);
    void installActionButtonsForRow(int row);
    void ensureProduitModuleVisible();
    void refreshProduitFeatureLists(const QStringList &selectedMod, int minQtyAdd = -1, int minQtyMod = -1, int idBacMod = -1);
    QString productStyleSheet() const;
    QString m_selectedEmplacement;

    // Card View implementation (Produit)
    void setupCardViewContainer();
    void refreshCardView();
    QWidget* createProductCard(int row);

    // Employee Card View implementation
    void setupEmpCardViewContainer();
    void refreshEmpCardView();
    QWidget* createEmployeeCard(int row);
    void setupEmployeModule();

    // Stock Module
    void setupStockModule();
    void setupStockTableData();
    void applyStockFilterAndSort();
    void buildStockStats();
    void checkStockNotifications();
    void showStockAlertsDialog();
    void on_btnSmartCart_clicked();

    // Stock Card View implementation
    void setupStockCardViewContainer();
    void refreshStockCardView();
    QWidget* createStockCard(int row);

    // Client Card View implementation
    void setupClientCardViewContainer();
    void refreshClientCardView();
    QWidget* createClientCard(int row);

    // Maintenance Card View implementation
    void setupMaintCardViewContainer();
    void refreshMaintCardView();
    QWidget* createMaintCard(int row);

    // Commande Card View implementation
    void setupCmdCardViewContainer();
    void refreshCmdCardView();
    QWidget* createCmdCard(int row);

    // Maintenance Module
    void setupMaintenanceModule();
    void populateMaintenanceBacCombo(int selectId = -1);
    void populateMaintenanceTechCombo();
    void setupMultiSelectCombo(QComboBox* combo);
    void updateMultiSelectComboText(QComboBox* combo);
    QList<int> selectedMaintenanceTechIds() const;
    QStringList selectedMaintenanceTechNames() const;
    void refreshInterventions(const QString &searchField = "", const QString &searchValue = "", const QString &sortCriteria = "id_inter ASC");
    QString getMaintSortCriteria(QComboBox* cb);
    void refreshMaintActionButtons();
    void installMaintActionButtonsForRow(int row);
    QTableWidget* maintenanceTable() const;
    void generateMaintenancePdf();

    void setupCommandesModule();
    void setupCommandesNavigation();
    void refreshCommandes(const QString &searchField = "", const QString &searchValue = "", const QString &sortCriteria = "");
    void refreshCmdStats();
    void buildCommandeStats();
    void updateCommandeMonthlyChart(int year);
    void populateCommandeBacList();
    void updateCommandeBacTotals();
    void populateCommandeBacListMod(int idCmd);
    void updateCommandeBacTotalsMod();
    void releaseBacsForCommande(int idCmd);
    void buildMaintenanceStats();
    void updateMaintenanceMonthlyChart(int year);
    void updateMaintenanceTypeCostChart(int year);
    void refreshDashboardTable(const QString &searchField = "", const QString &searchValue = "", const QString &sortCriteria = "");
    QString getCmdSortCriteria(QComboBox* cb);
    void on_btnPdf_Cmd_clicked();
    void generateCommandePdfForOrder(int commandeId, int row);
    void installCmdActions(int row);
    void installCmdActions2(int row);
    void reindexCmdActions();
    void reindexCmdActions2();
    void showCalendarView();
    void refreshCalendarEvents();
    void checkScheduledDeliveries();
    int currentCmdRow = -1;
    QTableWidget* m_lastCmdTable = nullptr;
    int m_lastCmdIndex = -1;
    int m_lastCmdId = 0;
    QString m_lastCmdRefer;
    void updateClientCombos();
    void loadCmdToModificationForm(int row);
    void openEmployeeTasksDialog();
    void openEmployeeLeaveDialog();
    bool isModuleAccessAllowed(const QString &moduleKey) const;
    bool guardModuleAccess(const QString &moduleKey, const QString &moduleLabel);
    void applySessionAccessControl();
    void installPageAccessGuard();
    void applyHomogeneousTheme();
    void ensureNavigationAndButtonConsistency();
    void installWindowGeometryGuard();
    void stabilizeWindowGeometry();
    void polishHomogeneousSurfaces();
    void ensureRetractableRightPanels();
    QWidget *ensureStockRightSidebarWidget();
    void installRetractableRightPanel(QWidget *panel);
    void updateRetractableRightPanel(QWidget *panel);
    void toggleRetractableRightPanel(const QString &panelObjectName);
    bool retractableRightPanelAvailable(const QString &panelObjectName) const;


    // Helpers for merged UI
    QStackedWidget *mainStacked() const;
    QWidget *produitRoot() const;
    QWidget *produitCentral() const;
    QStackedWidget *produitStacked() const;
    QWidget *produitPage(const char *objectName) const;
    QTableWidget *produitTable() const;

    Ui::MainWindow *ui;
    QButtonGroup *sidebarGroup;
    QWidget *homeDashboardPage;
    bool m_isAdminSession;
    QString m_sessionEmail;
    QString m_sessionSpecialite;
    QString m_employeeAllowedModule;

    bool m_employeeTaskPromptShown;
    int currentEmployeRow;
    QByteArray m_employeePhotoAjout;
    QString m_employeeFaceTemplateAjout;
    QByteArray m_employeePhotoModif;
    QString m_employeeFaceTemplateModif;
    QString m_currentEmployeeRfidOriginal;

    int currentProduitRow;
    int currentClientRow;
    int currentMaintRow;
    int m_lastMaintId = 0;
    int m_lastMaintBacId = 1;
    int m_lastMaintEmpId = 1;
    QString m_lastMaintStatut;
    int m_lastProdQty = 0;
    QWidget *globalStatsReturnPage;
    void addClientActionButtons(int row);
    int getRowForClientWidget(QWidget *widget);
    void refreshClients();
    void checkAndNotifyExpiringContracts();
    bool sendContractExpirationEmail(const QString &clientEmail, const QString &clientName, const QString &expirationDate, const QString &contractType = QString());
    void exportClientPdf();
    void showClientStats();
    void openEcoScoreInterface();
    void exportSingleClientContratPdf();
    void filterClients();
    bool validateClientForm(bool isModificationForm) const;
    void updateClientFormValidationState();
    void forceApplySidebarStyles();

    // Card View state (Produit)
    bool m_isCardView = false;
    QGridLayout *m_cardLayout = nullptr;
    int m_currentPage = 0;
    int m_itemsPerPage = 6;

    // Card View state (Employe)
    bool m_isEmpCardView = false;
    QGridLayout *m_empCardLayout = nullptr;
    int m_empCurrentPage = 0;
    int m_empItemsPerPage = 6;

    // Card View state (Stock)
    bool m_isStockCardView = false;
    QGridLayout *m_stockCardLayout = nullptr;
    int m_stockCurrentPage = 0;
    int m_stockItemsPerPage = 6;

    // Card View state (Client)
    bool m_isClientCardView = false;
    QGridLayout *m_clientCardLayout = nullptr;
    int m_clientCurrentPage = 0;
    int m_clientItemsPerPage = 6;
    QMap<QString, bool> m_rightPanelCollapsedStates;

    // Card View state (Maintenance)
    bool m_isMaintCardView = false;
    QGridLayout *m_maintCardLayout = nullptr;
    int m_maintCurrentPage = 0;
    int m_maintItemsPerPage = 6;

    // Card View state (Commande)
    bool m_isCmdCardView = false;
    QGridLayout *m_cmdCardLayout = nullptr;
    int m_cmdCurrentPage = 0;
    int m_cmdItemsPerPage = 6;

    // Maintenance photo paths
    QString m_photoAvantPath;
    QString m_photoApresPath;
    QString m_photoModPath;

    // Maintenance status delegate for row coloring
    MaintenanceStatusDelegate *m_maintenanceDelegate = nullptr;

    // Notification system
    QTimer *m_stockNotifTimer = nullptr;
    QTimer *m_clientContractNotifTimer = nullptr;
    QSystemTrayIcon *m_trayIcon = nullptr;

    // Produit photo paths
    QString m_prodImagePathAdd;
    QString m_prodImagePathMod;

    // Gemini PDF attributes
    QNetworkAccessManager *m_networkManager;
    QProgressDialog *m_geminiProgressDialog = nullptr;
    QMap<QString, QString> m_pdfTempImagePaths;
    int m_pendingImageDownloads = 0;
    QString m_geminiApiKey;
    QString m_geminiPrompt;

    // ============ ACCESSIBILITÉ ET VOIX ============
    // Accessibility
    AccessibilityHelper *m_accessibilityHelper;
    QPushButton *m_btnMicrophone;
    QPushButton *m_btnHighContrast;
    QPushButton *m_btnZoomPlus;
    QPushButton *m_btnZoomMinus;

    // Voice Assistant
    VoiceAssistant *m_voiceAssistant;
    QString m_lastVoiceRecognizedText;

    // Labib AI Assistant
    LabibAssistant *m_labibAssistant = nullptr;
    QDockWidget *m_labibDock = nullptr;
    void openLabibAssistant();

    QQuickWidget *m_mapAddOrder = nullptr;
    QQuickWidget *m_mapModOrder = nullptr;

    // Floating AI Button
    QPushButton *m_floatingAIButton = nullptr;
    QLabel *m_labibPeekLabel = nullptr;
    // Drag de la mascotte flottante (style bulle Messenger deplacable)
    QPoint m_floatingDragStartLocal;
    QPoint m_floatingDragStartGlobal;
    bool   m_floatingIsDragging   = false;
    bool   m_floatingHasCustomPos = false;
    void createFloatingAIButton();
    void onFloatingAIButtonPositionUpdate();

    // Email Notification Manager for client notifications
    EmailNotificationManager *m_emailManager = nullptr;

    // --- RFID / Pointage ---
    Arduino *m_arduino = nullptr;
    // Second Arduino dedicated to the servo block (lid + ultrasonic).
    // MainWindow forwards LID:OPEN / LID:CLOSE / MEASURE lines from the
    // main Arduino to this one, and parses the FILL:<pct> reply.
    Arduino *m_servoArduino = nullptr;
    QByteArray m_servoSerialBuffer;     // line accumulator for servo Arduino stdout
    int        m_lastClassifiedBin = 0; // bin of the most recent AI:<material>
    void ensureServoArduinoConnected();
    void forwardLidCommand(const QByteArray &cmd);
    void onServoArduinoSerialDataReady();
    // Auto-swap m_arduino <-> m_servoArduino when we discover the COM-port
    // assignments are reversed. Triggered by PING-probe mismatch, an IDENT
    // line on the wrong port, or an ERR:unknown:AI:* reply on the main port.
    void swapArduinoAssignments();
    // Synchronous probe: sends "PING\n" on the given QSerialPort and waits
    // briefly for "PONG". The servo sketch replies; the main sketch ignores.
    bool probePortIsServo(class QSerialPort *port);
    bool m_arduinoAssignmentVerified = false;
    QByteArray m_rfidSerialBuffer;
    QLineEdit *m_rfidScanTarget = nullptr;
    QDialog *m_rfidScanDialog = nullptr;
    void enterPointagePage();
    void ensureArduinoConnected();
    void performDailyAttendanceResetIfNeeded();
    void handleRfidTag(const QString &rfidTag);
    void appendPointageLog(const QString &rfidTag, const QString &employeName, const QString &status);
    void appendPointageLog(const QString &rfidTag, const QString &employeName, const QString &status, const QString &hoursDisplay);
    void startRfidCapture(QLineEdit *targetField);
    void stopRfidCapture();

    // --- Repair Mode (Technician RFID) ---
    bool m_repairModeActive = false;
    QString m_repairTechnicianName;
    int m_repairTechnicianId = -1;
    void handleTechnicianRfid(int empId, const QString &empName);
    void openRepairDialog();
    void closeRepairMode();

    // --- Capteur de mouvement (PIR sur A0 du meme Arduino) ---
    // Default: false (EN ATTENTE). Set to true when the sketch publishes
    // a MOTION:1 frame; held true for 60 s after the last detection, then
    // auto-revert to false. The bac status dialog listens to
    // bacMotionStateChanged() to flip its global badge live, and BAC_INTEL.
    // STATUT_BAC is updated in step (EN_ATTENTE <-> EN_SERVICE) - rows in
    // EN_PANNE / A_VIDER are never overwritten.
    bool m_bacMotionActive = false;
    int  m_motionTargetBacId = 0;   // 0 = no bac selected -> motion ignored
    QTimer *m_motionHoldTimer = nullptr;
    QProcess *m_aiProcess = nullptr;          // persistent classify_service.py
    QString   m_esp32CamUrl;                  // forwarded via ESP32_CAM_URL env var
    QByteArray m_aiStdoutBuffer;              // line-accumulator for service stdout
    bool      m_aiServiceReady = false;       // true once the daemon emits READY
    bool      m_aiPendingClassify = false;    // queued trigger while model is loading
    void onMotionHoldExpired();
    void applyBacMotionDbState(bool active, int idBac);
    void onAiServiceStdoutReady();
    void onAiServiceErrorReady();
    void onAiServiceFinished(int exitCode, QProcess::ExitStatus status);
public:
    // Lazy-starts the persistent waste-classification daemon. Safe to call
    // multiple times. The dialog calls this on open so the model loads
    // while the user is reading the bin info.
    void ensureClassificationService();
    bool isBacMotionActive() const { return m_bacMotionActive; }
    int  motionTargetBacId() const { return m_motionTargetBacId; }
    // Called by BacStatusDialog whenever the user picks a bac. Switching
    // bacs while motion is active immediately reverts the previous bac
    // back to EN_ATTENTE and resets the hold so the new bac will only
    // become EN_SERVICE on the next PIR edge.
    void setMotionTargetBac(int idBac);
    // Public so BacStatusDialog can mirror the physical pin-5 button
    // with an in-app button. Spawns classify_single.py, then forwards
    // AI:<material> to the Arduino which rotates the NEMA17 90 deg per bin.
    void triggerWasteClassification(const QString &source = QStringLiteral("manual"));
    // The bac-status dialog calls this so classify_single.py uses the
    // same ESP32-CAM URL the dialog is polling for the live preview.
    void setEsp32CamUrl(const QString &url) { m_esp32CamUrl = url; }
signals:
    void bacMotionStateChanged(bool active);
    // Emitted whenever a classification cycle progresses, so the bac
    // status dialog can mirror the live state next to the camera feed.
    // phase: "started" / "result" / "error"
    void wasteClassificationUpdate(const QString &phase, const QString &material);
    // Arduino published a fresh ultrasonic measurement after dropping
    // waste in compartment <binIndex>. percent in [0..100], or -1 on error.
    void binFillUpdated(int binIndex, int percent);
    // Arduino-side power state (button-driven for now, PIR later).
    void machineStateChanged(bool on);
private:

    // Initialize accessibility features
    void setupAccessibilityModule();
    void setupMaintenanceAccessibility();
    void addAccessibilityButtonsToMaintenance();
    void ensureScrollbarsVisible();

    // Settings / Theme customization module
    QWidget *m_settingsPage = nullptr;
    QPushButton *m_btnParametres = nullptr;
    void setupSettingsModule();
    void openSettingsPage();
    void rebuildSettingsCustomColorsList();
    void applyThemeFromManager();
    void recolorHardcodedStylesForTheme();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

private slots:
    void on_btnToggleSidebar_clicked();

private:
   bool m_sidebarExpanded;
   void updateSidebarState();
};

#endif // MAINWINDOW_H
