/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_Main;
    QFrame *sidebar;
    QVBoxLayout *verticalLayout_4;
    QPushButton *btnToggleSidebar;
    QLabel *label_logo;
    QPushButton *btnAccueil;
    QPushButton *btnStock;
    QPushButton *btnProduits;
    QPushButton *btnClient;
    QPushButton *btnEmployes;
    QPushButton *btnStatistiques;
    QPushButton *btnMaintenance;
    QPushButton *btnRetour;
    QPushButton *btnCommandes;
    QSpacerItem *verticalSpacer;
    QWidget *contentArea;
    QVBoxLayout *verticalLayout_Main;
    QStackedWidget *stackedWidget;
    QWidget *affichage;
    QHBoxLayout *horizontalLayout_Split;
    QVBoxLayout *verticalLayout_Left;
    QHBoxLayout *horizontalLayout_Header;
    QLabel *label_Title;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_User;
    QFrame *frameUser;
    QVBoxLayout *verticalLayout_User;
    QHBoxLayout *horizontalLayout_UserHeader;
    QVBoxLayout *verticalLayout_UserText;
    QLabel *lblUserName;
    QLabel *lblUserRole;
    QPushButton *btnNouveau;
    QHBoxLayout *horizontalLayout_Search;
    QLineEdit *txtSearch;
    QComboBox *cbSort;
    QTableWidget *tableEmployes;
    QFrame *sidePanel_Employe;
    QVBoxLayout *verticalLayout_Right;
    QFrame *statCard;
    QVBoxLayout *vl_stat1;
    QLabel *label_Stat1_Title;
    QLabel *lblStatNumber;
    QFrame *statCard_2;
    QVBoxLayout *vl_stat2;
    QLabel *label_Stat2_Title;
    QLabel *lblStatWorkload;
    QFrame *statCard_3;
    QVBoxLayout *vl_stat3;
    QLabel *label_Stat3_Title;
    QLabel *lblStatAvailability;
    QFrame *actionCard;
    QVBoxLayout *verticalLayout_Actions;
    QLabel *label_Actions;
    QPushButton *btnFichePaie;
    QPushButton *btnGoMission;
    QPushButton *btnGoPointage;
    QPushButton *btnGoStats;
    QSpacerItem *verticalSpacer_Right;
    QWidget *modification;
    QGridLayout *gridLayout_Modif;
    QHBoxLayout *headerLayout_Modif;
    QSpacerItem *horizontalSpacer_HeaderModif;
    QFrame *frameUser_Modif;
    QVBoxLayout *verticalLayout_User_Modif;
    QLabel *lblUserName_Modif;
    QLabel *lblUserRole_Modif;
    QFrame *cardFrame;
    QHBoxLayout *horizontalLayout_ModifFrame;
    QVBoxLayout *verticalLayout_Photo_Modif;
    QLabel *imageLabel;
    QPushButton *btnUploadPhoto;
    QSpacerItem *verticalSpacer_Photo;
    QVBoxLayout *verticalLayout_FormRight;
    QFormLayout *formLayout_Modif;
    QLabel *lblMatricule;
    QLineEdit *txtMatricule;
    QLabel *lblNom;
    QLineEdit *txtNom;
    QLabel *lblSpecialite;
    QComboBox *cbSpecialite;
    QLabel *lblStatut;
    QComboBox *cbStatut;
    QLabel *lblSalaire_Modif;
    QVBoxLayout *verticalLayout_Salaire_Modif;
    QSlider *sliderSalaire_Modif;
    QLabel *lblValSalaire_Modif;
    QLabel *label;
    QLabel *lblCIN_Modif;
    QLineEdit *txtCIN_Modif;
    QLineEdit *txtEmailModif;
    QSpacerItem *verticalSpacer_Form;
    QHBoxLayout *horizontalLayout_Buttons;
    QPushButton *btnDelete;
    QSpacerItem *horizontalSpacer_Buttons;
    QPushButton *btnAnnuler;
    QPushButton *btnSave;
    QWidget *ajout;
    QGridLayout *gridLayout_Ajout;
    QHBoxLayout *headerLayout_Ajout;
    QSpacerItem *horizontalSpacer_HeaderAjout;
    QFrame *frameUser_Ajout;
    QVBoxLayout *verticalLayout_User_Ajout;
    QLabel *lblUserName_Ajout;
    QLabel *lblUserRole_Ajout;
    QFrame *cardFrame_2;
    QHBoxLayout *horizontalLayout_AjoutFrame;
    QVBoxLayout *verticalLayout_Photo_Ajout;
    QLabel *imageLabel_2;
    QPushButton *btnUploadPhoto_Ajout;
    QSpacerItem *verticalSpacer_Photo_Ajout;
    QVBoxLayout *verticalLayout_FormRight_2;
    QFormLayout *formLayout_Ajout;
    QLabel *label_7;
    QLineEdit *txtNom_Ajout;
    QLabel *label_8;
    QComboBox *cbSpecialite_Ajout;
    QLabel *label_9;
    QComboBox *cbStatut_Ajout;
    QLabel *lblSalaire_Ajout;
    QVBoxLayout *verticalLayout_Salaire_Ajout;
    QSlider *sliderSalaire_Ajout;
    QLabel *lblValSalaire_Ajout;
    QLineEdit *txtEmailAjout;
    QLabel *label_2;
    QLabel *lblCIN_Ajout;
    QLineEdit *txtCIN_Ajout;
    QSpacerItem *verticalSpacer_Form_2;
    QHBoxLayout *horizontalLayout_Buttons_2;
    QSpacerItem *horizontalSpacer_Buttons_2;
    QPushButton *btnAnnuler_Ajout;
    QPushButton *btnAjouter;
    QWidget *mission;
    QVBoxLayout *verticalLayout_Mission;
    QLabel *lblTitreMission;
    QLineEdit *txtMission;
    QPushButton *btnAnalyser;
    QLabel *lblResultat;
    QTableWidget *tableResultat;
    QSpacerItem *verticalSpacer_Mission;
    QPushButton *btnAnnulerMission;
    QWidget *pointage;
    QVBoxLayout *verticalLayout_Pointage;
    QLabel *lblTitrePointage;
    QLabel *lblStatutRFID;
    QPushButton *btnSimulerBadge;
    QLabel *lblLog;
    QTableWidget *tablePointage;
    QSpacerItem *verticalSpacer_Pointage;
    QPushButton *btnAnnulerPointage;
    QWidget *statistiques;
    QVBoxLayout *verticalLayout_StatsPage;
    QHBoxLayout *horizontalLayout_Stats;
    QChartView *chartViewAbsence;
    QChartView *chartViewWorkload;
    QChartView *chartViewTasks;
    QComboBox *cbProjetStats;
    QSpacerItem *verticalSpacer_Stats;
    QPushButton *btnAnnulerStats;
    QWidget *pageProduit;
    QVBoxLayout *verticalLayout_ProduitPage;
    QHBoxLayout *prod_rootLayout;
    QFrame *prod_leftSidebar;
    QVBoxLayout *prod_verticalLayout_1;
    QLabel *prod_label_1;
    QSpacerItem *spacerItem;
    QPushButton *prod_btnhome_1;
    QPushButton *prod_btnstock_1;
    QPushButton *prod_btnproduit_1;
    QPushButton *prod_btnclient_1;
    QPushButton *prod_btnemploye_1;
    QPushButton *prod_btnmaintenance_1;
    QPushButton *prod_btncmd_1;
    QSpacerItem *spacerItem1;
    QVBoxLayout *prod_mainAreaLayout;
    QHBoxLayout *prod_dashboard_hLayout;
    QVBoxLayout *prod_dashboardRightLayout;
    QHBoxLayout *prod_contentLayout;
    QStackedWidget *prod_stackedWidget;
    QWidget *prod_affichage;
    QHBoxLayout *prod_affichageLayout;
    QVBoxLayout *prod_tableLayout;
    QHBoxLayout *prod_topBarLayout;
    QLabel *prod_label_Title;
    QSpacerItem *prod_topSpacer;
    QPushButton *prod_btnAddProduct;
    QSpacerItem *prod_topSpacer_User;
    QFrame *prod_frameUser;
    QVBoxLayout *prod_verticalLayout_User;
    QLabel *prod_lblUserName;
    QLabel *prod_lblUserRole;
    QHBoxLayout *prod_searchRow;
    QLineEdit *prod_searchInput;
    QComboBox *prod_cbSort;
    QPushButton *prod_btnToggleView;
    QStackedWidget *prod_viewStackedWidget;
    QWidget *prod_tableViewPage;
    QVBoxLayout *prod_tablePgLayout;
    QTableWidget *prod_tableProduits;
    QWidget *prod_cardViewPage;
    QVBoxLayout *prod_cardPgLayout;
    QScrollArea *prod_cardViewScrollArea;
    QWidget *prod_cardContainer;
    QGridLayout *prod_cardLayout;
    QFrame *prod_rightSidebar;
    QVBoxLayout *prod_rightLayout;
    QLabel *prod_lblFunc;
    QFrame *prod_stat1;
    QVBoxLayout *vboxLayout;
    QLabel *prod_st_val1;
    QLabel *prod_st_lbl1;
    QFrame *prod_stat2;
    QVBoxLayout *vboxLayout1;
    QLabel *prod_st_val2;
    QLabel *prod_st_lbl2;
    QFrame *prod_stat3;
    QVBoxLayout *vboxLayout2;
    QLabel *prod_st_val3;
    QLabel *prod_st_lbl3;
    QFrame *prod_statsPreview;
    QVBoxLayout *prod_statsPreviewLayout;
    QLabel *prod_lblStatsTitle;
    QFrame *prod_miniStatsFrame;
    QVBoxLayout *prod_miniStatsLayout;
    QHBoxLayout *prod_rowMiniLayout1;
    QLabel *prod_lblMini1;
    QProgressBar *prod_pbTrend;
    QHBoxLayout *prod_rowMiniLayout2;
    QLabel *prod_lblMini2;
    QProgressBar *prod_pbStock;
    QPushButton *prod_btnOpenStats;
    QPushButton *prod_btnMap3D;
    QPushButton *prod_btnVideo3D;
    QSpacerItem *spacerItem2;
    QLabel *prod_lblExport;
    QPushButton *prod_btnPdf;
    QSpacerItem *spacerItem3;
    QWidget *prod_ajout;
    QHBoxLayout *prod_ajout_hLayout;
    QVBoxLayout *prod_addRightLayout;
    QHBoxLayout *prod_hl_add_split;
    QVBoxLayout *prod_vl_img_add;
    QLabel *prod_lblImgPreview_Add;
    QPushButton *prod_btnUpload_Add;
    QSpacerItem *spacerItem4;
    QFrame *prod_cardAdd;
    QVBoxLayout *prod_verticalLayout_Add;
    QLabel *prod_lblTitleAdd;
    QGridLayout *prod_gl_inputs;
    QLabel *prod_l1;
    QLineEdit *prod_ln_ref_add;
    QLabel *prod_l2;
    QLineEdit *prod_ln_model_add;
    QLabel *prod_l_px;
    QDoubleSpinBox *prod_dsb_price_add;
    QLabel *prod_l_qty;
    QSpinBox *prod_sb_qty_add;
    QLabel *prod_l_stat;
    QComboBox *prod_cb_status_add;
    QHBoxLayout *prod_hl_sliders;
    QVBoxLayout *prod_vl_bat;
    QHBoxLayout *prod_horizontalLayout;
    QLabel *prod_l_bat;
    QLabel *prod_label;
    QSlider *prod_slider_bat_add;
    QVBoxLayout *prod_vl_cap;
    QHBoxLayout *prod_horizontalLayout_4;
    QLabel *prod_l_cap;
    QLabel *prod_label_4;
    QSlider *prod_slider_cap_add;
    QGroupBox *prod_gb_characteristics;
    QVBoxLayout *prod_vl_characteristics;
    QListWidget *prod_lstCharacteristics;
    QLabel *prod_l5;
    QHBoxLayout *prod_btns_add;
    QPushButton *prod_btnSave_Add;
    QPushButton *prod_btnCancel_Add;
    QWidget *prod_modification;
    QHBoxLayout *prod_modifier_hLayout;
    QVBoxLayout *prod_modRightLayout;
    QHBoxLayout *prod_hl_mod_split;
    QLabel *prod_imgBin;
    QFrame *prod_cardMod;
    QVBoxLayout *prod_verticalLayout_Mod;
    QLabel *prod_lblTitleMod;
    QGridLayout *prod_gl_inputs_mod;
    QSpinBox *prod_sb_qty_mod;
    QLabel *prod_l_stat_m;
    QLabel *prod_l_qty_m;
    QLineEdit *prod_ln_model_mod;
    QComboBox *prod_cb_status_mod;
    QDoubleSpinBox *prod_dsb_price_mod;
    QLineEdit *prod_ln_ref_mod;
    QLabel *prod_l2_m;
    QLabel *prod_l_px_m;
    QLabel *prod_l1_m;
    QHBoxLayout *prod_hl_sliders_mod;
    QVBoxLayout *prod_vl_bat_mod;
    QLabel *prod_l_bat_m;
    QSlider *prod_slider_bat_mod;
    QVBoxLayout *prod_vl_cap_mod;
    QLabel *prod_l_cap_m;
    QSlider *prod_slider_cap_mod;
    QGroupBox *prod_gb_characteristics_mod;
    QVBoxLayout *prod_vl_characteristics_mod;
    QListWidget *prod_lstCharacteristics_mod;
    QLabel *prod_l5_m;
    QHBoxLayout *prod_btns_mod;
    QPushButton *prod_btnSave_Mod;
    QPushButton *prod_btnCancel_Mod;
    QWidget *prod_statistiques;
    QVBoxLayout *prod_statsPopupLayout;
    QHBoxLayout *prod_statsHeaderLayout;
    QPushButton *prod_btnReturnStats;
    QSpacerItem *prod_horizontalSpacer_Stats;
    QLabel *prod_lblStatsHeader;
    QLabel *prod_lblStatsSub;
    QHBoxLayout *prod_graphsLayout;
    QFrame *prod_graphFrame1;
    QVBoxLayout *prod_graphFrame1Layout;
    QLabel *prod_graphFrame1Title;
    QFrame *prod_graphFrame1Plot;
    QVBoxLayout *prod_graphFrame1PlotLayout;
    QLabel *prod_graphFrame1Hint;
    QFrame *prod_graphFrame2;
    QVBoxLayout *prod_graphFrame2Layout;
    QLabel *prod_graphFrame2Title;
    QFrame *prod_graphFrame2Plot;
    QVBoxLayout *prod_graphFrame2PlotLayout;
    QLabel *prod_graphFrame2Hint;
    QLabel *prod_lblStatsNote;
    QSpacerItem *spacerItem5;
    QWidget *pageStock;
    QHBoxLayout *horizontalLayout_Main1;
    QVBoxLayout *mainVerticalLayout;
    QFrame *headerBar;
    QHBoxLayout *headerLayout;
    QLabel *headerTitle;
    QSpacerItem *horizontalSpacer_header;
    QPushButton *btnNew;
    QLabel *user;
    QStackedWidget *stock_stackedWidget;
    QWidget *pageInventaire;
    QHBoxLayout *dashboardLayout;
    QFrame *tableCard;
    QVBoxLayout *tableCardLayout;
    QHBoxLayout *tableHeaderActions;
    QLabel *lblTableTitle;
    QSpacerItem *horizontalSpacer_table;
    QHBoxLayout *stockSearchSortRow;
    QLineEdit *stock_searchInput;
    QComboBox *stock_sortCombo;
    QTableWidget *tableWidget;
    QVBoxLayout *rightColumn;
    QFrame *statsBox;
    QVBoxLayout *statsLayout;
    QLabel *lblStatsTitle;
    QLabel *totalStock;
    QLabel *lblCriticalStock;
    QFrame *orderBox;
    QVBoxLayout *orderLayout;
    QLabel *lblOrderTitle;
    QLabel *lblOrderSummary;
    QPushButton *btnOrder;
    QPushButton *btnPrediction;
    QPushButton *btnAlerts;
    QPushButton *btnGoStats_Stock;
    QSpacerItem *verticalSpacer_right;
    QWidget *pageAjouter;
    QVBoxLayout *layoutAjouter;
    QFrame *addCard;
    QVBoxLayout *addCardLayout;
    QLabel *lblAddTitle;
    QHBoxLayout *layoutRefNom_add;
    QLineEdit *inputRef_add;
    QLineEdit *inputNom_add;
    QHBoxLayout *layoutStock_add;
    QLineEdit *inputStock_add;
    QSlider *sliderStock_add;
    QHBoxLayout *layoutPrix_add;
    QLineEdit *inputPrix_add;
    QSlider *sliderPrix_add;
    QLineEdit *inputFournisseur_add;
    QHBoxLayout *addActions;
    QPushButton *btnCancel_add;
    QPushButton *btnSave_add;
    QWidget *pageModifier;
    QVBoxLayout *layoutModifier;
    QFrame *modifyCard;
    QVBoxLayout *modifyCardLayout;
    QLabel *lblModTitle;
    QHBoxLayout *layoutRefNom_mod;
    QLineEdit *inputRef_mod;
    QLineEdit *inputNom_mod;
    QHBoxLayout *layoutStock_mod;
    QLineEdit *inputStock_mod;
    QSlider *sliderStock_mod;
    QHBoxLayout *layoutPrix_mod;
    QLineEdit *inputPrix_mod;
    QSlider *sliderPrix_mod;
    QLineEdit *inputFournisseur_mod;
    QHBoxLayout *modifyActions;
    QPushButton *btnCancel_mod;
    QPushButton *btnSave_mod;
    QWidget *page_Maintenance_Tab;
    QVBoxLayout *vl_Maint_Root;
    QStackedWidget *stackedWidget_Maintenance;
    QWidget *page_Maint_Dash;
    QVBoxLayout *vl_page_Maint_Dash;
    QScrollArea *scrollAreaMaint;
    QWidget *scrollContentMaint;
    QVBoxLayout *vlScrollMaint;
    QHBoxLayout *horizontalLayout_1;
    QFrame *topBar;
    QHBoxLayout *topLayout_1;
    QLabel *titleLabel;
    QSpacerItem *spacerItem6;
    QPushButton *btnGotoAjout;
    QLabel *userLabel;
    QPushButton *btnNotif;
    QPushButton *btnProfil;
    QHBoxLayout *contentLayout_1;
    QVBoxLayout *tableLayout;
    QHBoxLayout *searchRow;
    QLabel *lblSort;
    QComboBox *cbSort1;
    QLineEdit *searchInput;
    QPushButton *btnGotoModifier;
    QFrame *card;
    QVBoxLayout *vl_card_dash;
    QLabel *lblCardTitle;
    QTableWidget *tableMaintenance;
    QFrame *rightSidebar;
    QVBoxLayout *rightLayout_1;
    QLabel *lblFunc;
    QFrame *stat1;
    QVBoxLayout *vlStat1;
    QLabel *st_icon1;
    QLabel *st_val1;
    QLabel *st_lbl1;
    QFrame *stat2;
    QVBoxLayout *vlStat2;
    QLabel *st_icon2;
    QLabel *st_val2;
    QLabel *st_lbl2;
    QFrame *stat3;
    QVBoxLayout *vlStat3;
    QLabel *st_icon3;
    QLabel *st_val3;
    QLabel *st_lbl3;
    QSpacerItem *spacerItem7;
    QPushButton *btnGoStats_Maint;
    QLabel *lblExport;
    QPushButton *btnPdf;
    QSpacerItem *vSpacerBottom_Maint;
    QFrame *miniChartFrame;
    QVBoxLayout *vlMiniChart;
    QLabel *lblMiniChartTitle;
    QHBoxLayout *hlMiniBar1;
    QLabel *lblMiniBar1;
    QProgressBar *pbMini1;
    QLabel *lblMiniPct1;
    QHBoxLayout *hlMiniBar2;
    QLabel *lblMiniBar2;
    QProgressBar *pbMini2;
    QLabel *lblMiniPct2;
    QHBoxLayout *hlMiniBar3;
    QLabel *lblMiniBar3;
    QProgressBar *pbMini3;
    QLabel *lblMiniPct3;
    QWidget *page_Maint_Ajout;
    QVBoxLayout *vl_page_Maint_Ajout;
    QHBoxLayout *hlTopAjout;
    QFrame *topBar_Aj;
    QHBoxLayout *hlTopInnerAjout;
    QLabel *titleLabel_Aj;
    QSpacerItem *spacerItem8;
    QLabel *userLabel_Aj;
    QVBoxLayout *vlAjoutRoot;
    QScrollArea *scrollArea_MaintAjout;
    QWidget *scrollContent_MaintAjout;
    QVBoxLayout *vlScrollMaintAjout;
    QPushButton *btnBack_Ajout;
    QFrame *cardAdd;
    QVBoxLayout *vlCardAdd;
    QLabel *lblTitreFormAdd;
    QFrame *sepMaintAdd1;
    QHBoxLayout *hlRefAdd;
    QVBoxLayout *vboxLayout3;
    QLabel *lblRefAdd;
    QLineEdit *editRefAdd;
    QHBoxLayout *hlDateDureeAdd;
    QVBoxLayout *vboxLayout4;
    QLabel *lblDateAdd;
    QDateEdit *dateAdd;
    QVBoxLayout *vboxLayout5;
    QLabel *lblDurAdd;
    QComboBox *comboDurAdd;
    QHBoxLayout *hlCoutAdd;
    QVBoxLayout *vboxLayout6;
    QLabel *lblCoutAdd;
    QHBoxLayout *hboxLayout;
    QDoubleSpinBox *spinCoutAdd;
    QLabel *lblEuroAdd;
    QHBoxLayout *hlPrioAdd;
    QVBoxLayout *vboxLayout7;
    QLabel *lblPrioAdd;
    QComboBox *comboPrioAdd;
    QFrame *sepMaintAdd2;
    QHBoxLayout *hlTechAddrAdd;
    QVBoxLayout *vboxLayout8;
    QLabel *lblTechAdd;
    QLineEdit *editTechAdd;
    QVBoxLayout *vboxLayout9;
    QLabel *lblAddrAdd;
    QLineEdit *editAddrAdd;
    QFrame *sepMaintAdd3;
    QVBoxLayout *vlDescAdd;
    QLabel *lblDescAdd;
    QTextEdit *txtDescAdd;
    QFrame *sepMaintAdd4;
    QHBoxLayout *hlPhotosAdd;
    QVBoxLayout *vboxLayout10;
    QLabel *lblPhotosAvantAdd;
    QLabel *lblImgPreview_Add;
    QVBoxLayout *vboxLayout11;
    QLabel *lblPhotosApresAdd;
    QLabel *lblImgPreview2_Add;
    QHBoxLayout *hlButtonsAdd;
    QSpacerItem *spacerItem9;
    QPushButton *btnCancel_Add;
    QPushButton *btnSave_Add;
    QWidget *page_Maint_Modif;
    QVBoxLayout *vl_page_Maint_Modif;
    QHBoxLayout *hlTopMod;
    QFrame *topBar_Mod;
    QHBoxLayout *hlTopInnerMod;
    QLabel *titleLabel_Mod;
    QSpacerItem *spacerItem10;
    QLabel *userLabel_Mod;
    QVBoxLayout *vlModRoot;
    QScrollArea *scrollArea_MaintModif;
    QWidget *scrollContent_MaintModif;
    QVBoxLayout *vlScrollMaintModif;
    QPushButton *btnBack_Modif;
    QFrame *cardMod;
    QVBoxLayout *vlCardMod;
    QLabel *lblTitreFormMod;
    QFrame *sepMaintMod1;
    QHBoxLayout *hlRefMod;
    QVBoxLayout *vboxLayout12;
    QLabel *lblRefMod;
    QLineEdit *editRefMod;
    QHBoxLayout *hlDateDureeMod;
    QVBoxLayout *vboxLayout13;
    QLabel *lblDateMod;
    QDateEdit *dateMod;
    QVBoxLayout *vboxLayout14;
    QLabel *lblDurMod;
    QComboBox *comboDurMod;
    QHBoxLayout *hlCoutMod;
    QVBoxLayout *vboxLayout15;
    QLabel *lblCoutMod;
    QHBoxLayout *hboxLayout1;
    QDoubleSpinBox *spinCoutMod;
    QLabel *lblEuroMod;
    QHBoxLayout *hlPrioMod;
    QVBoxLayout *vboxLayout16;
    QLabel *lblPrioMod;
    QComboBox *comboPrioMod;
    QFrame *sepMaintMod2;
    QLabel *lblSecTechMod;
    QHBoxLayout *hlTechAddrMod;
    QVBoxLayout *vboxLayout17;
    QLabel *lblTechMod;
    QLineEdit *editTechMod;
    QVBoxLayout *vboxLayout18;
    QLabel *lblAddrMod;
    QLineEdit *editAddrMod;
    QFrame *sepMaintMod3;
    QLabel *lblSecDescMod;
    QVBoxLayout *vlDescMod;
    QLabel *lblDescMod;
    QTextEdit *txtDescMod;
    QFrame *sepMaintMod4;
    QLabel *lblSecPhotoMod;
    QHBoxLayout *hlPhotosMod;
    QVBoxLayout *vboxLayout19;
    QLabel *lblPhotosAvantMod;
    QLabel *lblImgPreview_Mod;
    QLabel *lblLastUpdate;
    QHBoxLayout *hlButtonsMod;
    QSpacerItem *spacerItem11;
    QPushButton *btnCancel_Mod;
    QPushButton *btnSave_Mod;
    QWidget *pageClient;
    QVBoxLayout *verticalLayout_ClientRoot;
    QWidget *header_Client;
    QHBoxLayout *horizontalLayout_header_Client;
    QLabel *wasteguardhead_Client;
    QSpacerItem *horizontalSpacer_h_Client;
    QPushButton *btnNouveau_Client;
    QLabel *responsable_Client;
    QStackedWidget *stackedWidget_Client;
    QWidget *page_repertoire;
    QVBoxLayout *verticalLayout_repertoire;
    QHBoxLayout *horizontalLayout_filters;
    QLineEdit *recherche;
    QSpacerItem *spacer_filter_gap;
    QLabel *lblTrier;
    QComboBox *cbTrier;
    QSpacerItem *horizontalSpacer_f;
    QHBoxLayout *horizontalLayout_table;
    QTableWidget *tableWidget_Client;
    QWidget *sidepanel;
    QVBoxLayout *verticalLayout_sidepanel;
    QLabel *pilotage;
    QWidget *growthWidget;
    QVBoxLayout *vboxLayout20;
    QLabel *label1;
    QLabel *label2;
    QSpacerItem *spacerItem12;
    QPushButton *exportclient;
    QPushButton *btnGoStats_Client;
    QWidget *page_ajouter_client_wrapper;
    QVBoxLayout *verticalLayout_ajouter_client_wrapper;
    QSpacerItem *vs_a1_client;
    QFrame *formCard_ajouter;
    QVBoxLayout *vboxLayout21;
    QLabel *label3;
    QGridLayout *grid_ajouter;
    QLabel *lbl_matricule_a;
    QLineEdit *input_matricule_ajouter;
    QLabel *lbl_nom_a;
    QLineEdit *input_nom_ajouter;
    QLabel *lbl_email_a;
    QLineEdit *input_email_ajouter;
    QLabel *lbl_contrat_a;
    QComboBox *input_contrat_ajouter;
    QLabel *lbl_paiement_a;
    QComboBox *input_paiement_ajouter;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem13;
    QPushButton *btn_annuler_ajouter;
    QPushButton *btn_save_ajouter;
    QSpacerItem *spacerItem14;
    QSpacerItem *vs_a2_client;
    QWidget *page_modifier_client_wrapper;
    QVBoxLayout *verticalLayout_modifier_client_wrapper;
    QSpacerItem *vs_m1_client;
    QFrame *formCard_modifier;
    QVBoxLayout *vboxLayout22;
    QLabel *label4;
    QGridLayout *grid_modifier;
    QLabel *lbl_matricule_m;
    QLineEdit *input_matricule_modifier;
    QLabel *lbl_nom_m;
    QLineEdit *input_nom_modifier;
    QLabel *lbl_email_m;
    QLineEdit *input_email_modifier;
    QLabel *lbl_contrat_m;
    QComboBox *input_contrat_modifier;
    QLabel *lbl_paiement_m;
    QComboBox *input_paiement_modifier;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem15;
    QPushButton *btn_annuler_modifier;
    QPushButton *btn_save_modifier;
    QSpacerItem *spacerItem16;
    QSpacerItem *vs_m2_client;
    QWidget *page_stats_client;
    QVBoxLayout *vLayout_stats_client;
    QHBoxLayout *hLayout_statsHeader_client;
    QLabel *lblStatsTitle_client;
    QSpacerItem *hs_statsClient;
    QPushButton *btnRetour_stats_client;
    QHBoxLayout *hLayout_statsCharts_client;
    QChartView *chartEcoScorePie;
    QChartView *chartEcoScoreBar;
    QChartView *chartContratDist;
    QWidget *pageCmdDashboard;
    QHBoxLayout *horizontalLayout_Main2;
    QVBoxLayout *rightContentLayout;
    QHBoxLayout *horizontalLayout_11;
    QLabel *lb_1;
    QFrame *topFill_1;
    QPushButton *btnAddDashboard;
    QLabel *user_1;
    QFrame *topGap_1;
    QPushButton *btnnotif_1;
    QPushButton *btnprofil_1;
    QStackedWidget *contentStack;
    QWidget *pageHome;
    QHBoxLayout *pageHomeLayout;
    QVBoxLayout *dashboardTableArea;
    QHBoxLayout *searchRow_dashboard;
    QSpacerItem *spacerItem17;
    QLabel *lblSort_dashboard;
    QComboBox *cbSortDashboard;
    QLineEdit *searchInputDashboard;
    QPushButton *btnTempToModifier;
    QTableWidget *tableDashboard;
    QWidget *pageCommandes;
    QHBoxLayout *pageCommandesLayout;
    QVBoxLayout *tableLayout_2;
    QHBoxLayout *searchRow_2;
    QSpacerItem *spacerItem18;
    QLabel *lblSort_2;
    QComboBox *cbSort_2;
    QLineEdit *searchInput_2;
    QPushButton *btnAddProduct_2;
    QTableWidget *tableProduits_2;
    QFrame *rightSidebar_Commande;
    QVBoxLayout *rightLayout_Cmd;
    QLabel *lblFunc_Cmd;
    QFrame *stat1_Cmd;
    QVBoxLayout *vboxLayout23;
    QLabel *st_val1_Cmd;
    QLabel *st_lbl1_Cmd;
    QFrame *stat2_Cmd;
    QVBoxLayout *vboxLayout24;
    QLabel *st_val2_Cmd;
    QLabel *st_lbl2_Cmd;
    QFrame *stat3_Cmd;
    QVBoxLayout *vboxLayout25;
    QLabel *st_val3_Cmd;
    QLabel *st_lbl3_Cmd;
    QSpacerItem *spacerItem19;
    QPushButton *btnGoStats_Cmd;
    QLabel *lblExport_Cmd;
    QPushButton *btnPdf_Cmd;
    QWidget *pageCmdAjout;
    QHBoxLayout *hbox_ajout;
    QVBoxLayout *vbox_ajout_content;
    QHBoxLayout *horizontalLayout_9;
    QLabel *lb_5;
    QFrame *topFill_5;
    QLabel *user_5;
    QFrame *topGap_5;
    QPushButton *btnnotif_5;
    QPushButton *btnprofil_5;
    QScrollArea *scrollArea_CmdAjout;
    QWidget *scrollContent_CmdAjout;
    QHBoxLayout *hl_mod_split_3;
    QFrame *cardMod_3;
    QVBoxLayout *verticalLayout_Mod_3;
    QLabel *lblTitleMod_3;
    QGridLayout *gl_inputs_4;
    QDoubleSpinBox *dsb_price_add_4;
    QLabel *l_px_4;
    QLabel *l1_4;
    QLabel *l_stat_4;
    QComboBox *cb_client_add;
    QComboBox *cb_model_add_4;
    QComboBox *cb_status_add_4;
    QSpinBox *sb_qty_add_4;
    QLabel *l_qty_4;
    QLabel *l2_4;
    QSpacerItem *spacerBeforeDates;
    QFrame *separatorLine1;
    QLabel *lblSectionDates;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_27;
    QLabel *label_28;
    QComboBox *comboBox_19;
    QLabel *label_29;
    QComboBox *comboBox_20;
    QLabel *label_30;
    QComboBox *comboBox_21;
    QHBoxLayout *horizontalLayout_111;
    QLabel *label_31;
    QLabel *label_32;
    QComboBox *comboBox_22;
    QLabel *label_33;
    QComboBox *comboBox_23;
    QLabel *label_34;
    QComboBox *comboBox_24;
    QSpacerItem *spacerBeforeAdresse;
    QFrame *separatorLine2;
    QLabel *lblSectionAdresse;
    QHBoxLayout *horizontalLayout;
    QLabel *label5;
    QTextEdit *textEdit;
    QSpacerItem *spacerBeforeBtns;
    QFrame *separatorLine3;
    QHBoxLayout *btns_mod_3;
    QPushButton *btnSave_Mod_3;
    QPushButton *btnCancel_Mod_3;
    QWidget *pageCmdModifier;
    QHBoxLayout *hbox_mod;
    QVBoxLayout *vbox_mod_content;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lb_3;
    QFrame *topFill_3;
    QLabel *user_3;
    QFrame *topGap_3;
    QPushButton *btnnotif_3;
    QPushButton *btnprofil_3;
    QScrollArea *scrollArea_CmdModif;
    QWidget *scrollContent_CmdModif;
    QHBoxLayout *hl_mod_split;
    QFrame *cardMod1;
    QVBoxLayout *verticalLayout_Mod;
    QLabel *lblTitleMod;
    QGridLayout *gl_inputs_2;
    QDoubleSpinBox *dsb_price_add_2;
    QLabel *l_stat_2;
    QLabel *l_qty_2;
    QComboBox *cb_status_add_2;
    QLabel *l_px_2;
    QLabel *l2_2;
    QLabel *l1_2;
    QComboBox *cb_client_mod;
    QSpinBox *sb_qty_add_2;
    QComboBox *cb_model_add_2;
    QSpacerItem *spacerBeforeDatesMod;
    QFrame *sepLineMod1;
    QLabel *lblSectionDates_Mod;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_11;
    QLabel *label_12;
    QComboBox *comboBox_7;
    QLabel *label_13;
    QComboBox *comboBox_8;
    QLabel *label_14;
    QComboBox *comboBox_9;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_19;
    QLabel *label_20;
    QComboBox *comboBox_13;
    QLabel *label_21;
    QComboBox *comboBox_14;
    QLabel *label_22;
    QComboBox *comboBox_15;
    QSpacerItem *spacerBeforeAdresseMod;
    QFrame *sepLineMod2;
    QLabel *lblSectionAdresse_Mod;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_6;
    QTextEdit *textEdit_2;
    QSpacerItem *spacerBeforeBtnsMod;
    QFrame *sepLineMod3;
    QHBoxLayout *btns_mod;
    QPushButton *btnSave_CmdMod;
    QPushButton *btnCancel_CmdMod;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1505, 1030);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"QMainWindow, QWidget {\n"
"    background-color: #f4f6f9; \n"
"    font-family: \"Segoe UI\", sans-serif;\n"
"}\n"
"\n"
"QLabel {\n"
"    color: #333333 !important;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QFrame#sidebar {\n"
"    background-color: #0f2b4c;\n"
"    border: none;\n"
"}\n"
"QFrame#sidebar QLabel {\n"
"    color: white !important;\n"
"    background-color: transparent; \n"
"}\n"
"\n"
"QLabel#label_logo {\n"
"    background-color: transparent !important;\n"
"    border: none;\n"
"}\n"
"\n"
"QFrame#sidebar QPushButton {\n"
"    background-color: transparent;\n"
"    border: none;\n"
"    color: #bdc3c7;\n"
"    text-align: left;\n"
"    padding: 12px 20px;\n"
"    font-size: 14px;\n"
"    border-left: 3px solid transparent;\n"
"}\n"
"QFrame#sidebar QPushButton:hover, QFrame#sidebar QPushButton:checked {\n"
"    background-color: #1a4270;\n"
"    color: white;\n"
"    border-left: 3px solid #3498db;\n"
"}\n"
"\n"
"QFrame#statCard, QFrame#statCard_2, QFrame#statCard_3, QFrame#actionCard, QFrame#"
                        "cardFrame, QFrame#cardFrame_2 { \n"
"    background-color: #f4f6f9;\n"
"    border-radius: 12px;\n"
"    border: 1px solid #dce1e6;\n"
"    border-bottom: 2px solid #dce1e6;\n"
"}\n"
"\n"
"QLabel#lblStatNumber {\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    color: #0f2b4c !important;\n"
"}\n"
"\n"
"QPushButton#btnExport, QPushButton#btnFichePaie {\n"
"    background-color: #ffffff;\n"
"    color: #333333 !important;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 8px;\n"
"    padding: 10px 15px;\n"
"    text-align: left;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"    margin-top: 5px;\n"
"}\n"
"QPushButton#btnExport:hover, QPushButton#btnFichePaie:hover {\n"
"    background-color: #e3f2fd;\n"
"    border: 1px solid #3498db;\n"
"    color: #0f2b4c !important;\n"
"}\n"
"\n"
"QPushButton#btnSave, QPushButton#btnAjouter, QPushButton#btnNouveau {\n"
"    background-color: #28a745;\n"
"    color: white !important;\n"
"    border-radius: 5px;\n"
"    padding: 8px 15px;\n"
"    font"
                        "-weight: bold;\n"
"    border: none;\n"
"}\n"
"QPushButton#btnSave:hover, QPushButton#btnAjouter:hover, QPushButton#btnNouveau:hover {\n"
"    background-color: #218838;\n"
"}\n"
"QPushButton#btnNouveau {\n"
"    background-color: #0f2b4c;\n"
"}\n"
"QPushButton#btnNouveau:hover {\n"
"    background-color: #1a4270;\n"
"}\n"
"\n"
"QPushButton#btnAnnuler, QPushButton#btnAnnuler_Ajout {\n"
"    background-color: #6c757d;\n"
"    color: white !important;\n"
"    border-radius: 5px;\n"
"    padding: 8px 15px;\n"
"    font-weight: bold;\n"
"    border: none;\n"
"}\n"
"QPushButton#btnAnnuler:hover, QPushButton#btnAnnuler_Ajout:hover {\n"
"    background-color: #5a6268;\n"
"}\n"
"\n"
"QPushButton#btnDelete {\n"
"    background-color: #dc3545;\n"
"    color: white !important;\n"
"    border-radius: 5px;\n"
"    padding: 8px 15px;\n"
"    font-weight: bold;\n"
"    border: none;\n"
"}\n"
"QPushButton#btnDelete:hover {\n"
"    background-color: #c82333;\n"
"}\n"
"\n"
"QPushButton#btnUploadPhoto, QPushButton#btnUploadPhoto"
                        "_Ajout {\n"
"    background-color: #ffffff;\n"
"    color: #0f2b4c !important;\n"
"    border: 1px dashed #0f2b4c;\n"
"    border-radius: 8px;\n"
"    padding: 6px;\n"
"    font-weight: bold;\n"
"    margin-top: 5px;\n"
"}\n"
"QPushButton#btnUploadPhoto:hover, QPushButton#btnUploadPhoto_Ajout:hover {\n"
"    background-color: #e3f2fd;\n"
"    border: 1px solid #0f2b4c;\n"
"}\n"
"\n"
"QTableWidget {\n"
"    background-color: #f4f6f9;\n"
"    alternate-background-color: #e8ebf0;\n"
"    color: #000000 !important;\n"
"    gridline-color: transparent;\n"
"    border: 1px solid #dce1e6;\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"}\n"
"QTableWidget::item {\n"
"    color: #000000 !important;\n"
"    padding: 5px;\n"
"    border-bottom: 1px solid #e0e6ed;\n"
"}\n"
"QTableWidget::item:selected {\n"
"    background-color: #e3f2fd;\n"
"    color: #0f2b4c !important;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #0f2b4c;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    f"
                        "ont-weight: bold;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    padding: 6px;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 1px solid #3498db;\n"
"}\n"
"\n"
"QComboBox {\n"
"    background-color: #ffffff;\n"
"    color: #333333;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    padding: 6px;\n"
"}\n"
"QComboBox:focus {\n"
"    border: 1px solid #3498db;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #333333;\n"
"    selection-background-color: #0f2b4c;\n"
"    selection-color: #ffffff;\n"
"    border: 1px solid #cccccc;\n"
"}\n"
"\n"
"QLabel#imageLabel, QLabel#imageLabel_2 {\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 10px;\n"
"    background-color: #f4f6f9;\n"
"    color: #888888 !important;\n"
"}\n"
"\n"
"QTabWidget::pane { border: none; }\n"
"QTabBar::tab {\n"
"    background: #e0e4e8;\n"
"    color"
                        ": #333 !important;\n"
"    padding: 8px 20px;\n"
"    margin-right: 2px;\n"
"}\n"
"QTabBar::tab:selected {\n"
"    background: #0f2b4c;\n"
"    color: white !important;\n"
"}\n"
"\n"
"QLabel#lblUserName, QLabel#lblUserName_Modif, QLabel#lblUserName_Ajout {\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    color: #0f2b4c !important;\n"
"}\n"
"QLabel#lblUserRole, QLabel#lblUserRole_Modif, QLabel#lblUserRole_Ajout {\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    color: #28a745 !important;\n"
"    margin-top: -2px;\n"
"}\n"
"\n"
"/* Style Sp?cifique pour l'onglet Mission */\n"
"QPushButton#btnAnalyser {\n"
"    background-color: #6c5ce7;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"    padding: 12px;\n"
"    border-radius: 6px;\n"
"}\n"
"QPushButton#btnAnalyser:hover {\n"
"    background-color: #5b4cc4;\n"
"}\n"
"QLabel#lblTitreMission {\n"
"    font-size: 16px;\n"
"    color: #0f2b4c !important;\n"
"}\n"
"QLabel#lblResultat {\n"
"    font-size: 14px;\n"
"    color: #27ae60 !importan"
                        "t;\n"
"}\n"
"\n"
"/* Style pour l'onglet Pointage */\n"
"QLabel#lblStatutRFID {\n"
"    background-color: #ecf0f1;\n"
"    color: #7f8c8d;\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    border-radius: 10px;\n"
"    padding: 20px;\n"
"    border: 2px dashed #bdc3c7;\n"
"}\n"
"QPushButton#btnSimulerBadge {\n"
"    background-color: #e67e22;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"    padding: 10px;\n"
"    border-radius: 5px;\n"
"    margin-top: 10px;\n"
"}\n"
"QPushButton#btnSimulerBadge:hover {\n"
"    background-color: #d35400;\n"
"}\n"
"\n"
"QSlider::groove:horizontal {\n"
"    border: 1px solid #cccccc;\n"
"    height: 8px;\n"
"    background: #e0e0e0;\n"
"    margin: 2px 0;\n"
"    border-radius: 4px;\n"
"}\n"
"QSlider::handle:horizontal {\n"
"    background: #0f2b4c;\n"
"    border: 1px solid #0f2b4c;\n"
"    width: 18px;\n"
"    height: 18px;\n"
"    margin: -6px 0;\n"
"    border-radius: 9px;\n"
"}\n"
"/* ======================================================= */\n"
"/* STYLE U"
                        "NIFI? POUR LES 5 BOUTONS PRINCIPAUX              */\n"
"/* ======================================================= */\n"
"\n"
"/* 1. Style de base pour les deux boutons du haut ET les trois du bas */\n"
"QPushButton#btnFichePaie, QPushButton#btnExport,\n"
"QPushButton#btnGoMission, QPushButton#btnGoPointage, QPushButton#btnGoStats {\n"
"    background-color: #ffffff;\n"
"    color: #333333;\n"
"    border: 1px solid #dcdcdc;\n"
"    border-radius: 8px;\n"
"    padding: 15px 20px;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    text-align: left;\n"
"}\n"
"\n"
"/* 2. Effet quand la souris passe dessus (Hover) pour les 5 boutons */\n"
"QPushButton#btnFichePaie:hover, QPushButton#btnExport:hover,\n"
"QPushButton#btnGoMission:hover, QPushButton#btnGoPointage:hover, QPushButton#btnGoStats:hover {\n"
"    background-color: #f0f8ff;\n"
"    border: 1px solid #3498db;\n"
"    color: #000000 !important;\n"
"}\n"
"\n"
"/* 3. Effet quand on clique dessus (Pressed) pour les 5 boutons */\n"
"QPushButton#btnFiche"
                        "Paie:pressed, QPushButton#btnExport:pressed,\n"
"QPushButton#btnGoMission:pressed, QPushButton#btnGoPointage:pressed, QPushButton#btnGoStats:pressed {\n"
"    background-color: #e1ecf4;\n"
"    border: 1px solid #2980b9;\n"
"}\n"
"/* Style pour les boutons de RETOUR dans les pages Mission, Pointage et Stats */\n"
"QPushButton#btnAnnulerMission, \n"
"QPushButton#btnAnnulerPointage, \n"
"QPushButton#btnAnnulerStats {\n"
"    background-color: #6c757d;  \n"
"    color: white;               \n"
"    border: none;\n"
"    border-radius: 5px;\n"
"    padding: 10px 20px;\n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"    min-width: 200px;           \n"
"    margin-top: 20px;           \n"
"}\n"
"\n"
"/* Effet au survol */\n"
"QPushButton#btnAnnulerMission:hover, \n"
"QPushButton#btnAnnulerPointage:hover, \n"
"QPushButton#btnAnnulerStats:hover {\n"
"    background-color: #5a6268;  \n"
"}\n"
"/* ======================================================= */\n"
"/* 1. CORRECTION DES BOUTONS DE RETOUR (Pages interne"
                        "s)    */\n"
"/* ======================================================= */\n"
"/* Cela rendra les boutons \"Retour\" bien visibles en gris fonc? */\n"
"QPushButton#btnAnnulerMission, \n"
"QPushButton#btnAnnulerPointage, \n"
"QPushButton#btnAnnulerStats {\n"
"    background-color: #6c757d;  \n"
"    color: white !important;    \n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 10px 25px;         \n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"    margin-top: 15px;           \n"
"    min-width: 150px;           \n"
"}\n"
"\n"
"QPushButton#btnAnnulerMission:hover, \n"
"QPushButton#btnAnnulerPointage:hover, \n"
"QPushButton#btnAnnulerStats:hover {\n"
"    background-color: #5a6268;  \n"
"}\n"
"\n"
"/* ======================================================= */\n"
"/* 2. CORRECTION DE LA BOITE DE DIALOGUE (Supprimer)       */\n"
"/* ======================================================= */\n"
"QPushButton#msgBoxYes, QPushButton#msgBoxNo {\n"
"    background-color: #0f2b4c;\n"
"    color: w"
                        "hite !important;\n"
"    border-radius: 5px;\n"
"    padding: 6px 20px;\n"
"    min-width: 80px;\n"
"    font-weight: bold;\n"
"}\n"
"QMessageBox QPushButton {\n"
"    background-color: #0f2b4c;\n"
"    color: white !important;\n"
"    border-radius: 5px;\n"
"    padding: 6px 20px;\n"
"    min-width: 80px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QMessageBox QPushButton:hover {\n"
"    background-color: #1a4270;\n"
"}\n"
"\n"
"/* ======================================================= */\n"
"/* 3. UNIFICATION DES 5 BOUTONS DU DASHBOARD (Accueil)     */\n"
"/* ======================================================= */\n"
"QPushButton#btnFichePaie, QPushButton#btnExport,\n"
"QPushButton#btnGoMission, QPushButton#btnGoPointage, QPushButton#btnGoStats {\n"
"    background-color: #ffffff;\n"
"    color: #333333 !important;\n"
"    border: 1px solid #dcdcdc;\n"
"    border-radius: 8px;\n"
"    padding: 15px;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#b"
                        "tnFichePaie:hover, QPushButton#btnExport:hover,\n"
"QPushButton#btnGoMission:hover, QPushButton#btnGoPointage:hover, QPushButton#btnGoStats:hover {\n"
"    background-color: #e3f2fd;\n"
"    border: 1px solid #3498db;\n"
"    color: #000000 !important;\n"
"}\n"
"/* STYLE MAINTENANCE (AJOUTE)                              */\n"
"/* ======================================================= */\n"
"QWidget#sideDashboard, QWidget#sideAjout, QWidget#sideMod {\n"
"    background-color: #0f2b4c;\n"
"    border: none;\n"
"}\n"
"QWidget#sideDashboard QLabel, QWidget#sideAjout QLabel, QWidget#sideMod QLabel {\n"
"    color: white;\n"
"    background-color: transparent; \n"
"}\n"
"/* Boutons du menu lateral Maintenance */\n"
"QPushButton[objectName^=\"sideMenuButton\"] {\n"
"    background-color: transparent;\n"
"    border: none;\n"
"    color: #bdc3c7;\n"
"    text-align: left;\n"
"    padding: 12px 20px;\n"
"    font-size: 14px;\n"
"    border-left: 3px solid transparent;\n"
"}\n"
"QPushButton[objectName^=\"sideMenuButton"
                        "\"]:hover, \n"
"QPushButton[objectName^=\"sideMenuButton\"]:checked {\n"
"    background-color: #1a4270;\n"
"    color: white;\n"
"    border-left: 3px solid #3498db;\n"
"}\n"
"/* Bouton RETOUR special Sidebar */\n"
"QPushButton#btnBack_Ajout, QPushButton#btnBack_Modif {\n"
"    background-color: #d35400; \n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 4px;\n"
"    padding: 8px 15px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"    margin: 10px 15px;\n"
"}\n"
"QPushButton#btnBack_Ajout:hover, QPushButton#btnBack_Modif:hover {\n"
"    background-color: #e67e22;\n"
"}\n"
"/* TOP BAR Maintenance */\n"
"QFrame#topBar, QFrame#topBar_Aj, QFrame#topBar_Mod {\n"
"    background-color: #ffffff;\n"
"    border-bottom: 1px solid #dce1e6;\n"
"}\n"
"QFrame#topBar QLabel, QFrame#topBar_Aj QLabel, QFrame#topBar_Mod QLabel {\n"
"    color: #0f2b4c;\n"
"}\n"
"/* CARTES ET CONTENEURS Maintenance */\n"
"QFrame#card, QFrame#cardAdd, QFrame#cardMod, \n"
"QFrame#rightSidebar, QFrame#stat1, QFrame#s"
                        "tat2, QFrame#stat3 { \n"
"    background-color: #ffffff;\n"
"    border-radius: 12px;\n"
"    border: 1px solid #dce1e6;\n"
"    border-bottom: 2px solid #dce1e6;\n"
"}\n"
"/* Stats Textes Maintenance */\n"
"QLabel.statValue, QLabel.alertValue {\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    color: #0f2b4c;\n"
"}\n"
"QLabel.statLabel {\n"
"    color: #546e7a;\n"
"    font-size: 13px;\n"
"}\n"
"QLabel.alertValue {\n"
"    color: #c62828;\n"
"}\n"
"QFrame.alertCard {\n"
"    background-color: #ffebee;\n"
"    border: 1px solid #ffcdd2;\n"
"}\n"
"/* BOUTONS D'ACTION Maintenance */\n"
"QPushButton#btnSave_Add, QPushButton#btnSave_Mod, QPushButton#btnGotoModifier {\n"
"    background-color: #28a745;\n"
"    color: white;\n"
"    border-radius: 5px;\n"
"    padding: 8px 15px;\n"
"    font-weight: bold;\n"
"    border: none;\n"
"}\n"
"QPushButton#btnSave_Add:hover, QPushButton#btnSave_Mod:hover {\n"
"    background-color: #218838;\n"
"}\n"
"QPushButton#btnGotoAjout {\n"
"    background-color: #0f2b4c;\n"
""
                        "    color: #ffffff;\n"
"    border-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-size: 14px;\n"
"    font-weight: 700;\n"
"    border: none;\n"
"}\n"
"QPushButton#btnGotoAjout:hover {\n"
"    background-color: #1a4270;\n"
"}\n"
"QPushButton#btnCancel_Add, QPushButton#btnCancel_Mod {\n"
"    background-color: #6c757d;\n"
"    color: white;\n"
"    border-radius: 5px;\n"
"    padding: 8px 15px;\n"
"    font-weight: bold;\n"
"    border: none;\n"
"}\n"
"QPushButton#btnCancel_Add:hover, QPushButton#btnCancel_Mod:hover {\n"
"    background-color: #5a6268;\n"
"}\n"
"QPushButton#btnPdf {\n"
"    background-color: #ffffff;\n"
"    color: #1f2d3d;\n"
"    border: 1px solid #dce1e6;\n"
"    border-radius: 10px;\n"
"    padding: 12px 16px;\n"
"    text-align: center;\n"
"    font-weight: 700;\n"
"    font-size: 14px;\n"
"    min-height: 52px;\n"
"}\n"
"QPushButton#btnPdf:hover {\n"
"    background-color: #e3f2fd;\n"
"    border: 1px solid #3498db;\n"
"    color: #0f2b4c;\n"
"}\n"
"QPushButton#btnPdf:disabled {\n"
""
                        "    background-color: #ffffff;\n"
"    color: #1f2d3d;\n"
"    border: 1px solid #dce1e6;\n"
"}\n"
"QPushButton#btnNotif, QPushButton#btnProfil {\n"
"    background-color: #f0f2f5;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    border: 1px solid #dce1e6;\n"
"    color: #0f2b4c;\n"
"}\n"
"QPushButton#btnNotif:hover, QPushButton#btnProfil:hover {\n"
"    background-color: #e3f2fd;\n"
"    border-color: #3498db;\n"
"}\n"
"/* FORMULAIRES Maintenance */\n"
"QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    padding: 6px;\n"
"    min-height: 20px;\n"
"}\n"
"QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {\n"
"    border: 1px solid #3498db;\n"
"}\n"
"QComboBox {\n"
"    background-color: #ffffff;\n"
"    color: #333333;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    padding: 6px;\n"
"}\n"
"QComboBox:focus {\n"
"    border: 1px solid "
                        "#3498db;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #ffffff;\n"
"    color: #333333;\n"
"    selection-background-color: #0f2b4c;\n"
"    selection-color: #ffffff;\n"
"    border: 1px solid #cccccc;\n"
"}\n"
"QTextEdit {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 8px;\n"
"    padding: 6px;\n"
"    color: #333333;\n"
"}\n"
"QLabel.formLabel {\n"
"    color: #0f2b4c;\n"
"    font-weight: 700;\n"
"    font-size: 13px;\n"
"    margin-bottom: 4px;\n"
"}\n"
"QLabel#lblImgPreview_Add, QLabel#lblImgPreview_Mod, QLabel#lblImgPreview2_Add {\n"
"    border: 2px dashed #0f2b4c;\n"
"    border-radius: 10px;\n"
"    background-color: #f4f6f9;\n"
"    color: #888888;\n"
"}\n"
"\n"
" QPushButton#btnAddDashboard,\n"
"QPushButton#btnAddProduct_2,\n"
"QPushButton#btnSave_Mod,\n"
"QPushButton#btnSave_Mod_3 {\n"
"    background-color: #49b14f;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 14px;\n"
"    font-weigh"
                        "t: 700;\n"
"}\n"
"\n"
"QPushButton#btnAddDashboard:hover,\n"
"QPushButton#btnAddProduct_2:hover,\n"
"QPushButton#btnSave_Mod:hover,\n"
"QPushButton#btnSave_Mod_3:hover {\n"
"    background-color: #3ea445;\n"
"}\n"
"\n"
"QPushButton#btnTempToModifier {\n"
"    background-color: #124e84;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 14px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QPushButton#btnTempToModifier:hover {\n"
"    background-color: #0f3f6a;\n"
"}\n"
"\n"
"QPushButton#btnCancel_Mod,\n"
"QPushButton#btnCancel_Mod_3 {\n"
"    background-color: #a9b8c3;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 14px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QPushButton#btnCancel_Mod:hover,\n"
"QPushButton#btnCancel_Mod_3:hover {\n"
"    background-color: #96a7b4;\n"
"}\n"
"\n"
"QFrame#topFill_1, QFrame#topFill_3, QFrame#topFill_5,\n"
"QFrame#topGap_1, QFrame#topGap_3, QFrame#topGap_5 {\n"
"    background-color: #0f2f57;\n"
"  "
                        "  border: none;\n"
"    min-height: 56px;\n"
"    max-height: 56px;\n"
"    border-radius: 0px;\n"
"}\n"
"\n"
"QLabel#lb_1, QLabel#lb_3, QLabel#lb_5 {\n"
"    background-color: #0f2f57;\n"
"    color: #ffffff;\n"
"    font-size: 18px;\n"
"    font-weight: 800;\n"
"    padding: 0px 20px;\n"
"    min-height: 56px;\n"
"    max-height: 56px;\n"
"    border-radius: 0px;\n"
"}\n"
"\n"
"QLabel#user_1, QLabel#user_3, QLabel#user_5 {\n"
"    background-color: #0f2f57;\n"
"    color: #ffffff;\n"
"    font-size: 12px;\n"
"    font-weight: 700;\n"
"    padding: 0px 10px 0px 0px;\n"
"    min-height: 56px;\n"
"    max-height: 56px;\n"
"    border-radius: 0px;\n"
"}\n"
"\n"
"QPushButton#btnnotif_3, QPushButton#btnnotif_5 {\n"
"    background-color: #0f2f57;\n"
"    color: #f0b331;\n"
"    border: none;\n"
"    border-radius: 0px;\n"
"    min-width: 40px;\n"
"    max-width: 40px;\n"
"    min-height: 56px;\n"
"    max-height: 56px;\n"
"    padding: 0px;\n"
"}\n"
"\n"
"QPushButton#btnprofil_3, QPushButton#btnprofil_5 {\n"
"    "
                        "background-color: #0f2f57;\n"
"    color: #6b4b8a;\n"
"    border: none;\n"
"    border-radius: 0px;\n"
"    min-width: 40px;\n"
"    max-width: 40px;\n"
"    min-height: 56px;\n"
"    max-height: 56px;\n"
"    padding: 0px;\n"
"}\n"
"\n"
"/* --- Unified right sidebars (Employees reference) --- */\n"
"QFrame#sidePanel_Employe,\n"
"QFrame#rightSidebar,\n"
"QFrame#rightSidebar_Commande,\n"
"QWidget#sidepanel,\n"
"QFrame#prod_rightSidebar {\n"
"    background: transparent;\n"
"    border: none;\n"
"}\n"
"\n"
"QFrame#statCard, QFrame#statCard_2, QFrame#statCard_3, QFrame#actionCard,\n"
"QFrame#statsBox, QFrame#orderBox,\n"
"QWidget#growthWidget {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #dce1e6;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QFrame#stat1, QFrame#stat2, QFrame#stat3,\n"
"QFrame#stat1_Cmd, QFrame#stat2_Cmd, QFrame#stat3_Cmd {\n"
"    background-color: #ffffff;\n"
"    border: none;\n"
"    border-radius: 16px;\n"
"    padding: 18px 16px;\n"
"}\n"
"\n"
"QFrame#stat1:hover, QFrame#sta"
                        "t2:hover, QFrame#stat3:hover,\n"
"QFrame#stat1_Cmd:hover, QFrame#stat2_Cmd:hover, QFrame#stat3_Cmd:hover {\n"
"    background-color: #f0f7ff;\n"
"    border: 1px solid #b3d4fc;\n"
"}\n"
"\n"
"QLabel#lblFunc, QLabel#lblFunc_Cmd, QLabel#pilotage, QLabel#lblStatsTitle, QLabel#lblOrderTitle, QLabel#label_Actions {\n"
"    color: #1f2d3d;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"}\n"
"\n"
"QLabel#st_icon1, QLabel#st_icon2, QLabel#st_icon3,\n"
"QLabel#st_icon1_Cmd, QLabel#st_icon2_Cmd, QLabel#st_icon3_Cmd {\n"
"    font-size: 28px;\n"
"    padding: 0px;\n"
"    background: transparent;\n"
"    border: none;\n"
"}\n"
"\n"
"QFrame#miniChartFrame {\n"
"    background-color: #ffffff;\n"
"    border: none;\n"
"    border-radius: 16px;\n"
"    padding: 14px;\n"
"}\n"
"\n"
"QLabel#lblMiniChartTitle {\n"
"    font-size: 13px;\n"
"    font-weight: 700;\n"
"    color: #1f2d3d;\n"
"}\n"
"\n"
"QLabel#lblMiniBar1, QLabel#lblMiniBar2, QLabel#lblMiniBar3 {\n"
"    font-size: 12px;\n"
"    color: #4a5568;\n"
"    font"
                        "-weight: 500;\n"
"}\n"
"\n"
"QProgressBar#pbMini1, QProgressBar#pbMini2, QProgressBar#pbMini3 {\n"
"    border: none;\n"
"    border-radius: 5px;\n"
"    background-color: #e2e8f0;\n"
"    min-height: 10px;\n"
"    max-height: 10px;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QProgressBar#pbMini1::chunk {\n"
"    background-color: #38a169;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QProgressBar#pbMini2::chunk {\n"
"    background-color: #3182ce;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QProgressBar#pbMini3::chunk {\n"
"    background-color: #e53e3e;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QLabel#lblMiniPct1, QLabel#lblMiniPct2, QLabel#lblMiniPct3 {\n"
"    font-size: 11px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    min-width: 32px;\n"
"}\n"
"\n"
"QPushButton#btnFichePaie, QPushButton#btnGoMission, QPushButton#btnGoPointage, QPushButton#btnGoStats, QPushButton#btnExport,\n"
"QPushButton#btnOrder, QPushButton#btnPrediction, QPushButton#btnAlerts,\n"
"QPushButton#facture, QPushButton#score, QPu"
                        "shButton#exportclient,\n"
"QPushButton#btnPdf,\n"
"QPushButton#prod_btnPdf, QPushButton#prod_btnMap3D, QPushButton#prod_btnVideo3D, QPushButton#prod_btnOpenStats {\n"
"    background-color: #ffffff;\n"
"    color: #1f2d3d;\n"
"    border: 1px solid #dce1e6;\n"
"    border-radius: 10px;\n"
"    padding: 12px 16px;\n"
"    min-height: 48px;\n"
"    font-size: 14px;\n"
"    font-weight: 700;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnFichePaie:hover, QPushButton#btnGoMission:hover, QPushButton#btnGoPointage:hover, QPushButton#btnGoStats:hover, QPushButton#btnExport:hover,\n"
"QPushButton#btnOrder:hover, QPushButton#btnPrediction:hover, QPushButton#btnAlerts:hover,\n"
"QPushButton#facture:hover, QPushButton#score:hover, QPushButton#exportclient:hover,\n"
"QPushButton#btnPdf:hover,\n"
"QPushButton#prod_btnPdf:hover, QPushButton#prod_btnMap3D:hover, QPushButton#prod_btnVideo3D:hover, QPushButton#prod_btnOpenStats:hover {\n"
"    background-color: #e3f2fd;\n"
"    border: 1px solid #3498db;\n"
"    col"
                        "or: #0f2b4c;\n"
"}\n"
"\n"
"QPushButton#btnGoStats_Maint,\n"
"QPushButton#btnGoStats_Cmd,\n"
"QPushButton#btnPdf,\n"
"QPushButton#btnPdf_Cmd {\n"
"    background-color: #1a365d;\n"
"    color: #ffffff;\n"
"    border: none;\n"
"    border-radius: 12px;\n"
"    padding: 16px 20px;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"    min-height: 48px;\n"
"}\n"
"\n"
"QPushButton#btnGoStats_Maint:hover,\n"
"QPushButton#btnGoStats_Cmd:hover,\n"
"QPushButton#btnPdf:hover,\n"
"QPushButton#btnPdf_Cmd:hover {\n"
"    background-color: #2a4a7f;\n"
"}\n"
"\n"
"QLabel#st_val1, QLabel#st_val2, QLabel#st_val3,\n"
"QLabel#st_val1_Cmd, QLabel#st_val2_Cmd, QLabel#st_val3_Cmd {\n"
"    font-size: 28px;\n"
"    font-weight: 800;\n"
"    color: #1a365d;\n"
"    padding: 2px 0px;\n"
"}\n"
"\n"
"QLabel#st_lbl1, QLabel#st_lbl2, QLabel#st_lbl3,\n"
"QLabel#st_lbl1_Cmd, QLabel#st_lbl2_Cmd, QLabel#st_lbl3_Cmd {\n"
"    font-size: 12px;\n"
"    color: #a0aec0;\n"
"    font-weight: 600;\n"
"    letter-spacing: 1px;\n"
"}\n"
"\n"
"QW"
                        "idget#pageCmdAjout {\n"
"    background-color: #f0f2f5;\n"
"}\n"
"\n"
"QFrame#cardMod_3 {\n"
"    background-color: #ffffff;\n"
"    border-radius: 18px;\n"
"    border: 1px solid #e2e8f0;\n"
"    padding: 10px;\n"
"}\n"
"\n"
"QLabel#lblTitleMod_3 {\n"
"    font-size: 22px;\n"
"    font-weight: 800;\n"
"    color: #1a365d;\n"
"    padding: 8px 0px 12px 0px;\n"
"    border-bottom: 2px solid #e2e8f0;\n"
"    margin-bottom: 8px;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QLabel {\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    color: #4a5568;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QLineEdit {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 12px 16px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    min-height: 24px;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QLineEdit:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QComboBox {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e"
                        "2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 10px 14px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    min-height: 24px;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QComboBox:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QSpinBox,\n"
"QFrame#cardMod_3 QDoubleSpinBox {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 10px 14px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    min-height: 24px;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QSpinBox:focus,\n"
"QFrame#cardMod_3 QDoubleSpinBox:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QTextEdit {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 12px 16px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"}\n"
"\n"
"QFrame#cardMod_3 QTextEdit:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"   "
                        " background-color: #ffffff;\n"
"}\n"
"\n"
"QLabel#lblSectionDates {\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblSectionAdresse {\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QFrame#separatorLine1, QFrame#separatorLine2, QFrame#separatorLine3 {\n"
"    background-color: #e2e8f0;\n"
"    border: none;\n"
"    min-height: 1px;\n"
"    max-height: 1px;\n"
"}\n"
"\n"
"QPushButton#btnSave_Mod_3 {\n"
"    background-color: #38a169;\n"
"    color: #ffffff;\n"
"    border: none;\n"
"    border-radius: 12px;\n"
"    padding: 14px 20px;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"    min-height: 48px;\n"
"}\n"
"\n"
"QPushButton#btnSave_Mod_3:hover {\n"
"    background-color: #2f855a;\n"
"}\n"
"\n"
"QPushButton#btnCancel_Mod_3 {\n"
"    background-color: #edf2f7;\n"
"    color: #4a5568;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 12px;\n"
"    p"
                        "adding: 14px 20px;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"    min-height: 48px;\n"
"}\n"
"\n"
"QPushButton#btnCancel_Mod_3:hover {\n"
"    background-color: #e2e8f0;\n"
"    border-color: #cbd5e0;\n"
"    color: #2d3748;\n"
"}\n"
"\n"
"QLabel#label_27, QLabel#label_28, QLabel#label_29,\n"
"QLabel#label_30, QLabel#label_31, QLabel#label_32,\n"
"QLabel#label_33, QLabel#label_34 {\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    color: #718096;\n"
"}\n"
"\n"
"QLabel#l1_4, QLabel#l_px_4, QLabel#l_stat_4,\n"
"QLabel#l2_4, QLabel#l_qty_4 {\n"
"    font-size: 13px;\n"
"    font-weight: 700;\n"
"    color: #4a5568;\n"
"    padding: 4px 0px;\n"
"}\n"
"\n"
"QLabel#label {\n"
"    font-size: 13px;\n"
"    font-weight: 700;\n"
"    color: #4a5568;\n"
"    padding: 4px 0px;\n"
"}\n"
"\n"
"QWidget#pageCmdModifier {\n"
"    background-color: #f0f2f5;\n"
"}\n"
"\n"
"QFrame#cardMod {\n"
"    background-color: #ffffff;\n"
"    border-radius: 18px;\n"
"    border: 1px solid #e2e8f0;\n"
"    padding: 10p"
                        "x;\n"
"}\n"
"\n"
"QLabel#lblTitleMod {\n"
"    font-size: 22px;\n"
"    font-weight: 800;\n"
"    color: #1a365d;\n"
"    padding: 8px 0px 12px 0px;\n"
"    border-bottom: 2px solid #e2e8f0;\n"
"    margin-bottom: 8px;\n"
"}\n"
"\n"
"QFrame#cardMod QLabel {\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    color: #4a5568;\n"
"}\n"
"\n"
"QFrame#cardMod QLineEdit {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 12px 16px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    min-height: 24px;\n"
"}\n"
"\n"
"QFrame#cardMod QLineEdit:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QFrame#cardMod QComboBox {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 10px 14px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    min-height: 24px;\n"
"}\n"
"\n"
"QFrame#cardMod QComboBox:focus {\n"
"    border: 1.5px solid #429"
                        "9e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QFrame#cardMod QSpinBox,\n"
"QFrame#cardMod QDoubleSpinBox {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 10px 14px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    min-height: 24px;\n"
"}\n"
"\n"
"QFrame#cardMod QSpinBox:focus,\n"
"QFrame#cardMod QDoubleSpinBox:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QFrame#cardMod QTextEdit {\n"
"    background-color: #f7fafc;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    padding: 12px 16px;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"}\n"
"\n"
"QFrame#cardMod QTextEdit:focus {\n"
"    border: 1.5px solid #4299e1;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QLabel#lblSectionDates_Mod {\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblSectionAdresse_Mod {\n"
"    font-s"
                        "ize: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QFrame#sepLineMod1, QFrame#sepLineMod2, QFrame#sepLineMod3 {\n"
"    background-color: #e2e8f0;\n"
"    border: none;\n"
"    min-height: 1px;\n"
"    max-height: 1px;\n"
"}\n"
"\n"
"QLabel#l1_2, QLabel#l_px_2, QLabel#l_stat_2,\n"
"QLabel#l2_2, QLabel#l_qty_2 {\n"
"    font-size: 13px;\n"
"    font-weight: 700;\n"
"    color: #4a5568;\n"
"    padding: 4px 0px;\n"
"}\n"
"\n"
"QLabel#label_11, QLabel#label_12, QLabel#label_13,\n"
"QLabel#label_14, QLabel#label_19, QLabel#label_20,\n"
"QLabel#label_21, QLabel#label_22 {\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    color: #718096;\n"
"}\n"
"\n"
"QFrame#cardMod[objectName=\"cardMod\"] {\n"
"    background-color: #ffffff;\n"
"    border-radius: 18px;\n"
"    border: 1px solid #e2e8f0;\n"
"}\n"
"\n"
"QLabel#lblTitreFormMod {\n"
"    font-size: 22px;\n"
"    font-weight: 800;\n"
"    color: #1a365d;\n"
"    padding: 8px 0px 12px 0px;\n"
"    border-botto"
                        "m: 2px solid #e2e8f0;\n"
"    margin-bottom: 8px;\n"
"}\n"
"\n"
"QLabel#lblRefMod, QLabel#lblDateMod, QLabel#lblDurMod,\n"
"QLabel#lblCoutMod, QLabel#lblPrioMod, QLabel#lblTechMod,\n"
"QLabel#lblAddrMod, QLabel#lblDescMod, QLabel#lblPhotosAvantMod {\n"
"    font-size: 13px;\n"
"    font-weight: 700;\n"
"    color: #4a5568;\n"
"    padding: 4px 0px;\n"
"}\n"
"\n"
"QLabel#lblLastUpdate {\n"
"    font-size: 11px;\n"
"    font-weight: 500;\n"
"    color: #a0aec0;\n"
"    padding: 4px 0px;\n"
"}\n"
"\n"
"QLabel#lblImgPreview_Mod {\n"
"    border: 2.5px dashed #cbd5e0;\n"
"    border-radius: 14px;\n"
"    background-color: #f7fafc;\n"
"    color: #a0aec0;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    min-height: 120px;\n"
"    padding: 20px;\n"
"}\n"
"\n"
"QFrame#sepMaintMod1, QFrame#sepMaintMod2, QFrame#sepMaintMod3, QFrame#sepMaintMod4 {\n"
"    background-color: #e2e8f0;\n"
"    border: none;\n"
"    min-height: 1px;\n"
"    max-height: 1px;\n"
"}\n"
"\n"
"QLabel#lblSecInfoMod {\n"
"    font-size: 16"
                        "px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblSecTechMod {\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblSecDescMod {\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblSecPhotoMod {\n"
"    font-size: 16px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QFrame#cardAdd {\n"
"    background-color: #ffffff;\n"
"    border-radius: 18px;\n"
"    border: 1px solid #e2e8f0;\n"
"}\n"
"\n"
"QLabel#lblTitreFormAdd {\n"
"    font-size: 22px;\n"
"    font-weight: 800;\n"
"    color: #1a365d;\n"
"    padding: 8px 0px 12px 0px;\n"
"    border-bottom: 2px solid #e2e8f0;\n"
"    margin-bottom: 8px;\n"
"}\n"
"\n"
"QLabel#lblRefAdd, QLabel#lblDateAdd, QLabel#lblDurAdd,\n"
"QLabel#lblCoutAdd, QLabel#lblPrioAdd, QLabel#lblTechAdd,\n"
"QLabel#lblAddrAdd {\n"
"    font-size: 13"
                        "px;\n"
"    font-weight: 700;\n"
"    color: #4a5568;\n"
"    padding: 4px 0px;\n"
"}\n"
"\n"
"QLabel#lblDescAdd {\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblPhotosAvantAdd, QLabel#lblPhotosApresAdd {\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"    color: #2d3748;\n"
"    padding: 6px 0px;\n"
"}\n"
"\n"
"QLabel#lblImgPreview_Add, QLabel#lblImgPreview2_Add {\n"
"    border: 2.5px dashed #cbd5e0;\n"
"    border-radius: 14px;\n"
"    background-color: #f7fafc;\n"
"    color: #a0aec0;\n"
"    font-size: 13px;\n"
"    font-weight: 500;\n"
"    min-height: 140px;\n"
"    padding: 20px;\n"
"}\n"
"\n"
"QLabel#lblImgPreview_Add:hover, QLabel#lblImgPreview2_Add:hover {\n"
"    border-color: #90cdf4;\n"
"    background-color: #ebf8ff;\n"
"}\n"
"\n"
"QFrame#sepMaintAdd1, QFrame#sepMaintAdd2, QFrame#sepMaintAdd3, QFrame#sepMaintAdd4 {\n"
"    background-color: #e2e8f0;\n"
"    border: none;\n"
"    min-height: 1px;\n"
"    max-hei"
                        "ght: 1px;\n"
"}\n"
"\n"
"QPushButton#btnSave_Add {\n"
"    background-color: #38a169;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    font-size: 14px;\n"
"    font-weight: 700;\n"
"    padding: 12px 28px;\n"
"}\n"
"\n"
"QPushButton#btnSave_Add:hover {\n"
"    background-color: #2f855a;\n"
"}\n"
"\n"
"QPushButton#btnCancel_Add {\n"
"    background-color: #edf2f7;\n"
"    color: #4a5568;\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"    padding: 12px 28px;\n"
"}\n"
"\n"
"QPushButton#btnCancel_Add:hover {\n"
"    background-color: #e2e8f0;\n"
"}\n"
"\n"
"QFrame#topBar_Aj {\n"
"    background-color: #ffffff;\n"
"    border-bottom: 1px solid #e2e8f0;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QLabel#titleLabel_Aj {\n"
"    font-size: 16px;\n"
"    font-weight: 800;\n"
"    color: #1a365d;\n"
"    letter-spacing: 1px;\n"
"}\n"
"\n"
"QLabel#userLabel_Aj {\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    "
                        "color: #4a5568;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_Main = new QHBoxLayout(centralwidget);
        horizontalLayout_Main->setSpacing(0);
        horizontalLayout_Main->setObjectName("horizontalLayout_Main");
        horizontalLayout_Main->setContentsMargins(0, 0, 0, 0);
        sidebar = new QFrame(centralwidget);
        sidebar->setObjectName("sidebar");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sidebar->sizePolicy().hasHeightForWidth());
        sidebar->setSizePolicy(sizePolicy);
        sidebar->setMinimumSize(QSize(250, 0));
        sidebar->setMaximumSize(QSize(250, 16777215));
        sidebar->setFrameShape(QFrame::Shape::StyledPanel);
        sidebar->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_4 = new QVBoxLayout(sidebar);
        verticalLayout_4->setObjectName("verticalLayout_4");
        btnToggleSidebar = new QPushButton(sidebar);
        btnToggleSidebar->setObjectName("btnToggleSidebar");
        btnToggleSidebar->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnToggleSidebar->setStyleSheet(QString::fromUtf8("\n"
"           background: transparent;\n"
"           color: white;\n"
"           font-size: 20px;\n"
"           border: none;\n"
"           padding: 5px;\n"
"          "));

        verticalLayout_4->addWidget(btnToggleSidebar);

        label_logo = new QLabel(sidebar);
        label_logo->setObjectName("label_logo");
        label_logo->setMinimumSize(QSize(200, 200));
        label_logo->setMaximumSize(QSize(200, 200));
        label_logo->setPixmap(QPixmap(QString::fromUtf8(":/WASTEGUARD (1).png")));
        label_logo->setScaledContents(true);
        label_logo->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_4->addWidget(label_logo);

        btnAccueil = new QPushButton(sidebar);
        btnAccueil->setObjectName("btnAccueil");

        verticalLayout_4->addWidget(btnAccueil);

        btnStock = new QPushButton(sidebar);
        btnStock->setObjectName("btnStock");

        verticalLayout_4->addWidget(btnStock);

        btnProduits = new QPushButton(sidebar);
        btnProduits->setObjectName("btnProduits");

        verticalLayout_4->addWidget(btnProduits);

        btnClient = new QPushButton(sidebar);
        btnClient->setObjectName("btnClient");

        verticalLayout_4->addWidget(btnClient);

        btnEmployes = new QPushButton(sidebar);
        btnEmployes->setObjectName("btnEmployes");

        verticalLayout_4->addWidget(btnEmployes);

        btnStatistiques = new QPushButton(sidebar);
        btnStatistiques->setObjectName("btnStatistiques");
        btnStatistiques->setVisible(false);

        verticalLayout_4->addWidget(btnStatistiques);

        btnMaintenance = new QPushButton(sidebar);
        btnMaintenance->setObjectName("btnMaintenance");

        verticalLayout_4->addWidget(btnMaintenance);

        btnRetour = new QPushButton(sidebar);
        btnRetour->setObjectName("btnRetour");
        btnRetour->setVisible(false);

        verticalLayout_4->addWidget(btnRetour);

        btnCommandes = new QPushButton(sidebar);
        btnCommandes->setObjectName("btnCommandes");

        verticalLayout_4->addWidget(btnCommandes);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);


        horizontalLayout_Main->addWidget(sidebar);

        contentArea = new QWidget(centralwidget);
        contentArea->setObjectName("contentArea");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(contentArea->sizePolicy().hasHeightForWidth());
        contentArea->setSizePolicy(sizePolicy1);
        verticalLayout_Main = new QVBoxLayout(contentArea);
        verticalLayout_Main->setObjectName("verticalLayout_Main");
        stackedWidget = new QStackedWidget(contentArea);
        stackedWidget->setObjectName("stackedWidget");
        sizePolicy1.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy1);
        affichage = new QWidget();
        affichage->setObjectName("affichage");
        horizontalLayout_Split = new QHBoxLayout(affichage);
        horizontalLayout_Split->setObjectName("horizontalLayout_Split");
        verticalLayout_Left = new QVBoxLayout();
        verticalLayout_Left->setObjectName("verticalLayout_Left");
        verticalLayout_Left->setContentsMargins(20, -1, 20, -1);
        horizontalLayout_Header = new QHBoxLayout();
        horizontalLayout_Header->setObjectName("horizontalLayout_Header");
        label_Title = new QLabel(affichage);
        label_Title->setObjectName("label_Title");
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(18);
        font.setWeight(QFont::DemiBold);
        label_Title->setFont(font);

        horizontalLayout_Header->addWidget(label_Title);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_Header->addItem(horizontalSpacer);

        horizontalSpacer_User = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_Header->addItem(horizontalSpacer_User);

        frameUser = new QFrame(affichage);
        frameUser->setObjectName("frameUser");
        frameUser->setFrameShape(QFrame::Shape::NoFrame);
        verticalLayout_User = new QVBoxLayout(frameUser);
        verticalLayout_User->setSpacing(0);
        verticalLayout_User->setObjectName("verticalLayout_User");
        verticalLayout_User->setContentsMargins(0, 0, 5, 0);
        horizontalLayout_UserHeader = new QHBoxLayout();
        horizontalLayout_UserHeader->setSpacing(10);
        horizontalLayout_UserHeader->setObjectName("horizontalLayout_UserHeader");
        verticalLayout_UserText = new QVBoxLayout();
        verticalLayout_UserText->setSpacing(2);
        verticalLayout_UserText->setObjectName("verticalLayout_UserText");
        lblUserName = new QLabel(frameUser);
        lblUserName->setObjectName("lblUserName");
        lblUserName->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_UserText->addWidget(lblUserName);

        lblUserRole = new QLabel(frameUser);
        lblUserRole->setObjectName("lblUserRole");
        lblUserRole->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_UserText->addWidget(lblUserRole);


        horizontalLayout_UserHeader->addLayout(verticalLayout_UserText);

        btnNouveau = new QPushButton(frameUser);
        btnNouveau->setObjectName("btnNouveau");
        btnNouveau->setMinimumSize(QSize(0, 40));
        btnNouveau->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        horizontalLayout_UserHeader->addWidget(btnNouveau);


        verticalLayout_User->addLayout(horizontalLayout_UserHeader);


        horizontalLayout_Header->addWidget(frameUser);


        verticalLayout_Left->addLayout(horizontalLayout_Header);

        horizontalLayout_Search = new QHBoxLayout();
        horizontalLayout_Search->setObjectName("horizontalLayout_Search");
        txtSearch = new QLineEdit(affichage);
        txtSearch->setObjectName("txtSearch");
        txtSearch->setMinimumSize(QSize(0, 34));

        horizontalLayout_Search->addWidget(txtSearch);

        cbSort = new QComboBox(affichage);
        cbSort->addItem(QString());
        cbSort->addItem(QString());
        cbSort->addItem(QString());
        cbSort->addItem(QString());
        cbSort->setObjectName("cbSort");
        cbSort->setMinimumSize(QSize(180, 40));

        horizontalLayout_Search->addWidget(cbSort);


        verticalLayout_Left->addLayout(horizontalLayout_Search);

        tableEmployes = new QTableWidget(affichage);
        if (tableEmployes->columnCount() < 5)
            tableEmployes->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableEmployes->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableEmployes->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableEmployes->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableEmployes->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableEmployes->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        if (tableEmployes->rowCount() < 4)
            tableEmployes->setRowCount(4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableEmployes->setVerticalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableEmployes->setVerticalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableEmployes->setVerticalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableEmployes->setVerticalHeaderItem(3, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableEmployes->setItem(0, 0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableEmployes->setItem(0, 1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableEmployes->setItem(0, 2, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableEmployes->setItem(0, 3, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        __qtablewidgetitem13->setTextAlignment(Qt::AlignCenter);
        tableEmployes->setItem(0, 4, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableEmployes->setItem(1, 0, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableEmployes->setItem(1, 1, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableEmployes->setItem(1, 2, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableEmployes->setItem(1, 3, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        __qtablewidgetitem18->setTextAlignment(Qt::AlignCenter);
        tableEmployes->setItem(1, 4, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableEmployes->setItem(2, 0, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableEmployes->setItem(2, 1, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tableEmployes->setItem(2, 2, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tableEmployes->setItem(2, 3, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        __qtablewidgetitem23->setTextAlignment(Qt::AlignCenter);
        tableEmployes->setItem(2, 4, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tableEmployes->setItem(3, 0, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tableEmployes->setItem(3, 1, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tableEmployes->setItem(3, 2, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tableEmployes->setItem(3, 3, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        __qtablewidgetitem28->setTextAlignment(Qt::AlignCenter);
        tableEmployes->setItem(3, 4, __qtablewidgetitem28);
        tableEmployes->setObjectName("tableEmployes");
        sizePolicy1.setHeightForWidth(tableEmployes->sizePolicy().hasHeightForWidth());
        tableEmployes->setSizePolicy(sizePolicy1);
        tableEmployes->setAlternatingRowColors(true);
        tableEmployes->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tableEmployes->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        tableEmployes->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        tableEmployes->setShowGrid(false);
        tableEmployes->horizontalHeader()->setStretchLastSection(true);
        tableEmployes->verticalHeader()->setVisible(false);

        verticalLayout_Left->addWidget(tableEmployes);


        horizontalLayout_Split->addLayout(verticalLayout_Left);

        sidePanel_Employe = new QFrame(affichage);
        sidePanel_Employe->setObjectName("sidePanel_Employe");
        sidePanel_Employe->setMinimumSize(QSize(300, 0));
        sidePanel_Employe->setMaximumSize(QSize(350, 16777215));
        verticalLayout_Right = new QVBoxLayout(sidePanel_Employe);
        verticalLayout_Right->setSpacing(20);
        verticalLayout_Right->setObjectName("verticalLayout_Right");
        verticalLayout_Right->setContentsMargins(15, 20, -1, -1);
        statCard = new QFrame(sidePanel_Employe);
        statCard->setObjectName("statCard");
        statCard->setFrameShape(QFrame::Shape::StyledPanel);
        vl_stat1 = new QVBoxLayout(statCard);
        vl_stat1->setObjectName("vl_stat1");
        label_Stat1_Title = new QLabel(statCard);
        label_Stat1_Title->setObjectName("label_Stat1_Title");

        vl_stat1->addWidget(label_Stat1_Title);

        lblStatNumber = new QLabel(statCard);
        lblStatNumber->setObjectName("lblStatNumber");

        vl_stat1->addWidget(lblStatNumber);


        verticalLayout_Right->addWidget(statCard);

        statCard_2 = new QFrame(sidePanel_Employe);
        statCard_2->setObjectName("statCard_2");
        statCard_2->setFrameShape(QFrame::Shape::StyledPanel);
        vl_stat2 = new QVBoxLayout(statCard_2);
        vl_stat2->setObjectName("vl_stat2");
        label_Stat2_Title = new QLabel(statCard_2);
        label_Stat2_Title->setObjectName("label_Stat2_Title");

        vl_stat2->addWidget(label_Stat2_Title);

        lblStatWorkload = new QLabel(statCard_2);
        lblStatWorkload->setObjectName("lblStatWorkload");

        vl_stat2->addWidget(lblStatWorkload);


        verticalLayout_Right->addWidget(statCard_2);

        statCard_3 = new QFrame(sidePanel_Employe);
        statCard_3->setObjectName("statCard_3");
        statCard_3->setFrameShape(QFrame::Shape::StyledPanel);
        vl_stat3 = new QVBoxLayout(statCard_3);
        vl_stat3->setObjectName("vl_stat3");
        label_Stat3_Title = new QLabel(statCard_3);
        label_Stat3_Title->setObjectName("label_Stat3_Title");

        vl_stat3->addWidget(label_Stat3_Title);

        lblStatAvailability = new QLabel(statCard_3);
        lblStatAvailability->setObjectName("lblStatAvailability");

        vl_stat3->addWidget(lblStatAvailability);


        verticalLayout_Right->addWidget(statCard_3);

        actionCard = new QFrame(sidePanel_Employe);
        actionCard->setObjectName("actionCard");
        actionCard->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_Actions = new QVBoxLayout(actionCard);
        verticalLayout_Actions->setSpacing(10);
        verticalLayout_Actions->setObjectName("verticalLayout_Actions");
        label_Actions = new QLabel(actionCard);
        label_Actions->setObjectName("label_Actions");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setBold(true);
        label_Actions->setFont(font1);

        verticalLayout_Actions->addWidget(label_Actions);

        btnFichePaie = new QPushButton(actionCard);
        btnFichePaie->setObjectName("btnFichePaie");
        btnFichePaie->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Actions->addWidget(btnFichePaie);

        btnGoMission = new QPushButton(actionCard);
        btnGoMission->setObjectName("btnGoMission");
        btnGoMission->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Actions->addWidget(btnGoMission);

        btnGoPointage = new QPushButton(actionCard);
        btnGoPointage->setObjectName("btnGoPointage");
        btnGoPointage->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Actions->addWidget(btnGoPointage);

        btnGoStats = new QPushButton(actionCard);
        btnGoStats->setObjectName("btnGoStats");
        btnGoStats->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Actions->addWidget(btnGoStats);


        verticalLayout_Right->addWidget(actionCard);

        verticalSpacer_Right = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Right->addItem(verticalSpacer_Right);


        horizontalLayout_Split->addWidget(sidePanel_Employe);

        horizontalLayout_Split->setStretch(0, 3);
        stackedWidget->addWidget(affichage);
        modification = new QWidget();
        modification->setObjectName("modification");
        gridLayout_Modif = new QGridLayout(modification);
        gridLayout_Modif->setObjectName("gridLayout_Modif");
        headerLayout_Modif = new QHBoxLayout();
        headerLayout_Modif->setObjectName("headerLayout_Modif");
        horizontalSpacer_HeaderModif = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout_Modif->addItem(horizontalSpacer_HeaderModif);

        frameUser_Modif = new QFrame(modification);
        frameUser_Modif->setObjectName("frameUser_Modif");
        frameUser_Modif->setFrameShape(QFrame::Shape::NoFrame);
        verticalLayout_User_Modif = new QVBoxLayout(frameUser_Modif);
        verticalLayout_User_Modif->setSpacing(2);
        verticalLayout_User_Modif->setObjectName("verticalLayout_User_Modif");
        verticalLayout_User_Modif->setContentsMargins(0, 0, 5, 0);
        lblUserName_Modif = new QLabel(frameUser_Modif);
        lblUserName_Modif->setObjectName("lblUserName_Modif");
        lblUserName_Modif->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_User_Modif->addWidget(lblUserName_Modif);

        lblUserRole_Modif = new QLabel(frameUser_Modif);
        lblUserRole_Modif->setObjectName("lblUserRole_Modif");
        lblUserRole_Modif->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_User_Modif->addWidget(lblUserRole_Modif);


        headerLayout_Modif->addWidget(frameUser_Modif);


        gridLayout_Modif->addLayout(headerLayout_Modif, 0, 0, 1, 2);

        cardFrame = new QFrame(modification);
        cardFrame->setObjectName("cardFrame");
        cardFrame->setFrameShape(QFrame::Shape::StyledPanel);
        cardFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_ModifFrame = new QHBoxLayout(cardFrame);
        horizontalLayout_ModifFrame->setObjectName("horizontalLayout_ModifFrame");
        verticalLayout_Photo_Modif = new QVBoxLayout();
        verticalLayout_Photo_Modif->setSpacing(10);
        verticalLayout_Photo_Modif->setObjectName("verticalLayout_Photo_Modif");
        imageLabel = new QLabel(cardFrame);
        imageLabel->setObjectName("imageLabel");
        imageLabel->setMinimumSize(QSize(200, 300));
        imageLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_Photo_Modif->addWidget(imageLabel);

        btnUploadPhoto = new QPushButton(cardFrame);
        btnUploadPhoto->setObjectName("btnUploadPhoto");
        btnUploadPhoto->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Photo_Modif->addWidget(btnUploadPhoto);

        verticalSpacer_Photo = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Photo_Modif->addItem(verticalSpacer_Photo);


        horizontalLayout_ModifFrame->addLayout(verticalLayout_Photo_Modif);

        verticalLayout_FormRight = new QVBoxLayout();
        verticalLayout_FormRight->setObjectName("verticalLayout_FormRight");
        formLayout_Modif = new QFormLayout();
        formLayout_Modif->setObjectName("formLayout_Modif");
        formLayout_Modif->setVerticalSpacing(65);
        lblMatricule = new QLabel(cardFrame);
        lblMatricule->setObjectName("lblMatricule");

        formLayout_Modif->setWidget(0, QFormLayout::LabelRole, lblMatricule);

        txtMatricule = new QLineEdit(cardFrame);
        txtMatricule->setObjectName("txtMatricule");
        txtMatricule->setReadOnly(true);

        formLayout_Modif->setWidget(0, QFormLayout::FieldRole, txtMatricule);

        lblNom = new QLabel(cardFrame);
        lblNom->setObjectName("lblNom");

        formLayout_Modif->setWidget(2, QFormLayout::LabelRole, lblNom);

        txtNom = new QLineEdit(cardFrame);
        txtNom->setObjectName("txtNom");

        formLayout_Modif->setWidget(2, QFormLayout::FieldRole, txtNom);

        lblSpecialite = new QLabel(cardFrame);
        lblSpecialite->setObjectName("lblSpecialite");

        formLayout_Modif->setWidget(3, QFormLayout::LabelRole, lblSpecialite);

        cbSpecialite = new QComboBox(cardFrame);
        cbSpecialite->addItem(QString());
        cbSpecialite->addItem(QString());
        cbSpecialite->addItem(QString());
        cbSpecialite->setObjectName("cbSpecialite");

        formLayout_Modif->setWidget(3, QFormLayout::FieldRole, cbSpecialite);

        lblStatut = new QLabel(cardFrame);
        lblStatut->setObjectName("lblStatut");

        formLayout_Modif->setWidget(4, QFormLayout::LabelRole, lblStatut);

        cbStatut = new QComboBox(cardFrame);
        cbStatut->addItem(QString());
        cbStatut->addItem(QString());
        cbStatut->addItem(QString());
        cbStatut->setObjectName("cbStatut");

        formLayout_Modif->setWidget(4, QFormLayout::FieldRole, cbStatut);

        lblSalaire_Modif = new QLabel(cardFrame);
        lblSalaire_Modif->setObjectName("lblSalaire_Modif");

        formLayout_Modif->setWidget(5, QFormLayout::LabelRole, lblSalaire_Modif);

        verticalLayout_Salaire_Modif = new QVBoxLayout();
        verticalLayout_Salaire_Modif->setObjectName("verticalLayout_Salaire_Modif");
        sliderSalaire_Modif = new QSlider(cardFrame);
        sliderSalaire_Modif->setObjectName("sliderSalaire_Modif");
        sliderSalaire_Modif->setMinimum(600);
        sliderSalaire_Modif->setMaximum(5000);
        sliderSalaire_Modif->setValue(1200);
        sliderSalaire_Modif->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_Salaire_Modif->addWidget(sliderSalaire_Modif);

        lblValSalaire_Modif = new QLabel(cardFrame);
        lblValSalaire_Modif->setObjectName("lblValSalaire_Modif");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(8);
        font2.setWeight(QFont::DemiBold);
        lblValSalaire_Modif->setFont(font2);
        lblValSalaire_Modif->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_Salaire_Modif->addWidget(lblValSalaire_Modif);


        formLayout_Modif->setLayout(5, QFormLayout::FieldRole, verticalLayout_Salaire_Modif);

        label = new QLabel(cardFrame);
        label->setObjectName("label");

        formLayout_Modif->setWidget(1, QFormLayout::LabelRole, label);

        lblCIN_Modif = new QLabel(cardFrame);
        lblCIN_Modif->setObjectName("lblCIN_Modif");

        formLayout_Modif->setWidget(6, QFormLayout::LabelRole, lblCIN_Modif);

        txtCIN_Modif = new QLineEdit(cardFrame);
        txtCIN_Modif->setObjectName("txtCIN_Modif");

        formLayout_Modif->setWidget(6, QFormLayout::FieldRole, txtCIN_Modif);

        txtEmailModif = new QLineEdit(cardFrame);
        txtEmailModif->setObjectName("txtEmailModif");

        formLayout_Modif->setWidget(1, QFormLayout::FieldRole, txtEmailModif);


        verticalLayout_FormRight->addLayout(formLayout_Modif);

        verticalSpacer_Form = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_FormRight->addItem(verticalSpacer_Form);

        horizontalLayout_Buttons = new QHBoxLayout();
        horizontalLayout_Buttons->setObjectName("horizontalLayout_Buttons");
        btnDelete = new QPushButton(cardFrame);
        btnDelete->setObjectName("btnDelete");
        btnDelete->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        horizontalLayout_Buttons->addWidget(btnDelete);

        horizontalSpacer_Buttons = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_Buttons->addItem(horizontalSpacer_Buttons);

        btnAnnuler = new QPushButton(cardFrame);
        btnAnnuler->setObjectName("btnAnnuler");

        horizontalLayout_Buttons->addWidget(btnAnnuler);

        btnSave = new QPushButton(cardFrame);
        btnSave->setObjectName("btnSave");

        horizontalLayout_Buttons->addWidget(btnSave);


        verticalLayout_FormRight->addLayout(horizontalLayout_Buttons);


        horizontalLayout_ModifFrame->addLayout(verticalLayout_FormRight);


        gridLayout_Modif->addWidget(cardFrame, 1, 0, 1, 1);

        stackedWidget->addWidget(modification);
        ajout = new QWidget();
        ajout->setObjectName("ajout");
        gridLayout_Ajout = new QGridLayout(ajout);
        gridLayout_Ajout->setObjectName("gridLayout_Ajout");
        headerLayout_Ajout = new QHBoxLayout();
        headerLayout_Ajout->setObjectName("headerLayout_Ajout");
        horizontalSpacer_HeaderAjout = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout_Ajout->addItem(horizontalSpacer_HeaderAjout);

        frameUser_Ajout = new QFrame(ajout);
        frameUser_Ajout->setObjectName("frameUser_Ajout");
        frameUser_Ajout->setFrameShape(QFrame::Shape::NoFrame);
        verticalLayout_User_Ajout = new QVBoxLayout(frameUser_Ajout);
        verticalLayout_User_Ajout->setSpacing(2);
        verticalLayout_User_Ajout->setObjectName("verticalLayout_User_Ajout");
        verticalLayout_User_Ajout->setContentsMargins(0, 0, 5, 0);
        lblUserName_Ajout = new QLabel(frameUser_Ajout);
        lblUserName_Ajout->setObjectName("lblUserName_Ajout");
        lblUserName_Ajout->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_User_Ajout->addWidget(lblUserName_Ajout);

        lblUserRole_Ajout = new QLabel(frameUser_Ajout);
        lblUserRole_Ajout->setObjectName("lblUserRole_Ajout");
        lblUserRole_Ajout->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_User_Ajout->addWidget(lblUserRole_Ajout);


        headerLayout_Ajout->addWidget(frameUser_Ajout);


        gridLayout_Ajout->addLayout(headerLayout_Ajout, 0, 0, 1, 2);

        cardFrame_2 = new QFrame(ajout);
        cardFrame_2->setObjectName("cardFrame_2");
        cardFrame_2->setFrameShape(QFrame::Shape::StyledPanel);
        horizontalLayout_AjoutFrame = new QHBoxLayout(cardFrame_2);
        horizontalLayout_AjoutFrame->setObjectName("horizontalLayout_AjoutFrame");
        verticalLayout_Photo_Ajout = new QVBoxLayout();
        verticalLayout_Photo_Ajout->setSpacing(10);
        verticalLayout_Photo_Ajout->setObjectName("verticalLayout_Photo_Ajout");
        imageLabel_2 = new QLabel(cardFrame_2);
        imageLabel_2->setObjectName("imageLabel_2");
        imageLabel_2->setMinimumSize(QSize(200, 300));
        imageLabel_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_Photo_Ajout->addWidget(imageLabel_2);

        btnUploadPhoto_Ajout = new QPushButton(cardFrame_2);
        btnUploadPhoto_Ajout->setObjectName("btnUploadPhoto_Ajout");
        btnUploadPhoto_Ajout->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Photo_Ajout->addWidget(btnUploadPhoto_Ajout);

        verticalSpacer_Photo_Ajout = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Photo_Ajout->addItem(verticalSpacer_Photo_Ajout);


        horizontalLayout_AjoutFrame->addLayout(verticalLayout_Photo_Ajout);

        verticalLayout_FormRight_2 = new QVBoxLayout();
        verticalLayout_FormRight_2->setObjectName("verticalLayout_FormRight_2");
        formLayout_Ajout = new QFormLayout();
        formLayout_Ajout->setObjectName("formLayout_Ajout");
        formLayout_Ajout->setVerticalSpacing(65);
        label_7 = new QLabel(cardFrame_2);
        label_7->setObjectName("label_7");

        formLayout_Ajout->setWidget(0, QFormLayout::LabelRole, label_7);

        txtNom_Ajout = new QLineEdit(cardFrame_2);
        txtNom_Ajout->setObjectName("txtNom_Ajout");

        formLayout_Ajout->setWidget(0, QFormLayout::FieldRole, txtNom_Ajout);

        label_8 = new QLabel(cardFrame_2);
        label_8->setObjectName("label_8");

        formLayout_Ajout->setWidget(2, QFormLayout::LabelRole, label_8);

        cbSpecialite_Ajout = new QComboBox(cardFrame_2);
        cbSpecialite_Ajout->addItem(QString());
        cbSpecialite_Ajout->addItem(QString());
        cbSpecialite_Ajout->setObjectName("cbSpecialite_Ajout");

        formLayout_Ajout->setWidget(2, QFormLayout::FieldRole, cbSpecialite_Ajout);

        label_9 = new QLabel(cardFrame_2);
        label_9->setObjectName("label_9");

        formLayout_Ajout->setWidget(3, QFormLayout::LabelRole, label_9);

        cbStatut_Ajout = new QComboBox(cardFrame_2);
        cbStatut_Ajout->addItem(QString());
        cbStatut_Ajout->addItem(QString());
        cbStatut_Ajout->setObjectName("cbStatut_Ajout");

        formLayout_Ajout->setWidget(3, QFormLayout::FieldRole, cbStatut_Ajout);

        lblSalaire_Ajout = new QLabel(cardFrame_2);
        lblSalaire_Ajout->setObjectName("lblSalaire_Ajout");

        formLayout_Ajout->setWidget(4, QFormLayout::LabelRole, lblSalaire_Ajout);

        verticalLayout_Salaire_Ajout = new QVBoxLayout();
        verticalLayout_Salaire_Ajout->setObjectName("verticalLayout_Salaire_Ajout");
        sliderSalaire_Ajout = new QSlider(cardFrame_2);
        sliderSalaire_Ajout->setObjectName("sliderSalaire_Ajout");
        sliderSalaire_Ajout->setMinimum(600);
        sliderSalaire_Ajout->setMaximum(5000);
        sliderSalaire_Ajout->setValue(1200);
        sliderSalaire_Ajout->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_Salaire_Ajout->addWidget(sliderSalaire_Ajout);

        lblValSalaire_Ajout = new QLabel(cardFrame_2);
        lblValSalaire_Ajout->setObjectName("lblValSalaire_Ajout");
        lblValSalaire_Ajout->setMinimumSize(QSize(70, 0));
        lblValSalaire_Ajout->setFont(font2);
        lblValSalaire_Ajout->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_Salaire_Ajout->addWidget(lblValSalaire_Ajout);


        formLayout_Ajout->setLayout(4, QFormLayout::FieldRole, verticalLayout_Salaire_Ajout);

        txtEmailAjout = new QLineEdit(cardFrame_2);
        txtEmailAjout->setObjectName("txtEmailAjout");

        formLayout_Ajout->setWidget(1, QFormLayout::FieldRole, txtEmailAjout);

        label_2 = new QLabel(cardFrame_2);
        label_2->setObjectName("label_2");

        formLayout_Ajout->setWidget(1, QFormLayout::LabelRole, label_2);

        lblCIN_Ajout = new QLabel(cardFrame_2);
        lblCIN_Ajout->setObjectName("lblCIN_Ajout");

        formLayout_Ajout->setWidget(5, QFormLayout::LabelRole, lblCIN_Ajout);

        txtCIN_Ajout = new QLineEdit(cardFrame_2);
        txtCIN_Ajout->setObjectName("txtCIN_Ajout");

        formLayout_Ajout->setWidget(5, QFormLayout::FieldRole, txtCIN_Ajout);


        verticalLayout_FormRight_2->addLayout(formLayout_Ajout);

        verticalSpacer_Form_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_FormRight_2->addItem(verticalSpacer_Form_2);

        horizontalLayout_Buttons_2 = new QHBoxLayout();
        horizontalLayout_Buttons_2->setObjectName("horizontalLayout_Buttons_2");
        horizontalSpacer_Buttons_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_Buttons_2->addItem(horizontalSpacer_Buttons_2);

        btnAnnuler_Ajout = new QPushButton(cardFrame_2);
        btnAnnuler_Ajout->setObjectName("btnAnnuler_Ajout");

        horizontalLayout_Buttons_2->addWidget(btnAnnuler_Ajout);

        btnAjouter = new QPushButton(cardFrame_2);
        btnAjouter->setObjectName("btnAjouter");

        horizontalLayout_Buttons_2->addWidget(btnAjouter);


        verticalLayout_FormRight_2->addLayout(horizontalLayout_Buttons_2);


        horizontalLayout_AjoutFrame->addLayout(verticalLayout_FormRight_2);


        gridLayout_Ajout->addWidget(cardFrame_2, 1, 0, 1, 1);

        stackedWidget->addWidget(ajout);
        mission = new QWidget();
        mission->setObjectName("mission");
        verticalLayout_Mission = new QVBoxLayout(mission);
        verticalLayout_Mission->setObjectName("verticalLayout_Mission");
        lblTitreMission = new QLabel(mission);
        lblTitreMission->setObjectName("lblTitreMission");

        verticalLayout_Mission->addWidget(lblTitreMission);

        txtMission = new QLineEdit(mission);
        txtMission->setObjectName("txtMission");

        verticalLayout_Mission->addWidget(txtMission);

        btnAnalyser = new QPushButton(mission);
        btnAnalyser->setObjectName("btnAnalyser");
        btnAnalyser->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Mission->addWidget(btnAnalyser);

        lblResultat = new QLabel(mission);
        lblResultat->setObjectName("lblResultat");

        verticalLayout_Mission->addWidget(lblResultat);

        tableResultat = new QTableWidget(mission);
        if (tableResultat->columnCount() < 4)
            tableResultat->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableResultat->setHorizontalHeaderItem(0, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableResultat->setHorizontalHeaderItem(1, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableResultat->setHorizontalHeaderItem(2, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        tableResultat->setHorizontalHeaderItem(3, __qtablewidgetitem32);
        tableResultat->setObjectName("tableResultat");
        tableResultat->setStyleSheet(QString::fromUtf8("background-color: white; gridline-color: #eee;"));
        tableResultat->horizontalHeader()->setStretchLastSection(true);
        tableResultat->verticalHeader()->setVisible(false);

        verticalLayout_Mission->addWidget(tableResultat);

        verticalSpacer_Mission = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mission->addItem(verticalSpacer_Mission);

        btnAnnulerMission = new QPushButton(mission);
        btnAnnulerMission->setObjectName("btnAnnulerMission");

        verticalLayout_Mission->addWidget(btnAnnulerMission);

        stackedWidget->addWidget(mission);
        pointage = new QWidget();
        pointage->setObjectName("pointage");
        verticalLayout_Pointage = new QVBoxLayout(pointage);
        verticalLayout_Pointage->setObjectName("verticalLayout_Pointage");
        lblTitrePointage = new QLabel(pointage);
        lblTitrePointage->setObjectName("lblTitrePointage");

        verticalLayout_Pointage->addWidget(lblTitrePointage);

        lblStatutRFID = new QLabel(pointage);
        lblStatutRFID->setObjectName("lblStatutRFID");
        lblStatutRFID->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_Pointage->addWidget(lblStatutRFID);

        btnSimulerBadge = new QPushButton(pointage);
        btnSimulerBadge->setObjectName("btnSimulerBadge");
        btnSimulerBadge->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        verticalLayout_Pointage->addWidget(btnSimulerBadge);

        lblLog = new QLabel(pointage);
        lblLog->setObjectName("lblLog");

        verticalLayout_Pointage->addWidget(lblLog);

        tablePointage = new QTableWidget(pointage);
        if (tablePointage->columnCount() < 4)
            tablePointage->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        tablePointage->setHorizontalHeaderItem(0, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        tablePointage->setHorizontalHeaderItem(1, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        tablePointage->setHorizontalHeaderItem(2, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        tablePointage->setHorizontalHeaderItem(3, __qtablewidgetitem36);
        tablePointage->setObjectName("tablePointage");
        tablePointage->setStyleSheet(QString::fromUtf8("background-color: white; gridline-color: #eee;"));
        tablePointage->horizontalHeader()->setStretchLastSection(true);
        tablePointage->verticalHeader()->setVisible(false);

        verticalLayout_Pointage->addWidget(tablePointage);

        verticalSpacer_Pointage = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Pointage->addItem(verticalSpacer_Pointage);

        btnAnnulerPointage = new QPushButton(pointage);
        btnAnnulerPointage->setObjectName("btnAnnulerPointage");

        verticalLayout_Pointage->addWidget(btnAnnulerPointage);

        stackedWidget->addWidget(pointage);
        statistiques = new QWidget();
        statistiques->setObjectName("statistiques");
        verticalLayout_StatsPage = new QVBoxLayout(statistiques);
        verticalLayout_StatsPage->setObjectName("verticalLayout_StatsPage");
        horizontalLayout_Stats = new QHBoxLayout();
        horizontalLayout_Stats->setSpacing(20);
        horizontalLayout_Stats->setObjectName("horizontalLayout_Stats");
        chartViewAbsence = new QChartView(statistiques);
        chartViewAbsence->setObjectName("chartViewAbsence");
        chartViewAbsence->setMinimumSize(QSize(0, 350));

        horizontalLayout_Stats->addWidget(chartViewAbsence);

        chartViewWorkload = new QChartView(statistiques);
        chartViewWorkload->setObjectName("chartViewWorkload");
        chartViewWorkload->setMinimumSize(QSize(0, 350));

        horizontalLayout_Stats->addWidget(chartViewWorkload);


        verticalLayout_StatsPage->addLayout(horizontalLayout_Stats);

        chartViewTasks = new QChartView(statistiques);
        chartViewTasks->setObjectName("chartViewTasks");
        chartViewTasks->setMinimumSize(QSize(0, 300));

        verticalLayout_StatsPage->addWidget(chartViewTasks);

        cbProjetStats = new QComboBox(statistiques);
        cbProjetStats->addItem(QString());
        cbProjetStats->addItem(QString());
        cbProjetStats->addItem(QString());
        cbProjetStats->setObjectName("cbProjetStats");

        verticalLayout_StatsPage->addWidget(cbProjetStats);

        verticalSpacer_Stats = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_StatsPage->addItem(verticalSpacer_Stats);

        btnAnnulerStats = new QPushButton(statistiques);
        btnAnnulerStats->setObjectName("btnAnnulerStats");

        verticalLayout_StatsPage->addWidget(btnAnnulerStats);

        stackedWidget->addWidget(statistiques);
        pageProduit = new QWidget();
        pageProduit->setObjectName("pageProduit");
        verticalLayout_ProduitPage = new QVBoxLayout(pageProduit);
        verticalLayout_ProduitPage->setSpacing(0);
        verticalLayout_ProduitPage->setObjectName("verticalLayout_ProduitPage");
        verticalLayout_ProduitPage->setContentsMargins(0, 0, 0, 0);
        prod_rootLayout = new QHBoxLayout();
        prod_rootLayout->setSpacing(0);
        prod_rootLayout->setObjectName("prod_rootLayout");
        prod_rootLayout->setContentsMargins(0, 0, 0, 0);
        prod_leftSidebar = new QFrame(pageProduit);
        prod_leftSidebar->setObjectName("prod_leftSidebar");
        sizePolicy.setHeightForWidth(prod_leftSidebar->sizePolicy().hasHeightForWidth());
        prod_leftSidebar->setSizePolicy(sizePolicy);
        prod_leftSidebar->setMinimumSize(QSize(250, 0));
        prod_leftSidebar->setMaximumSize(QSize(250, 16777215));
        prod_leftSidebar->setFrameShape(QFrame::Shape::NoFrame);
        prod_leftSidebar->setFrameShadow(QFrame::Shadow::Plain);
        prod_verticalLayout_1 = new QVBoxLayout(prod_leftSidebar);
        prod_verticalLayout_1->setSpacing(5);
        prod_verticalLayout_1->setObjectName("prod_verticalLayout_1");
        prod_verticalLayout_1->setContentsMargins(-1, -1, -1, 20);
        prod_label_1 = new QLabel(prod_leftSidebar);
        prod_label_1->setObjectName("prod_label_1");
        prod_label_1->setPixmap(QPixmap(QString::fromUtf8(":/wasteguard_logo.png")));
        prod_label_1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        prod_verticalLayout_1->addWidget(prod_label_1);

        spacerItem = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        prod_verticalLayout_1->addItem(spacerItem);

        prod_btnhome_1 = new QPushButton(prod_leftSidebar);
        prod_btnhome_1->setObjectName("prod_btnhome_1");

        prod_verticalLayout_1->addWidget(prod_btnhome_1);

        prod_btnstock_1 = new QPushButton(prod_leftSidebar);
        prod_btnstock_1->setObjectName("prod_btnstock_1");

        prod_verticalLayout_1->addWidget(prod_btnstock_1);

        prod_btnproduit_1 = new QPushButton(prod_leftSidebar);
        prod_btnproduit_1->setObjectName("prod_btnproduit_1");

        prod_verticalLayout_1->addWidget(prod_btnproduit_1);

        prod_btnclient_1 = new QPushButton(prod_leftSidebar);
        prod_btnclient_1->setObjectName("prod_btnclient_1");

        prod_verticalLayout_1->addWidget(prod_btnclient_1);

        prod_btnemploye_1 = new QPushButton(prod_leftSidebar);
        prod_btnemploye_1->setObjectName("prod_btnemploye_1");

        prod_verticalLayout_1->addWidget(prod_btnemploye_1);

        prod_btnmaintenance_1 = new QPushButton(prod_leftSidebar);
        prod_btnmaintenance_1->setObjectName("prod_btnmaintenance_1");

        prod_verticalLayout_1->addWidget(prod_btnmaintenance_1);

        prod_btncmd_1 = new QPushButton(prod_leftSidebar);
        prod_btncmd_1->setObjectName("prod_btncmd_1");

        prod_verticalLayout_1->addWidget(prod_btncmd_1);

        spacerItem1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        prod_verticalLayout_1->addItem(spacerItem1);


        prod_rootLayout->addWidget(prod_leftSidebar);

        prod_mainAreaLayout = new QVBoxLayout();
        prod_mainAreaLayout->setObjectName("prod_mainAreaLayout");
        prod_dashboard_hLayout = new QHBoxLayout();
        prod_dashboard_hLayout->setSpacing(0);
        prod_dashboard_hLayout->setObjectName("prod_dashboard_hLayout");
        prod_dashboard_hLayout->setContentsMargins(0, 0, 0, 0);
        prod_dashboardRightLayout = new QVBoxLayout();
        prod_dashboardRightLayout->setSpacing(0);
        prod_dashboardRightLayout->setObjectName("prod_dashboardRightLayout");
        prod_dashboardRightLayout->setContentsMargins(0, 0, 0, 0);
        prod_contentLayout = new QHBoxLayout();
        prod_contentLayout->setSpacing(20);
        prod_contentLayout->setObjectName("prod_contentLayout");
        prod_stackedWidget = new QStackedWidget(pageProduit);
        prod_stackedWidget->setObjectName("prod_stackedWidget");
        sizePolicy1.setHeightForWidth(prod_stackedWidget->sizePolicy().hasHeightForWidth());
        prod_stackedWidget->setSizePolicy(sizePolicy1);
        prod_affichage = new QWidget();
        prod_affichage->setObjectName("prod_affichage");
        sizePolicy1.setHeightForWidth(prod_affichage->sizePolicy().hasHeightForWidth());
        prod_affichage->setSizePolicy(sizePolicy1);
        prod_affichageLayout = new QHBoxLayout(prod_affichage);
        prod_affichageLayout->setSpacing(12);
        prod_affichageLayout->setObjectName("prod_affichageLayout");
        prod_affichageLayout->setContentsMargins(0, 0, 0, 0);
        prod_tableLayout = new QVBoxLayout();
        prod_tableLayout->setObjectName("prod_tableLayout");
        prod_topBarLayout = new QHBoxLayout();
        prod_topBarLayout->setObjectName("prod_topBarLayout");
        prod_topBarLayout->setContentsMargins(0, 10, 0, 10);
        prod_label_Title = new QLabel(prod_affichage);
        prod_label_Title->setObjectName("prod_label_Title");
        prod_label_Title->setFont(font);

        prod_topBarLayout->addWidget(prod_label_Title);

        prod_topSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        prod_topBarLayout->addItem(prod_topSpacer);

        prod_btnAddProduct = new QPushButton(prod_affichage);
        prod_btnAddProduct->setObjectName("prod_btnAddProduct");
        prod_btnAddProduct->setMinimumSize(QSize(0, 40));

        prod_topBarLayout->addWidget(prod_btnAddProduct);

        prod_topSpacer_User = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        prod_topBarLayout->addItem(prod_topSpacer_User);

        prod_frameUser = new QFrame(prod_affichage);
        prod_frameUser->setObjectName("prod_frameUser");
        prod_frameUser->setFrameShape(QFrame::Shape::NoFrame);
        prod_verticalLayout_User = new QVBoxLayout(prod_frameUser);
        prod_verticalLayout_User->setSpacing(2);
        prod_verticalLayout_User->setObjectName("prod_verticalLayout_User");
        prod_verticalLayout_User->setContentsMargins(0, 0, 5, 0);
        prod_lblUserName = new QLabel(prod_frameUser);
        prod_lblUserName->setObjectName("prod_lblUserName");
        prod_lblUserName->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        prod_verticalLayout_User->addWidget(prod_lblUserName);

        prod_lblUserRole = new QLabel(prod_frameUser);
        prod_lblUserRole->setObjectName("prod_lblUserRole");
        prod_lblUserRole->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        prod_verticalLayout_User->addWidget(prod_lblUserRole);


        prod_topBarLayout->addWidget(prod_frameUser);


        prod_tableLayout->addLayout(prod_topBarLayout);

        prod_searchRow = new QHBoxLayout();
        prod_searchRow->setObjectName("prod_searchRow");
        prod_searchInput = new QLineEdit(prod_affichage);
        prod_searchInput->setObjectName("prod_searchInput");
        prod_searchInput->setMinimumSize(QSize(0, 34));

        prod_searchRow->addWidget(prod_searchInput);

        prod_cbSort = new QComboBox(prod_affichage);
        prod_cbSort->addItem(QString());
        prod_cbSort->addItem(QString());
        prod_cbSort->setObjectName("prod_cbSort");
        prod_cbSort->setMinimumSize(QSize(180, 40));

        prod_searchRow->addWidget(prod_cbSort);

        prod_btnToggleView = new QPushButton(prod_affichage);
        prod_btnToggleView->setObjectName("prod_btnToggleView");
        prod_btnToggleView->setMinimumSize(QSize(40, 40));
        prod_btnToggleView->setMaximumSize(QSize(40, 40));

        prod_searchRow->addWidget(prod_btnToggleView);


        prod_tableLayout->addLayout(prod_searchRow);

        prod_viewStackedWidget = new QStackedWidget(prod_affichage);
        prod_viewStackedWidget->setObjectName("prod_viewStackedWidget");
        prod_tableViewPage = new QWidget();
        prod_tableViewPage->setObjectName("prod_tableViewPage");
        prod_tablePgLayout = new QVBoxLayout(prod_tableViewPage);
        prod_tablePgLayout->setSpacing(0);
        prod_tablePgLayout->setObjectName("prod_tablePgLayout");
        prod_tablePgLayout->setContentsMargins(0, 0, 0, 0);
        prod_tableProduits = new QTableWidget(prod_tableViewPage);
        if (prod_tableProduits->columnCount() < 8)
            prod_tableProduits->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(0, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(1, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(2, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(3, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(4, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(5, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(6, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        prod_tableProduits->setHorizontalHeaderItem(7, __qtablewidgetitem44);
        prod_tableProduits->setObjectName("prod_tableProduits");
        sizePolicy1.setHeightForWidth(prod_tableProduits->sizePolicy().hasHeightForWidth());
        prod_tableProduits->setSizePolicy(sizePolicy1);
        prod_tableProduits->setRowCount(0);
        prod_tableProduits->setColumnCount(8);
        prod_tableProduits->verticalHeader()->setVisible(false);

        prod_tablePgLayout->addWidget(prod_tableProduits);

        prod_viewStackedWidget->addWidget(prod_tableViewPage);
        prod_cardViewPage = new QWidget();
        prod_cardViewPage->setObjectName("prod_cardViewPage");
        prod_cardPgLayout = new QVBoxLayout(prod_cardViewPage);
        prod_cardPgLayout->setSpacing(0);
        prod_cardPgLayout->setObjectName("prod_cardPgLayout");
        prod_cardPgLayout->setContentsMargins(0, 0, 0, 0);
        prod_cardViewScrollArea = new QScrollArea(prod_cardViewPage);
        prod_cardViewScrollArea->setObjectName("prod_cardViewScrollArea");
        prod_cardViewScrollArea->setFrameShape(QFrame::Shape::NoFrame);
        prod_cardViewScrollArea->setWidgetResizable(true);
        prod_cardContainer = new QWidget();
        prod_cardContainer->setObjectName("prod_cardContainer");
        prod_cardContainer->setGeometry(QRect(0, 0, 100, 30));
        prod_cardLayout = new QGridLayout(prod_cardContainer);
        prod_cardLayout->setSpacing(20);
        prod_cardLayout->setObjectName("prod_cardLayout");
        prod_cardLayout->setContentsMargins(10, 10, 10, 10);
        prod_cardViewScrollArea->setWidget(prod_cardContainer);

        prod_cardPgLayout->addWidget(prod_cardViewScrollArea);

        prod_viewStackedWidget->addWidget(prod_cardViewPage);

        prod_tableLayout->addWidget(prod_viewStackedWidget);


        prod_affichageLayout->addLayout(prod_tableLayout);

        prod_rightSidebar = new QFrame(prod_affichage);
        prod_rightSidebar->setObjectName("prod_rightSidebar");
        sizePolicy1.setHeightForWidth(prod_rightSidebar->sizePolicy().hasHeightForWidth());
        prod_rightSidebar->setSizePolicy(sizePolicy1);
        prod_rightSidebar->setMinimumSize(QSize(0, 0));
        prod_rightSidebar->setMaximumSize(QSize(16777215, 16777215));
        prod_rightSidebar->setFrameShape(QFrame::Shape::StyledPanel);
        prod_rightLayout = new QVBoxLayout(prod_rightSidebar);
        prod_rightLayout->setObjectName("prod_rightLayout");
        prod_lblFunc = new QLabel(prod_rightSidebar);
        prod_lblFunc->setObjectName("prod_lblFunc");

        prod_rightLayout->addWidget(prod_lblFunc);

        prod_stat1 = new QFrame(prod_rightSidebar);
        prod_stat1->setObjectName("prod_stat1");
        vboxLayout = new QVBoxLayout(prod_stat1);
        vboxLayout->setObjectName("vboxLayout");
        prod_st_val1 = new QLabel(prod_stat1);
        prod_st_val1->setObjectName("prod_st_val1");

        vboxLayout->addWidget(prod_st_val1);

        prod_st_lbl1 = new QLabel(prod_stat1);
        prod_st_lbl1->setObjectName("prod_st_lbl1");

        vboxLayout->addWidget(prod_st_lbl1);


        prod_rightLayout->addWidget(prod_stat1);

        prod_stat2 = new QFrame(prod_rightSidebar);
        prod_stat2->setObjectName("prod_stat2");
        vboxLayout1 = new QVBoxLayout(prod_stat2);
        vboxLayout1->setObjectName("vboxLayout1");
        prod_st_val2 = new QLabel(prod_stat2);
        prod_st_val2->setObjectName("prod_st_val2");

        vboxLayout1->addWidget(prod_st_val2);

        prod_st_lbl2 = new QLabel(prod_stat2);
        prod_st_lbl2->setObjectName("prod_st_lbl2");

        vboxLayout1->addWidget(prod_st_lbl2);


        prod_rightLayout->addWidget(prod_stat2);

        prod_stat3 = new QFrame(prod_rightSidebar);
        prod_stat3->setObjectName("prod_stat3");
        vboxLayout2 = new QVBoxLayout(prod_stat3);
        vboxLayout2->setObjectName("vboxLayout2");
        prod_st_val3 = new QLabel(prod_stat3);
        prod_st_val3->setObjectName("prod_st_val3");

        vboxLayout2->addWidget(prod_st_val3);

        prod_st_lbl3 = new QLabel(prod_stat3);
        prod_st_lbl3->setObjectName("prod_st_lbl3");

        vboxLayout2->addWidget(prod_st_lbl3);


        prod_rightLayout->addWidget(prod_stat3);

        prod_statsPreview = new QFrame(prod_rightSidebar);
        prod_statsPreview->setObjectName("prod_statsPreview");
        prod_statsPreviewLayout = new QVBoxLayout(prod_statsPreview);
        prod_statsPreviewLayout->setObjectName("prod_statsPreviewLayout");
        prod_lblStatsTitle = new QLabel(prod_statsPreview);
        prod_lblStatsTitle->setObjectName("prod_lblStatsTitle");

        prod_statsPreviewLayout->addWidget(prod_lblStatsTitle);

        prod_miniStatsFrame = new QFrame(prod_statsPreview);
        prod_miniStatsFrame->setObjectName("prod_miniStatsFrame");
        prod_miniStatsFrame->setMinimumSize(QSize(0, 70));
        prod_miniStatsFrame->setFrameShape(QFrame::Shape::StyledPanel);
        prod_miniStatsFrame->setFrameShadow(QFrame::Shadow::Sunken);
        prod_miniStatsLayout = new QVBoxLayout(prod_miniStatsFrame);
        prod_miniStatsLayout->setSpacing(6);
        prod_miniStatsLayout->setObjectName("prod_miniStatsLayout");
        prod_miniStatsLayout->setContentsMargins(8, 8, 8, 8);
        prod_rowMiniLayout1 = new QHBoxLayout();
        prod_rowMiniLayout1->setSpacing(8);
        prod_rowMiniLayout1->setObjectName("prod_rowMiniLayout1");
        prod_rowMiniLayout1->setContentsMargins(0, 0, 0, 0);
        prod_lblMini1 = new QLabel(prod_miniStatsFrame);
        prod_lblMini1->setObjectName("prod_lblMini1");

        prod_rowMiniLayout1->addWidget(prod_lblMini1);

        prod_pbTrend = new QProgressBar(prod_miniStatsFrame);
        prod_pbTrend->setObjectName("prod_pbTrend");
        prod_pbTrend->setValue(68);
        prod_pbTrend->setTextVisible(false);

        prod_rowMiniLayout1->addWidget(prod_pbTrend);


        prod_miniStatsLayout->addLayout(prod_rowMiniLayout1);

        prod_rowMiniLayout2 = new QHBoxLayout();
        prod_rowMiniLayout2->setSpacing(8);
        prod_rowMiniLayout2->setObjectName("prod_rowMiniLayout2");
        prod_rowMiniLayout2->setContentsMargins(0, 0, 0, 0);
        prod_lblMini2 = new QLabel(prod_miniStatsFrame);
        prod_lblMini2->setObjectName("prod_lblMini2");

        prod_rowMiniLayout2->addWidget(prod_lblMini2);

        prod_pbStock = new QProgressBar(prod_miniStatsFrame);
        prod_pbStock->setObjectName("prod_pbStock");
        prod_pbStock->setValue(42);
        prod_pbStock->setTextVisible(false);

        prod_rowMiniLayout2->addWidget(prod_pbStock);


        prod_miniStatsLayout->addLayout(prod_rowMiniLayout2);


        prod_statsPreviewLayout->addWidget(prod_miniStatsFrame);

        prod_btnOpenStats = new QPushButton(prod_statsPreview);
        prod_btnOpenStats->setObjectName("prod_btnOpenStats");
        prod_btnOpenStats->setStyleSheet(QString::fromUtf8("background-color: #ffffff;\n"
"color: #1f2d3d;\n"
"border: 1px solid #dce1e6;\n"
"border-radius: 10px;\n"
"padding: 12px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));
        prod_btnOpenStats->setFlat(false);

        prod_statsPreviewLayout->addWidget(prod_btnOpenStats);


        prod_rightLayout->addWidget(prod_statsPreview);

        prod_btnMap3D = new QPushButton(prod_rightSidebar);
        prod_btnMap3D->setObjectName("prod_btnMap3D");
        prod_btnMap3D->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        prod_btnMap3D->setStyleSheet(QString::fromUtf8("background-color: #ffffff;\n"
"color: #1f2d3d;\n"
"border: 1px solid #dce1e6;\n"
"border-radius: 10px;\n"
"padding: 12px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        prod_rightLayout->addWidget(prod_btnMap3D);

        prod_btnVideo3D = new QPushButton(prod_rightSidebar);
        prod_btnVideo3D->setObjectName("prod_btnVideo3D");
        prod_btnVideo3D->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        prod_btnVideo3D->setStyleSheet(QString::fromUtf8("background-color: #ffffff;\n"
"color: #1f2d3d;\n"
"border: 1px solid #dce1e6;\n"
"border-radius: 10px;\n"
"padding: 12px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        prod_rightLayout->addWidget(prod_btnVideo3D);

        spacerItem2 = new QSpacerItem(0, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        prod_rightLayout->addItem(spacerItem2);

        prod_lblExport = new QLabel(prod_rightSidebar);
        prod_lblExport->setObjectName("prod_lblExport");

        prod_rightLayout->addWidget(prod_lblExport);

        prod_btnPdf = new QPushButton(prod_rightSidebar);
        prod_btnPdf->setObjectName("prod_btnPdf");

        prod_rightLayout->addWidget(prod_btnPdf);

        spacerItem3 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        prod_rightLayout->addItem(spacerItem3);


        prod_affichageLayout->addWidget(prod_rightSidebar);

        prod_affichageLayout->setStretch(0, 3);
        prod_affichageLayout->setStretch(1, 1);
        prod_stackedWidget->addWidget(prod_affichage);
        prod_ajout = new QWidget();
        prod_ajout->setObjectName("prod_ajout");
        prod_ajout->setAutoFillBackground(false);
        prod_ajout_hLayout = new QHBoxLayout(prod_ajout);
        prod_ajout_hLayout->setSpacing(0);
        prod_ajout_hLayout->setObjectName("prod_ajout_hLayout");
        prod_ajout_hLayout->setContentsMargins(0, 0, 0, 0);
        prod_addRightLayout = new QVBoxLayout();
        prod_addRightLayout->setSpacing(0);
        prod_addRightLayout->setObjectName("prod_addRightLayout");
        prod_addRightLayout->setContentsMargins(0, 0, 0, 0);
        prod_hl_add_split = new QHBoxLayout();
        prod_hl_add_split->setSpacing(20);
        prod_hl_add_split->setObjectName("prod_hl_add_split");
        prod_vl_img_add = new QVBoxLayout();
        prod_vl_img_add->setObjectName("prod_vl_img_add");
        prod_lblImgPreview_Add = new QLabel(prod_ajout);
        prod_lblImgPreview_Add->setObjectName("prod_lblImgPreview_Add");
        prod_lblImgPreview_Add->setMinimumSize(QSize(0, 250));
        prod_lblImgPreview_Add->setAlignment(Qt::AlignmentFlag::AlignCenter);

        prod_vl_img_add->addWidget(prod_lblImgPreview_Add);

        prod_btnUpload_Add = new QPushButton(prod_ajout);
        prod_btnUpload_Add->setObjectName("prod_btnUpload_Add");

        prod_vl_img_add->addWidget(prod_btnUpload_Add);

        spacerItem4 = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        prod_vl_img_add->addItem(spacerItem4);


        prod_hl_add_split->addLayout(prod_vl_img_add);

        prod_cardAdd = new QFrame(prod_ajout);
        prod_cardAdd->setObjectName("prod_cardAdd");
        prod_cardAdd->setFrameShape(QFrame::Shape::StyledPanel);
        prod_verticalLayout_Add = new QVBoxLayout(prod_cardAdd);
        prod_verticalLayout_Add->setSpacing(10);
        prod_verticalLayout_Add->setObjectName("prod_verticalLayout_Add");
        prod_verticalLayout_Add->setContentsMargins(30, 20, 30, 20);
        prod_lblTitleAdd = new QLabel(prod_cardAdd);
        prod_lblTitleAdd->setObjectName("prod_lblTitleAdd");

        prod_verticalLayout_Add->addWidget(prod_lblTitleAdd);

        prod_gl_inputs = new QGridLayout();
        prod_gl_inputs->setObjectName("prod_gl_inputs");
        prod_l1 = new QLabel(prod_cardAdd);
        prod_l1->setObjectName("prod_l1");

        prod_gl_inputs->addWidget(prod_l1, 0, 0, 1, 1);

        prod_ln_ref_add = new QLineEdit(prod_cardAdd);
        prod_ln_ref_add->setObjectName("prod_ln_ref_add");

        prod_gl_inputs->addWidget(prod_ln_ref_add, 0, 1, 1, 1);

        prod_l2 = new QLabel(prod_cardAdd);
        prod_l2->setObjectName("prod_l2");

        prod_gl_inputs->addWidget(prod_l2, 0, 2, 1, 1);

        prod_ln_model_add = new QLineEdit(prod_cardAdd);
        prod_ln_model_add->setObjectName("prod_ln_model_add");

        prod_gl_inputs->addWidget(prod_ln_model_add, 0, 3, 1, 1);

        prod_l_px = new QLabel(prod_cardAdd);
        prod_l_px->setObjectName("prod_l_px");

        prod_gl_inputs->addWidget(prod_l_px, 1, 0, 1, 1);

        prod_dsb_price_add = new QDoubleSpinBox(prod_cardAdd);
        prod_dsb_price_add->setObjectName("prod_dsb_price_add");
        prod_dsb_price_add->setMaximum(99999.000000000000000);

        prod_gl_inputs->addWidget(prod_dsb_price_add, 1, 1, 1, 1);

        prod_l_qty = new QLabel(prod_cardAdd);
        prod_l_qty->setObjectName("prod_l_qty");

        prod_gl_inputs->addWidget(prod_l_qty, 1, 2, 1, 1);

        prod_sb_qty_add = new QSpinBox(prod_cardAdd);
        prod_sb_qty_add->setObjectName("prod_sb_qty_add");

        prod_gl_inputs->addWidget(prod_sb_qty_add, 1, 3, 1, 1);

        prod_l_stat = new QLabel(prod_cardAdd);
        prod_l_stat->setObjectName("prod_l_stat");

        prod_gl_inputs->addWidget(prod_l_stat, 2, 0, 1, 1);

        prod_cb_status_add = new QComboBox(prod_cardAdd);
        prod_cb_status_add->addItem(QString());
        prod_cb_status_add->setObjectName("prod_cb_status_add");

        prod_gl_inputs->addWidget(prod_cb_status_add, 2, 1, 1, 1);


        prod_verticalLayout_Add->addLayout(prod_gl_inputs);

        prod_hl_sliders = new QHBoxLayout();
        prod_hl_sliders->setObjectName("prod_hl_sliders");
        prod_vl_bat = new QVBoxLayout();
        prod_vl_bat->setObjectName("prod_vl_bat");
        prod_horizontalLayout = new QHBoxLayout();
        prod_horizontalLayout->setObjectName("prod_horizontalLayout");
        prod_l_bat = new QLabel(prod_cardAdd);
        prod_l_bat->setObjectName("prod_l_bat");

        prod_horizontalLayout->addWidget(prod_l_bat);

        prod_label = new QLabel(prod_cardAdd);
        prod_label->setObjectName("prod_label");

        prod_horizontalLayout->addWidget(prod_label);


        prod_vl_bat->addLayout(prod_horizontalLayout);

        prod_slider_bat_add = new QSlider(prod_cardAdd);
        prod_slider_bat_add->setObjectName("prod_slider_bat_add");
        prod_slider_bat_add->setMinimum(2000);
        prod_slider_bat_add->setMaximum(20000);
        prod_slider_bat_add->setOrientation(Qt::Orientation::Horizontal);

        prod_vl_bat->addWidget(prod_slider_bat_add);


        prod_hl_sliders->addLayout(prod_vl_bat);

        prod_vl_cap = new QVBoxLayout();
        prod_vl_cap->setObjectName("prod_vl_cap");
        prod_horizontalLayout_4 = new QHBoxLayout();
        prod_horizontalLayout_4->setObjectName("prod_horizontalLayout_4");
        prod_l_cap = new QLabel(prod_cardAdd);
        prod_l_cap->setObjectName("prod_l_cap");

        prod_horizontalLayout_4->addWidget(prod_l_cap);

        prod_label_4 = new QLabel(prod_cardAdd);
        prod_label_4->setObjectName("prod_label_4");

        prod_horizontalLayout_4->addWidget(prod_label_4);


        prod_vl_cap->addLayout(prod_horizontalLayout_4);

        prod_slider_cap_add = new QSlider(prod_cardAdd);
        prod_slider_cap_add->setObjectName("prod_slider_cap_add");
        prod_slider_cap_add->setMinimum(50);
        prod_slider_cap_add->setMaximum(1000);
        prod_slider_cap_add->setOrientation(Qt::Orientation::Horizontal);

        prod_vl_cap->addWidget(prod_slider_cap_add);


        prod_hl_sliders->addLayout(prod_vl_cap);


        prod_verticalLayout_Add->addLayout(prod_hl_sliders);

        prod_gb_characteristics = new QGroupBox(prod_cardAdd);
        prod_gb_characteristics->setObjectName("prod_gb_characteristics");
        prod_vl_characteristics = new QVBoxLayout(prod_gb_characteristics);
        prod_vl_characteristics->setObjectName("prod_vl_characteristics");
        prod_lstCharacteristics = new QListWidget(prod_gb_characteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        new QListWidgetItem(prod_lstCharacteristics);
        prod_lstCharacteristics->setObjectName("prod_lstCharacteristics");
        prod_lstCharacteristics->setAlternatingRowColors(true);
        prod_lstCharacteristics->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

        prod_vl_characteristics->addWidget(prod_lstCharacteristics);


        prod_verticalLayout_Add->addWidget(prod_gb_characteristics);

        prod_l5 = new QLabel(prod_cardAdd);
        prod_l5->setObjectName("prod_l5");

        prod_verticalLayout_Add->addWidget(prod_l5);

        prod_btns_add = new QHBoxLayout();
        prod_btns_add->setObjectName("prod_btns_add");
        prod_btnSave_Add = new QPushButton(prod_cardAdd);
        prod_btnSave_Add->setObjectName("prod_btnSave_Add");
        prod_btnSave_Add->setMinimumSize(QSize(0, 35));

        prod_btns_add->addWidget(prod_btnSave_Add);

        prod_btnCancel_Add = new QPushButton(prod_cardAdd);
        prod_btnCancel_Add->setObjectName("prod_btnCancel_Add");
        prod_btnCancel_Add->setMinimumSize(QSize(0, 35));

        prod_btns_add->addWidget(prod_btnCancel_Add);


        prod_verticalLayout_Add->addLayout(prod_btns_add);


        prod_hl_add_split->addWidget(prod_cardAdd);


        prod_addRightLayout->addLayout(prod_hl_add_split);


        prod_ajout_hLayout->addLayout(prod_addRightLayout);

        prod_stackedWidget->addWidget(prod_ajout);
        prod_modification = new QWidget();
        prod_modification->setObjectName("prod_modification");
        prod_modification->setAutoFillBackground(false);
        prod_modifier_hLayout = new QHBoxLayout(prod_modification);
        prod_modifier_hLayout->setSpacing(0);
        prod_modifier_hLayout->setObjectName("prod_modifier_hLayout");
        prod_modifier_hLayout->setContentsMargins(0, 0, 0, 0);
        prod_modRightLayout = new QVBoxLayout();
        prod_modRightLayout->setSpacing(0);
        prod_modRightLayout->setObjectName("prod_modRightLayout");
        prod_modRightLayout->setContentsMargins(0, 0, 0, 0);
        prod_hl_mod_split = new QHBoxLayout();
        prod_hl_mod_split->setSpacing(20);
        prod_hl_mod_split->setObjectName("prod_hl_mod_split");
        prod_imgBin = new QLabel(prod_modification);
        prod_imgBin->setObjectName("prod_imgBin");
        prod_imgBin->setMaximumSize(QSize(16777215, 250));
        prod_imgBin->setAlignment(Qt::AlignmentFlag::AlignCenter);

        prod_hl_mod_split->addWidget(prod_imgBin);

        prod_cardMod = new QFrame(prod_modification);
        prod_cardMod->setObjectName("prod_cardMod");
        prod_cardMod->setFrameShape(QFrame::Shape::StyledPanel);
        prod_verticalLayout_Mod = new QVBoxLayout(prod_cardMod);
        prod_verticalLayout_Mod->setSpacing(10);
        prod_verticalLayout_Mod->setObjectName("prod_verticalLayout_Mod");
        prod_verticalLayout_Mod->setContentsMargins(30, 20, 30, 20);
        prod_lblTitleMod = new QLabel(prod_cardMod);
        prod_lblTitleMod->setObjectName("prod_lblTitleMod");

        prod_verticalLayout_Mod->addWidget(prod_lblTitleMod);

        prod_gl_inputs_mod = new QGridLayout();
        prod_gl_inputs_mod->setObjectName("prod_gl_inputs_mod");
        prod_sb_qty_mod = new QSpinBox(prod_cardMod);
        prod_sb_qty_mod->setObjectName("prod_sb_qty_mod");
        prod_sb_qty_mod->setValue(45);

        prod_gl_inputs_mod->addWidget(prod_sb_qty_mod, 1, 3, 1, 1);

        prod_l_stat_m = new QLabel(prod_cardMod);
        prod_l_stat_m->setObjectName("prod_l_stat_m");

        prod_gl_inputs_mod->addWidget(prod_l_stat_m, 2, 0, 1, 1);

        prod_l_qty_m = new QLabel(prod_cardMod);
        prod_l_qty_m->setObjectName("prod_l_qty_m");

        prod_gl_inputs_mod->addWidget(prod_l_qty_m, 1, 2, 1, 1);

        prod_ln_model_mod = new QLineEdit(prod_cardMod);
        prod_ln_model_mod->setObjectName("prod_ln_model_mod");

        prod_gl_inputs_mod->addWidget(prod_ln_model_mod, 0, 3, 1, 1);

        prod_cb_status_mod = new QComboBox(prod_cardMod);
        prod_cb_status_mod->addItem(QString());
        prod_cb_status_mod->setObjectName("prod_cb_status_mod");

        prod_gl_inputs_mod->addWidget(prod_cb_status_mod, 2, 1, 1, 1);

        prod_dsb_price_mod = new QDoubleSpinBox(prod_cardMod);
        prod_dsb_price_mod->setObjectName("prod_dsb_price_mod");
        prod_dsb_price_mod->setValue(0.000000000000000);

        prod_gl_inputs_mod->addWidget(prod_dsb_price_mod, 1, 1, 1, 1);

        prod_ln_ref_mod = new QLineEdit(prod_cardMod);
        prod_ln_ref_mod->setObjectName("prod_ln_ref_mod");
        prod_ln_ref_mod->setReadOnly(true);

        prod_gl_inputs_mod->addWidget(prod_ln_ref_mod, 0, 1, 1, 1);

        prod_l2_m = new QLabel(prod_cardMod);
        prod_l2_m->setObjectName("prod_l2_m");

        prod_gl_inputs_mod->addWidget(prod_l2_m, 0, 2, 1, 1);

        prod_l_px_m = new QLabel(prod_cardMod);
        prod_l_px_m->setObjectName("prod_l_px_m");

        prod_gl_inputs_mod->addWidget(prod_l_px_m, 1, 0, 1, 1);

        prod_l1_m = new QLabel(prod_cardMod);
        prod_l1_m->setObjectName("prod_l1_m");

        prod_gl_inputs_mod->addWidget(prod_l1_m, 0, 0, 1, 1);


        prod_verticalLayout_Mod->addLayout(prod_gl_inputs_mod);

        prod_hl_sliders_mod = new QHBoxLayout();
        prod_hl_sliders_mod->setObjectName("prod_hl_sliders_mod");
        prod_vl_bat_mod = new QVBoxLayout();
        prod_vl_bat_mod->setObjectName("prod_vl_bat_mod");
        prod_l_bat_m = new QLabel(prod_cardMod);
        prod_l_bat_m->setObjectName("prod_l_bat_m");

        prod_vl_bat_mod->addWidget(prod_l_bat_m);

        prod_slider_bat_mod = new QSlider(prod_cardMod);
        prod_slider_bat_mod->setObjectName("prod_slider_bat_mod");
        prod_slider_bat_mod->setMaximum(20000);
        prod_slider_bat_mod->setValue(12000);
        prod_slider_bat_mod->setOrientation(Qt::Orientation::Horizontal);

        prod_vl_bat_mod->addWidget(prod_slider_bat_mod);


        prod_hl_sliders_mod->addLayout(prod_vl_bat_mod);

        prod_vl_cap_mod = new QVBoxLayout();
        prod_vl_cap_mod->setObjectName("prod_vl_cap_mod");
        prod_l_cap_m = new QLabel(prod_cardMod);
        prod_l_cap_m->setObjectName("prod_l_cap_m");

        prod_vl_cap_mod->addWidget(prod_l_cap_m);

        prod_slider_cap_mod = new QSlider(prod_cardMod);
        prod_slider_cap_mod->setObjectName("prod_slider_cap_mod");
        prod_slider_cap_mod->setMaximum(1000);
        prod_slider_cap_mod->setOrientation(Qt::Orientation::Horizontal);

        prod_vl_cap_mod->addWidget(prod_slider_cap_mod);


        prod_hl_sliders_mod->addLayout(prod_vl_cap_mod);


        prod_verticalLayout_Mod->addLayout(prod_hl_sliders_mod);

        prod_gb_characteristics_mod = new QGroupBox(prod_cardMod);
        prod_gb_characteristics_mod->setObjectName("prod_gb_characteristics_mod");
        prod_vl_characteristics_mod = new QVBoxLayout(prod_gb_characteristics_mod);
        prod_vl_characteristics_mod->setObjectName("prod_vl_characteristics_mod");
        prod_lstCharacteristics_mod = new QListWidget(prod_gb_characteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        new QListWidgetItem(prod_lstCharacteristics_mod);
        prod_lstCharacteristics_mod->setObjectName("prod_lstCharacteristics_mod");
        prod_lstCharacteristics_mod->setAlternatingRowColors(true);
        prod_lstCharacteristics_mod->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

        prod_vl_characteristics_mod->addWidget(prod_lstCharacteristics_mod);


        prod_verticalLayout_Mod->addWidget(prod_gb_characteristics_mod);

        prod_l5_m = new QLabel(prod_cardMod);
        prod_l5_m->setObjectName("prod_l5_m");

        prod_verticalLayout_Mod->addWidget(prod_l5_m);

        prod_btns_mod = new QHBoxLayout();
        prod_btns_mod->setObjectName("prod_btns_mod");
        prod_btnSave_Mod = new QPushButton(prod_cardMod);
        prod_btnSave_Mod->setObjectName("prod_btnSave_Mod");
        prod_btnSave_Mod->setMinimumSize(QSize(0, 35));

        prod_btns_mod->addWidget(prod_btnSave_Mod);

        prod_btnCancel_Mod = new QPushButton(prod_cardMod);
        prod_btnCancel_Mod->setObjectName("prod_btnCancel_Mod");
        prod_btnCancel_Mod->setMinimumSize(QSize(0, 35));

        prod_btns_mod->addWidget(prod_btnCancel_Mod);


        prod_verticalLayout_Mod->addLayout(prod_btns_mod);


        prod_hl_mod_split->addWidget(prod_cardMod);


        prod_modRightLayout->addLayout(prod_hl_mod_split);


        prod_modifier_hLayout->addLayout(prod_modRightLayout);

        prod_stackedWidget->addWidget(prod_modification);
        prod_statistiques = new QWidget();
        prod_statistiques->setObjectName("prod_statistiques");
        prod_statsPopupLayout = new QVBoxLayout(prod_statistiques);
        prod_statsPopupLayout->setSpacing(14);
        prod_statsPopupLayout->setObjectName("prod_statsPopupLayout");
        prod_statsPopupLayout->setContentsMargins(20, 20, 20, 20);
        prod_statsHeaderLayout = new QHBoxLayout();
        prod_statsHeaderLayout->setObjectName("prod_statsHeaderLayout");
        prod_btnReturnStats = new QPushButton(prod_statistiques);
        prod_btnReturnStats->setObjectName("prod_btnReturnStats");
        prod_btnReturnStats->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        prod_statsHeaderLayout->addWidget(prod_btnReturnStats);

        prod_horizontalSpacer_Stats = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        prod_statsHeaderLayout->addItem(prod_horizontalSpacer_Stats);


        prod_statsPopupLayout->addLayout(prod_statsHeaderLayout);

        prod_lblStatsHeader = new QLabel(prod_statistiques);
        prod_lblStatsHeader->setObjectName("prod_lblStatsHeader");

        prod_statsPopupLayout->addWidget(prod_lblStatsHeader);

        prod_lblStatsSub = new QLabel(prod_statistiques);
        prod_lblStatsSub->setObjectName("prod_lblStatsSub");

        prod_statsPopupLayout->addWidget(prod_lblStatsSub);

        prod_graphsLayout = new QHBoxLayout();
        prod_graphsLayout->setSpacing(16);
        prod_graphsLayout->setObjectName("prod_graphsLayout");
        prod_graphsLayout->setContentsMargins(0, 0, 0, 0);
        prod_graphFrame1 = new QFrame(prod_statistiques);
        prod_graphFrame1->setObjectName("prod_graphFrame1");
        prod_graphFrame1->setFrameShape(QFrame::Shape::StyledPanel);
        prod_graphFrame1->setFrameShadow(QFrame::Shadow::Raised);
        prod_graphFrame1Layout = new QVBoxLayout(prod_graphFrame1);
        prod_graphFrame1Layout->setSpacing(8);
        prod_graphFrame1Layout->setObjectName("prod_graphFrame1Layout");
        prod_graphFrame1Layout->setContentsMargins(12, 12, 12, 12);
        prod_graphFrame1Title = new QLabel(prod_graphFrame1);
        prod_graphFrame1Title->setObjectName("prod_graphFrame1Title");

        prod_graphFrame1Layout->addWidget(prod_graphFrame1Title);

        prod_graphFrame1Plot = new QFrame(prod_graphFrame1);
        prod_graphFrame1Plot->setObjectName("prod_graphFrame1Plot");
        prod_graphFrame1Plot->setMinimumSize(QSize(0, 0));
        prod_graphFrame1Plot->setFrameShape(QFrame::Shape::StyledPanel);
        prod_graphFrame1Plot->setFrameShadow(QFrame::Shadow::Sunken);
        prod_graphFrame1PlotLayout = new QVBoxLayout(prod_graphFrame1Plot);
        prod_graphFrame1PlotLayout->setObjectName("prod_graphFrame1PlotLayout");
        prod_graphFrame1PlotLayout->setContentsMargins(8, 8, 8, 8);
        prod_graphFrame1Hint = new QLabel(prod_graphFrame1Plot);
        prod_graphFrame1Hint->setObjectName("prod_graphFrame1Hint");
        prod_graphFrame1Hint->setAlignment(Qt::AlignmentFlag::AlignCenter);

        prod_graphFrame1PlotLayout->addWidget(prod_graphFrame1Hint);


        prod_graphFrame1Layout->addWidget(prod_graphFrame1Plot);


        prod_graphsLayout->addWidget(prod_graphFrame1);

        prod_graphFrame2 = new QFrame(prod_statistiques);
        prod_graphFrame2->setObjectName("prod_graphFrame2");
        prod_graphFrame2->setFrameShape(QFrame::Shape::StyledPanel);
        prod_graphFrame2->setFrameShadow(QFrame::Shadow::Raised);
        prod_graphFrame2Layout = new QVBoxLayout(prod_graphFrame2);
        prod_graphFrame2Layout->setSpacing(8);
        prod_graphFrame2Layout->setObjectName("prod_graphFrame2Layout");
        prod_graphFrame2Layout->setContentsMargins(12, 12, 12, 12);
        prod_graphFrame2Title = new QLabel(prod_graphFrame2);
        prod_graphFrame2Title->setObjectName("prod_graphFrame2Title");

        prod_graphFrame2Layout->addWidget(prod_graphFrame2Title);

        prod_graphFrame2Plot = new QFrame(prod_graphFrame2);
        prod_graphFrame2Plot->setObjectName("prod_graphFrame2Plot");
        prod_graphFrame2Plot->setMinimumSize(QSize(0, 0));
        prod_graphFrame2Plot->setFrameShape(QFrame::Shape::StyledPanel);
        prod_graphFrame2Plot->setFrameShadow(QFrame::Shadow::Sunken);
        prod_graphFrame2PlotLayout = new QVBoxLayout(prod_graphFrame2Plot);
        prod_graphFrame2PlotLayout->setObjectName("prod_graphFrame2PlotLayout");
        prod_graphFrame2PlotLayout->setContentsMargins(8, 8, 8, 8);
        prod_graphFrame2Hint = new QLabel(prod_graphFrame2Plot);
        prod_graphFrame2Hint->setObjectName("prod_graphFrame2Hint");
        prod_graphFrame2Hint->setAlignment(Qt::AlignmentFlag::AlignCenter);

        prod_graphFrame2PlotLayout->addWidget(prod_graphFrame2Hint);


        prod_graphFrame2Layout->addWidget(prod_graphFrame2Plot);


        prod_graphsLayout->addWidget(prod_graphFrame2);


        prod_statsPopupLayout->addLayout(prod_graphsLayout);

        prod_lblStatsNote = new QLabel(prod_statistiques);
        prod_lblStatsNote->setObjectName("prod_lblStatsNote");

        prod_statsPopupLayout->addWidget(prod_lblStatsNote);

        spacerItem5 = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        prod_statsPopupLayout->addItem(spacerItem5);

        prod_stackedWidget->addWidget(prod_statistiques);

        prod_contentLayout->addWidget(prod_stackedWidget);


        prod_dashboardRightLayout->addLayout(prod_contentLayout);


        prod_dashboard_hLayout->addLayout(prod_dashboardRightLayout);


        prod_mainAreaLayout->addLayout(prod_dashboard_hLayout);


        prod_rootLayout->addLayout(prod_mainAreaLayout);


        verticalLayout_ProduitPage->addLayout(prod_rootLayout);

        stackedWidget->addWidget(pageProduit);
        pageStock = new QWidget();
        pageStock->setObjectName("pageStock");
        horizontalLayout_Main1 = new QHBoxLayout(pageStock);
        horizontalLayout_Main1->setSpacing(0);
        horizontalLayout_Main1->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_Main1->setObjectName("horizontalLayout_Main1");
        mainVerticalLayout = new QVBoxLayout();
        mainVerticalLayout->setSpacing(0);
        mainVerticalLayout->setObjectName("mainVerticalLayout");
        headerBar = new QFrame(pageStock);
        headerBar->setObjectName("headerBar");
        headerBar->setMinimumSize(QSize(0, 60));
        headerLayout = new QHBoxLayout(headerBar);
        headerLayout->setObjectName("headerLayout");
        headerTitle = new QLabel(headerBar);
        headerTitle->setObjectName("headerTitle");

        headerLayout->addWidget(headerTitle);

        horizontalSpacer_header = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(horizontalSpacer_header);

        btnNew = new QPushButton(headerBar);
        btnNew->setObjectName("btnNew");

        headerLayout->addWidget(btnNew);

        user = new QLabel(headerBar);
        user->setObjectName("user");

        headerLayout->addWidget(user);


        mainVerticalLayout->addWidget(headerBar);

        stock_stackedWidget = new QStackedWidget(pageStock);
        stock_stackedWidget->setObjectName("stock_stackedWidget");
        pageInventaire = new QWidget();
        pageInventaire->setObjectName("pageInventaire");
        dashboardLayout = new QHBoxLayout(pageInventaire);
        dashboardLayout->setSpacing(25);
        dashboardLayout->setContentsMargins(18, 18, 18, 18);
        dashboardLayout->setObjectName("dashboardLayout");
        tableCard = new QFrame(pageInventaire);
        tableCard->setObjectName("tableCard");
        tableCardLayout = new QVBoxLayout(tableCard);
        tableCardLayout->setContentsMargins(20, 20, 20, 20);
        tableCardLayout->setObjectName("tableCardLayout");
        tableHeaderActions = new QHBoxLayout();
        tableHeaderActions->setObjectName("tableHeaderActions");
        lblTableTitle = new QLabel(tableCard);
        lblTableTitle->setObjectName("lblTableTitle");

        tableHeaderActions->addWidget(lblTableTitle);

        horizontalSpacer_table = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        tableHeaderActions->addItem(horizontalSpacer_table);


        tableCardLayout->addLayout(tableHeaderActions);

        stockSearchSortRow = new QHBoxLayout();
        stockSearchSortRow->setSpacing(12);
        stockSearchSortRow->setObjectName("stockSearchSortRow");
        stock_searchInput = new QLineEdit(tableCard);
        stock_searchInput->setObjectName("stock_searchInput");
        stock_searchInput->setMinimumSize(QSize(0, 34));

        stockSearchSortRow->addWidget(stock_searchInput);

        stock_sortCombo = new QComboBox(tableCard);
        stock_sortCombo->addItem(QString());
        stock_sortCombo->addItem(QString());
        stock_sortCombo->addItem(QString());
        stock_sortCombo->setObjectName("stock_sortCombo");
        stock_sortCombo->setMinimumSize(QSize(210, 40));

        stockSearchSortRow->addWidget(stock_sortCombo);


        tableCardLayout->addLayout(stockSearchSortRow);

        tableWidget = new QTableWidget(tableCard);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setAutoFillBackground(true);

        tableCardLayout->addWidget(tableWidget);


        dashboardLayout->addWidget(tableCard);

        rightColumn = new QVBoxLayout();
        rightColumn->setSpacing(14);
        rightColumn->setObjectName("rightColumn");
        rightColumn->setContentsMargins(0, 0, 0, 0);
        statsBox = new QFrame(pageInventaire);
        statsBox->setObjectName("statsBox");
        statsLayout = new QVBoxLayout(statsBox);
        statsLayout->setObjectName("statsLayout");
        lblStatsTitle = new QLabel(statsBox);
        lblStatsTitle->setObjectName("lblStatsTitle");

        statsLayout->addWidget(lblStatsTitle);

        totalStock = new QLabel(statsBox);
        totalStock->setObjectName("totalStock");

        statsLayout->addWidget(totalStock);

        lblCriticalStock = new QLabel(statsBox);
        lblCriticalStock->setObjectName("lblCriticalStock");

        statsLayout->addWidget(lblCriticalStock);


        rightColumn->addWidget(statsBox);

        orderBox = new QFrame(pageInventaire);
        orderBox->setObjectName("orderBox");
        orderLayout = new QVBoxLayout(orderBox);
        orderLayout->setObjectName("orderLayout");
        lblOrderTitle = new QLabel(orderBox);
        lblOrderTitle->setObjectName("lblOrderTitle");

        orderLayout->addWidget(lblOrderTitle);

        lblOrderSummary = new QLabel(orderBox);
        lblOrderSummary->setObjectName("lblOrderSummary");

        orderLayout->addWidget(lblOrderSummary);

        btnOrder = new QPushButton(orderBox);
        btnOrder->setObjectName("btnOrder");
        btnOrder->setStyleSheet(QString::fromUtf8("background-color: #ffffff;\n"
"color: #1f2d3d;\n"
"border: 1px solid #dce1e6;\n"
"border-radius: 10px;\n"
"padding: 12px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        orderLayout->addWidget(btnOrder);


        rightColumn->addWidget(orderBox);

        btnPrediction = new QPushButton(pageInventaire);
        btnPrediction->setObjectName("btnPrediction");
        btnPrediction->setStyleSheet(QString::fromUtf8("background-color: #ffffff;\n"
"color: #1f2d3d;\n"
"border: 1px solid #dce1e6;\n"
"border-radius: 10px;\n"
"padding: 12px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        rightColumn->addWidget(btnPrediction);

        btnAlerts = new QPushButton(pageInventaire);
        btnAlerts->setObjectName("btnAlerts");
        btnAlerts->setStyleSheet(QString::fromUtf8("background-color: #ffffff;\n"
"color: #1f2d3d;\n"
"border: 1px solid #dce1e6;\n"
"border-radius: 10px;\n"
"padding: 12px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        rightColumn->addWidget(btnAlerts);

        btnGoStats_Stock = new QPushButton(pageInventaire);
        btnGoStats_Stock->setObjectName("btnGoStats_Stock");

        rightColumn->addWidget(btnGoStats_Stock);

        verticalSpacer_right = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        rightColumn->addItem(verticalSpacer_right);


        dashboardLayout->addLayout(rightColumn);

        dashboardLayout->setStretch(0, 3);
        dashboardLayout->setStretch(1, 1);
        stock_stackedWidget->addWidget(pageInventaire);
        pageAjouter = new QWidget();
        pageAjouter->setObjectName("pageAjouter");
        layoutAjouter = new QVBoxLayout(pageAjouter);
        layoutAjouter->setObjectName("layoutAjouter");
        addCard = new QFrame(pageAjouter);
        addCard->setObjectName("addCard");
        addCard->setMinimumSize(QSize(600, 0));
        addCardLayout = new QVBoxLayout(addCard);
        addCardLayout->setSpacing(20);
        addCardLayout->setContentsMargins(40, 40, 40, 40);
        addCardLayout->setObjectName("addCardLayout");
        lblAddTitle = new QLabel(addCard);
        lblAddTitle->setObjectName("lblAddTitle");

        addCardLayout->addWidget(lblAddTitle);

        layoutRefNom_add = new QHBoxLayout();
        layoutRefNom_add->setSpacing(15);
        layoutRefNom_add->setObjectName("layoutRefNom_add");
        inputRef_add = new QLineEdit(addCard);
        inputRef_add->setObjectName("inputRef_add");

        layoutRefNom_add->addWidget(inputRef_add);

        inputNom_add = new QLineEdit(addCard);
        inputNom_add->setObjectName("inputNom_add");

        layoutRefNom_add->addWidget(inputNom_add);


        addCardLayout->addLayout(layoutRefNom_add);

        layoutStock_add = new QHBoxLayout();
        layoutStock_add->setSpacing(15);
        layoutStock_add->setObjectName("layoutStock_add");
        inputStock_add = new QLineEdit(addCard);
        inputStock_add->setObjectName("inputStock_add");

        layoutStock_add->addWidget(inputStock_add);

        sliderStock_add = new QSlider(addCard);
        sliderStock_add->setObjectName("sliderStock_add");
        sliderStock_add->setMaximum(500);
        sliderStock_add->setOrientation(Qt::Orientation::Horizontal);

        layoutStock_add->addWidget(sliderStock_add);


        addCardLayout->addLayout(layoutStock_add);

        layoutPrix_add = new QHBoxLayout();
        layoutPrix_add->setSpacing(15);
        layoutPrix_add->setObjectName("layoutPrix_add");
        inputPrix_add = new QLineEdit(addCard);
        inputPrix_add->setObjectName("inputPrix_add");

        layoutPrix_add->addWidget(inputPrix_add);

        sliderPrix_add = new QSlider(addCard);
        sliderPrix_add->setObjectName("sliderPrix_add");
        sliderPrix_add->setMaximum(500);
        sliderPrix_add->setOrientation(Qt::Orientation::Horizontal);

        layoutPrix_add->addWidget(sliderPrix_add);


        addCardLayout->addLayout(layoutPrix_add);

        inputFournisseur_add = new QLineEdit(addCard);
        inputFournisseur_add->setObjectName("inputFournisseur_add");

        addCardLayout->addWidget(inputFournisseur_add);

        addActions = new QHBoxLayout();
        addActions->setObjectName("addActions");
        btnCancel_add = new QPushButton(addCard);
        btnCancel_add->setObjectName("btnCancel_add");

        addActions->addWidget(btnCancel_add);

        btnSave_add = new QPushButton(addCard);
        btnSave_add->setObjectName("btnSave_add");

        addActions->addWidget(btnSave_add);


        addCardLayout->addLayout(addActions);


        layoutAjouter->addWidget(addCard);

        stock_stackedWidget->addWidget(pageAjouter);
        pageModifier = new QWidget();
        pageModifier->setObjectName("pageModifier");
        layoutModifier = new QVBoxLayout(pageModifier);
        layoutModifier->setObjectName("layoutModifier");
        modifyCard = new QFrame(pageModifier);
        modifyCard->setObjectName("modifyCard");
        modifyCard->setMinimumSize(QSize(600, 0));
        modifyCardLayout = new QVBoxLayout(modifyCard);
        modifyCardLayout->setSpacing(20);
        modifyCardLayout->setContentsMargins(40, 40, 40, 40);
        modifyCardLayout->setObjectName("modifyCardLayout");
        lblModTitle = new QLabel(modifyCard);
        lblModTitle->setObjectName("lblModTitle");

        modifyCardLayout->addWidget(lblModTitle);

        layoutRefNom_mod = new QHBoxLayout();
        layoutRefNom_mod->setSpacing(15);
        layoutRefNom_mod->setObjectName("layoutRefNom_mod");
        inputRef_mod = new QLineEdit(modifyCard);
        inputRef_mod->setObjectName("inputRef_mod");

        layoutRefNom_mod->addWidget(inputRef_mod);

        inputNom_mod = new QLineEdit(modifyCard);
        inputNom_mod->setObjectName("inputNom_mod");

        layoutRefNom_mod->addWidget(inputNom_mod);


        modifyCardLayout->addLayout(layoutRefNom_mod);

        layoutStock_mod = new QHBoxLayout();
        layoutStock_mod->setSpacing(15);
        layoutStock_mod->setObjectName("layoutStock_mod");
        inputStock_mod = new QLineEdit(modifyCard);
        inputStock_mod->setObjectName("inputStock_mod");

        layoutStock_mod->addWidget(inputStock_mod);

        sliderStock_mod = new QSlider(modifyCard);
        sliderStock_mod->setObjectName("sliderStock_mod");
        sliderStock_mod->setMaximum(500);
        sliderStock_mod->setOrientation(Qt::Orientation::Horizontal);

        layoutStock_mod->addWidget(sliderStock_mod);


        modifyCardLayout->addLayout(layoutStock_mod);

        layoutPrix_mod = new QHBoxLayout();
        layoutPrix_mod->setSpacing(15);
        layoutPrix_mod->setObjectName("layoutPrix_mod");
        inputPrix_mod = new QLineEdit(modifyCard);
        inputPrix_mod->setObjectName("inputPrix_mod");

        layoutPrix_mod->addWidget(inputPrix_mod);

        sliderPrix_mod = new QSlider(modifyCard);
        sliderPrix_mod->setObjectName("sliderPrix_mod");
        sliderPrix_mod->setMaximum(500);
        sliderPrix_mod->setOrientation(Qt::Orientation::Horizontal);

        layoutPrix_mod->addWidget(sliderPrix_mod);


        modifyCardLayout->addLayout(layoutPrix_mod);

        inputFournisseur_mod = new QLineEdit(modifyCard);
        inputFournisseur_mod->setObjectName("inputFournisseur_mod");

        modifyCardLayout->addWidget(inputFournisseur_mod);

        modifyActions = new QHBoxLayout();
        modifyActions->setObjectName("modifyActions");
        btnCancel_mod = new QPushButton(modifyCard);
        btnCancel_mod->setObjectName("btnCancel_mod");

        modifyActions->addWidget(btnCancel_mod);

        btnSave_mod = new QPushButton(modifyCard);
        btnSave_mod->setObjectName("btnSave_mod");

        modifyActions->addWidget(btnSave_mod);


        modifyCardLayout->addLayout(modifyActions);


        layoutModifier->addWidget(modifyCard);

        stock_stackedWidget->addWidget(pageModifier);

        mainVerticalLayout->addWidget(stock_stackedWidget);


        horizontalLayout_Main1->addLayout(mainVerticalLayout);

        stackedWidget->addWidget(pageStock);
        page_Maintenance_Tab = new QWidget();
        page_Maintenance_Tab->setObjectName("page_Maintenance_Tab");
        vl_Maint_Root = new QVBoxLayout(page_Maintenance_Tab);
        vl_Maint_Root->setObjectName("vl_Maint_Root");
        vl_Maint_Root->setContentsMargins(0, 0, 0, 0);
        stackedWidget_Maintenance = new QStackedWidget(page_Maintenance_Tab);
        stackedWidget_Maintenance->setObjectName("stackedWidget_Maintenance");
        page_Maint_Dash = new QWidget();
        page_Maint_Dash->setObjectName("page_Maint_Dash");
        vl_page_Maint_Dash = new QVBoxLayout(page_Maint_Dash);
        vl_page_Maint_Dash->setSpacing(0);
        vl_page_Maint_Dash->setObjectName("vl_page_Maint_Dash");
        vl_page_Maint_Dash->setContentsMargins(0, 0, 0, 0);
        scrollAreaMaint = new QScrollArea(page_Maint_Dash);
        scrollAreaMaint->setObjectName("scrollAreaMaint");
        scrollAreaMaint->setFrameShape(QFrame::Shape::NoFrame);
        scrollAreaMaint->setWidgetResizable(true);
        scrollContentMaint = new QWidget();
        scrollContentMaint->setObjectName("scrollContentMaint");
        scrollContentMaint->setGeometry(QRect(0, 0, 781, 1057));
        vlScrollMaint = new QVBoxLayout(scrollContentMaint);
        vlScrollMaint->setSpacing(20);
        vlScrollMaint->setObjectName("vlScrollMaint");
        vlScrollMaint->setContentsMargins(20, 0, 20, 20);
        horizontalLayout_1 = new QHBoxLayout();
        horizontalLayout_1->setObjectName("horizontalLayout_1");
        horizontalLayout_1->setContentsMargins(20, -1, 20, -1);
        topBar = new QFrame(scrollContentMaint);
        topBar->setObjectName("topBar");
        topLayout_1 = new QHBoxLayout(topBar);
        topLayout_1->setObjectName("topLayout_1");
        topLayout_1->setContentsMargins(12, -1, 12, -1);
        titleLabel = new QLabel(topBar);
        titleLabel->setObjectName("titleLabel");

        topLayout_1->addWidget(titleLabel);

        spacerItem6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topLayout_1->addItem(spacerItem6);

        btnGotoAjout = new QPushButton(topBar);
        btnGotoAjout->setObjectName("btnGotoAjout");

        topLayout_1->addWidget(btnGotoAjout);

        userLabel = new QLabel(topBar);
        userLabel->setObjectName("userLabel");

        topLayout_1->addWidget(userLabel);

        btnNotif = new QPushButton(topBar);
        btnNotif->setObjectName("btnNotif");

        topLayout_1->addWidget(btnNotif);

        btnProfil = new QPushButton(topBar);
        btnProfil->setObjectName("btnProfil");

        topLayout_1->addWidget(btnProfil);


        horizontalLayout_1->addWidget(topBar);


        vlScrollMaint->addLayout(horizontalLayout_1);

        contentLayout_1 = new QHBoxLayout();
        contentLayout_1->setSpacing(16);
        contentLayout_1->setObjectName("contentLayout_1");
        contentLayout_1->setContentsMargins(0, 0, 0, 0);
        tableLayout = new QVBoxLayout();
        tableLayout->setObjectName("tableLayout");
        searchRow = new QHBoxLayout();
        searchRow->setObjectName("searchRow");
        lblSort = new QLabel(scrollContentMaint);
        lblSort->setObjectName("lblSort");

        searchRow->addWidget(lblSort);

        cbSort1 = new QComboBox(scrollContentMaint);
        cbSort1->addItem(QString());
        cbSort1->addItem(QString());
        cbSort1->addItem(QString());
        cbSort1->addItem(QString());
        cbSort1->addItem(QString());
        cbSort1->setObjectName("cbSort1");

        searchRow->addWidget(cbSort1);

        searchInput = new QLineEdit(scrollContentMaint);
        searchInput->setObjectName("searchInput");

        searchRow->addWidget(searchInput);

        btnGotoModifier = new QPushButton(scrollContentMaint);
        btnGotoModifier->setObjectName("btnGotoModifier");

        searchRow->addWidget(btnGotoModifier);


        tableLayout->addLayout(searchRow);

        card = new QFrame(scrollContentMaint);
        card->setObjectName("card");
        vl_card_dash = new QVBoxLayout(card);
        vl_card_dash->setObjectName("vl_card_dash");
        lblCardTitle = new QLabel(card);
        lblCardTitle->setObjectName("lblCardTitle");
        lblCardTitle->setStyleSheet(QString::fromUtf8("color:#333333;"));

        vl_card_dash->addWidget(lblCardTitle);

        tableMaintenance = new QTableWidget(card);
        if (tableMaintenance->columnCount() < 7)
            tableMaintenance->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(0, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(1, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(2, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(3, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(4, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(5, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        tableMaintenance->setHorizontalHeaderItem(6, __qtablewidgetitem51);
        tableMaintenance->setObjectName("tableMaintenance");
        tableMaintenance->setColumnCount(7);
        tableMaintenance->verticalHeader()->setVisible(false);

        vl_card_dash->addWidget(tableMaintenance);


        tableLayout->addWidget(card);


        contentLayout_1->addLayout(tableLayout);

        rightSidebar = new QFrame(scrollContentMaint);
        rightSidebar->setObjectName("rightSidebar");
        rightSidebar->setMinimumSize(QSize(300, 0));
        rightSidebar->setMaximumSize(QSize(350, 16777215));
        rightLayout_1 = new QVBoxLayout(rightSidebar);
        rightLayout_1->setSpacing(14);
        rightLayout_1->setObjectName("rightLayout_1");
        rightLayout_1->setContentsMargins(8, 8, 8, 8);
        lblFunc = new QLabel(rightSidebar);
        lblFunc->setObjectName("lblFunc");

        rightLayout_1->addWidget(lblFunc);

        stat1 = new QFrame(rightSidebar);
        stat1->setObjectName("stat1");
        vlStat1 = new QVBoxLayout(stat1);
        vlStat1->setSpacing(4);
        vlStat1->setObjectName("vlStat1");
        st_icon1 = new QLabel(stat1);
        st_icon1->setObjectName("st_icon1");

        vlStat1->addWidget(st_icon1);

        st_val1 = new QLabel(stat1);
        st_val1->setObjectName("st_val1");

        vlStat1->addWidget(st_val1);

        st_lbl1 = new QLabel(stat1);
        st_lbl1->setObjectName("st_lbl1");

        vlStat1->addWidget(st_lbl1);


        rightLayout_1->addWidget(stat1);

        stat2 = new QFrame(rightSidebar);
        stat2->setObjectName("stat2");
        vlStat2 = new QVBoxLayout(stat2);
        vlStat2->setSpacing(4);
        vlStat2->setObjectName("vlStat2");
        st_icon2 = new QLabel(stat2);
        st_icon2->setObjectName("st_icon2");

        vlStat2->addWidget(st_icon2);

        st_val2 = new QLabel(stat2);
        st_val2->setObjectName("st_val2");

        vlStat2->addWidget(st_val2);

        st_lbl2 = new QLabel(stat2);
        st_lbl2->setObjectName("st_lbl2");

        vlStat2->addWidget(st_lbl2);


        rightLayout_1->addWidget(stat2);

        stat3 = new QFrame(rightSidebar);
        stat3->setObjectName("stat3");
        vlStat3 = new QVBoxLayout(stat3);
        vlStat3->setSpacing(4);
        vlStat3->setObjectName("vlStat3");
        st_icon3 = new QLabel(stat3);
        st_icon3->setObjectName("st_icon3");

        vlStat3->addWidget(st_icon3);

        st_val3 = new QLabel(stat3);
        st_val3->setObjectName("st_val3");

        vlStat3->addWidget(st_val3);

        st_lbl3 = new QLabel(stat3);
        st_lbl3->setObjectName("st_lbl3");

        vlStat3->addWidget(st_lbl3);


        rightLayout_1->addWidget(stat3);

        spacerItem7 = new QSpacerItem(0, 6, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        rightLayout_1->addItem(spacerItem7);

        btnGoStats_Maint = new QPushButton(rightSidebar);
        btnGoStats_Maint->setObjectName("btnGoStats_Maint");

        rightLayout_1->addWidget(btnGoStats_Maint);

        lblExport = new QLabel(rightSidebar);
        lblExport->setObjectName("lblExport");
        lblExport->setMaximumSize(QSize(0, 0));
        lblExport->setVisible(false);

        rightLayout_1->addWidget(lblExport);

        btnPdf = new QPushButton(rightSidebar);
        btnPdf->setObjectName("btnPdf");

        rightLayout_1->addWidget(btnPdf);

        vSpacerBottom_Maint = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        rightLayout_1->addItem(vSpacerBottom_Maint);


        contentLayout_1->addWidget(rightSidebar);


        vlScrollMaint->addLayout(contentLayout_1);

        miniChartFrame = new QFrame(scrollContentMaint);
        miniChartFrame->setObjectName("miniChartFrame");
        vlMiniChart = new QVBoxLayout(miniChartFrame);
        vlMiniChart->setSpacing(10);
        vlMiniChart->setObjectName("vlMiniChart");
        lblMiniChartTitle = new QLabel(miniChartFrame);
        lblMiniChartTitle->setObjectName("lblMiniChartTitle");

        vlMiniChart->addWidget(lblMiniChartTitle);

        hlMiniBar1 = new QHBoxLayout();
        hlMiniBar1->setSpacing(8);
        hlMiniBar1->setObjectName("hlMiniBar1");
        lblMiniBar1 = new QLabel(miniChartFrame);
        lblMiniBar1->setObjectName("lblMiniBar1");
        lblMiniBar1->setMinimumSize(QSize(70, 0));

        hlMiniBar1->addWidget(lblMiniBar1);

        pbMini1 = new QProgressBar(miniChartFrame);
        pbMini1->setObjectName("pbMini1");
        pbMini1->setValue(55);
        pbMini1->setTextVisible(false);

        hlMiniBar1->addWidget(pbMini1);

        lblMiniPct1 = new QLabel(miniChartFrame);
        lblMiniPct1->setObjectName("lblMiniPct1");
        lblMiniPct1->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        hlMiniBar1->addWidget(lblMiniPct1);


        vlMiniChart->addLayout(hlMiniBar1);

        hlMiniBar2 = new QHBoxLayout();
        hlMiniBar2->setSpacing(8);
        hlMiniBar2->setObjectName("hlMiniBar2");
        lblMiniBar2 = new QLabel(miniChartFrame);
        lblMiniBar2->setObjectName("lblMiniBar2");
        lblMiniBar2->setMinimumSize(QSize(70, 0));

        hlMiniBar2->addWidget(lblMiniBar2);

        pbMini2 = new QProgressBar(miniChartFrame);
        pbMini2->setObjectName("pbMini2");
        pbMini2->setValue(30);
        pbMini2->setTextVisible(false);

        hlMiniBar2->addWidget(pbMini2);

        lblMiniPct2 = new QLabel(miniChartFrame);
        lblMiniPct2->setObjectName("lblMiniPct2");
        lblMiniPct2->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        hlMiniBar2->addWidget(lblMiniPct2);


        vlMiniChart->addLayout(hlMiniBar2);

        hlMiniBar3 = new QHBoxLayout();
        hlMiniBar3->setSpacing(8);
        hlMiniBar3->setObjectName("hlMiniBar3");
        lblMiniBar3 = new QLabel(miniChartFrame);
        lblMiniBar3->setObjectName("lblMiniBar3");
        lblMiniBar3->setMinimumSize(QSize(70, 0));

        hlMiniBar3->addWidget(lblMiniBar3);

        pbMini3 = new QProgressBar(miniChartFrame);
        pbMini3->setObjectName("pbMini3");
        pbMini3->setValue(15);
        pbMini3->setTextVisible(false);

        hlMiniBar3->addWidget(pbMini3);

        lblMiniPct3 = new QLabel(miniChartFrame);
        lblMiniPct3->setObjectName("lblMiniPct3");
        lblMiniPct3->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        hlMiniBar3->addWidget(lblMiniPct3);


        vlMiniChart->addLayout(hlMiniBar3);


        vlScrollMaint->addWidget(miniChartFrame);

        scrollAreaMaint->setWidget(scrollContentMaint);

        vl_page_Maint_Dash->addWidget(scrollAreaMaint);

        stackedWidget_Maintenance->addWidget(page_Maint_Dash);
        page_Maint_Ajout = new QWidget();
        page_Maint_Ajout->setObjectName("page_Maint_Ajout");
        vl_page_Maint_Ajout = new QVBoxLayout(page_Maint_Ajout);
        vl_page_Maint_Ajout->setSpacing(20);
        vl_page_Maint_Ajout->setObjectName("vl_page_Maint_Ajout");
        vl_page_Maint_Ajout->setContentsMargins(20, 0, 20, 20);
        hlTopAjout = new QHBoxLayout();
        hlTopAjout->setObjectName("hlTopAjout");
        topBar_Aj = new QFrame(page_Maint_Ajout);
        topBar_Aj->setObjectName("topBar_Aj");
        hlTopInnerAjout = new QHBoxLayout(topBar_Aj);
        hlTopInnerAjout->setObjectName("hlTopInnerAjout");
        titleLabel_Aj = new QLabel(topBar_Aj);
        titleLabel_Aj->setObjectName("titleLabel_Aj");

        hlTopInnerAjout->addWidget(titleLabel_Aj);

        spacerItem8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hlTopInnerAjout->addItem(spacerItem8);

        userLabel_Aj = new QLabel(topBar_Aj);
        userLabel_Aj->setObjectName("userLabel_Aj");

        hlTopInnerAjout->addWidget(userLabel_Aj);


        hlTopAjout->addWidget(topBar_Aj);


        vl_page_Maint_Ajout->addLayout(hlTopAjout);

        vlAjoutRoot = new QVBoxLayout();
        vlAjoutRoot->setObjectName("vlAjoutRoot");
        scrollArea_MaintAjout = new QScrollArea(page_Maint_Ajout);
        scrollArea_MaintAjout->setObjectName("scrollArea_MaintAjout");
        scrollArea_MaintAjout->setFrameShape(QFrame::Shape::NoFrame);
        scrollArea_MaintAjout->setWidgetResizable(true);
        scrollContent_MaintAjout = new QWidget();
        scrollContent_MaintAjout->setObjectName("scrollContent_MaintAjout");
        scrollContent_MaintAjout->setGeometry(QRect(0, 0, 394, 1154));
        vlScrollMaintAjout = new QVBoxLayout(scrollContent_MaintAjout);
        vlScrollMaintAjout->setObjectName("vlScrollMaintAjout");
        vlScrollMaintAjout->setContentsMargins(20, -1, 20, 20);
        btnBack_Ajout = new QPushButton(scrollContent_MaintAjout);
        btnBack_Ajout->setObjectName("btnBack_Ajout");
        btnBack_Ajout->setMaximumSize(QSize(0, 0));
        btnBack_Ajout->setVisible(false);

        vlScrollMaintAjout->addWidget(btnBack_Ajout);

        cardAdd = new QFrame(scrollContent_MaintAjout);
        cardAdd->setObjectName("cardAdd");
        vlCardAdd = new QVBoxLayout(cardAdd);
        vlCardAdd->setSpacing(14);
        vlCardAdd->setObjectName("vlCardAdd");
        vlCardAdd->setContentsMargins(32, 28, 32, 28);
        lblTitreFormAdd = new QLabel(cardAdd);
        lblTitreFormAdd->setObjectName("lblTitreFormAdd");
        lblTitreFormAdd->setMinimumSize(QSize(0, 50));

        vlCardAdd->addWidget(lblTitreFormAdd);

        sepMaintAdd1 = new QFrame(cardAdd);
        sepMaintAdd1->setObjectName("sepMaintAdd1");
        sepMaintAdd1->setFrameShape(QFrame::Shape::HLine);
        sepMaintAdd1->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardAdd->addWidget(sepMaintAdd1);

        hlRefAdd = new QHBoxLayout();
        hlRefAdd->setObjectName("hlRefAdd");
        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setObjectName("vboxLayout3");
        lblRefAdd = new QLabel(cardAdd);
        lblRefAdd->setObjectName("lblRefAdd");

        vboxLayout3->addWidget(lblRefAdd);

        editRefAdd = new QLineEdit(cardAdd);
        editRefAdd->setObjectName("editRefAdd");

        vboxLayout3->addWidget(editRefAdd);


        hlRefAdd->addLayout(vboxLayout3);


        vlCardAdd->addLayout(hlRefAdd);

        hlDateDureeAdd = new QHBoxLayout();
        hlDateDureeAdd->setObjectName("hlDateDureeAdd");
        vboxLayout4 = new QVBoxLayout();
        vboxLayout4->setObjectName("vboxLayout4");
        lblDateAdd = new QLabel(cardAdd);
        lblDateAdd->setObjectName("lblDateAdd");

        vboxLayout4->addWidget(lblDateAdd);

        dateAdd = new QDateEdit(cardAdd);
        dateAdd->setObjectName("dateAdd");
        dateAdd->setCalendarPopup(true);

        vboxLayout4->addWidget(dateAdd);


        hlDateDureeAdd->addLayout(vboxLayout4);

        vboxLayout5 = new QVBoxLayout();
        vboxLayout5->setObjectName("vboxLayout5");
        lblDurAdd = new QLabel(cardAdd);
        lblDurAdd->setObjectName("lblDurAdd");

        vboxLayout5->addWidget(lblDurAdd);

        comboDurAdd = new QComboBox(cardAdd);
        comboDurAdd->addItem(QString());
        comboDurAdd->addItem(QString());
        comboDurAdd->addItem(QString());
        comboDurAdd->setObjectName("comboDurAdd");

        vboxLayout5->addWidget(comboDurAdd);


        hlDateDureeAdd->addLayout(vboxLayout5);


        vlCardAdd->addLayout(hlDateDureeAdd);

        hlCoutAdd = new QHBoxLayout();
        hlCoutAdd->setObjectName("hlCoutAdd");
        vboxLayout6 = new QVBoxLayout();
        vboxLayout6->setObjectName("vboxLayout6");
        lblCoutAdd = new QLabel(cardAdd);
        lblCoutAdd->setObjectName("lblCoutAdd");

        vboxLayout6->addWidget(lblCoutAdd);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        spinCoutAdd = new QDoubleSpinBox(cardAdd);
        spinCoutAdd->setObjectName("spinCoutAdd");
        spinCoutAdd->setDecimals(2);
        spinCoutAdd->setMinimum(0.000000000000000);
        spinCoutAdd->setMaximum(1000000.000000000000000);

        hboxLayout->addWidget(spinCoutAdd);

        lblEuroAdd = new QLabel(cardAdd);
        lblEuroAdd->setObjectName("lblEuroAdd");

        hboxLayout->addWidget(lblEuroAdd);


        vboxLayout6->addLayout(hboxLayout);


        hlCoutAdd->addLayout(vboxLayout6);


        vlCardAdd->addLayout(hlCoutAdd);

        hlPrioAdd = new QHBoxLayout();
        hlPrioAdd->setObjectName("hlPrioAdd");
        vboxLayout7 = new QVBoxLayout();
        vboxLayout7->setObjectName("vboxLayout7");
        lblPrioAdd = new QLabel(cardAdd);
        lblPrioAdd->setObjectName("lblPrioAdd");

        vboxLayout7->addWidget(lblPrioAdd);

        comboPrioAdd = new QComboBox(cardAdd);
        comboPrioAdd->addItem(QString());
        comboPrioAdd->addItem(QString());
        comboPrioAdd->addItem(QString());
        comboPrioAdd->setObjectName("comboPrioAdd");

        vboxLayout7->addWidget(comboPrioAdd);


        hlPrioAdd->addLayout(vboxLayout7);


        vlCardAdd->addLayout(hlPrioAdd);

        sepMaintAdd2 = new QFrame(cardAdd);
        sepMaintAdd2->setObjectName("sepMaintAdd2");
        sepMaintAdd2->setFrameShape(QFrame::Shape::HLine);
        sepMaintAdd2->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardAdd->addWidget(sepMaintAdd2);

        hlTechAddrAdd = new QHBoxLayout();
        hlTechAddrAdd->setObjectName("hlTechAddrAdd");
        vboxLayout8 = new QVBoxLayout();
        vboxLayout8->setObjectName("vboxLayout8");
        lblTechAdd = new QLabel(cardAdd);
        lblTechAdd->setObjectName("lblTechAdd");

        vboxLayout8->addWidget(lblTechAdd);

        editTechAdd = new QLineEdit(cardAdd);
        editTechAdd->setObjectName("editTechAdd");

        vboxLayout8->addWidget(editTechAdd);


        hlTechAddrAdd->addLayout(vboxLayout8);

        vboxLayout9 = new QVBoxLayout();
        vboxLayout9->setObjectName("vboxLayout9");
        lblAddrAdd = new QLabel(cardAdd);
        lblAddrAdd->setObjectName("lblAddrAdd");

        vboxLayout9->addWidget(lblAddrAdd);

        editAddrAdd = new QLineEdit(cardAdd);
        editAddrAdd->setObjectName("editAddrAdd");

        vboxLayout9->addWidget(editAddrAdd);


        hlTechAddrAdd->addLayout(vboxLayout9);


        vlCardAdd->addLayout(hlTechAddrAdd);

        sepMaintAdd3 = new QFrame(cardAdd);
        sepMaintAdd3->setObjectName("sepMaintAdd3");
        sepMaintAdd3->setFrameShape(QFrame::Shape::HLine);
        sepMaintAdd3->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardAdd->addWidget(sepMaintAdd3);

        vlDescAdd = new QVBoxLayout();
        vlDescAdd->setObjectName("vlDescAdd");
        lblDescAdd = new QLabel(cardAdd);
        lblDescAdd->setObjectName("lblDescAdd");

        vlDescAdd->addWidget(lblDescAdd);

        txtDescAdd = new QTextEdit(cardAdd);
        txtDescAdd->setObjectName("txtDescAdd");
        txtDescAdd->setMinimumSize(QSize(0, 80));
        txtDescAdd->setMaximumSize(QSize(16777215, 120));

        vlDescAdd->addWidget(txtDescAdd);


        vlCardAdd->addLayout(vlDescAdd);

        sepMaintAdd4 = new QFrame(cardAdd);
        sepMaintAdd4->setObjectName("sepMaintAdd4");
        sepMaintAdd4->setFrameShape(QFrame::Shape::HLine);
        sepMaintAdd4->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardAdd->addWidget(sepMaintAdd4);

        hlPhotosAdd = new QHBoxLayout();
        hlPhotosAdd->setSpacing(16);
        hlPhotosAdd->setObjectName("hlPhotosAdd");
        vboxLayout10 = new QVBoxLayout();
        vboxLayout10->setObjectName("vboxLayout10");
        lblPhotosAvantAdd = new QLabel(cardAdd);
        lblPhotosAvantAdd->setObjectName("lblPhotosAvantAdd");

        vboxLayout10->addWidget(lblPhotosAvantAdd);

        lblImgPreview_Add = new QLabel(cardAdd);
        lblImgPreview_Add->setObjectName("lblImgPreview_Add");
        lblImgPreview_Add->setMinimumSize(QSize(0, 186));
        lblImgPreview_Add->setAlignment(Qt::AlignmentFlag::AlignCenter);
        lblImgPreview_Add->setWordWrap(true);

        vboxLayout10->addWidget(lblImgPreview_Add);


        hlPhotosAdd->addLayout(vboxLayout10);

        vboxLayout11 = new QVBoxLayout();
        vboxLayout11->setObjectName("vboxLayout11");
        lblPhotosApresAdd = new QLabel(cardAdd);
        lblPhotosApresAdd->setObjectName("lblPhotosApresAdd");

        vboxLayout11->addWidget(lblPhotosApresAdd);

        lblImgPreview2_Add = new QLabel(cardAdd);
        lblImgPreview2_Add->setObjectName("lblImgPreview2_Add");
        lblImgPreview2_Add->setMinimumSize(QSize(0, 186));
        lblImgPreview2_Add->setAlignment(Qt::AlignmentFlag::AlignCenter);
        lblImgPreview2_Add->setWordWrap(true);

        vboxLayout11->addWidget(lblImgPreview2_Add);


        hlPhotosAdd->addLayout(vboxLayout11);


        vlCardAdd->addLayout(hlPhotosAdd);


        vlScrollMaintAjout->addWidget(cardAdd);

        hlButtonsAdd = new QHBoxLayout();
        hlButtonsAdd->setSpacing(16);
        hlButtonsAdd->setObjectName("hlButtonsAdd");
        spacerItem9 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hlButtonsAdd->addItem(spacerItem9);

        btnCancel_Add = new QPushButton(scrollContent_MaintAjout);
        btnCancel_Add->setObjectName("btnCancel_Add");
        btnCancel_Add->setMinimumSize(QSize(160, 48));
        btnCancel_Add->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        hlButtonsAdd->addWidget(btnCancel_Add);

        btnSave_Add = new QPushButton(scrollContent_MaintAjout);
        btnSave_Add->setObjectName("btnSave_Add");
        btnSave_Add->setMinimumSize(QSize(160, 48));
        btnSave_Add->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        hlButtonsAdd->addWidget(btnSave_Add);


        vlScrollMaintAjout->addLayout(hlButtonsAdd);

        scrollArea_MaintAjout->setWidget(scrollContent_MaintAjout);

        vlAjoutRoot->addWidget(scrollArea_MaintAjout);


        vl_page_Maint_Ajout->addLayout(vlAjoutRoot);

        stackedWidget_Maintenance->addWidget(page_Maint_Ajout);
        page_Maint_Modif = new QWidget();
        page_Maint_Modif->setObjectName("page_Maint_Modif");
        vl_page_Maint_Modif = new QVBoxLayout(page_Maint_Modif);
        vl_page_Maint_Modif->setSpacing(20);
        vl_page_Maint_Modif->setObjectName("vl_page_Maint_Modif");
        vl_page_Maint_Modif->setContentsMargins(20, 0, 20, 20);
        hlTopMod = new QHBoxLayout();
        hlTopMod->setObjectName("hlTopMod");
        topBar_Mod = new QFrame(page_Maint_Modif);
        topBar_Mod->setObjectName("topBar_Mod");
        hlTopInnerMod = new QHBoxLayout(topBar_Mod);
        hlTopInnerMod->setObjectName("hlTopInnerMod");
        titleLabel_Mod = new QLabel(topBar_Mod);
        titleLabel_Mod->setObjectName("titleLabel_Mod");

        hlTopInnerMod->addWidget(titleLabel_Mod);

        spacerItem10 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hlTopInnerMod->addItem(spacerItem10);

        userLabel_Mod = new QLabel(topBar_Mod);
        userLabel_Mod->setObjectName("userLabel_Mod");

        hlTopInnerMod->addWidget(userLabel_Mod);


        hlTopMod->addWidget(topBar_Mod);


        vl_page_Maint_Modif->addLayout(hlTopMod);

        vlModRoot = new QVBoxLayout();
        vlModRoot->setObjectName("vlModRoot");
        scrollArea_MaintModif = new QScrollArea(page_Maint_Modif);
        scrollArea_MaintModif->setObjectName("scrollArea_MaintModif");
        scrollArea_MaintModif->setFrameShape(QFrame::Shape::NoFrame);
        scrollArea_MaintModif->setWidgetResizable(true);
        scrollContent_MaintModif = new QWidget();
        scrollContent_MaintModif->setObjectName("scrollContent_MaintModif");
        scrollContent_MaintModif->setGeometry(QRect(0, 0, 439, 1277));
        vlScrollMaintModif = new QVBoxLayout(scrollContent_MaintModif);
        vlScrollMaintModif->setObjectName("vlScrollMaintModif");
        vlScrollMaintModif->setContentsMargins(20, -1, 20, 20);
        btnBack_Modif = new QPushButton(scrollContent_MaintModif);
        btnBack_Modif->setObjectName("btnBack_Modif");
        btnBack_Modif->setMaximumSize(QSize(0, 0));
        btnBack_Modif->setVisible(false);

        vlScrollMaintModif->addWidget(btnBack_Modif);

        cardMod = new QFrame(scrollContent_MaintModif);
        cardMod->setObjectName("cardMod");
        vlCardMod = new QVBoxLayout(cardMod);
        vlCardMod->setSpacing(14);
        vlCardMod->setObjectName("vlCardMod");
        vlCardMod->setContentsMargins(32, 28, 32, 28);
        lblTitreFormMod = new QLabel(cardMod);
        lblTitreFormMod->setObjectName("lblTitreFormMod");

        vlCardMod->addWidget(lblTitreFormMod);

        sepMaintMod1 = new QFrame(cardMod);
        sepMaintMod1->setObjectName("sepMaintMod1");
        sepMaintMod1->setFrameShape(QFrame::Shape::HLine);
        sepMaintMod1->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardMod->addWidget(sepMaintMod1);

        hlRefMod = new QHBoxLayout();
        hlRefMod->setObjectName("hlRefMod");
        vboxLayout12 = new QVBoxLayout();
        vboxLayout12->setObjectName("vboxLayout12");
        lblRefMod = new QLabel(cardMod);
        lblRefMod->setObjectName("lblRefMod");

        vboxLayout12->addWidget(lblRefMod);

        editRefMod = new QLineEdit(cardMod);
        editRefMod->setObjectName("editRefMod");

        vboxLayout12->addWidget(editRefMod);


        hlRefMod->addLayout(vboxLayout12);


        vlCardMod->addLayout(hlRefMod);

        hlDateDureeMod = new QHBoxLayout();
        hlDateDureeMod->setObjectName("hlDateDureeMod");
        vboxLayout13 = new QVBoxLayout();
        vboxLayout13->setObjectName("vboxLayout13");
        lblDateMod = new QLabel(cardMod);
        lblDateMod->setObjectName("lblDateMod");

        vboxLayout13->addWidget(lblDateMod);

        dateMod = new QDateEdit(cardMod);
        dateMod->setObjectName("dateMod");
        dateMod->setCalendarPopup(true);

        vboxLayout13->addWidget(dateMod);


        hlDateDureeMod->addLayout(vboxLayout13);

        vboxLayout14 = new QVBoxLayout();
        vboxLayout14->setObjectName("vboxLayout14");
        lblDurMod = new QLabel(cardMod);
        lblDurMod->setObjectName("lblDurMod");

        vboxLayout14->addWidget(lblDurMod);

        comboDurMod = new QComboBox(cardMod);
        comboDurMod->addItem(QString());
        comboDurMod->addItem(QString());
        comboDurMod->addItem(QString());
        comboDurMod->setObjectName("comboDurMod");

        vboxLayout14->addWidget(comboDurMod);


        hlDateDureeMod->addLayout(vboxLayout14);


        vlCardMod->addLayout(hlDateDureeMod);

        hlCoutMod = new QHBoxLayout();
        hlCoutMod->setObjectName("hlCoutMod");
        vboxLayout15 = new QVBoxLayout();
        vboxLayout15->setObjectName("vboxLayout15");
        lblCoutMod = new QLabel(cardMod);
        lblCoutMod->setObjectName("lblCoutMod");

        vboxLayout15->addWidget(lblCoutMod);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName("hboxLayout1");
        spinCoutMod = new QDoubleSpinBox(cardMod);
        spinCoutMod->setObjectName("spinCoutMod");
        spinCoutMod->setDecimals(2);
        spinCoutMod->setMinimum(0.000000000000000);
        spinCoutMod->setMaximum(1000000.000000000000000);

        hboxLayout1->addWidget(spinCoutMod);

        lblEuroMod = new QLabel(cardMod);
        lblEuroMod->setObjectName("lblEuroMod");

        hboxLayout1->addWidget(lblEuroMod);


        vboxLayout15->addLayout(hboxLayout1);


        hlCoutMod->addLayout(vboxLayout15);


        vlCardMod->addLayout(hlCoutMod);

        hlPrioMod = new QHBoxLayout();
        hlPrioMod->setObjectName("hlPrioMod");
        vboxLayout16 = new QVBoxLayout();
        vboxLayout16->setObjectName("vboxLayout16");
        lblPrioMod = new QLabel(cardMod);
        lblPrioMod->setObjectName("lblPrioMod");

        vboxLayout16->addWidget(lblPrioMod);

        comboPrioMod = new QComboBox(cardMod);
        comboPrioMod->addItem(QString());
        comboPrioMod->addItem(QString());
        comboPrioMod->addItem(QString());
        comboPrioMod->setObjectName("comboPrioMod");

        vboxLayout16->addWidget(comboPrioMod);


        hlPrioMod->addLayout(vboxLayout16);


        vlCardMod->addLayout(hlPrioMod);

        sepMaintMod2 = new QFrame(cardMod);
        sepMaintMod2->setObjectName("sepMaintMod2");
        sepMaintMod2->setFrameShape(QFrame::Shape::HLine);
        sepMaintMod2->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardMod->addWidget(sepMaintMod2);

        lblSecTechMod = new QLabel(cardMod);
        lblSecTechMod->setObjectName("lblSecTechMod");

        vlCardMod->addWidget(lblSecTechMod);

        hlTechAddrMod = new QHBoxLayout();
        hlTechAddrMod->setObjectName("hlTechAddrMod");
        vboxLayout17 = new QVBoxLayout();
        vboxLayout17->setObjectName("vboxLayout17");
        lblTechMod = new QLabel(cardMod);
        lblTechMod->setObjectName("lblTechMod");

        vboxLayout17->addWidget(lblTechMod);

        editTechMod = new QLineEdit(cardMod);
        editTechMod->setObjectName("editTechMod");

        vboxLayout17->addWidget(editTechMod);


        hlTechAddrMod->addLayout(vboxLayout17);

        vboxLayout18 = new QVBoxLayout();
        vboxLayout18->setObjectName("vboxLayout18");
        lblAddrMod = new QLabel(cardMod);
        lblAddrMod->setObjectName("lblAddrMod");

        vboxLayout18->addWidget(lblAddrMod);

        editAddrMod = new QLineEdit(cardMod);
        editAddrMod->setObjectName("editAddrMod");

        vboxLayout18->addWidget(editAddrMod);


        hlTechAddrMod->addLayout(vboxLayout18);


        vlCardMod->addLayout(hlTechAddrMod);

        sepMaintMod3 = new QFrame(cardMod);
        sepMaintMod3->setObjectName("sepMaintMod3");
        sepMaintMod3->setFrameShape(QFrame::Shape::HLine);
        sepMaintMod3->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardMod->addWidget(sepMaintMod3);

        lblSecDescMod = new QLabel(cardMod);
        lblSecDescMod->setObjectName("lblSecDescMod");

        vlCardMod->addWidget(lblSecDescMod);

        vlDescMod = new QVBoxLayout();
        vlDescMod->setObjectName("vlDescMod");
        lblDescMod = new QLabel(cardMod);
        lblDescMod->setObjectName("lblDescMod");
        lblDescMod->setMaximumSize(QSize(0, 0));
        lblDescMod->setVisible(false);

        vlDescMod->addWidget(lblDescMod);

        txtDescMod = new QTextEdit(cardMod);
        txtDescMod->setObjectName("txtDescMod");
        txtDescMod->setMinimumSize(QSize(0, 80));
        txtDescMod->setMaximumSize(QSize(16777215, 120));

        vlDescMod->addWidget(txtDescMod);


        vlCardMod->addLayout(vlDescMod);

        sepMaintMod4 = new QFrame(cardMod);
        sepMaintMod4->setObjectName("sepMaintMod4");
        sepMaintMod4->setFrameShape(QFrame::Shape::HLine);
        sepMaintMod4->setFrameShadow(QFrame::Shadow::Sunken);

        vlCardMod->addWidget(sepMaintMod4);

        lblSecPhotoMod = new QLabel(cardMod);
        lblSecPhotoMod->setObjectName("lblSecPhotoMod");

        vlCardMod->addWidget(lblSecPhotoMod);

        hlPhotosMod = new QHBoxLayout();
        hlPhotosMod->setObjectName("hlPhotosMod");
        vboxLayout19 = new QVBoxLayout();
        vboxLayout19->setObjectName("vboxLayout19");
        lblPhotosAvantMod = new QLabel(cardMod);
        lblPhotosAvantMod->setObjectName("lblPhotosAvantMod");
        lblPhotosAvantMod->setMaximumSize(QSize(0, 0));
        lblPhotosAvantMod->setVisible(false);

        vboxLayout19->addWidget(lblPhotosAvantMod);

        lblImgPreview_Mod = new QLabel(cardMod);
        lblImgPreview_Mod->setObjectName("lblImgPreview_Mod");
        lblImgPreview_Mod->setMinimumSize(QSize(0, 166));
        lblImgPreview_Mod->setAlignment(Qt::AlignmentFlag::AlignCenter);

        vboxLayout19->addWidget(lblImgPreview_Mod);


        hlPhotosMod->addLayout(vboxLayout19);


        vlCardMod->addLayout(hlPhotosMod);

        lblLastUpdate = new QLabel(cardMod);
        lblLastUpdate->setObjectName("lblLastUpdate");

        vlCardMod->addWidget(lblLastUpdate);


        vlScrollMaintModif->addWidget(cardMod);

        hlButtonsMod = new QHBoxLayout();
        hlButtonsMod->setSpacing(16);
        hlButtonsMod->setObjectName("hlButtonsMod");
        spacerItem11 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hlButtonsMod->addItem(spacerItem11);

        btnCancel_Mod = new QPushButton(scrollContent_MaintModif);
        btnCancel_Mod->setObjectName("btnCancel_Mod");
        btnCancel_Mod->setMinimumSize(QSize(160, 48));
        btnCancel_Mod->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        hlButtonsMod->addWidget(btnCancel_Mod);

        btnSave_Mod = new QPushButton(scrollContent_MaintModif);
        btnSave_Mod->setObjectName("btnSave_Mod");
        btnSave_Mod->setMinimumSize(QSize(160, 48));
        btnSave_Mod->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        hlButtonsMod->addWidget(btnSave_Mod);


        vlScrollMaintModif->addLayout(hlButtonsMod);

        scrollArea_MaintModif->setWidget(scrollContent_MaintModif);

        vlModRoot->addWidget(scrollArea_MaintModif);


        vl_page_Maint_Modif->addLayout(vlModRoot);

        stackedWidget_Maintenance->addWidget(page_Maint_Modif);

        vl_Maint_Root->addWidget(stackedWidget_Maintenance);

        stackedWidget->addWidget(page_Maintenance_Tab);
        pageClient = new QWidget();
        pageClient->setObjectName("pageClient");
        verticalLayout_ClientRoot = new QVBoxLayout(pageClient);
        verticalLayout_ClientRoot->setSpacing(0);
        verticalLayout_ClientRoot->setObjectName("verticalLayout_ClientRoot");
        header_Client = new QWidget(pageClient);
        header_Client->setObjectName("header_Client");
        header_Client->setMinimumSize(QSize(0, 72));
        header_Client->setMaximumSize(QSize(16777215, 72));
        header_Client->setStyleSheet(QString::fromUtf8("background-color: #ffffff; border: none; border-bottom: 1px solid #dce1e6;"));
        horizontalLayout_header_Client = new QHBoxLayout(header_Client);
        horizontalLayout_header_Client->setObjectName("horizontalLayout_header_Client");
        wasteguardhead_Client = new QLabel(header_Client);
        wasteguardhead_Client->setObjectName("wasteguardhead_Client");

        horizontalLayout_header_Client->addWidget(wasteguardhead_Client);

        horizontalSpacer_h_Client = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_header_Client->addItem(horizontalSpacer_h_Client);

        btnNouveau_Client = new QPushButton(header_Client);
        btnNouveau_Client->setObjectName("btnNouveau_Client");
        btnNouveau_Client->setMinimumSize(QSize(120, 30));
        btnNouveau_Client->setStyleSheet(QString::fromUtf8("background-color: #0f2b4c; color: white; border-radius: 5px; font-weight: bold;"));

        horizontalLayout_header_Client->addWidget(btnNouveau_Client);

        responsable_Client = new QLabel(header_Client);
        responsable_Client->setObjectName("responsable_Client");

        horizontalLayout_header_Client->addWidget(responsable_Client);


        verticalLayout_ClientRoot->addWidget(header_Client);

        stackedWidget_Client = new QStackedWidget(pageClient);
        stackedWidget_Client->setObjectName("stackedWidget_Client");
        page_repertoire = new QWidget();
        page_repertoire->setObjectName("page_repertoire");
        verticalLayout_repertoire = new QVBoxLayout(page_repertoire);
        verticalLayout_repertoire->setObjectName("verticalLayout_repertoire");
        horizontalLayout_filters = new QHBoxLayout();
        horizontalLayout_filters->setObjectName("horizontalLayout_filters");
        horizontalLayout_filters->setContentsMargins(20, 10, 20, 6);
        recherche = new QLineEdit(page_repertoire);
        recherche->setObjectName("recherche");
        recherche->setMinimumSize(QSize(220, 34));
        recherche->setMaximumSize(QSize(350, 36));
        recherche->setStyleSheet(QString::fromUtf8("QLineEdit { border: 1px solid #d1d5db; border-radius: 8px; padding: 6px 12px; font-size: 13px; background: white; color: #333; } QLineEdit:focus { border-color: #2a5298; }"));

        horizontalLayout_filters->addWidget(recherche);

        spacer_filter_gap = new QSpacerItem(12, 0, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_filters->addItem(spacer_filter_gap);

        lblTrier = new QLabel(page_repertoire);
        lblTrier->setObjectName("lblTrier");
        lblTrier->setStyleSheet(QString::fromUtf8("font-weight: 600; color: #0f2b4c; font-size: 13px;"));

        horizontalLayout_filters->addWidget(lblTrier);

        cbTrier = new QComboBox(page_repertoire);
        cbTrier->addItem(QString());
        cbTrier->addItem(QString());
        cbTrier->addItem(QString());
        cbTrier->addItem(QString());
        cbTrier->setObjectName("cbTrier");
        cbTrier->setMinimumSize(QSize(160, 36));
        cbTrier->setStyleSheet(QString::fromUtf8("QComboBox { border: 1px solid #d1d5db; border-radius: 8px; padding: 6px 12px; font-size: 13px; background: white; color: #333; } QComboBox:focus { border-color: #2a5298; } QComboBox::drop-down { border: none; } QComboBox QAbstractItemView { border: 1px solid #d1d5db; border-radius: 4px; background: white; selection-background-color: #e0e7ff; }"));
        cbTrier->setEditable(true);

        horizontalLayout_filters->addWidget(cbTrier);

        horizontalSpacer_f = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_filters->addItem(horizontalSpacer_f);


        verticalLayout_repertoire->addLayout(horizontalLayout_filters);

        horizontalLayout_table = new QHBoxLayout();
        horizontalLayout_table->setObjectName("horizontalLayout_table");
        horizontalLayout_table->setContentsMargins(0, 0, 0, 0);
        tableWidget_Client = new QTableWidget(page_repertoire);
        if (tableWidget_Client->columnCount() < 7)
            tableWidget_Client->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(0, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(1, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(2, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(3, __qtablewidgetitem55);
        QTableWidgetItem *__qtablewidgetitem56 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(4, __qtablewidgetitem56);
        QTableWidgetItem *__qtablewidgetitem57 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(5, __qtablewidgetitem57);
        QTableWidgetItem *__qtablewidgetitem58 = new QTableWidgetItem();
        tableWidget_Client->setHorizontalHeaderItem(6, __qtablewidgetitem58);
        tableWidget_Client->setObjectName("tableWidget_Client");
        tableWidget_Client->setMinimumSize(QSize(0, 0));
        tableWidget_Client->setStyleSheet(QString::fromUtf8("color:rgb(0, 0, 0); font-size: 14px;"));

        horizontalLayout_table->addWidget(tableWidget_Client);

        sidepanel = new QWidget(page_repertoire);
        sidepanel->setObjectName("sidepanel");
        sidepanel->setMinimumSize(QSize(300, 0));
        sidepanel->setMaximumSize(QSize(350, 16777215));
        sidepanel->setStyleSheet(QString::fromUtf8("background-color: #f8f9fa; border: 1px solid #ddd; border-radius: 10px;"));
        verticalLayout_sidepanel = new QVBoxLayout(sidepanel);
        verticalLayout_sidepanel->setSpacing(10);
        verticalLayout_sidepanel->setObjectName("verticalLayout_sidepanel");
        verticalLayout_sidepanel->setContentsMargins(10, 2, 10, 10);
        pilotage = new QLabel(sidepanel);
        pilotage->setObjectName("pilotage");
        pilotage->setStyleSheet(QString::fromUtf8("font-weight: bold; color: black;"));

        verticalLayout_sidepanel->addWidget(pilotage);

        growthWidget = new QWidget(sidepanel);
        growthWidget->setObjectName("growthWidget");
        growthWidget->setStyleSheet(QString::fromUtf8("background-color: #e9ecef; border-radius: 5px; padding: 10px;"));
        vboxLayout20 = new QVBoxLayout(growthWidget);
        vboxLayout20->setObjectName("vboxLayout20");
        label1 = new QLabel(growthWidget);
        label1->setObjectName("label1");
        label1->setStyleSheet(QString::fromUtf8("color: green; font-size: 18px; font-weight: bold;"));

        vboxLayout20->addWidget(label1);

        label2 = new QLabel(growthWidget);
        label2->setObjectName("label2");
        label2->setStyleSheet(QString::fromUtf8("color: #666; font-size: 10px;"));

        vboxLayout20->addWidget(label2);


        verticalLayout_sidepanel->addWidget(growthWidget);

        spacerItem12 = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_sidepanel->addItem(spacerItem12);

        exportclient = new QPushButton(sidepanel);
        exportclient->setObjectName("exportclient");
        exportclient->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        exportclient->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);\n"
"color: white;\n"
"border: none;\n"
"border-radius: 12px;\n"
"padding: 14px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        verticalLayout_sidepanel->addWidget(exportclient);

        btnGoStats_Client = new QPushButton(sidepanel);
        btnGoStats_Client->setObjectName("btnGoStats_Client");
        btnGoStats_Client->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnGoStats_Client->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0f2b4c, stop:1 #2a5298);\n"
"color: white;\n"
"border: none;\n"
"border-radius: 12px;\n"
"padding: 14px 16px;\n"
"font-size: 14px;\n"
"font-weight: 700;\n"
"text-align: center;"));

        verticalLayout_sidepanel->addWidget(btnGoStats_Client);


        horizontalLayout_table->addWidget(sidepanel);


        verticalLayout_repertoire->addLayout(horizontalLayout_table);

        stackedWidget_Client->addWidget(page_repertoire);
        page_ajouter_client_wrapper = new QWidget();
        page_ajouter_client_wrapper->setObjectName("page_ajouter_client_wrapper");
        verticalLayout_ajouter_client_wrapper = new QVBoxLayout(page_ajouter_client_wrapper);
        verticalLayout_ajouter_client_wrapper->setObjectName("verticalLayout_ajouter_client_wrapper");
        vs_a1_client = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_ajouter_client_wrapper->addItem(vs_a1_client);

        formCard_ajouter = new QFrame(page_ajouter_client_wrapper);
        formCard_ajouter->setObjectName("formCard_ajouter");
        formCard_ajouter->setStyleSheet(QString::fromUtf8("\n"
"                   QFrame#formCard_ajouter { background-color: white; border-radius: 15px; border: 1px solid #e0e0e0; }\n"
"                   QLabel { color: #333; font-size: 14px; font-weight: bold; }\n"
"                   QLineEdit, QComboBox, QSpinBox { padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px; background-color: #fafafa; color: black; }\n"
"                   QPushButton#btn_save_ajouter { background-color: #28a745; color: white; font-weight: bold; border-radius: 5px; padding: 12px; }\n"
"                   QPushButton#btn_annuler_ajouter { background-color: #6c757d; color: white; font-weight: bold; border-radius: 5px; padding: 12px; }\n"
"                  "));
        vboxLayout21 = new QVBoxLayout(formCard_ajouter);
        vboxLayout21->setSpacing(20);
        vboxLayout21->setObjectName("vboxLayout21");
        vboxLayout21->setContentsMargins(40, 30, 40, 30);
        label3 = new QLabel(formCard_ajouter);
        label3->setObjectName("label3");
        label3->setStyleSheet(QString::fromUtf8("font-size: 20px; color: #1A365D;"));
        label3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        vboxLayout21->addWidget(label3);

        grid_ajouter = new QGridLayout();
        grid_ajouter->setObjectName("grid_ajouter");
        grid_ajouter->setVerticalSpacing(20);
        lbl_matricule_a = new QLabel(formCard_ajouter);
        lbl_matricule_a->setObjectName("lbl_matricule_a");

        grid_ajouter->addWidget(lbl_matricule_a, 0, 0, 1, 1);

        input_matricule_ajouter = new QLineEdit(formCard_ajouter);
        input_matricule_ajouter->setObjectName("input_matricule_ajouter");

        grid_ajouter->addWidget(input_matricule_ajouter, 0, 1, 1, 1);

        lbl_nom_a = new QLabel(formCard_ajouter);
        lbl_nom_a->setObjectName("lbl_nom_a");

        grid_ajouter->addWidget(lbl_nom_a, 0, 2, 1, 1);

        input_nom_ajouter = new QLineEdit(formCard_ajouter);
        input_nom_ajouter->setObjectName("input_nom_ajouter");

        grid_ajouter->addWidget(input_nom_ajouter, 0, 3, 1, 1);

        lbl_email_a = new QLabel(formCard_ajouter);
        lbl_email_a->setObjectName("lbl_email_a");

        grid_ajouter->addWidget(lbl_email_a, 1, 0, 1, 1);

        input_email_ajouter = new QLineEdit(formCard_ajouter);
        input_email_ajouter->setObjectName("input_email_ajouter");

        grid_ajouter->addWidget(input_email_ajouter, 1, 1, 1, 1);

        lbl_contrat_a = new QLabel(formCard_ajouter);
        lbl_contrat_a->setObjectName("lbl_contrat_a");

        grid_ajouter->addWidget(lbl_contrat_a, 1, 2, 1, 1);

        input_contrat_ajouter = new QComboBox(formCard_ajouter);
        input_contrat_ajouter->addItem(QString());
        input_contrat_ajouter->addItem(QString());
        input_contrat_ajouter->addItem(QString());
        input_contrat_ajouter->setObjectName("input_contrat_ajouter");

        grid_ajouter->addWidget(input_contrat_ajouter, 1, 3, 1, 1);

        lbl_paiement_a = new QLabel(formCard_ajouter);
        lbl_paiement_a->setObjectName("lbl_paiement_a");

        grid_ajouter->addWidget(lbl_paiement_a, 2, 0, 1, 1);

        input_paiement_ajouter = new QComboBox(formCard_ajouter);
        input_paiement_ajouter->addItem(QString());
        input_paiement_ajouter->addItem(QString());
        input_paiement_ajouter->setObjectName("input_paiement_ajouter");

        grid_ajouter->addWidget(input_paiement_ajouter, 2, 1, 1, 1);


        vboxLayout21->addLayout(grid_ajouter);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName("hboxLayout2");
        spacerItem13 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout2->addItem(spacerItem13);

        btn_annuler_ajouter = new QPushButton(formCard_ajouter);
        btn_annuler_ajouter->setObjectName("btn_annuler_ajouter");

        hboxLayout2->addWidget(btn_annuler_ajouter);

        btn_save_ajouter = new QPushButton(formCard_ajouter);
        btn_save_ajouter->setObjectName("btn_save_ajouter");

        hboxLayout2->addWidget(btn_save_ajouter);

        spacerItem14 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout2->addItem(spacerItem14);


        vboxLayout21->addLayout(hboxLayout2);


        verticalLayout_ajouter_client_wrapper->addWidget(formCard_ajouter);

        vs_a2_client = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_ajouter_client_wrapper->addItem(vs_a2_client);

        stackedWidget_Client->addWidget(page_ajouter_client_wrapper);
        page_modifier_client_wrapper = new QWidget();
        page_modifier_client_wrapper->setObjectName("page_modifier_client_wrapper");
        verticalLayout_modifier_client_wrapper = new QVBoxLayout(page_modifier_client_wrapper);
        verticalLayout_modifier_client_wrapper->setObjectName("verticalLayout_modifier_client_wrapper");
        vs_m1_client = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_modifier_client_wrapper->addItem(vs_m1_client);

        formCard_modifier = new QFrame(page_modifier_client_wrapper);
        formCard_modifier->setObjectName("formCard_modifier");
        formCard_modifier->setStyleSheet(QString::fromUtf8("\n"
"                   QFrame#formCard_modifier { background-color: white; border-radius: 15px; border: 1px solid #e0e0e0; }\n"
"                   QLabel { color: #333; font-size: 14px; font-weight: bold; }\n"
"                   QLineEdit, QComboBox, QSpinBox { padding: 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 14px; background-color: #fafafa; color: black; }\n"
"                   QPushButton#btn_save_modifier { background-color: #007bff; color: white; font-weight: bold; border-radius: 5px; padding: 12px; }\n"
"                   QPushButton#btn_annuler_modifier { background-color: #6c757d; color: white; font-weight: bold; border-radius: 5px; padding: 12px; }\n"
"                  "));
        vboxLayout22 = new QVBoxLayout(formCard_modifier);
        vboxLayout22->setSpacing(20);
        vboxLayout22->setObjectName("vboxLayout22");
        vboxLayout22->setContentsMargins(40, 30, 40, 30);
        label4 = new QLabel(formCard_modifier);
        label4->setObjectName("label4");
        label4->setStyleSheet(QString::fromUtf8("font-size: 20px; color: #1A365D;"));
        label4->setAlignment(Qt::AlignmentFlag::AlignCenter);

        vboxLayout22->addWidget(label4);

        grid_modifier = new QGridLayout();
        grid_modifier->setObjectName("grid_modifier");
        grid_modifier->setVerticalSpacing(20);
        lbl_matricule_m = new QLabel(formCard_modifier);
        lbl_matricule_m->setObjectName("lbl_matricule_m");

        grid_modifier->addWidget(lbl_matricule_m, 0, 0, 1, 1);

        input_matricule_modifier = new QLineEdit(formCard_modifier);
        input_matricule_modifier->setObjectName("input_matricule_modifier");

        grid_modifier->addWidget(input_matricule_modifier, 0, 1, 1, 1);

        lbl_nom_m = new QLabel(formCard_modifier);
        lbl_nom_m->setObjectName("lbl_nom_m");

        grid_modifier->addWidget(lbl_nom_m, 0, 2, 1, 1);

        input_nom_modifier = new QLineEdit(formCard_modifier);
        input_nom_modifier->setObjectName("input_nom_modifier");

        grid_modifier->addWidget(input_nom_modifier, 0, 3, 1, 1);

        lbl_email_m = new QLabel(formCard_modifier);
        lbl_email_m->setObjectName("lbl_email_m");

        grid_modifier->addWidget(lbl_email_m, 1, 0, 1, 1);

        input_email_modifier = new QLineEdit(formCard_modifier);
        input_email_modifier->setObjectName("input_email_modifier");

        grid_modifier->addWidget(input_email_modifier, 1, 1, 1, 1);

        lbl_contrat_m = new QLabel(formCard_modifier);
        lbl_contrat_m->setObjectName("lbl_contrat_m");

        grid_modifier->addWidget(lbl_contrat_m, 1, 2, 1, 1);

        input_contrat_modifier = new QComboBox(formCard_modifier);
        input_contrat_modifier->addItem(QString());
        input_contrat_modifier->addItem(QString());
        input_contrat_modifier->addItem(QString());
        input_contrat_modifier->setObjectName("input_contrat_modifier");

        grid_modifier->addWidget(input_contrat_modifier, 1, 3, 1, 1);

        lbl_paiement_m = new QLabel(formCard_modifier);
        lbl_paiement_m->setObjectName("lbl_paiement_m");

        grid_modifier->addWidget(lbl_paiement_m, 2, 0, 1, 1);

        input_paiement_modifier = new QComboBox(formCard_modifier);
        input_paiement_modifier->addItem(QString());
        input_paiement_modifier->addItem(QString());
        input_paiement_modifier->setObjectName("input_paiement_modifier");

        grid_modifier->addWidget(input_paiement_modifier, 2, 1, 1, 1);


        vboxLayout22->addLayout(grid_modifier);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setObjectName("hboxLayout3");
        spacerItem15 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout3->addItem(spacerItem15);

        btn_annuler_modifier = new QPushButton(formCard_modifier);
        btn_annuler_modifier->setObjectName("btn_annuler_modifier");

        hboxLayout3->addWidget(btn_annuler_modifier);

        btn_save_modifier = new QPushButton(formCard_modifier);
        btn_save_modifier->setObjectName("btn_save_modifier");

        hboxLayout3->addWidget(btn_save_modifier);

        spacerItem16 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout3->addItem(spacerItem16);


        vboxLayout22->addLayout(hboxLayout3);


        verticalLayout_modifier_client_wrapper->addWidget(formCard_modifier);

        vs_m2_client = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_modifier_client_wrapper->addItem(vs_m2_client);

        stackedWidget_Client->addWidget(page_modifier_client_wrapper);
        page_stats_client = new QWidget();
        page_stats_client->setObjectName("page_stats_client");
        vLayout_stats_client = new QVBoxLayout(page_stats_client);
        vLayout_stats_client->setObjectName("vLayout_stats_client");
        vLayout_stats_client->setContentsMargins(20, 15, 20, 10);
        hLayout_statsHeader_client = new QHBoxLayout();
        hLayout_statsHeader_client->setObjectName("hLayout_statsHeader_client");
        lblStatsTitle_client = new QLabel(page_stats_client);
        lblStatsTitle_client->setObjectName("lblStatsTitle_client");
        lblStatsTitle_client->setStyleSheet(QString::fromUtf8("font-size: 18px; font-weight: bold; color: #0f2b4c;"));

        hLayout_statsHeader_client->addWidget(lblStatsTitle_client);

        hs_statsClient = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hLayout_statsHeader_client->addItem(hs_statsClient);

        btnRetour_stats_client = new QPushButton(page_stats_client);
        btnRetour_stats_client->setObjectName("btnRetour_stats_client");
        btnRetour_stats_client->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnRetour_stats_client->setStyleSheet(QString::fromUtf8("background-color: #0f2b4c; color: white; border-radius: 8px; padding: 8px 18px; font-weight: bold; font-size: 13px;"));

        hLayout_statsHeader_client->addWidget(btnRetour_stats_client);


        vLayout_stats_client->addLayout(hLayout_statsHeader_client);

        hLayout_statsCharts_client = new QHBoxLayout();
        hLayout_statsCharts_client->setSpacing(15);
        hLayout_statsCharts_client->setObjectName("hLayout_statsCharts_client");
        chartEcoScorePie = new QChartView(page_stats_client);
        chartEcoScorePie->setObjectName("chartEcoScorePie");
        chartEcoScorePie->setMinimumSize(QSize(0, 320));

        hLayout_statsCharts_client->addWidget(chartEcoScorePie);

        chartEcoScoreBar = new QChartView(page_stats_client);
        chartEcoScoreBar->setObjectName("chartEcoScoreBar");
        chartEcoScoreBar->setMinimumSize(QSize(0, 320));

        hLayout_statsCharts_client->addWidget(chartEcoScoreBar);


        vLayout_stats_client->addLayout(hLayout_statsCharts_client);

        chartContratDist = new QChartView(page_stats_client);
        chartContratDist->setObjectName("chartContratDist");
        chartContratDist->setMinimumSize(QSize(0, 280));

        vLayout_stats_client->addWidget(chartContratDist);

        stackedWidget_Client->addWidget(page_stats_client);

        verticalLayout_ClientRoot->addWidget(stackedWidget_Client);

        stackedWidget->addWidget(pageClient);
        pageCmdDashboard = new QWidget();
        pageCmdDashboard->setObjectName("pageCmdDashboard");
        horizontalLayout_Main2 = new QHBoxLayout(pageCmdDashboard);
        horizontalLayout_Main2->setSpacing(0);
        horizontalLayout_Main2->setObjectName("horizontalLayout_Main2");
        horizontalLayout_Main2->setContentsMargins(0, 0, 0, 0);
        rightContentLayout = new QVBoxLayout();
        rightContentLayout->setSpacing(0);
        rightContentLayout->setObjectName("rightContentLayout");
        rightContentLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(4);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        horizontalLayout_11->setContentsMargins(16, 0, 10, 0);
        lb_1 = new QLabel(pageCmdDashboard);
        lb_1->setObjectName("lb_1");

        horizontalLayout_11->addWidget(lb_1);

        topFill_1 = new QFrame(pageCmdDashboard);
        topFill_1->setObjectName("topFill_1");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(topFill_1->sizePolicy().hasHeightForWidth());
        topFill_1->setSizePolicy(sizePolicy2);
        topFill_1->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout_11->addWidget(topFill_1);

        btnAddDashboard = new QPushButton(pageCmdDashboard);
        btnAddDashboard->setObjectName("btnAddDashboard");

        horizontalLayout_11->addWidget(btnAddDashboard);

        user_1 = new QLabel(pageCmdDashboard);
        user_1->setObjectName("user_1");

        horizontalLayout_11->addWidget(user_1);

        topGap_1 = new QFrame(pageCmdDashboard);
        topGap_1->setObjectName("topGap_1");
        topGap_1->setMinimumSize(QSize(2, 56));
        topGap_1->setMaximumSize(QSize(2, 56));
        topGap_1->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout_11->addWidget(topGap_1);

        btnnotif_1 = new QPushButton(pageCmdDashboard);
        btnnotif_1->setObjectName("btnnotif_1");
        btnnotif_1->setVisible(false);

        horizontalLayout_11->addWidget(btnnotif_1);

        btnprofil_1 = new QPushButton(pageCmdDashboard);
        btnprofil_1->setObjectName("btnprofil_1");
        btnprofil_1->setVisible(false);

        horizontalLayout_11->addWidget(btnprofil_1);


        rightContentLayout->addLayout(horizontalLayout_11);

        contentStack = new QStackedWidget(pageCmdDashboard);
        contentStack->setObjectName("contentStack");
        pageHome = new QWidget();
        pageHome->setObjectName("pageHome");
        pageHomeLayout = new QHBoxLayout(pageHome);
        pageHomeLayout->setSpacing(12);
        pageHomeLayout->setObjectName("pageHomeLayout");
        pageHomeLayout->setContentsMargins(0, 0, 0, 0);
        dashboardTableArea = new QVBoxLayout();
        dashboardTableArea->setObjectName("dashboardTableArea");
        searchRow_dashboard = new QHBoxLayout();
        searchRow_dashboard->setObjectName("searchRow_dashboard");
        spacerItem17 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        searchRow_dashboard->addItem(spacerItem17);

        lblSort_dashboard = new QLabel(pageHome);
        lblSort_dashboard->setObjectName("lblSort_dashboard");

        searchRow_dashboard->addWidget(lblSort_dashboard);

        cbSortDashboard = new QComboBox(pageHome);
        cbSortDashboard->addItem(QString());
        cbSortDashboard->addItem(QString());
        cbSortDashboard->addItem(QString());
        cbSortDashboard->addItem(QString());
        cbSortDashboard->addItem(QString());
        cbSortDashboard->addItem(QString());
        cbSortDashboard->setObjectName("cbSortDashboard");

        searchRow_dashboard->addWidget(cbSortDashboard);

        searchInputDashboard = new QLineEdit(pageHome);
        searchInputDashboard->setObjectName("searchInputDashboard");

        searchRow_dashboard->addWidget(searchInputDashboard);

        btnTempToModifier = new QPushButton(pageHome);
        btnTempToModifier->setObjectName("btnTempToModifier");

        searchRow_dashboard->addWidget(btnTempToModifier);


        dashboardTableArea->addLayout(searchRow_dashboard);

        tableDashboard = new QTableWidget(pageHome);
        if (tableDashboard->columnCount() < 9)
            tableDashboard->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem59 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(0, __qtablewidgetitem59);
        QTableWidgetItem *__qtablewidgetitem60 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(1, __qtablewidgetitem60);
        QTableWidgetItem *__qtablewidgetitem61 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(2, __qtablewidgetitem61);
        QTableWidgetItem *__qtablewidgetitem62 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(3, __qtablewidgetitem62);
        QTableWidgetItem *__qtablewidgetitem63 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(4, __qtablewidgetitem63);
        QTableWidgetItem *__qtablewidgetitem64 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(5, __qtablewidgetitem64);
        QTableWidgetItem *__qtablewidgetitem65 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(6, __qtablewidgetitem65);
        QTableWidgetItem *__qtablewidgetitem66 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(7, __qtablewidgetitem66);
        QTableWidgetItem *__qtablewidgetitem67 = new QTableWidgetItem();
        tableDashboard->setHorizontalHeaderItem(8, __qtablewidgetitem67);
        tableDashboard->setObjectName("tableDashboard");
        tableDashboard->setColumnCount(9);
        tableDashboard->verticalHeader()->setVisible(false);

        dashboardTableArea->addWidget(tableDashboard);


        pageHomeLayout->addLayout(dashboardTableArea);

        contentStack->addWidget(pageHome);
        pageCommandes = new QWidget();
        pageCommandes->setObjectName("pageCommandes");
        pageCommandesLayout = new QHBoxLayout(pageCommandes);
        pageCommandesLayout->setObjectName("pageCommandesLayout");
        tableLayout_2 = new QVBoxLayout();
        tableLayout_2->setObjectName("tableLayout_2");
        searchRow_2 = new QHBoxLayout();
        searchRow_2->setObjectName("searchRow_2");
        spacerItem18 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        searchRow_2->addItem(spacerItem18);

        lblSort_2 = new QLabel(pageCommandes);
        lblSort_2->setObjectName("lblSort_2");

        searchRow_2->addWidget(lblSort_2);

        cbSort_2 = new QComboBox(pageCommandes);
        cbSort_2->addItem(QString());
        cbSort_2->addItem(QString());
        cbSort_2->addItem(QString());
        cbSort_2->addItem(QString());
        cbSort_2->addItem(QString());
        cbSort_2->addItem(QString());
        cbSort_2->setObjectName("cbSort_2");

        searchRow_2->addWidget(cbSort_2);

        searchInput_2 = new QLineEdit(pageCommandes);
        searchInput_2->setObjectName("searchInput_2");

        searchRow_2->addWidget(searchInput_2);

        btnAddProduct_2 = new QPushButton(pageCommandes);
        btnAddProduct_2->setObjectName("btnAddProduct_2");

        searchRow_2->addWidget(btnAddProduct_2);


        tableLayout_2->addLayout(searchRow_2);

        tableProduits_2 = new QTableWidget(pageCommandes);
        if (tableProduits_2->columnCount() < 9)
            tableProduits_2->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem68 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(0, __qtablewidgetitem68);
        QTableWidgetItem *__qtablewidgetitem69 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(1, __qtablewidgetitem69);
        QTableWidgetItem *__qtablewidgetitem70 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(2, __qtablewidgetitem70);
        QTableWidgetItem *__qtablewidgetitem71 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(3, __qtablewidgetitem71);
        QTableWidgetItem *__qtablewidgetitem72 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(4, __qtablewidgetitem72);
        QTableWidgetItem *__qtablewidgetitem73 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(5, __qtablewidgetitem73);
        QTableWidgetItem *__qtablewidgetitem74 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(6, __qtablewidgetitem74);
        QTableWidgetItem *__qtablewidgetitem75 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(7, __qtablewidgetitem75);
        QTableWidgetItem *__qtablewidgetitem76 = new QTableWidgetItem();
        tableProduits_2->setHorizontalHeaderItem(8, __qtablewidgetitem76);
        tableProduits_2->setObjectName("tableProduits_2");
        tableProduits_2->setMinimumSize(QSize(900, 0));
        tableProduits_2->setColumnCount(9);
        tableProduits_2->verticalHeader()->setVisible(false);

        tableLayout_2->addWidget(tableProduits_2);


        pageCommandesLayout->addLayout(tableLayout_2);

        rightSidebar_Commande = new QFrame(pageCommandes);
        rightSidebar_Commande->setObjectName("rightSidebar_Commande");
        rightSidebar_Commande->setMinimumSize(QSize(300, 0));
        rightSidebar_Commande->setMaximumSize(QSize(350, 16777215));
        rightLayout_Cmd = new QVBoxLayout(rightSidebar_Commande);
        rightLayout_Cmd->setSpacing(10);
        rightLayout_Cmd->setObjectName("rightLayout_Cmd");
        rightLayout_Cmd->setContentsMargins(0, 0, 0, 0);
        lblFunc_Cmd = new QLabel(rightSidebar_Commande);
        lblFunc_Cmd->setObjectName("lblFunc_Cmd");

        rightLayout_Cmd->addWidget(lblFunc_Cmd);

        stat1_Cmd = new QFrame(rightSidebar_Commande);
        stat1_Cmd->setObjectName("stat1_Cmd");
        vboxLayout23 = new QVBoxLayout(stat1_Cmd);
        vboxLayout23->setObjectName("vboxLayout23");
        st_val1_Cmd = new QLabel(stat1_Cmd);
        st_val1_Cmd->setObjectName("st_val1_Cmd");

        vboxLayout23->addWidget(st_val1_Cmd);

        st_lbl1_Cmd = new QLabel(stat1_Cmd);
        st_lbl1_Cmd->setObjectName("st_lbl1_Cmd");

        vboxLayout23->addWidget(st_lbl1_Cmd);


        rightLayout_Cmd->addWidget(stat1_Cmd);

        stat2_Cmd = new QFrame(rightSidebar_Commande);
        stat2_Cmd->setObjectName("stat2_Cmd");
        vboxLayout24 = new QVBoxLayout(stat2_Cmd);
        vboxLayout24->setObjectName("vboxLayout24");
        st_val2_Cmd = new QLabel(stat2_Cmd);
        st_val2_Cmd->setObjectName("st_val2_Cmd");

        vboxLayout24->addWidget(st_val2_Cmd);

        st_lbl2_Cmd = new QLabel(stat2_Cmd);
        st_lbl2_Cmd->setObjectName("st_lbl2_Cmd");

        vboxLayout24->addWidget(st_lbl2_Cmd);


        rightLayout_Cmd->addWidget(stat2_Cmd);

        stat3_Cmd = new QFrame(rightSidebar_Commande);
        stat3_Cmd->setObjectName("stat3_Cmd");
        vboxLayout25 = new QVBoxLayout(stat3_Cmd);
        vboxLayout25->setObjectName("vboxLayout25");
        st_val3_Cmd = new QLabel(stat3_Cmd);
        st_val3_Cmd->setObjectName("st_val3_Cmd");

        vboxLayout25->addWidget(st_val3_Cmd);

        st_lbl3_Cmd = new QLabel(stat3_Cmd);
        st_lbl3_Cmd->setObjectName("st_lbl3_Cmd");

        vboxLayout25->addWidget(st_lbl3_Cmd);


        rightLayout_Cmd->addWidget(stat3_Cmd);

        spacerItem19 = new QSpacerItem(0, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        rightLayout_Cmd->addItem(spacerItem19);

        btnGoStats_Cmd = new QPushButton(rightSidebar_Commande);
        btnGoStats_Cmd->setObjectName("btnGoStats_Cmd");

        rightLayout_Cmd->addWidget(btnGoStats_Cmd);

        lblExport_Cmd = new QLabel(rightSidebar_Commande);
        lblExport_Cmd->setObjectName("lblExport_Cmd");
        lblExport_Cmd->setMaximumSize(QSize(0, 0));
        lblExport_Cmd->setVisible(false);

        rightLayout_Cmd->addWidget(lblExport_Cmd);

        btnPdf_Cmd = new QPushButton(rightSidebar_Commande);
        btnPdf_Cmd->setObjectName("btnPdf_Cmd");

        rightLayout_Cmd->addWidget(btnPdf_Cmd);


        pageCommandesLayout->addWidget(rightSidebar_Commande);

        contentStack->addWidget(pageCommandes);

        rightContentLayout->addWidget(contentStack);


        horizontalLayout_Main2->addLayout(rightContentLayout);

        stackedWidget->addWidget(pageCmdDashboard);
        pageCmdAjout = new QWidget();
        pageCmdAjout->setObjectName("pageCmdAjout");
        hbox_ajout = new QHBoxLayout(pageCmdAjout);
        hbox_ajout->setSpacing(0);
        hbox_ajout->setObjectName("hbox_ajout");
        hbox_ajout->setContentsMargins(0, 0, 0, 0);
        vbox_ajout_content = new QVBoxLayout();
        vbox_ajout_content->setSpacing(0);
        vbox_ajout_content->setObjectName("vbox_ajout_content");
        vbox_ajout_content->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(0);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        horizontalLayout_9->setContentsMargins(0, -1, 0, -1);
        lb_5 = new QLabel(pageCmdAjout);
        lb_5->setObjectName("lb_5");

        horizontalLayout_9->addWidget(lb_5);

        topFill_5 = new QFrame(pageCmdAjout);
        topFill_5->setObjectName("topFill_5");
        sizePolicy2.setHeightForWidth(topFill_5->sizePolicy().hasHeightForWidth());
        topFill_5->setSizePolicy(sizePolicy2);
        topFill_5->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout_9->addWidget(topFill_5);

        user_5 = new QLabel(pageCmdAjout);
        user_5->setObjectName("user_5");

        horizontalLayout_9->addWidget(user_5);

        topGap_5 = new QFrame(pageCmdAjout);
        topGap_5->setObjectName("topGap_5");
        topGap_5->setMinimumSize(QSize(10, 56));
        topGap_5->setMaximumSize(QSize(10, 56));
        topGap_5->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout_9->addWidget(topGap_5);

        btnnotif_5 = new QPushButton(pageCmdAjout);
        btnnotif_5->setObjectName("btnnotif_5");

        horizontalLayout_9->addWidget(btnnotif_5);

        btnprofil_5 = new QPushButton(pageCmdAjout);
        btnprofil_5->setObjectName("btnprofil_5");

        horizontalLayout_9->addWidget(btnprofil_5);


        vbox_ajout_content->addLayout(horizontalLayout_9);

        scrollArea_CmdAjout = new QScrollArea(pageCmdAjout);
        scrollArea_CmdAjout->setObjectName("scrollArea_CmdAjout");
        scrollArea_CmdAjout->setFrameShape(QFrame::Shape::NoFrame);
        scrollArea_CmdAjout->setWidgetResizable(true);
        scrollContent_CmdAjout = new QWidget();
        scrollContent_CmdAjout->setObjectName("scrollContent_CmdAjout");
        scrollContent_CmdAjout->setGeometry(QRect(0, 0, 690, 953));
        hl_mod_split_3 = new QHBoxLayout(scrollContent_CmdAjout);
        hl_mod_split_3->setSpacing(0);
        hl_mod_split_3->setObjectName("hl_mod_split_3");
        hl_mod_split_3->setContentsMargins(40, 25, 40, 30);
        cardMod_3 = new QFrame(scrollContent_CmdAjout);
        cardMod_3->setObjectName("cardMod_3");
        cardMod_3->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_Mod_3 = new QVBoxLayout(cardMod_3);
        verticalLayout_Mod_3->setSpacing(16);
        verticalLayout_Mod_3->setObjectName("verticalLayout_Mod_3");
        verticalLayout_Mod_3->setContentsMargins(32, 24, 32, 28);
        lblTitleMod_3 = new QLabel(cardMod_3);
        lblTitleMod_3->setObjectName("lblTitleMod_3");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(lblTitleMod_3->sizePolicy().hasHeightForWidth());
        lblTitleMod_3->setSizePolicy(sizePolicy3);
        lblTitleMod_3->setMinimumSize(QSize(0, 50));

        verticalLayout_Mod_3->addWidget(lblTitleMod_3);

        gl_inputs_4 = new QGridLayout();
        gl_inputs_4->setSpacing(20);
        gl_inputs_4->setObjectName("gl_inputs_4");
        dsb_price_add_4 = new QDoubleSpinBox(cardMod_3);
        dsb_price_add_4->setObjectName("dsb_price_add_4");
        dsb_price_add_4->setMaximum(99999.000000000000000);

        gl_inputs_4->addWidget(dsb_price_add_4, 1, 1, 1, 1);

        l_px_4 = new QLabel(cardMod_3);
        l_px_4->setObjectName("l_px_4");

        gl_inputs_4->addWidget(l_px_4, 1, 0, 1, 1);

        l1_4 = new QLabel(cardMod_3);
        l1_4->setObjectName("l1_4");

        gl_inputs_4->addWidget(l1_4, 0, 0, 1, 1);

        l_stat_4 = new QLabel(cardMod_3);
        l_stat_4->setObjectName("l_stat_4");

        gl_inputs_4->addWidget(l_stat_4, 2, 0, 1, 1);

        cb_client_add = new QComboBox(cardMod_3);
        cb_client_add->setObjectName("cb_client_add");

        gl_inputs_4->addWidget(cb_client_add, 0, 1, 1, 1);

        cb_model_add_4 = new QComboBox(cardMod_3);
        cb_model_add_4->addItem(QString());
        cb_model_add_4->addItem(QString());
        cb_model_add_4->addItem(QString());
        cb_model_add_4->addItem(QString());
        cb_model_add_4->setObjectName("cb_model_add_4");

        gl_inputs_4->addWidget(cb_model_add_4, 0, 3, 1, 1);

        cb_status_add_4 = new QComboBox(cardMod_3);
        cb_status_add_4->addItem(QString());
        cb_status_add_4->addItem(QString());
        cb_status_add_4->addItem(QString());
        cb_status_add_4->addItem(QString());
        cb_status_add_4->addItem(QString());
        cb_status_add_4->setObjectName("cb_status_add_4");

        gl_inputs_4->addWidget(cb_status_add_4, 2, 1, 1, 1);

        sb_qty_add_4 = new QSpinBox(cardMod_3);
        sb_qty_add_4->setObjectName("sb_qty_add_4");

        gl_inputs_4->addWidget(sb_qty_add_4, 1, 3, 1, 1);

        l_qty_4 = new QLabel(cardMod_3);
        l_qty_4->setObjectName("l_qty_4");

        gl_inputs_4->addWidget(l_qty_4, 1, 2, 1, 1);

        l2_4 = new QLabel(cardMod_3);
        l2_4->setObjectName("l2_4");

        gl_inputs_4->addWidget(l2_4, 0, 2, 1, 1);


        verticalLayout_Mod_3->addLayout(gl_inputs_4);

        spacerBeforeDates = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mod_3->addItem(spacerBeforeDates);

        separatorLine1 = new QFrame(cardMod_3);
        separatorLine1->setObjectName("separatorLine1");
        separatorLine1->setFrameShape(QFrame::Shape::HLine);
        separatorLine1->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_Mod_3->addWidget(separatorLine1);

        lblSectionDates = new QLabel(cardMod_3);
        lblSectionDates->setObjectName("lblSectionDates");

        verticalLayout_Mod_3->addWidget(lblSectionDates);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(12);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        label_27 = new QLabel(cardMod_3);
        label_27->setObjectName("label_27");

        horizontalLayout_10->addWidget(label_27);

        label_28 = new QLabel(cardMod_3);
        label_28->setObjectName("label_28");

        horizontalLayout_10->addWidget(label_28);

        comboBox_19 = new QComboBox(cardMod_3);
        comboBox_19->addItem(QString());
        comboBox_19->setObjectName("comboBox_19");

        horizontalLayout_10->addWidget(comboBox_19);

        label_29 = new QLabel(cardMod_3);
        label_29->setObjectName("label_29");

        horizontalLayout_10->addWidget(label_29);

        comboBox_20 = new QComboBox(cardMod_3);
        comboBox_20->addItem(QString());
        comboBox_20->setObjectName("comboBox_20");

        horizontalLayout_10->addWidget(comboBox_20);

        label_30 = new QLabel(cardMod_3);
        label_30->setObjectName("label_30");

        horizontalLayout_10->addWidget(label_30);

        comboBox_21 = new QComboBox(cardMod_3);
        comboBox_21->addItem(QString());
        comboBox_21->setObjectName("comboBox_21");

        horizontalLayout_10->addWidget(comboBox_21);


        verticalLayout_Mod_3->addLayout(horizontalLayout_10);

        horizontalLayout_111 = new QHBoxLayout();
        horizontalLayout_111->setSpacing(12);
        horizontalLayout_111->setObjectName("horizontalLayout_111");
        label_31 = new QLabel(cardMod_3);
        label_31->setObjectName("label_31");

        horizontalLayout_111->addWidget(label_31);

        label_32 = new QLabel(cardMod_3);
        label_32->setObjectName("label_32");

        horizontalLayout_111->addWidget(label_32);

        comboBox_22 = new QComboBox(cardMod_3);
        comboBox_22->addItem(QString());
        comboBox_22->setObjectName("comboBox_22");

        horizontalLayout_111->addWidget(comboBox_22);

        label_33 = new QLabel(cardMod_3);
        label_33->setObjectName("label_33");

        horizontalLayout_111->addWidget(label_33);

        comboBox_23 = new QComboBox(cardMod_3);
        comboBox_23->addItem(QString());
        comboBox_23->setObjectName("comboBox_23");

        horizontalLayout_111->addWidget(comboBox_23);

        label_34 = new QLabel(cardMod_3);
        label_34->setObjectName("label_34");

        horizontalLayout_111->addWidget(label_34);

        comboBox_24 = new QComboBox(cardMod_3);
        comboBox_24->addItem(QString());
        comboBox_24->setObjectName("comboBox_24");

        horizontalLayout_111->addWidget(comboBox_24);


        verticalLayout_Mod_3->addLayout(horizontalLayout_111);

        spacerBeforeAdresse = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mod_3->addItem(spacerBeforeAdresse);

        separatorLine2 = new QFrame(cardMod_3);
        separatorLine2->setObjectName("separatorLine2");
        separatorLine2->setFrameShape(QFrame::Shape::HLine);
        separatorLine2->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_Mod_3->addWidget(separatorLine2);

        lblSectionAdresse = new QLabel(cardMod_3);
        lblSectionAdresse->setObjectName("lblSectionAdresse");

        verticalLayout_Mod_3->addWidget(lblSectionAdresse);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label5 = new QLabel(cardMod_3);
        label5->setObjectName("label5");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(label5->sizePolicy().hasHeightForWidth());
        label5->setSizePolicy(sizePolicy4);
        label5->setMaximumSize(QSize(0, 0));
        label5->setVisible(false);

        horizontalLayout->addWidget(label5);

        textEdit = new QTextEdit(cardMod_3);
        textEdit->setObjectName("textEdit");
        sizePolicy3.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy3);
        textEdit->setMinimumSize(QSize(0, 80));
        textEdit->setMaximumSize(QSize(16777215, 100));
        textEdit->setAutoFillBackground(false);

        horizontalLayout->addWidget(textEdit);


        verticalLayout_Mod_3->addLayout(horizontalLayout);

        spacerBeforeBtns = new QSpacerItem(20, 12, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mod_3->addItem(spacerBeforeBtns);

        separatorLine3 = new QFrame(cardMod_3);
        separatorLine3->setObjectName("separatorLine3");
        separatorLine3->setFrameShape(QFrame::Shape::HLine);
        separatorLine3->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_Mod_3->addWidget(separatorLine3);

        btns_mod_3 = new QHBoxLayout();
        btns_mod_3->setSpacing(16);
        btns_mod_3->setObjectName("btns_mod_3");
        btnSave_Mod_3 = new QPushButton(cardMod_3);
        btnSave_Mod_3->setObjectName("btnSave_Mod_3");
        sizePolicy3.setHeightForWidth(btnSave_Mod_3->sizePolicy().hasHeightForWidth());
        btnSave_Mod_3->setSizePolicy(sizePolicy3);
        btnSave_Mod_3->setMinimumSize(QSize(0, 76));
        btnSave_Mod_3->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btns_mod_3->addWidget(btnSave_Mod_3);

        btnCancel_Mod_3 = new QPushButton(cardMod_3);
        btnCancel_Mod_3->setObjectName("btnCancel_Mod_3");
        sizePolicy3.setHeightForWidth(btnCancel_Mod_3->sizePolicy().hasHeightForWidth());
        btnCancel_Mod_3->setSizePolicy(sizePolicy3);
        btnCancel_Mod_3->setMinimumSize(QSize(0, 80));
        btnCancel_Mod_3->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btns_mod_3->addWidget(btnCancel_Mod_3);


        verticalLayout_Mod_3->addLayout(btns_mod_3);


        hl_mod_split_3->addWidget(cardMod_3);

        scrollArea_CmdAjout->setWidget(scrollContent_CmdAjout);

        vbox_ajout_content->addWidget(scrollArea_CmdAjout);


        hbox_ajout->addLayout(vbox_ajout_content);

        stackedWidget->addWidget(pageCmdAjout);
        pageCmdModifier = new QWidget();
        pageCmdModifier->setObjectName("pageCmdModifier");
        hbox_mod = new QHBoxLayout(pageCmdModifier);
        hbox_mod->setSpacing(0);
        hbox_mod->setObjectName("hbox_mod");
        hbox_mod->setContentsMargins(0, 0, 0, 0);
        vbox_mod_content = new QVBoxLayout();
        vbox_mod_content->setSpacing(0);
        vbox_mod_content->setObjectName("vbox_mod_content");
        vbox_mod_content->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, -1, 0, -1);
        lb_3 = new QLabel(pageCmdModifier);
        lb_3->setObjectName("lb_3");

        horizontalLayout_3->addWidget(lb_3);

        topFill_3 = new QFrame(pageCmdModifier);
        topFill_3->setObjectName("topFill_3");
        sizePolicy2.setHeightForWidth(topFill_3->sizePolicy().hasHeightForWidth());
        topFill_3->setSizePolicy(sizePolicy2);
        topFill_3->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout_3->addWidget(topFill_3);

        user_3 = new QLabel(pageCmdModifier);
        user_3->setObjectName("user_3");

        horizontalLayout_3->addWidget(user_3);

        topGap_3 = new QFrame(pageCmdModifier);
        topGap_3->setObjectName("topGap_3");
        topGap_3->setMinimumSize(QSize(10, 56));
        topGap_3->setMaximumSize(QSize(10, 56));
        topGap_3->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout_3->addWidget(topGap_3);

        btnnotif_3 = new QPushButton(pageCmdModifier);
        btnnotif_3->setObjectName("btnnotif_3");

        horizontalLayout_3->addWidget(btnnotif_3);

        btnprofil_3 = new QPushButton(pageCmdModifier);
        btnprofil_3->setObjectName("btnprofil_3");

        horizontalLayout_3->addWidget(btnprofil_3);


        vbox_mod_content->addLayout(horizontalLayout_3);

        scrollArea_CmdModif = new QScrollArea(pageCmdModifier);
        scrollArea_CmdModif->setObjectName("scrollArea_CmdModif");
        scrollArea_CmdModif->setFrameShape(QFrame::Shape::NoFrame);
        scrollArea_CmdModif->setWidgetResizable(true);
        scrollContent_CmdModif = new QWidget();
        scrollContent_CmdModif->setObjectName("scrollContent_CmdModif");
        scrollContent_CmdModif->setGeometry(QRect(0, 0, 690, 923));
        hl_mod_split = new QHBoxLayout(scrollContent_CmdModif);
        hl_mod_split->setSpacing(0);
        hl_mod_split->setObjectName("hl_mod_split");
        hl_mod_split->setContentsMargins(40, 25, 40, 30);
        cardMod1 = new QFrame(scrollContent_CmdModif);
        cardMod1->setObjectName("cardMod1");
        cardMod1->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_Mod = new QVBoxLayout(cardMod1);
        verticalLayout_Mod->setSpacing(16);
        verticalLayout_Mod->setObjectName("verticalLayout_Mod");
        verticalLayout_Mod->setContentsMargins(32, 24, 32, 28);
        lblTitleMod = new QLabel(cardMod1);
        lblTitleMod->setObjectName("lblTitleMod");
        sizePolicy3.setHeightForWidth(lblTitleMod->sizePolicy().hasHeightForWidth());
        lblTitleMod->setSizePolicy(sizePolicy3);
        lblTitleMod->setMinimumSize(QSize(0, 50));

        verticalLayout_Mod->addWidget(lblTitleMod);

        gl_inputs_2 = new QGridLayout();
        gl_inputs_2->setSpacing(20);
        gl_inputs_2->setObjectName("gl_inputs_2");
        dsb_price_add_2 = new QDoubleSpinBox(cardMod1);
        dsb_price_add_2->setObjectName("dsb_price_add_2");
        dsb_price_add_2->setMaximum(99999.000000000000000);

        gl_inputs_2->addWidget(dsb_price_add_2, 1, 1, 1, 1);

        l_stat_2 = new QLabel(cardMod1);
        l_stat_2->setObjectName("l_stat_2");

        gl_inputs_2->addWidget(l_stat_2, 2, 0, 1, 1);

        l_qty_2 = new QLabel(cardMod1);
        l_qty_2->setObjectName("l_qty_2");

        gl_inputs_2->addWidget(l_qty_2, 1, 2, 1, 1);

        cb_status_add_2 = new QComboBox(cardMod1);
        cb_status_add_2->addItem(QString());
        cb_status_add_2->addItem(QString());
        cb_status_add_2->addItem(QString());
        cb_status_add_2->addItem(QString());
        cb_status_add_2->addItem(QString());
        cb_status_add_2->setObjectName("cb_status_add_2");

        gl_inputs_2->addWidget(cb_status_add_2, 2, 1, 1, 1);

        l_px_2 = new QLabel(cardMod1);
        l_px_2->setObjectName("l_px_2");

        gl_inputs_2->addWidget(l_px_2, 1, 0, 1, 1);

        l2_2 = new QLabel(cardMod1);
        l2_2->setObjectName("l2_2");

        gl_inputs_2->addWidget(l2_2, 0, 2, 1, 1);

        l1_2 = new QLabel(cardMod1);
        l1_2->setObjectName("l1_2");

        gl_inputs_2->addWidget(l1_2, 0, 0, 1, 1);

        cb_client_mod = new QComboBox(cardMod1);
        cb_client_mod->setObjectName("cb_client_mod");

        gl_inputs_2->addWidget(cb_client_mod, 0, 1, 1, 1);

        sb_qty_add_2 = new QSpinBox(cardMod1);
        sb_qty_add_2->setObjectName("sb_qty_add_2");

        gl_inputs_2->addWidget(sb_qty_add_2, 1, 3, 1, 1);

        cb_model_add_2 = new QComboBox(cardMod1);
        cb_model_add_2->addItem(QString());
        cb_model_add_2->addItem(QString());
        cb_model_add_2->addItem(QString());
        cb_model_add_2->addItem(QString());
        cb_model_add_2->setObjectName("cb_model_add_2");

        gl_inputs_2->addWidget(cb_model_add_2, 0, 3, 1, 1);


        verticalLayout_Mod->addLayout(gl_inputs_2);

        spacerBeforeDatesMod = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mod->addItem(spacerBeforeDatesMod);

        sepLineMod1 = new QFrame(cardMod1);
        sepLineMod1->setObjectName("sepLineMod1");
        sepLineMod1->setFrameShape(QFrame::Shape::HLine);
        sepLineMod1->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_Mod->addWidget(sepLineMod1);

        lblSectionDates_Mod = new QLabel(cardMod1);
        lblSectionDates_Mod->setObjectName("lblSectionDates_Mod");

        verticalLayout_Mod->addWidget(lblSectionDates_Mod);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(12);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_11 = new QLabel(cardMod1);
        label_11->setObjectName("label_11");

        horizontalLayout_5->addWidget(label_11);

        label_12 = new QLabel(cardMod1);
        label_12->setObjectName("label_12");

        horizontalLayout_5->addWidget(label_12);

        comboBox_7 = new QComboBox(cardMod1);
        comboBox_7->addItem(QString());
        comboBox_7->setObjectName("comboBox_7");

        horizontalLayout_5->addWidget(comboBox_7);

        label_13 = new QLabel(cardMod1);
        label_13->setObjectName("label_13");

        horizontalLayout_5->addWidget(label_13);

        comboBox_8 = new QComboBox(cardMod1);
        comboBox_8->addItem(QString());
        comboBox_8->setObjectName("comboBox_8");

        horizontalLayout_5->addWidget(comboBox_8);

        label_14 = new QLabel(cardMod1);
        label_14->setObjectName("label_14");

        horizontalLayout_5->addWidget(label_14);

        comboBox_9 = new QComboBox(cardMod1);
        comboBox_9->addItem(QString());
        comboBox_9->setObjectName("comboBox_9");

        horizontalLayout_5->addWidget(comboBox_9);


        verticalLayout_Mod->addLayout(horizontalLayout_5);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(12);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        label_19 = new QLabel(cardMod1);
        label_19->setObjectName("label_19");

        horizontalLayout_7->addWidget(label_19);

        label_20 = new QLabel(cardMod1);
        label_20->setObjectName("label_20");

        horizontalLayout_7->addWidget(label_20);

        comboBox_13 = new QComboBox(cardMod1);
        comboBox_13->addItem(QString());
        comboBox_13->setObjectName("comboBox_13");

        horizontalLayout_7->addWidget(comboBox_13);

        label_21 = new QLabel(cardMod1);
        label_21->setObjectName("label_21");

        horizontalLayout_7->addWidget(label_21);

        comboBox_14 = new QComboBox(cardMod1);
        comboBox_14->addItem(QString());
        comboBox_14->setObjectName("comboBox_14");

        horizontalLayout_7->addWidget(comboBox_14);

        label_22 = new QLabel(cardMod1);
        label_22->setObjectName("label_22");

        horizontalLayout_7->addWidget(label_22);

        comboBox_15 = new QComboBox(cardMod1);
        comboBox_15->addItem(QString());
        comboBox_15->setObjectName("comboBox_15");

        horizontalLayout_7->addWidget(comboBox_15);


        verticalLayout_Mod->addLayout(horizontalLayout_7);

        spacerBeforeAdresseMod = new QSpacerItem(20, 8, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mod->addItem(spacerBeforeAdresseMod);

        sepLineMod2 = new QFrame(cardMod1);
        sepLineMod2->setObjectName("sepLineMod2");
        sepLineMod2->setFrameShape(QFrame::Shape::HLine);
        sepLineMod2->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_Mod->addWidget(sepLineMod2);

        lblSectionAdresse_Mod = new QLabel(cardMod1);
        lblSectionAdresse_Mod->setObjectName("lblSectionAdresse_Mod");

        verticalLayout_Mod->addWidget(lblSectionAdresse_Mod);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        label_6 = new QLabel(cardMod1);
        label_6->setObjectName("label_6");
        sizePolicy4.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy4);
        label_6->setMaximumSize(QSize(0, 0));
        label_6->setVisible(false);

        horizontalLayout_12->addWidget(label_6);

        textEdit_2 = new QTextEdit(cardMod1);
        textEdit_2->setObjectName("textEdit_2");
        sizePolicy3.setHeightForWidth(textEdit_2->sizePolicy().hasHeightForWidth());
        textEdit_2->setSizePolicy(sizePolicy3);
        textEdit_2->setMinimumSize(QSize(0, 80));
        textEdit_2->setMaximumSize(QSize(16777215, 100));
        textEdit_2->setAutoFillBackground(false);

        horizontalLayout_12->addWidget(textEdit_2);


        verticalLayout_Mod->addLayout(horizontalLayout_12);

        spacerBeforeBtnsMod = new QSpacerItem(20, 12, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_Mod->addItem(spacerBeforeBtnsMod);

        sepLineMod3 = new QFrame(cardMod1);
        sepLineMod3->setObjectName("sepLineMod3");
        sepLineMod3->setFrameShape(QFrame::Shape::HLine);
        sepLineMod3->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_Mod->addWidget(sepLineMod3);

        btns_mod = new QHBoxLayout();
        btns_mod->setSpacing(16);
        btns_mod->setObjectName("btns_mod");
        btnSave_CmdMod = new QPushButton(cardMod1);
        btnSave_CmdMod->setObjectName("btnSave_CmdMod");
        sizePolicy3.setHeightForWidth(btnSave_CmdMod->sizePolicy().hasHeightForWidth());
        btnSave_CmdMod->setSizePolicy(sizePolicy3);
        btnSave_CmdMod->setMinimumSize(QSize(0, 50));
        btnSave_CmdMod->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnSave_CmdMod->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #38a169;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 12px;\n"
"    padding: 14px 20px;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #2f855a;\n"
"}"));

        btns_mod->addWidget(btnSave_CmdMod);

        btnCancel_CmdMod = new QPushButton(cardMod1);
        btnCancel_CmdMod->setObjectName("btnCancel_CmdMod");
        sizePolicy3.setHeightForWidth(btnCancel_CmdMod->sizePolicy().hasHeightForWidth());
        btnCancel_CmdMod->setSizePolicy(sizePolicy3);
        btnCancel_CmdMod->setMinimumSize(QSize(0, 50));
        btnCancel_CmdMod->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnCancel_CmdMod->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #edf2f7;\n"
"    color: #4a5568;\n"
"    border: 1.5px solid #e2e8f0;\n"
"    border-radius: 12px;\n"
"    padding: 14px 20px;\n"
"    font-size: 15px;\n"
"    font-weight: 700;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #e2e8f0;\n"
"    border-color: #cbd5e0;\n"
"    color: #2d3748;\n"
"}"));

        btns_mod->addWidget(btnCancel_CmdMod);


        verticalLayout_Mod->addLayout(btns_mod);


        hl_mod_split->addWidget(cardMod1);

        scrollArea_CmdModif->setWidget(scrollContent_CmdModif);

        vbox_mod_content->addWidget(scrollArea_CmdModif);


        hbox_mod->addLayout(vbox_mod_content);

        stackedWidget->addWidget(pageCmdModifier);

        verticalLayout_Main->addWidget(stackedWidget);


        horizontalLayout_Main->addWidget(contentArea);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1505, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);
        QObject::connect(btnGotoAjout, &QPushButton::clicked, stackedWidget_Maintenance, &QStackedWidget::setCurrentIndex);
        QObject::connect(btnGotoModifier, &QPushButton::clicked, stackedWidget_Maintenance, &QStackedWidget::setCurrentIndex);
        QObject::connect(btnCancel_Add, &QPushButton::clicked, stackedWidget_Maintenance, &QStackedWidget::setCurrentIndex);
        QObject::connect(btnCancel_Mod, &QPushButton::clicked, stackedWidget_Maintenance, &QStackedWidget::setCurrentIndex);
        QObject::connect(btnBack_Ajout, &QPushButton::clicked, stackedWidget_Maintenance, &QStackedWidget::setCurrentIndex);
        QObject::connect(btnBack_Modif, &QPushButton::clicked, stackedWidget_Maintenance, &QStackedWidget::setCurrentIndex);

        stackedWidget->setCurrentIndex(9);
        prod_stackedWidget->setCurrentIndex(2);
        prod_viewStackedWidget->setCurrentIndex(0);
        stackedWidget_Maintenance->setCurrentIndex(0);
        contentStack->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "WasteGuard - Gestion des Employes", nullptr));
        btnToggleSidebar->setText(QCoreApplication::translate("MainWindow", "\342\230\260", nullptr));
        label_logo->setText(QString());
        btnAccueil->setText(QCoreApplication::translate("MainWindow", "\360\237\217\240 Accueils", nullptr));
        btnStock->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246 Stock", nullptr));
        btnProduits->setText(QCoreApplication::translate("MainWindow", "\360\237\233\215\357\270\217 Produits", nullptr));
        btnClient->setText(QCoreApplication::translate("MainWindow", "\360\237\221\245 Clients", nullptr));
        btnEmployes->setText(QCoreApplication::translate("MainWindow", "\360\237\221\267 Employes", nullptr));
        btnStatistiques->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques", nullptr));
        btnMaintenance->setText(QCoreApplication::translate("MainWindow", "\360\237\224\247 Maintenances", nullptr));
        btnRetour->setText(QCoreApplication::translate("MainWindow", "Retour", nullptr));
        btnCommandes->setText(QCoreApplication::translate("MainWindow", "\360\237\223\235 Commandes", nullptr));
        label_Title->setText(QCoreApplication::translate("MainWindow", "Tableau de Bord", nullptr));
        lblUserName->setText(QCoreApplication::translate("MainWindow", "Admin System", nullptr));
        lblUserRole->setText(QCoreApplication::translate("MainWindow", "Chef d'Atelier", nullptr));
        btnNouveau->setText(QCoreApplication::translate("MainWindow", "+ Nouvel Employe", nullptr));
        txtSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher un employ\303\251 par matricule", nullptr));
        cbSort->setItemText(0, QCoreApplication::translate("MainWindow", "Trier par: Specialite", nullptr));
        cbSort->setItemText(1, QCoreApplication::translate("MainWindow", "Trier par: note de fin d annes", nullptr));
        cbSort->setItemText(2, QCoreApplication::translate("MainWindow", "Trier par: salaire", nullptr));
        cbSort->setItemText(3, QCoreApplication::translate("MainWindow", "Trier par: Nom", nullptr));

        QTableWidgetItem *___qtablewidgetitem = tableEmployes->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "MATRICULE", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableEmployes->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "NOM COMPLET", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableEmployes->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "SP\303\211CIALIT\303\211", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableEmployes->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "DISPONIBILIT\303\211", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableEmployes->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "ACTIONS", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableEmployes->verticalHeaderItem(0);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableEmployes->verticalHeaderItem(1);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableEmployes->verticalHeaderItem(2);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableEmployes->verticalHeaderItem(3);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "4", nullptr));

        const bool __sortingEnabled = tableEmployes->isSortingEnabled();
        tableEmployes->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem9 = tableEmployes->item(0, 0);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "EMP-001", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tableEmployes->item(0, 1);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "Ali Ben Salah", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tableEmployes->item(0, 2);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "M\303\251canicien", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tableEmployes->item(0, 3);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "Disponible", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tableEmployes->item(0, 4);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 | \360\237\227\221\357\270\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tableEmployes->item(1, 0);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "EMP-002", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tableEmployes->item(1, 1);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "Sara Tounsi", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tableEmployes->item(1, 2);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "Chauffeur", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = tableEmployes->item(1, 3);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "En Mission", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = tableEmployes->item(1, 4);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 | \360\237\227\221\357\270\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = tableEmployes->item(2, 0);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "EMP-003", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = tableEmployes->item(2, 1);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "Mohamed Amine", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = tableEmployes->item(2, 2);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "\303\211lectronicien", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = tableEmployes->item(2, 3);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "Conge", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = tableEmployes->item(2, 4);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 | \360\237\227\221\357\270\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = tableEmployes->item(3, 0);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "EMP-004", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = tableEmployes->item(3, 1);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "Rania Kallel", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = tableEmployes->item(3, 2);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "Responsable", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = tableEmployes->item(3, 3);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "Disponible", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = tableEmployes->item(3, 4);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 | \360\237\227\221\357\270\217", nullptr));
        tableEmployes->setSortingEnabled(__sortingEnabled);

        label_Stat1_Title->setText(QCoreApplication::translate("MainWindow", "Total Employes", nullptr));
        lblStatNumber->setText(QCoreApplication::translate("MainWindow", "12", nullptr));
        label_Stat2_Title->setText(QCoreApplication::translate("MainWindow", "Charge de Travail de l'equipe", nullptr));
        lblStatWorkload->setText(QCoreApplication::translate("MainWindow", "75%", nullptr));
        label_Stat3_Title->setText(QCoreApplication::translate("MainWindow", "Disponibilite", nullptr));
        lblStatAvailability->setText(QCoreApplication::translate("MainWindow", "4 Dispo", nullptr));
        label_Actions->setText(QCoreApplication::translate("MainWindow", "Actions Rapides", nullptr));
        btnFichePaie->setText(QCoreApplication::translate("MainWindow", "Generer Fiche de Paie", nullptr));
        btnGoMission->setText(QCoreApplication::translate("MainWindow", "Affectation IA", nullptr));
        btnGoPointage->setText(QCoreApplication::translate("MainWindow", "Pointage", nullptr));
        btnGoStats->setText(QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        lblUserName_Modif->setText(QCoreApplication::translate("MainWindow", "Admin System", nullptr));
        lblUserRole_Modif->setText(QCoreApplication::translate("MainWindow", "Chef d'Atelier", nullptr));
        imageLabel->setText(QCoreApplication::translate("MainWindow", "Photo", nullptr));
        btnUploadPhoto->setText(QCoreApplication::translate("MainWindow", "\360\237\223\267 Charger une photo", nullptr));
        lblMatricule->setText(QCoreApplication::translate("MainWindow", "Matricule", nullptr));
        txtMatricule->setPlaceholderText(QCoreApplication::translate("MainWindow", "Auto", nullptr));
        lblNom->setText(QCoreApplication::translate("MainWindow", "Nom Complet", nullptr));
        lblSpecialite->setText(QCoreApplication::translate("MainWindow", "Specialite", nullptr));
        cbSpecialite->setItemText(0, QCoreApplication::translate("MainWindow", "M\303\251canicien", nullptr));
        cbSpecialite->setItemText(1, QCoreApplication::translate("MainWindow", "Chauffeur", nullptr));
        cbSpecialite->setItemText(2, QCoreApplication::translate("MainWindow", "\303\211lectronicien", nullptr));

        lblStatut->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        cbStatut->setItemText(0, QCoreApplication::translate("MainWindow", "Disponible", nullptr));
        cbStatut->setItemText(1, QCoreApplication::translate("MainWindow", "En intervention", nullptr));
        cbStatut->setItemText(2, QCoreApplication::translate("MainWindow", "Conge", nullptr));

        lblSalaire_Modif->setText(QCoreApplication::translate("MainWindow", "Salaire", nullptr));
        lblValSalaire_Modif->setText(QCoreApplication::translate("MainWindow", "1200 DT", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "EMAIL", nullptr));
        lblCIN_Modif->setText(QCoreApplication::translate("MainWindow", "CIN", nullptr));
        txtCIN_Modif->setPlaceholderText(QCoreApplication::translate("MainWindow", "00000000", nullptr));
        txtEmailModif->setPlaceholderText(QCoreApplication::translate("MainWindow", "xyz@emeple.com", nullptr));
        btnDelete->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        btnAnnuler->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        btnSave->setText(QCoreApplication::translate("MainWindow", "Sauvegarder", nullptr));
        lblUserName_Ajout->setText(QCoreApplication::translate("MainWindow", "Admin System", nullptr));
        lblUserRole_Ajout->setText(QCoreApplication::translate("MainWindow", "Chef d'Atelier", nullptr));
        imageLabel_2->setText(QCoreApplication::translate("MainWindow", "Photo", nullptr));
        btnUploadPhoto_Ajout->setText(QCoreApplication::translate("MainWindow", "\360\237\223\267 Charger une photo", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Nom Complet", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Specialite", nullptr));
        cbSpecialite_Ajout->setItemText(0, QCoreApplication::translate("MainWindow", "M\303\251canicien", nullptr));
        cbSpecialite_Ajout->setItemText(1, QCoreApplication::translate("MainWindow", "Chauffeur", nullptr));

        label_9->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        cbStatut_Ajout->setItemText(0, QCoreApplication::translate("MainWindow", "Disponible", nullptr));
        cbStatut_Ajout->setItemText(1, QCoreApplication::translate("MainWindow", "En intervention", nullptr));

        lblSalaire_Ajout->setText(QCoreApplication::translate("MainWindow", "Salaire", nullptr));
        lblValSalaire_Ajout->setText(QCoreApplication::translate("MainWindow", "1200 DT", nullptr));
        txtEmailAjout->setPlaceholderText(QCoreApplication::translate("MainWindow", "xyz@emeple.com", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "EMAIL", nullptr));
        lblCIN_Ajout->setText(QCoreApplication::translate("MainWindow", "CIN", nullptr));
        txtCIN_Ajout->setPlaceholderText(QCoreApplication::translate("MainWindow", "00000000", nullptr));
        btnAnnuler_Ajout->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        btnAjouter->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        lblTitreMission->setText(QCoreApplication::translate("MainWindow", "\360\237\223\242 D\303\251finition de la Mission", nullptr));
        txtMission->setText(QCoreApplication::translate("MainWindow", "Reparation Moteur bac Zone Nord - Urgent", nullptr));
        txtMission->setPlaceholderText(QCoreApplication::translate("MainWindow", "Ex: Reparation Moteur bac Urgent...", nullptr));
        btnAnalyser->setText(QCoreApplication::translate("MainWindow", "\360\237\244\226 Lancer l'Analyse IA & Trouver l'\303\211quipe", nullptr));
        lblResultat->setText(QCoreApplication::translate("MainWindow", "\342\234\205 \303\211quipe Optimale Recommand\303\251e (Score: 98%)", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = tableResultat->horizontalHeaderItem(0);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "Matricule", nullptr));
        QTableWidgetItem *___qtablewidgetitem30 = tableResultat->horizontalHeaderItem(1);
        ___qtablewidgetitem30->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem31 = tableResultat->horizontalHeaderItem(2);
        ___qtablewidgetitem31->setText(QCoreApplication::translate("MainWindow", "Competence Cle", nullptr));
        QTableWidgetItem *___qtablewidgetitem32 = tableResultat->horizontalHeaderItem(3);
        ___qtablewidgetitem32->setText(QCoreApplication::translate("MainWindow", "Score Match", nullptr));
        btnAnnulerMission->setText(QCoreApplication::translate("MainWindow", "Retour / Annuler", nullptr));
        lblTitrePointage->setText(QCoreApplication::translate("MainWindow", "\303\211tat du Lecteur RFID :", nullptr));
        lblStatutRFID->setText(QCoreApplication::translate("MainWindow", "\360\237\225\222 EN ATTENTE DE BADGE...", nullptr));
        btnSimulerBadge->setText(QCoreApplication::translate("MainWindow", "\360\237\224\256 Simuler un passage de badge (D\303\251mo)", nullptr));
        lblLog->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 Historique des Entr\303\251es / Sorties (Aujourd'hui)", nullptr));
        QTableWidgetItem *___qtablewidgetitem33 = tablePointage->horizontalHeaderItem(0);
        ___qtablewidgetitem33->setText(QCoreApplication::translate("MainWindow", "Heure", nullptr));
        QTableWidgetItem *___qtablewidgetitem34 = tablePointage->horizontalHeaderItem(1);
        ___qtablewidgetitem34->setText(QCoreApplication::translate("MainWindow", "ID Badge", nullptr));
        QTableWidgetItem *___qtablewidgetitem35 = tablePointage->horizontalHeaderItem(2);
        ___qtablewidgetitem35->setText(QCoreApplication::translate("MainWindow", "Employe", nullptr));
        QTableWidgetItem *___qtablewidgetitem36 = tablePointage->horizontalHeaderItem(3);
        ___qtablewidgetitem36->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        btnAnnulerPointage->setText(QCoreApplication::translate("MainWindow", "Retour / Annuler", nullptr));
        cbProjetStats->setItemText(0, QCoreApplication::translate("MainWindow", "Projet A", nullptr));
        cbProjetStats->setItemText(1, QCoreApplication::translate("MainWindow", "Projet B", nullptr));
        cbProjetStats->setItemText(2, QCoreApplication::translate("MainWindow", "Projet C", nullptr));

        cbProjetStats->setPlaceholderText(QCoreApplication::translate("MainWindow", "Selectionnez un projet...", nullptr));
        btnAnnulerStats->setText(QCoreApplication::translate("MainWindow", "Retour / Annuler", nullptr));
        prod_label_1->setText(QString());
        prod_btnhome_1->setText(QCoreApplication::translate("MainWindow", "  Accueils", nullptr));
        prod_btnstock_1->setText(QCoreApplication::translate("MainWindow", "  Stocks", nullptr));
        prod_btnproduit_1->setText(QCoreApplication::translate("MainWindow", "  Produits", nullptr));
        prod_btnclient_1->setText(QCoreApplication::translate("MainWindow", "  Clients", nullptr));
        prod_btnemploye_1->setText(QCoreApplication::translate("MainWindow", "  Employes", nullptr));
        prod_btnmaintenance_1->setText(QCoreApplication::translate("MainWindow", "  Maintenances", nullptr));
        prod_btncmd_1->setText(QCoreApplication::translate("MainWindow", "  Commandes", nullptr));
        prod_label_Title->setText(QCoreApplication::translate("MainWindow", "Tableau de Bord", nullptr));
        prod_btnAddProduct->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        prod_lblUserName->setText(QCoreApplication::translate("MainWindow", "Admin System", nullptr));
        prod_lblUserRole->setText(QCoreApplication::translate("MainWindow", "Chef d'Atelier", nullptr));
        prod_searchInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher par modele/num serie", nullptr));
        prod_cbSort->setItemText(0, QCoreApplication::translate("MainWindow", "capacit\303\251 (Croissant)", nullptr));
        prod_cbSort->setItemText(1, QCoreApplication::translate("MainWindow", "capacit\303\251 (Decroissant)", nullptr));

#if QT_CONFIG(tooltip)
        prod_btnToggleView->setToolTip(QCoreApplication::translate("MainWindow", "Changer de vue (Tableau/Cartes)", nullptr));
#endif // QT_CONFIG(tooltip)
        prod_btnToggleView->setText(QCoreApplication::translate("MainWindow", "\342\212\236", nullptr));
        QTableWidgetItem *___qtablewidgetitem37 = prod_tableProduits->horizontalHeaderItem(0);
        ___qtablewidgetitem37->setText(QCoreApplication::translate("MainWindow", "Reference", nullptr));
        QTableWidgetItem *___qtablewidgetitem38 = prod_tableProduits->horizontalHeaderItem(1);
        ___qtablewidgetitem38->setText(QCoreApplication::translate("MainWindow", "Modele", nullptr));
        QTableWidgetItem *___qtablewidgetitem39 = prod_tableProduits->horizontalHeaderItem(2);
        ___qtablewidgetitem39->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem40 = prod_tableProduits->horizontalHeaderItem(3);
        ___qtablewidgetitem40->setText(QCoreApplication::translate("MainWindow", "Capacite", nullptr));
        QTableWidgetItem *___qtablewidgetitem41 = prod_tableProduits->horizontalHeaderItem(4);
        ___qtablewidgetitem41->setText(QCoreApplication::translate("MainWindow", "Prix (TND)", nullptr));
        QTableWidgetItem *___qtablewidgetitem42 = prod_tableProduits->horizontalHeaderItem(5);
        ___qtablewidgetitem42->setText(QCoreApplication::translate("MainWindow", "Stock", nullptr));
        QTableWidgetItem *___qtablewidgetitem43 = prod_tableProduits->horizontalHeaderItem(6);
        ___qtablewidgetitem43->setText(QCoreApplication::translate("MainWindow", "Etat", nullptr));
        QTableWidgetItem *___qtablewidgetitem44 = prod_tableProduits->horizontalHeaderItem(7);
        ___qtablewidgetitem44->setText(QCoreApplication::translate("MainWindow", "Actions", nullptr));
        prod_lblFunc->setText(QCoreApplication::translate("MainWindow", "INFOS GLOBALES", nullptr));
        prod_stat1->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        prod_st_val1->setText(QCoreApplication::translate("MainWindow", "1,245", nullptr));
        prod_st_val1->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statValue", nullptr)));
        prod_st_lbl1->setText(QCoreApplication::translate("MainWindow", "Total Interventions", nullptr));
        prod_st_lbl1->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statLabel", nullptr)));
        prod_stat2->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        prod_st_val2->setText(QCoreApplication::translate("MainWindow", "85,400 TND", nullptr));
        prod_st_val2->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statValue", nullptr)));
        prod_st_lbl2->setText(QCoreApplication::translate("MainWindow", "Chiffre d'Affaire", nullptr));
        prod_st_lbl2->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statLabel", nullptr)));
        prod_stat3->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        prod_st_val3->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        prod_st_val3->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statValue", nullptr)));
        prod_st_lbl3->setText(QCoreApplication::translate("MainWindow", "\303\211quipements en Panne", nullptr));
        prod_st_lbl3->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statLabel", nullptr)));
        prod_statsPreview->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        prod_lblStatsTitle->setText(QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        prod_lblMini1->setStyleSheet(QCoreApplication::translate("MainWindow", "color: white;", nullptr));
        prod_lblMini1->setText(QCoreApplication::translate("MainWindow", "Produits", nullptr));
        prod_lblMini2->setStyleSheet(QCoreApplication::translate("MainWindow", "color: white;", nullptr));
        prod_lblMini2->setText(QCoreApplication::translate("MainWindow", "Stock", nullptr));
        prod_btnOpenStats->setText(QCoreApplication::translate("MainWindow", "Voir details  ->", nullptr));
        prod_btnMap3D->setText(QCoreApplication::translate("MainWindow", "Visualisation Map 3D", nullptr));
        prod_btnVideo3D->setText(QCoreApplication::translate("MainWindow", "Generer Video 3D", nullptr));
        prod_lblExport->setText(QCoreApplication::translate("MainWindow", "EXPORTER", nullptr));
        prod_btnPdf->setText(QCoreApplication::translate("MainWindow", "Rapport PDF", nullptr));
        prod_lblImgPreview_Add->setText(QCoreApplication::translate("MainWindow", "Glisser Image Ici", nullptr));
        prod_btnUpload_Add->setText(QCoreApplication::translate("MainWindow", "\360\237\223\244 Importer Image", nullptr));
        prod_lblTitleAdd->setText(QCoreApplication::translate("MainWindow", "Nouveau Stock", nullptr));
        prod_l1->setText(QCoreApplication::translate("MainWindow", "Reference", nullptr));
        prod_ln_ref_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "REF-2024", nullptr));
        prod_l2->setText(QCoreApplication::translate("MainWindow", "Modele", nullptr));
        prod_ln_model_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Ex: ProBin 240L", nullptr));
        prod_l_px->setText(QCoreApplication::translate("MainWindow", "Prix (TND)", nullptr));
        prod_l_qty->setText(QCoreApplication::translate("MainWindow", "Quantite", nullptr));
        prod_l_stat->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        prod_cb_status_add->setItemText(0, QCoreApplication::translate("MainWindow", "En Stock", nullptr));

        prod_l_bat->setText(QCoreApplication::translate("MainWindow", "Batterie (mAh)", nullptr));
        prod_label->setText(QCoreApplication::translate("MainWindow", "10000 mah", nullptr));
        prod_l_cap->setText(QCoreApplication::translate("MainWindow", "Capacite", nullptr));
        prod_label_4->setText(QCoreApplication::translate("MainWindow", "750 L", nullptr));
        prod_gb_characteristics->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"                            QGroupBox {\n"
"                             font-weight: bold;\n"
"                             border: 1px solid #bdc3c7;\n"
"                             border-radius: 6px;\n"
"                             margin-top: 10px;\n"
"                             padding-top: 10px;\n"
"                            }\n"
"                            QGroupBox::title {\n"
"                             subcontrol-origin: margin;\n"
"                             left: 10px;\n"
"                             padding: 0 5px;\n"
"                             color: black;\n"
"                            }\n"
"                           ", nullptr));
        prod_gb_characteristics->setTitle(QCoreApplication::translate("MainWindow", "Caracteristiques Techniques", nullptr));

        const bool __sortingEnabled1 = prod_lstCharacteristics->isSortingEnabled();
        prod_lstCharacteristics->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = prod_lstCharacteristics->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("MainWindow", "\360\237\223\215 GPS Tracking", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = prod_lstCharacteristics->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("MainWindow", "\360\237\223\241 Capteur Ultrason", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = prod_lstCharacteristics->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("MainWindow", "\342\230\200\357\270\217 Compacteur Solaire", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = prod_lstCharacteristics->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("MainWindow", "\360\237\224\245 D\303\251tecteur de Flamme", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = prod_lstCharacteristics->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("MainWindow", "\360\237\224\222 Verrouillage Auto", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = prod_lstCharacteristics->item(5);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("MainWindow", "\360\237\214\220 Int\303\251gration IoT", nullptr));
        QListWidgetItem *___qlistwidgetitem6 = prod_lstCharacteristics->item(6);
        ___qlistwidgetitem6->setText(QCoreApplication::translate("MainWindow", "\360\237\214\254\357\270\217 Anti-Odeur", nullptr));
        prod_lstCharacteristics->setSortingEnabled(__sortingEnabled1);

        prod_lstCharacteristics->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"                               QListWidget {\n"
"                                 border: 2px solid #bdc3c7;\n"
"                                 background-color: white;\n"
"                                 border-radius: 6px;\n"
"                                 padding: 8px;\n"
"                                 font-size: 13px;\n"
"                                }\n"
"                                QListWidget::item {\n"
"                                 padding: 10px;\n"
"                                 margin: 2px;\n"
"                                 border: 1px solid #e0e0e0;\n"
"                                 border-radius: 5px;\n"
"                                 background-color: #f8f9fa;\n"
"                                 color: #2c3e50;\n"
"                                }\n"
"                                QListWidget::item:selected {\n"
"                                 background-color: #3498db;\n"
"                                 color: white;\n"
"                               "
                        "  border: 2px solid #2980b9;\n"
"                                 font-weight: bold;\n"
"                                }\n"
"                                QListWidget::item:hover:!selected {\n"
"                                 background-color: #e3f2fd;\n"
"                                 border: 1px solid #90caf9;\n"
"                                }\n"
"                              ", nullptr));
        prod_l5->setText(QCoreApplication::translate("MainWindow", "Plan du Depot cliquer ici pour ouvrir", nullptr));
        prod_btnSave_Add->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        prod_btnCancel_Add->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        prod_imgBin->setText(QCoreApplication::translate("MainWindow", "[ Image Actuelle ]", nullptr));
        prod_lblTitleMod->setText(QCoreApplication::translate("MainWindow", "Modifier Stock", nullptr));
        prod_l_stat_m->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        prod_l_qty_m->setText(QCoreApplication::translate("MainWindow", "Quantite", nullptr));
        prod_ln_model_mod->setPlaceholderText(QCoreApplication::translate("MainWindow", "Ex: ProBin 240L", nullptr));
        prod_cb_status_mod->setItemText(0, QCoreApplication::translate("MainWindow", "En Stock", nullptr));

        prod_ln_ref_mod->setText(QCoreApplication::translate("MainWindow", "REF-2023", nullptr));
        prod_l2_m->setText(QCoreApplication::translate("MainWindow", "Modele", nullptr));
        prod_l_px_m->setText(QCoreApplication::translate("MainWindow", "Prix (TND)", nullptr));
        prod_l1_m->setText(QCoreApplication::translate("MainWindow", "Reference", nullptr));
        prod_l_bat_m->setText(QCoreApplication::translate("MainWindow", "Batterie", nullptr));
        prod_l_cap_m->setText(QCoreApplication::translate("MainWindow", "Capacite", nullptr));
        prod_gb_characteristics_mod->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"                            QGroupBox {\n"
"                             font-weight: bold;\n"
"                             border: 1px solid #bdc3c7;\n"
"                             border-radius: 6px;\n"
"                             margin-top: 10px;\n"
"                             padding-top: 10px;\n"
"                            }\n"
"                            QGroupBox::title {\n"
"                             subcontrol-origin: margin;\n"
"                             left: 10px;\n"
"                             padding: 0 5px;\n"
"                             color: black;\n"
"                             color: black;\n"
"                            }\n"
"                           ", nullptr));
        prod_gb_characteristics_mod->setTitle(QCoreApplication::translate("MainWindow", "Caracteristiques Techniques", nullptr));

        const bool __sortingEnabled2 = prod_lstCharacteristics_mod->isSortingEnabled();
        prod_lstCharacteristics_mod->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem7 = prod_lstCharacteristics_mod->item(0);
        ___qlistwidgetitem7->setText(QCoreApplication::translate("MainWindow", "\360\237\223\215 GPS Tracking", nullptr));
        QListWidgetItem *___qlistwidgetitem8 = prod_lstCharacteristics_mod->item(1);
        ___qlistwidgetitem8->setText(QCoreApplication::translate("MainWindow", "\360\237\223\241 Capteur Ultrason", nullptr));
        QListWidgetItem *___qlistwidgetitem9 = prod_lstCharacteristics_mod->item(2);
        ___qlistwidgetitem9->setText(QCoreApplication::translate("MainWindow", "\342\230\200\357\270\217 Compacteur Solaire", nullptr));
        QListWidgetItem *___qlistwidgetitem10 = prod_lstCharacteristics_mod->item(3);
        ___qlistwidgetitem10->setText(QCoreApplication::translate("MainWindow", "\360\237\224\245 D\303\251tecteur de Flamme", nullptr));
        QListWidgetItem *___qlistwidgetitem11 = prod_lstCharacteristics_mod->item(4);
        ___qlistwidgetitem11->setText(QCoreApplication::translate("MainWindow", "\360\237\224\222 Verrouillage Auto", nullptr));
        QListWidgetItem *___qlistwidgetitem12 = prod_lstCharacteristics_mod->item(5);
        ___qlistwidgetitem12->setText(QCoreApplication::translate("MainWindow", "\360\237\214\220 Int\303\251gration IoT", nullptr));
        QListWidgetItem *___qlistwidgetitem13 = prod_lstCharacteristics_mod->item(6);
        ___qlistwidgetitem13->setText(QCoreApplication::translate("MainWindow", "\360\237\214\254\357\270\217 Anti-Odeur", nullptr));
        prod_lstCharacteristics_mod->setSortingEnabled(__sortingEnabled2);

        prod_lstCharacteristics_mod->setStyleSheet(QCoreApplication::translate("MainWindow", "QListWidget {\n"
"                                 border: 2px solid #bdc3c7;\n"
"                                 background-color: white;\n"
"                                 border-radius: 6px;\n"
"                                 padding: 8px;\n"
"                                 font-size: 13px;\n"
"                                }\n"
"                                QListWidget::item {\n"
"                                 padding: 10px;\n"
"                                 margin: 2px;\n"
"                                 border: 1px solid #e0e0e0;\n"
"                                 border-radius: 5px;\n"
"                                 background-color: #f8f9fa;\n"
"                                 color: #2c3e50;\n"
"                                }\n"
"                                QListWidget::item:selected {\n"
"                                 background-color: #3498db;\n"
"                                 color: white;\n"
"                                 border: 2px solid #2980b9;\n"
"   "
                        "                              font-weight: bold;\n"
"                                }\n"
"                                QListWidget::item:hover:!selected {\n"
"                                 background-color: #e3f2fd;\n"
"                                 border: 1px solid #90caf9;\n"
"                                }\n"
"                               ", nullptr));
        prod_l5_m->setText(QCoreApplication::translate("MainWindow", "cliquer ici pour ouvrir map", nullptr));
        prod_btnSave_Mod->setText(QCoreApplication::translate("MainWindow", "Sauvegarder", nullptr));
        prod_btnCancel_Mod->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        prod_btnReturnStats->setStyleSheet(QCoreApplication::translate("MainWindow", "\n"
"                      background-color: #546e7a; color: white; padding: 6px 12px; border-radius: 4px; font-weight: bold;\n"
"                     ", nullptr));
        prod_btnReturnStats->setText(QCoreApplication::translate("MainWindow", "\342\254\205\357\270\217 Retour", nullptr));
        prod_lblStatsHeader->setText(QCoreApplication::translate("MainWindow", "Apercu Statistique", nullptr));
        prod_lblStatsSub->setText(QCoreApplication::translate("MainWindow", "(Donn\303\251es exemple \342\200\224 graphiques g\303\251n\303\251raux)", nullptr));
        prod_graphFrame1->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        prod_graphFrame1Title->setText(QCoreApplication::translate("MainWindow", "Graphique 1 \342\200\224 R\303\251partition (exemple)", nullptr));
        prod_graphFrame1Hint->setText(QCoreApplication::translate("MainWindow", "[Graph placeholder]", nullptr));
        prod_graphFrame2->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        prod_graphFrame2Title->setText(QCoreApplication::translate("MainWindow", "Graphique 2 \342\200\224 \303\211volution (exemple)", nullptr));
        prod_graphFrame2Hint->setText(QCoreApplication::translate("MainWindow", "[Graph placeholder]", nullptr));
        prod_lblStatsNote->setText(QCoreApplication::translate("MainWindow", "Astuce: vous pouvez remplacer ces placeholders par QtCharts plus tard.", nullptr));
        headerBar->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: white; border-bottom: 1px solid #EDF2F7;", nullptr));
        headerTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #1f2d3d; font-size: 18px; font-weight: 700; margin-left: 20px;", nullptr));
        headerTitle->setText(QCoreApplication::translate("MainWindow", "Tableau de Bord", nullptr));
        btnNew->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #0f2b4c; color: white; padding: 10px 22px; border-radius: 12px; font-weight: bold;", nullptr));
        btnNew->setText(QCoreApplication::translate("MainWindow", "+ Nouveau Composant", nullptr));
        user->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #4A5568; font-size: 12px; margin-right: 20px;", nullptr));
        user->setText(QCoreApplication::translate("MainWindow", "Admin System\n"
"Responsable du stock", nullptr));
        tableCard->setStyleSheet(QCoreApplication::translate("MainWindow", "#tableCard { background-color: white; border-radius: 12px; border: 1px solid #E2E8F0; }", nullptr));
        lblTableTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 18px; font-weight: bold;", nullptr));
        lblTableTitle->setText(QCoreApplication::translate("MainWindow", "Tableau de Bord", nullptr));
        stock_searchInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher par reference, nom ou fournisseur...", nullptr));
        stock_sortCombo->setItemText(0, QCoreApplication::translate("MainWindow", "Trier: aucun", nullptr));
        stock_sortCombo->setItemText(1, QCoreApplication::translate("MainWindow", "Stock (croissant)", nullptr));
        stock_sortCombo->setItemText(2, QCoreApplication::translate("MainWindow", "Stock (decroissant)", nullptr));

        statsBox->setStyleSheet(QCoreApplication::translate("MainWindow", "background: white; border-radius: 12px; border: 1px solid #E2E8F0;", nullptr));
        lblStatsTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 11px; color: #718096;", nullptr));
        lblStatsTitle->setText(QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        totalStock->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 22px; font-weight: bold;", nullptr));
        totalStock->setText(QCoreApplication::translate("MainWindow", "85,400 TND", nullptr));
        lblCriticalStock->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 14px; color: #E53E3E; font-weight: 600; margin-top: 5px;", nullptr));
        lblCriticalStock->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        orderBox->setStyleSheet(QCoreApplication::translate("MainWindow", "background: white; border-radius: 12px; border: 1px solid #E2E8F0;", nullptr));
        lblOrderTitle->setText(QCoreApplication::translate("MainWindow", "BESOIN DE COMMANDE", nullptr));
        lblOrderSummary->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 13px; color: #718096; margin-bottom: 5px;", nullptr));
        lblOrderSummary->setText(QCoreApplication::translate("MainWindow", "Analyse en cours...", nullptr));
        btnOrder->setText(QCoreApplication::translate("MainWindow", "Generer Commande", nullptr));
        btnPrediction->setText(QCoreApplication::translate("MainWindow", "Predictions de Stock", nullptr));
        btnAlerts->setText(QCoreApplication::translate("MainWindow", "Alertes \303\251ch\303\251ance", nullptr));
        btnGoStats_Stock->setText(QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        addCard->setStyleSheet(QCoreApplication::translate("MainWindow", "#addCard { background: white; border-radius: 15px; border: 1px solid #E2E8F0; } QLineEdit { padding: 15px; border-radius: 8px; background: #F8FAFC; }", nullptr));
        lblAddTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 24px; font-weight: bold; color: #1A365D;", nullptr));
        lblAddTitle->setText(QCoreApplication::translate("MainWindow", "Ajouter un Nouveau Composant", nullptr));
        inputRef_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Reference (ex: REF-001)", nullptr));
        inputNom_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Nom du Composant", nullptr));
        inputStock_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Stock Initial", nullptr));
        inputPrix_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Prix Unitaire (TND)", nullptr));
        inputFournisseur_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Fournisseur", nullptr));
        btnCancel_add->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #dc3545; color: white; padding: 12px; border-radius: 8px;", nullptr));
        btnCancel_add->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        btnSave_add->setStyleSheet(QCoreApplication::translate("MainWindow", "background: #3182CE; color: white; padding: 12px; border-radius: 8px;", nullptr));
        btnSave_add->setText(QCoreApplication::translate("MainWindow", "Enregistrer", nullptr));
        modifyCard->setStyleSheet(QCoreApplication::translate("MainWindow", "#modifyCard { background: white; border-radius: 15px; border: 1px solid #E2E8F0; } QLineEdit { padding: 15px; border-radius: 8px; background: #F8FAFC; }", nullptr));
        lblModTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 24px; font-weight: bold; color: #1A365D;", nullptr));
        lblModTitle->setText(QCoreApplication::translate("MainWindow", "Modifier le Composant", nullptr));
        btnCancel_mod->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #dc3545; color: white; padding: 12px; border-radius: 8px;", nullptr));
        btnCancel_mod->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        btnSave_mod->setStyleSheet(QCoreApplication::translate("MainWindow", "background: #48BB78; color: white; padding: 12px; border-radius: 8px;", nullptr));
        btnSave_mod->setText(QCoreApplication::translate("MainWindow", "Mettre a jour", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "Tableau de Bord", nullptr));
        btnGotoAjout->setText(QCoreApplication::translate("MainWindow", "Aller a Ajout", nullptr));
        userLabel->setText(QCoreApplication::translate("MainWindow", "Admin System\n"
"Chef d'Atelier", nullptr));
        btnNotif->setText(QCoreApplication::translate("MainWindow", "\360\237\224\224", nullptr));
        btnProfil->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244", nullptr));
        lblSort->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        cbSort1->setItemText(0, QCoreApplication::translate("MainWindow", "Reference", nullptr));
        cbSort1->setItemText(1, QCoreApplication::translate("MainWindow", "Prix (Croissant)", nullptr));
        cbSort1->setItemText(2, QCoreApplication::translate("MainWindow", "Prix (Decroissant)", nullptr));
        cbSort1->setItemText(3, QCoreApplication::translate("MainWindow", "Quantite", nullptr));
        cbSort1->setItemText(4, QCoreApplication::translate("MainWindow", "Priorite", nullptr));

        searchInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher...", nullptr));
        btnGotoModifier->setText(QCoreApplication::translate("MainWindow", "Aller a Modifier", nullptr));
        lblCardTitle->setText(QCoreApplication::translate("MainWindow", "LISTE DES INTERVENTIONS", nullptr));
        QTableWidgetItem *___qtablewidgetitem45 = tableMaintenance->horizontalHeaderItem(0);
        ___qtablewidgetitem45->setText(QCoreApplication::translate("MainWindow", "Reference", nullptr));
        QTableWidgetItem *___qtablewidgetitem46 = tableMaintenance->horizontalHeaderItem(1);
        ___qtablewidgetitem46->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem47 = tableMaintenance->horizontalHeaderItem(2);
        ___qtablewidgetitem47->setText(QCoreApplication::translate("MainWindow", "Technicien", nullptr));
        QTableWidgetItem *___qtablewidgetitem48 = tableMaintenance->horizontalHeaderItem(3);
        ___qtablewidgetitem48->setText(QCoreApplication::translate("MainWindow", "Cout (TND)", nullptr));
        QTableWidgetItem *___qtablewidgetitem49 = tableMaintenance->horizontalHeaderItem(4);
        ___qtablewidgetitem49->setText(QCoreApplication::translate("MainWindow", "Dur\303\251e", nullptr));
        QTableWidgetItem *___qtablewidgetitem50 = tableMaintenance->horizontalHeaderItem(5);
        ___qtablewidgetitem50->setText(QCoreApplication::translate("MainWindow", "Priorit\303\251", nullptr));
        QTableWidgetItem *___qtablewidgetitem51 = tableMaintenance->horizontalHeaderItem(6);
        ___qtablewidgetitem51->setText(QCoreApplication::translate("MainWindow", "Actions", nullptr));
        lblFunc->setText(QCoreApplication::translate("MainWindow", "INFOS GLOBALES", nullptr));
        st_icon1->setText(QCoreApplication::translate("MainWindow", "\360\237\233\240", nullptr));
        st_val1->setText(QCoreApplication::translate("MainWindow", "1,245", nullptr));
        st_lbl1->setText(QCoreApplication::translate("MainWindow", "TOTAL INTERVENTIONS", nullptr));
        st_icon2->setText(QCoreApplication::translate("MainWindow", "\360\237\222\260", nullptr));
        st_val2->setText(QCoreApplication::translate("MainWindow", "85,400 TND", nullptr));
        st_lbl2->setText(QCoreApplication::translate("MainWindow", "TOTAL COUT", nullptr));
        st_icon3->setText(QCoreApplication::translate("MainWindow", "\342\217\261", nullptr));
        st_val3->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        st_lbl3->setText(QCoreApplication::translate("MainWindow", "HEURES DE MAINTENANCE", nullptr));
        btnGoStats_Maint->setText(QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        lblExport->setText(QCoreApplication::translate("MainWindow", "EXPORTER", nullptr));
        btnPdf->setText(QCoreApplication::translate("MainWindow", "Rapport PDF", nullptr));
        lblMiniChartTitle->setText(QCoreApplication::translate("MainWindow", "Repartition par Priorite", nullptr));
        lblMiniBar1->setText(QCoreApplication::translate("MainWindow", "Normale", nullptr));
        lblMiniPct1->setText(QCoreApplication::translate("MainWindow", "55%", nullptr));
        lblMiniBar2->setText(QCoreApplication::translate("MainWindow", "Urgente", nullptr));
        lblMiniPct2->setText(QCoreApplication::translate("MainWindow", "30%", nullptr));
        lblMiniBar3->setText(QCoreApplication::translate("MainWindow", "Critique", nullptr));
        lblMiniPct3->setText(QCoreApplication::translate("MainWindow", "15%", nullptr));
        titleLabel_Aj->setText(QCoreApplication::translate("MainWindow", "MAINTENANCE", nullptr));
        userLabel_Aj->setText(QCoreApplication::translate("MainWindow", "Responsable Technique", nullptr));
        btnBack_Ajout->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #e67e22; color: white; font-weight: bold; border-radius: 5px; padding: 8px;", nullptr));
        btnBack_Ajout->setText(QCoreApplication::translate("MainWindow", "\342\254\205\357\270\217 Retour au Tableau de Bord", nullptr));
        lblTitreFormAdd->setText(QCoreApplication::translate("MainWindow", "Nouvelle Intervention", nullptr));
        lblRefAdd->setText(QCoreApplication::translate("MainWindow", "Reference", nullptr));
        lblRefAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        editRefAdd->setPlaceholderText(QCoreApplication::translate("MainWindow", "INT010", nullptr));
        lblDateAdd->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        lblDateAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        lblDurAdd->setText(QCoreApplication::translate("MainWindow", "Duree", nullptr));
        lblDurAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        comboDurAdd->setItemText(0, QCoreApplication::translate("MainWindow", "2 heures", nullptr));
        comboDurAdd->setItemText(1, QCoreApplication::translate("MainWindow", "1 heure", nullptr));
        comboDurAdd->setItemText(2, QCoreApplication::translate("MainWindow", "3 heures", nullptr));

        lblCoutAdd->setText(QCoreApplication::translate("MainWindow", "Cout", nullptr));
        lblCoutAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        lblEuroAdd->setText(QCoreApplication::translate("MainWindow", "\342\202\254", nullptr));
        lblPrioAdd->setText(QCoreApplication::translate("MainWindow", "Priorite", nullptr));
        lblPrioAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        comboPrioAdd->setItemText(0, QCoreApplication::translate("MainWindow", "Basse", nullptr));
        comboPrioAdd->setItemText(1, QCoreApplication::translate("MainWindow", "Normale", nullptr));
        comboPrioAdd->setItemText(2, QCoreApplication::translate("MainWindow", "\303\251lev\303\251e", nullptr));

        lblTechAdd->setText(QCoreApplication::translate("MainWindow", "Technicien", nullptr));
        lblTechAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        editTechAdd->setPlaceholderText(QCoreApplication::translate("MainWindow", "Sophie Dupont", nullptr));
        lblAddrAdd->setText(QCoreApplication::translate("MainWindow", "Adresse", nullptr));
        lblAddrAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        editAddrAdd->setPlaceholderText(QCoreApplication::translate("MainWindow", "B045 - Rue Voltaire, Paris", nullptr));
        lblDescAdd->setText(QCoreApplication::translate("MainWindow", "Description", nullptr));
        lblDescAdd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        txtDescAdd->setPlaceholderText(QCoreApplication::translate("MainWindow", "Inspection du systeme de capteurs et reset du compacteur...", nullptr));
        lblPhotosAvantAdd->setText(QCoreApplication::translate("MainWindow", "Photos Avant", nullptr));
        lblImgPreview_Add->setText(QCoreApplication::translate("MainWindow", "Cliquez ou d\303\251posez vos images ici pour les t\303\251l\303\251charger", nullptr));
        lblPhotosApresAdd->setText(QCoreApplication::translate("MainWindow", "Photos Apr\303\250s", nullptr));
        lblImgPreview2_Add->setText(QCoreApplication::translate("MainWindow", "Cliquez ou d\303\251posez vos images ici pour les t\303\251l\303\251charger", nullptr));
        btnCancel_Add->setText(QCoreApplication::translate("MainWindow", "\342\235\214  Annuler", nullptr));
        btnSave_Add->setText(QCoreApplication::translate("MainWindow", "\342\234\205  Enregistrer", nullptr));
        titleLabel_Mod->setText(QCoreApplication::translate("MainWindow", "MAINTENANCE", nullptr));
        userLabel_Mod->setText(QCoreApplication::translate("MainWindow", "Responsable Technique", nullptr));
        btnBack_Modif->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #e67e22; color: white; font-weight: bold; border-radius: 5px; padding: 8px;", nullptr));
        btnBack_Modif->setText(QCoreApplication::translate("MainWindow", "\342\254\205\357\270\217 Retour au Tableau de Bord", nullptr));
        lblTitreFormMod->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217  Modifier Intervention", nullptr));
        lblTitreFormMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        lblRefMod->setText(QCoreApplication::translate("MainWindow", "Reference", nullptr));
        lblRefMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        editRefMod->setPlaceholderText(QCoreApplication::translate("MainWindow", "INT001", nullptr));
        lblDateMod->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        lblDateMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        lblDurMod->setText(QCoreApplication::translate("MainWindow", "Duree", nullptr));
        lblDurMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        comboDurMod->setItemText(0, QCoreApplication::translate("MainWindow", "2 heures", nullptr));
        comboDurMod->setItemText(1, QCoreApplication::translate("MainWindow", "1 heure", nullptr));
        comboDurMod->setItemText(2, QCoreApplication::translate("MainWindow", "3 heures", nullptr));

        lblCoutMod->setText(QCoreApplication::translate("MainWindow", "Cout", nullptr));
        lblCoutMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        lblEuroMod->setText(QCoreApplication::translate("MainWindow", "\342\202\254", nullptr));
        lblPrioMod->setText(QCoreApplication::translate("MainWindow", "Priorite", nullptr));
        lblPrioMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        comboPrioMod->setItemText(0, QCoreApplication::translate("MainWindow", "Basse", nullptr));
        comboPrioMod->setItemText(1, QCoreApplication::translate("MainWindow", "Normale", nullptr));
        comboPrioMod->setItemText(2, QCoreApplication::translate("MainWindow", "\303\251lev\303\251e", nullptr));

        lblSecTechMod->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244  Technicien & Localisation", nullptr));
        lblTechMod->setText(QCoreApplication::translate("MainWindow", "Technicien", nullptr));
        lblTechMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        editTechMod->setPlaceholderText(QCoreApplication::translate("MainWindow", "Sophie Dupont", nullptr));
        lblAddrMod->setText(QCoreApplication::translate("MainWindow", "Adresse", nullptr));
        lblAddrMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        editAddrMod->setPlaceholderText(QCoreApplication::translate("MainWindow", "B012 - Rue du Commerce, Paris", nullptr));
        lblSecDescMod->setText(QCoreApplication::translate("MainWindow", "\360\237\223\235  Description", nullptr));
        lblDescMod->setText(QCoreApplication::translate("MainWindow", "Description", nullptr));
        lblDescMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        txtDescMod->setPlaceholderText(QCoreApplication::translate("MainWindow", "Remplacement de la batterie apres detection de faible charge...", nullptr));
        lblSecPhotoMod->setText(QCoreApplication::translate("MainWindow", "\360\237\223\267  Photos Avant / Apr\303\250s", nullptr));
        lblPhotosAvantMod->setText(QCoreApplication::translate("MainWindow", "Photos Avant / Apres", nullptr));
        lblPhotosAvantMod->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "formLabel", nullptr)));
        lblImgPreview_Mod->setText(QCoreApplication::translate("MainWindow", "\360\237\223\267  D\303\251posez vos images AVANT / APR\303\210S ici", nullptr));
        lblLastUpdate->setText(QCoreApplication::translate("MainWindow", "Derniere modification : 25 avril 2024 a 10:00", nullptr));
        btnCancel_Mod->setText(QCoreApplication::translate("MainWindow", "\342\235\214  Annuler", nullptr));
        btnSave_Mod->setText(QCoreApplication::translate("MainWindow", "\342\234\205  Enregistrer", nullptr));
        wasteguardhead_Client->setText(QCoreApplication::translate("MainWindow", "Gestion de Clients", nullptr));
        btnNouveau_Client->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        responsable_Client->setText(QCoreApplication::translate("MainWindow", "Admin System\n"
"Responsable Commercial", nullptr));
        recherche->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\215  Rechercher (Matricule, Nom, Email)...", nullptr));
        lblTrier->setText(QCoreApplication::translate("MainWindow", "Contrat :", nullptr));
        cbTrier->setItemText(0, QCoreApplication::translate("MainWindow", "Tous", nullptr));
        cbTrier->setItemText(1, QCoreApplication::translate("MainWindow", "Mensuel", nullptr));
        cbTrier->setItemText(2, QCoreApplication::translate("MainWindow", "Trimestriel", nullptr));
        cbTrier->setItemText(3, QCoreApplication::translate("MainWindow", "Annuel", nullptr));

        cbTrier->setCurrentText(QCoreApplication::translate("MainWindow", "Type de contrat", nullptr));
        QTableWidgetItem *___qtablewidgetitem52 = tableWidget_Client->horizontalHeaderItem(0);
        ___qtablewidgetitem52->setText(QCoreApplication::translate("MainWindow", "Matricule", nullptr));
        QTableWidgetItem *___qtablewidgetitem53 = tableWidget_Client->horizontalHeaderItem(1);
        ___qtablewidgetitem53->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem54 = tableWidget_Client->horizontalHeaderItem(2);
        ___qtablewidgetitem54->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        QTableWidgetItem *___qtablewidgetitem55 = tableWidget_Client->horizontalHeaderItem(3);
        ___qtablewidgetitem55->setText(QCoreApplication::translate("MainWindow", "Bacs", nullptr));
        QTableWidgetItem *___qtablewidgetitem56 = tableWidget_Client->horizontalHeaderItem(4);
        ___qtablewidgetitem56->setText(QCoreApplication::translate("MainWindow", "Score", nullptr));
        QTableWidgetItem *___qtablewidgetitem57 = tableWidget_Client->horizontalHeaderItem(5);
        ___qtablewidgetitem57->setText(QCoreApplication::translate("MainWindow", "Paiement", nullptr));
        QTableWidgetItem *___qtablewidgetitem58 = tableWidget_Client->horizontalHeaderItem(6);
        ___qtablewidgetitem58->setText(QCoreApplication::translate("MainWindow", "Actions", nullptr));
        pilotage->setText(QCoreApplication::translate("MainWindow", "Pilotage Commercial", nullptr));
        label1->setText(QCoreApplication::translate("MainWindow", "+12.4%", nullptr));
        label2->setText(QCoreApplication::translate("MainWindow", "Croissance Mensuelle", nullptr));
        exportclient->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204  Export PDF", nullptr));
        btnGoStats_Client->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212  Statistiques", nullptr));
        label3->setText(QCoreApplication::translate("MainWindow", "Ajouter un Nouveau Client", nullptr));
        lbl_matricule_a->setText(QCoreApplication::translate("MainWindow", "Matricule:", nullptr));
        lbl_nom_a->setText(QCoreApplication::translate("MainWindow", "Nom:", nullptr));
        lbl_email_a->setText(QCoreApplication::translate("MainWindow", "Email:", nullptr));
        lbl_contrat_a->setText(QCoreApplication::translate("MainWindow", "Type Contrat:", nullptr));
        input_contrat_ajouter->setItemText(0, QCoreApplication::translate("MainWindow", "Mensuel", nullptr));
        input_contrat_ajouter->setItemText(1, QCoreApplication::translate("MainWindow", "Trimestriel", nullptr));
        input_contrat_ajouter->setItemText(2, QCoreApplication::translate("MainWindow", "Annuel", nullptr));

        lbl_paiement_a->setText(QCoreApplication::translate("MainWindow", "Statut Paiement:", nullptr));
        input_paiement_ajouter->setItemText(0, QCoreApplication::translate("MainWindow", "Pay\303\251", nullptr));
        input_paiement_ajouter->setItemText(1, QCoreApplication::translate("MainWindow", "En Retard", nullptr));

        btn_annuler_ajouter->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        btn_save_ajouter->setText(QCoreApplication::translate("MainWindow", "Ajouter Client", nullptr));
        label4->setText(QCoreApplication::translate("MainWindow", "Modifier les Informations du Client", nullptr));
        lbl_matricule_m->setText(QCoreApplication::translate("MainWindow", "Matricule:", nullptr));
        lbl_nom_m->setText(QCoreApplication::translate("MainWindow", "Nom:", nullptr));
        lbl_email_m->setText(QCoreApplication::translate("MainWindow", "Email:", nullptr));
        lbl_contrat_m->setText(QCoreApplication::translate("MainWindow", "Type Contrat:", nullptr));
        input_contrat_modifier->setItemText(0, QCoreApplication::translate("MainWindow", "Mensuel", nullptr));
        input_contrat_modifier->setItemText(1, QCoreApplication::translate("MainWindow", "Trimestriel", nullptr));
        input_contrat_modifier->setItemText(2, QCoreApplication::translate("MainWindow", "Annuel", nullptr));

        lbl_paiement_m->setText(QCoreApplication::translate("MainWindow", "Statut Paiement:", nullptr));
        input_paiement_modifier->setItemText(0, QCoreApplication::translate("MainWindow", "Pay\303\251", nullptr));
        input_paiement_modifier->setItemText(1, QCoreApplication::translate("MainWindow", "En Retard", nullptr));

        btn_annuler_modifier->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        btn_save_modifier->setText(QCoreApplication::translate("MainWindow", "Enregistrer Modifications", nullptr));
        lblStatsTitle_client->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212  EcoScore Client \342\200\224 Taux de Tri Correct", nullptr));
        btnRetour_stats_client->setText(QCoreApplication::translate("MainWindow", "\342\206\220 Retour", nullptr));
        lb_1->setText(QCoreApplication::translate("MainWindow", "Tableau de Bord", nullptr));
        btnAddDashboard->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        user_1->setText(QCoreApplication::translate("MainWindow", "Admin System\n"
"Chef d'Atelier", nullptr));
        btnnotif_1->setText(QCoreApplication::translate("MainWindow", "\360\237\224\224", nullptr));
        btnprofil_1->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244", nullptr));
        lblSort_dashboard->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        cbSortDashboard->setItemText(0, QCoreApplication::translate("MainWindow", "R\357\277\275f\357\277\275rence", nullptr));
        cbSortDashboard->setItemText(1, QCoreApplication::translate("MainWindow", "Prix (Croissant)", nullptr));
        cbSortDashboard->setItemText(2, QCoreApplication::translate("MainWindow", "Prix (D\357\277\275croissant)", nullptr));
        cbSortDashboard->setItemText(3, QCoreApplication::translate("MainWindow", "Stock", nullptr));
        cbSortDashboard->setItemText(4, QCoreApplication::translate("MainWindow", "Date de Commande", nullptr));
        cbSortDashboard->setItemText(5, QCoreApplication::translate("MainWindow", "Status", nullptr));

        searchInputDashboard->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher...", nullptr));
        btnTempToModifier->setText(QCoreApplication::translate("MainWindow", "Modifier (Temp)", nullptr));
        QTableWidgetItem *___qtablewidgetitem59 = tableDashboard->horizontalHeaderItem(0);
        ___qtablewidgetitem59->setText(QCoreApplication::translate("MainWindow", "ID Commande", nullptr));
        QTableWidgetItem *___qtablewidgetitem60 = tableDashboard->horizontalHeaderItem(1);
        ___qtablewidgetitem60->setText(QCoreApplication::translate("MainWindow", "Quantit\357\277\275", nullptr));
        QTableWidgetItem *___qtablewidgetitem61 = tableDashboard->horizontalHeaderItem(2);
        ___qtablewidgetitem61->setText(QCoreApplication::translate("MainWindow", "Priorit\357\277\275", nullptr));
        QTableWidgetItem *___qtablewidgetitem62 = tableDashboard->horizontalHeaderItem(3);
        ___qtablewidgetitem62->setText(QCoreApplication::translate("MainWindow", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem63 = tableDashboard->horizontalHeaderItem(4);
        ___qtablewidgetitem63->setText(QCoreApplication::translate("MainWindow", "Adresse", nullptr));
        QTableWidgetItem *___qtablewidgetitem64 = tableDashboard->horizontalHeaderItem(5);
        ___qtablewidgetitem64->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem65 = tableDashboard->horizontalHeaderItem(6);
        ___qtablewidgetitem65->setText(QCoreApplication::translate("MainWindow", "Stock", nullptr));
        QTableWidgetItem *___qtablewidgetitem66 = tableDashboard->horizontalHeaderItem(7);
        ___qtablewidgetitem66->setText(QCoreApplication::translate("MainWindow", "Date Livraison", nullptr));
        QTableWidgetItem *___qtablewidgetitem67 = tableDashboard->horizontalHeaderItem(8);
        ___qtablewidgetitem67->setText(QCoreApplication::translate("MainWindow", "Prix Total", nullptr));
        lblSort_2->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        cbSort_2->setItemText(0, QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence", nullptr));
        cbSort_2->setItemText(1, QCoreApplication::translate("MainWindow", "Prix (Croissant)", nullptr));
        cbSort_2->setItemText(2, QCoreApplication::translate("MainWindow", "Prix (D\303\251croissant)", nullptr));
        cbSort_2->setItemText(3, QCoreApplication::translate("MainWindow", "Stock", nullptr));
        cbSort_2->setItemText(4, QCoreApplication::translate("MainWindow", "Date de Commande", nullptr));
        cbSort_2->setItemText(5, QCoreApplication::translate("MainWindow", "Status", nullptr));

        searchInput_2->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher...", nullptr));
        btnAddProduct_2->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        QTableWidgetItem *___qtablewidgetitem68 = tableProduits_2->horizontalHeaderItem(0);
        ___qtablewidgetitem68->setText(QCoreApplication::translate("MainWindow", "ID Commande", nullptr));
        QTableWidgetItem *___qtablewidgetitem69 = tableProduits_2->horizontalHeaderItem(1);
        ___qtablewidgetitem69->setText(QCoreApplication::translate("MainWindow", "Quantit\303\251", nullptr));
        QTableWidgetItem *___qtablewidgetitem70 = tableProduits_2->horizontalHeaderItem(2);
        ___qtablewidgetitem70->setText(QCoreApplication::translate("MainWindow", "Priorit\303\251", nullptr));
        QTableWidgetItem *___qtablewidgetitem71 = tableProduits_2->horizontalHeaderItem(3);
        ___qtablewidgetitem71->setText(QCoreApplication::translate("MainWindow", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem72 = tableProduits_2->horizontalHeaderItem(4);
        ___qtablewidgetitem72->setText(QCoreApplication::translate("MainWindow", "Adresse", nullptr));
        QTableWidgetItem *___qtablewidgetitem73 = tableProduits_2->horizontalHeaderItem(5);
        ___qtablewidgetitem73->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem74 = tableProduits_2->horizontalHeaderItem(6);
        ___qtablewidgetitem74->setText(QCoreApplication::translate("MainWindow", "Stock", nullptr));
        QTableWidgetItem *___qtablewidgetitem75 = tableProduits_2->horizontalHeaderItem(7);
        ___qtablewidgetitem75->setText(QCoreApplication::translate("MainWindow", "Date Livraison", nullptr));
        QTableWidgetItem *___qtablewidgetitem76 = tableProduits_2->horizontalHeaderItem(8);
        ___qtablewidgetitem76->setText(QCoreApplication::translate("MainWindow", "Prix Total", nullptr));
        lblFunc_Cmd->setText(QCoreApplication::translate("MainWindow", "INFOS GLOBALES", nullptr));
        stat1_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        st_val1_Cmd->setText(QCoreApplication::translate("MainWindow", "1,245", nullptr));
        st_val1_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statValue", nullptr)));
        st_lbl1_Cmd->setText(QCoreApplication::translate("MainWindow", "Total des Commandes", nullptr));
        st_lbl1_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statLabel", nullptr)));
        stat2_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        st_val2_Cmd->setText(QCoreApplication::translate("MainWindow", "85,400 TND", nullptr));
        st_val2_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statValue", nullptr)));
        st_lbl2_Cmd->setText(QCoreApplication::translate("MainWindow", "Chiffre d'Affaire", nullptr));
        st_lbl2_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statLabel", nullptr)));
        stat3_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statCard", nullptr)));
        st_val3_Cmd->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        st_val3_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statValue", nullptr)));
        st_lbl3_Cmd->setText(QCoreApplication::translate("MainWindow", "Bacs Vendus", nullptr));
        st_lbl3_Cmd->setProperty("class", QVariant(QCoreApplication::translate("MainWindow", "statLabel", nullptr)));
        btnGoStats_Cmd->setText(QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        lblExport_Cmd->setText(QCoreApplication::translate("MainWindow", "EXPORTER", nullptr));
        btnPdf_Cmd->setText(QCoreApplication::translate("MainWindow", "Rapport PDF Commandes", nullptr));
        lb_5->setText(QCoreApplication::translate("MainWindow", "AJOUTER UNE COMMANDE", nullptr));
        user_5->setText(QCoreApplication::translate("MainWindow", "Admin System\n"
"Chef d'Atelier", nullptr));
        btnnotif_5->setText(QCoreApplication::translate("MainWindow", "\360\237\224\224", nullptr));
        btnprofil_5->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244", nullptr));
        lblTitleMod_3->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213  Ajouter Une Commande", nullptr));
        l_px_4->setText(QCoreApplication::translate("MainWindow", "Prix (TND)", nullptr));
        l1_4->setText(QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence", nullptr));
        l_stat_4->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        cb_client_add->setPlaceholderText(QCoreApplication::translate("MainWindow", "Selectionner Client", nullptr));
        cb_model_add_4->setItemText(0, QCoreApplication::translate("MainWindow", "Normale", nullptr));
        cb_model_add_4->setItemText(1, QCoreApplication::translate("MainWindow", "Importante", nullptr));
        cb_model_add_4->setItemText(2, QCoreApplication::translate("MainWindow", "tr\303\251s importante", nullptr));
        cb_model_add_4->setItemText(3, QCoreApplication::translate("MainWindow", "Urgente", nullptr));

        cb_status_add_4->setItemText(0, QCoreApplication::translate("MainWindow", "Confirmation Pendante", nullptr));
        cb_status_add_4->setItemText(1, QCoreApplication::translate("MainWindow", "Livraison en cours", nullptr));
        cb_status_add_4->setItemText(2, QCoreApplication::translate("MainWindow", "Livr\303\251e", nullptr));
        cb_status_add_4->setItemText(3, QCoreApplication::translate("MainWindow", "Annul\303\251e", nullptr));
        cb_status_add_4->setItemText(4, QString());

        l_qty_4->setText(QCoreApplication::translate("MainWindow", "Quantit\303\251", nullptr));
        l2_4->setText(QCoreApplication::translate("MainWindow", "Priorit\303\251", nullptr));
        lblSectionDates->setText(QCoreApplication::translate("MainWindow", "\360\237\223\205  Dates", nullptr));
        label_27->setText(QCoreApplication::translate("MainWindow", "\360\237\223\206  Date Demande :", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "Jour", nullptr));
        comboBox_19->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_29->setText(QCoreApplication::translate("MainWindow", "Mois", nullptr));
        comboBox_20->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_30->setText(QCoreApplication::translate("MainWindow", "Ann\303\251e", nullptr));
        comboBox_21->setItemText(0, QCoreApplication::translate("MainWindow", "----", nullptr));

        label_31->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246  Date Livraison :", nullptr));
        label_32->setText(QCoreApplication::translate("MainWindow", "Jour", nullptr));
        comboBox_22->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_33->setText(QCoreApplication::translate("MainWindow", "Mois", nullptr));
        comboBox_23->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_34->setText(QCoreApplication::translate("MainWindow", "Ann\303\251e", nullptr));
        comboBox_24->setItemText(0, QCoreApplication::translate("MainWindow", "----", nullptr));

        lblSectionAdresse->setText(QCoreApplication::translate("MainWindow", "\360\237\223\215  Adresse de Livraison", nullptr));
        label5->setText(QCoreApplication::translate("MainWindow", "Adresse de livraison", nullptr));
        textEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Entrez l'adresse de livraison...", nullptr));
        btnSave_Mod_3->setText(QCoreApplication::translate("MainWindow", "\342\234\205  Sauvegarder", nullptr));
        btnCancel_Mod_3->setText(QCoreApplication::translate("MainWindow", "\342\235\214  Annuler", nullptr));
        lb_3->setText(QCoreApplication::translate("MainWindow", "MODIFIER UNE COMMANDE", nullptr));
        user_3->setText(QCoreApplication::translate("MainWindow", "Admin System\n"
"Chef d'Atelier", nullptr));
        btnnotif_3->setText(QCoreApplication::translate("MainWindow", "\360\237\224\224", nullptr));
        btnprofil_3->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244", nullptr));
        lblTitleMod->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217  Modifier Une Commande", nullptr));
        l_stat_2->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        l_qty_2->setText(QCoreApplication::translate("MainWindow", "Quantit\303\251", nullptr));
        cb_status_add_2->setItemText(0, QCoreApplication::translate("MainWindow", "Confirmation Pendante", nullptr));
        cb_status_add_2->setItemText(1, QCoreApplication::translate("MainWindow", "Livraison en cours", nullptr));
        cb_status_add_2->setItemText(2, QCoreApplication::translate("MainWindow", "Livr\303\251e", nullptr));
        cb_status_add_2->setItemText(3, QCoreApplication::translate("MainWindow", "Annul\303\251e", nullptr));
        cb_status_add_2->setItemText(4, QString());

        l_px_2->setText(QCoreApplication::translate("MainWindow", "Prix (TND)", nullptr));
        l2_2->setText(QCoreApplication::translate("MainWindow", "Priorit\303\251", nullptr));
        l1_2->setText(QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence", nullptr));
        cb_model_add_2->setItemText(0, QCoreApplication::translate("MainWindow", "Normale", nullptr));
        cb_model_add_2->setItemText(1, QCoreApplication::translate("MainWindow", "Importante", nullptr));
        cb_model_add_2->setItemText(2, QCoreApplication::translate("MainWindow", "tr\303\251s importante", nullptr));
        cb_model_add_2->setItemText(3, QCoreApplication::translate("MainWindow", "Urgente", nullptr));

        lblSectionDates_Mod->setText(QCoreApplication::translate("MainWindow", "\360\237\223\205  Dates", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\360\237\223\206  Date Demande :", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Jour", nullptr));
        comboBox_7->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_13->setText(QCoreApplication::translate("MainWindow", "Mois", nullptr));
        comboBox_8->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_14->setText(QCoreApplication::translate("MainWindow", "Ann\303\251e", nullptr));
        comboBox_9->setItemText(0, QCoreApplication::translate("MainWindow", "----", nullptr));

        label_19->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246  Date Livraison :", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "Jour", nullptr));
        comboBox_13->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_21->setText(QCoreApplication::translate("MainWindow", "Mois", nullptr));
        comboBox_14->setItemText(0, QCoreApplication::translate("MainWindow", "--", nullptr));

        label_22->setText(QCoreApplication::translate("MainWindow", "Ann\303\251e", nullptr));
        comboBox_15->setItemText(0, QCoreApplication::translate("MainWindow", "----", nullptr));

        lblSectionAdresse_Mod->setText(QCoreApplication::translate("MainWindow", "\360\237\223\215  Adresse de Livraison", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Adresse de livraison", nullptr));
        textEdit_2->setPlaceholderText(QCoreApplication::translate("MainWindow", "Entrez l'adresse de livraison...", nullptr));
        btnSave_CmdMod->setText(QCoreApplication::translate("MainWindow", "\342\234\205  Sauvegarder", nullptr));
        btnCancel_CmdMod->setText(QCoreApplication::translate("MainWindow", "\342\235\214  Annuler", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
