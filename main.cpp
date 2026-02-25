#include "mainwindow.h"
#include "mascotwidget.h"
#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QEvent>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QTimer>
#include <QVideoWidget>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QMediaDevices>
#include <QMessageBox>
#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>
// Simple Login Dialog
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("WasteGuard - Connexion");
        setMinimumSize(450, 650);
        setStyleSheet("QDialog { background-color: #1F110B; }"); // Terra Night
        
        this->setFixedSize(450, 600);
        this->setWindowFlags(Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);

        // --- LUXE-BLUE PALETTE ---
        QString blueBase = "#0F1A2B";
        QString accentBlue = "#3498DB";
        QString lightBlue = "#EBF5FB";

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QFrame *container = new QFrame(this);
        container->setObjectName("loginContainer");
        container->setStyleSheet(QString(
            "QFrame#loginContainer {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);"
            "   border: 2px solid rgba(52, 152, 219, 0.3);"
            "   border-radius: 40px;"
            "}"
            "QLineEdit {"
            "   background: rgba(255, 255, 255, 0.08);"
            "   border: 1px solid rgba(52, 152, 219, 0.4);"
            "   border-radius: 12px;"
            "   padding: 12px;"
            "   color: white;"
            "   font-size: 14px;"
            "}"
            "QLineEdit:focus {"
            "   border: 2px solid %3;"
            "   background: rgba(255, 255, 255, 0.12);"
            "}"
            "QPushButton#btnLogin {"
            "   background: %3;"
            "   color: white;"
            "   border-radius: 15px;"
            "   padding: 15px;"
            "   font-weight: 800;"
            "   font-size: 16px;"
            "}"
            "QPushButton#btnLogin:hover {"
            "   background: #2980B9;"
            "}"
        ).arg(blueBase).arg("#1A2F45").arg(accentBlue));
        
        QVBoxLayout *containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(40, 20, 40, 40);
        containerLayout->setSpacing(15);
        
        // --- INNOVATIVE MASCOT ---
        mascot = new MascotWidget(this);
        mascot->setMinimumHeight(240);
        containerLayout->addWidget(mascot);

        // --- MASCOT NAME LABEL ---
        QLabel *lblMascotName = new QLabel("LABIB", this);
        lblMascotName->setAlignment(Qt::AlignCenter);
        // Added margin-bottom: 20px and increased container height to prevent clipping
        lblMascotName->setStyleSheet(QString("font-size: 14px; font-weight: 800; color: #00F2FE; letter-spacing: 4px; margin-top: 5px; margin-bottom: 15px;"));
        containerLayout->addWidget(lblMascotName);

        // Stacked widget for login/face ID pages
        stackedWidget = new QStackedWidget(this);
        
        // PAGE 0: Login form
        QWidget *pageLogin = new QWidget();
        QVBoxLayout *loginLayout = new QVBoxLayout(pageLogin);
        loginLayout->setSpacing(12);
        
        // Title
        QLabel *lblTitle = new QLabel("ACCÈS SÉCURISÉ", this);
        lblTitle->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1; letter-spacing: 2px;").arg(accentBlue));
        lblTitle->setAlignment(Qt::AlignCenter);
        loginLayout->addWidget(lblTitle);
        
        // Username input
        txtUser = new QLineEdit(this);
        txtUser->setPlaceholderText("Identifiant");
        // Stylesheet handled by container
        txtUser->setMinimumHeight(48);
        loginLayout->addWidget(txtUser);
        
        // Password input
        txtPass = new QLineEdit(this);
        txtPass->setPlaceholderText("Mot de passe");
        txtPass->setEchoMode(QLineEdit::Password);
        // Stylesheet handled by container
        txtPass->setMinimumHeight(48);
        loginLayout->addWidget(txtPass);
        
        // Login button
        QPushButton *btnLogin = new QPushButton("CONNEXION", this);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setMinimumHeight(52);
        btnLogin->setCursor(Qt::PointingHandCursor);
        loginLayout->addWidget(btnLogin);
        
        // Face ID button  
        QPushButton *btnFaceId = new QPushButton("BIOMÉTRIE FACIALE", this);
        btnFaceId->setStyleSheet(QString(
            "QPushButton { background: transparent; color: %1; font-weight: 700; "
            "font-size: 13px; border-radius: 10px; padding: 10px; border: 1px solid rgba(52, 152, 219, 0.5); } "
            "QPushButton:hover { background-color: rgba(52, 152, 219, 0.1); }"
        ).arg(accentBlue));
        btnFaceId->setMinimumHeight(45);
        btnFaceId->setCursor(Qt::PointingHandCursor);
        loginLayout->addWidget(btnFaceId);
        
        // PAGE 1: Face ID
        QWidget *pageFaceId = new QWidget();
        QVBoxLayout *faceLayout = new QVBoxLayout(pageFaceId);
        faceLayout->setSpacing(15);
        
        QLabel *lblFaceTitle = new QLabel("SCAN BIOMÉTRIQUE", this);
        lblFaceTitle->setStyleSheet("font-size: 18px; font-weight: 800; color: #E9BC99;");
        lblFaceTitle->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblFaceTitle);
        
        // Camera placeholder
        videoWidget = new QWidget(this);
        videoWidget->setMinimumSize(320, 240);
        videoWidget->setStyleSheet(QString("background-color: rgba(0,0,0,0.5); border: 2px solid %1; border-radius: 15px;").arg(accentBlue));
        faceLayout->addWidget(videoWidget);
        
        lblStatus = new QLabel("Initialisation du capteur...", this);
        lblStatus->setStyleSheet(QString("font-size: 14px; color: %1;").arg(lightBlue));
        lblStatus->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblStatus);
        
        QPushButton *btnBack = new QPushButton("Annuler et revenir", this);
        btnBack->setStyleSheet(QString("color: %1; text-decoration: underline; font-size: 12px; border: none;").arg(accentBlue));
        btnBack->setFlat(true);
        btnBack->setCursor(Qt::PointingHandCursor);
        faceLayout->addWidget(btnBack);
        
        // Add pages to stacked widget
        stackedWidget->addWidget(pageLogin);
        stackedWidget->addWidget(pageFaceId);
        stackedWidget->setCurrentIndex(0);
        
        containerLayout->addWidget(stackedWidget);
        mainLayout->addWidget(container);
        
        // --- MASCOT INTERACTION LOGIC ---
        // Animation for covering eyes
        eyeAnim = new QPropertyAnimation(mascot, "eyeCoverFactor", this);
        eyeAnim->setDuration(450);
        eyeAnim->setEasingCurve(QEasingCurve::InOutBack);

        // Event filter for password focus
        txtPass->installEventFilter(this);
        txtUser->installEventFilter(this); // Install filter on username too
        setMouseTracking(true);
        pageLogin->setMouseTracking(true);
        
        // Connections
        connect(btnLogin, &QPushButton::clicked, this, &QDialog::accept);
        connect(txtPass, &QLineEdit::returnPressed, this, &QDialog::accept);
        connect(btnFaceId, &QPushButton::clicked, this, [this](){
            startFaceId();
            mascot->setState(MascotWidget::Magnifier); // Trigger Magnifier
        });
        connect(btnBack, &QPushButton::clicked, [this](){ 
            stackedWidget->setCurrentIndex(0); 
            mascot->setState(MascotWidget::Idle); // Reset to Idle
        });
        
        // Notepad interactions
        connect(txtUser, &QLineEdit::textEdited, this, [this](){
             mascot->setState(MascotWidget::Notepad);
             // Revert to idle after 1.5s of no typing
             QTimer::singleShot(1500, this, [this](){
                 if (!txtUser->hasFocus()) mascot->setState(MascotWidget::Idle);
             });
        });
    }

    bool eventFilter(QObject *obj, QEvent *event) override {
        if (obj == txtPass) {
            if (event->type() == QEvent::FocusIn) {
                eyeAnim->stop();
                eyeAnim->setEndValue(1.0);
                eyeAnim->start();
                mascot->setState(MascotWidget::Idle); // Ensure no props when hiding eyes
            } else if (event->type() == QEvent::FocusOut) {
                eyeAnim->stop();
                eyeAnim->setEndValue(0.0);
                eyeAnim->start();
            }
        }
        else if (obj == txtUser) {
            if (event->type() == QEvent::FocusIn) {
                mascot->setState(MascotWidget::Notepad);
            } else if (event->type() == QEvent::FocusOut) {
                mascot->setState(MascotWidget::Idle);
            }
        }
        return QDialog::eventFilter(obj, event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        mascot->setLookAt(event->globalPosition().toPoint());
        QDialog::mouseMoveEvent(event);
    }
    
private slots:
    void startFaceId() {
        stackedWidget->setCurrentIndex(1);
        lblStatus->setText("Scan de rétine en cours...");
        lblStatus->setStyleSheet("font-size: 16px; color: #C66A4E; font-weight: 800;");
        
        QTimer::singleShot(2000, this, [this](){
            lblStatus->setText("Analyse des vecteurs...");
            lblStatus->setStyleSheet("font-size: 16px; color: #E9BC99; font-weight: 800;");
            
            QTimer::singleShot(2000, this, [this](){
                lblStatus->setText("VÉRIFIÉ");
                lblStatus->setStyleSheet("font-size: 20px; color: #8cc63f; font-weight: 900;");
                QTimer::singleShot(800, this, &QDialog::accept);
            });
        });
    }
    
private:
    MascotWidget *mascot;
    QStackedWidget *stackedWidget;
    QLineEdit *txtUser;
    QLineEdit *txtPass;
    QLabel *lblStatus;
    QWidget *videoWidget;
    QPropertyAnimation *eyeAnim;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "Drivers disponibles:" << QSqlDatabase::drivers();

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0; // User cancelled
    }

    Connection* c = Connection::instance();
    bool test = c->createConnect();
    if (test)
    {
        MainWindow w;
        w.showMaximized();
        QMessageBox::information(nullptr, QObject::tr("database is open"),
                                 QObject::tr("connection successful.\n"
                                             "Click Cancel to exit."), QMessageBox::Cancel);
        return a.exec();

    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("database is not open"),
                              QObject::tr("connection failed.\n"
                                          "Click Cancel to exit."), QMessageBox::Cancel);
        return 0;
    }

}

#include "main.moc"
