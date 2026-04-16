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
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QStackedWidget;
class QTableWidget;
class QWidget;
class QPushButton;

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

private slots:
    // Employe
    void on_btnNouveau_clicked();
    void on_btnAjouter_clicked();
    void on_btnAnnuler_Ajout_clicked();
    void on_btnModifier_clicked();
    void on_btnSave_clicked();
    void on_btnAnalyser_clicked();
    void on_btnSimulerBadge_clicked();
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
    void sendContractExpirationEmail(const QString &clientEmail, const QString &clientName, const QString &expirationDate);
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

    // Notification system
    QTimer *m_stockNotifTimer = nullptr;
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
    void openLabibAssistant();

    QQuickWidget *m_mapAddOrder = nullptr;
    QQuickWidget *m_mapModOrder = nullptr;

    // Floating AI Button
    QPushButton *m_floatingAIButton = nullptr;
    void createFloatingAIButton();
    void onFloatingAIButtonPositionUpdate();

    // Email Notification Manager for client notifications
    EmailNotificationManager *m_emailManager = nullptr;

    // Initialize accessibility features
    void setupAccessibilityModule();
    void setupMaintenanceAccessibility();
    void addAccessibilityButtonsToMaintenance();
    void ensureScrollbarsVisible();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnToggleSidebar_clicked();

private:
   bool m_sidebarExpanded;
   void updateSidebarState();
};

#endif // MAINWINDOW_H
