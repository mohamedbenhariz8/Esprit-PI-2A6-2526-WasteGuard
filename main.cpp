#include "mainwindow.h"
#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QVideoWidget>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QMediaDevices>
#include <QMessageBox>

// Simple Login Dialog
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("WasteGuard - Connexion");
        setMinimumSize(450, 550);
        setStyleSheet("QDialog { background-color: #0f2c4f; }");
        
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(40, 40, 40, 40);
        mainLayout->setSpacing(20);
        
        // Stacked widget for login/face ID pages
        stackedWidget = new QStackedWidget(this);
        
        // PAGE 0: Login form
        QWidget *pageLogin = new QWidget();
        QVBoxLayout *loginLayout = new QVBoxLayout(pageLogin);
        loginLayout->setSpacing(15);
        
        // Logo spacer
        loginLayout->addStretch();
        
        // Title
        QLabel *lblTitle = new QLabel("Bienvenue", this);
        lblTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
        lblTitle->setAlignment(Qt::AlignCenter);
        loginLayout->addWidget(lblTitle);
        
        // Username input
        txtUser = new QLineEdit(this);
        txtUser->setPlaceholderText("Nom d'utilisateur");
        txtUser->setStyleSheet(
            "QLineEdit { background-color: white; border: 1px solid #cfd8dc; "
            "border-radius: 8px; padding: 12px; color: #37474f; font-size: 14px; }"
        );
        txtUser->setMinimumHeight(45);
        loginLayout->addWidget(txtUser);
        
        // Password input
        txtPass = new QLineEdit(this);
        txtPass->setPlaceholderText("Mot de passe");
        txtPass->setEchoMode(QLineEdit::Password);
        txtPass->setStyleSheet(txtUser->styleSheet());
        txtPass->setMinimumHeight(45);
        loginLayout->addWidget(txtPass);
        
        // Login button
        QPushButton *btnLogin = new QPushButton("SE CONNECTER", this);
        btnLogin->setStyleSheet(
            "QPushButton { background-color: #8cc63f; color: white; font-weight: bold; "
            "font-size: 15px; border-radius: 8px; padding: 12px; } "
            "QPushButton:hover { background-color: #7ab234; }"
        );
        btnLogin->setMinimumHeight(50);
        btnLogin->setCursor(Qt::PointingHandCursor);
        loginLayout->addWidget(btnLogin);
        
        // Face ID button  
        QPushButton *btnFaceId = new QPushButton("SE CONNECTER AVEC FACE ID", this);
        btnFaceId->setStyleSheet(
            "QPushButton { background-color: #3b6998; color: white; font-weight: bold; "
            "font-size: 14px; border-radius: 8px; padding: 10px; } "
            "QPushButton:hover { background-color: #2d5278; }"
        );
        btnFaceId->setMinimumHeight(45);
        btnFaceId->setCursor(Qt::PointingHandCursor);
        loginLayout->addWidget(btnFaceId);
        
        loginLayout->addStretch();
        
        // PAGE 1: Face ID
        QWidget *pageFaceId = new QWidget();
        QVBoxLayout *faceLayout = new QVBoxLayout(pageFaceId);
        faceLayout->setSpacing(15);
        
        QLabel *lblFaceTitle = new QLabel("Reconnaissance Faciale", this);
        lblFaceTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
        lblFaceTitle->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblFaceTitle);
        
        // Camera placeholder
        videoWidget = new QWidget(this);
        videoWidget->setMinimumSize(320, 240);
        videoWidget->setStyleSheet("background-color: black; border-radius: 10px;");
        faceLayout->addWidget(videoWidget);
        
        lblStatus = new QLabel("Initialisation...", this);
        lblStatus->setStyleSheet("font-size: 14px; color: white;");
        lblStatus->setAlignment(Qt::AlignCenter);
        faceLayout->addWidget(lblStatus);
        
        QPushButton *btnBack = new QPushButton("Retour au mot de passe", this);
        btnBack->setStyleSheet("color: #8cc63f; text-decoration: underline; font-size: 13px; border: none;");
        btnBack->setFlat(true);
        btnBack->setCursor(Qt::PointingHandCursor);
        faceLayout->addWidget(btnBack);
        
        // Add pages to stacked widget
        stackedWidget->addWidget(pageLogin);
        stackedWidget->addWidget(pageFaceId);
        stackedWidget->setCurrentIndex(0);
        
        mainLayout->addWidget(stackedWidget);
        
        // Connections
        connect(btnLogin, &QPushButton::clicked, this, &QDialog::accept);
        connect(txtPass, &QLineEdit::returnPressed, this, &QDialog::accept);
        connect(btnFaceId, &QPushButton::clicked, this, &LoginDialog::startFaceId);
        connect(btnBack, &QPushButton::clicked, [this](){ stackedWidget->setCurrentIndex(0); });
    }
    
private slots:
    void startFaceId() {
        stackedWidget->setCurrentIndex(1);
        lblStatus->setText("Recherche de visage...");
        lblStatus->setStyleSheet("font-size: 16px; color: #e67e22; font-weight: bold;");
        
        // Simulate Face ID recognition
        QTimer::singleShot(2000, this, [this](){
            lblStatus->setText("Analyse biométrique...");
            lblStatus->setStyleSheet("font-size: 16px; color: #3498db; font-weight: bold;");
            
            QTimer::singleShot(2000, this, [this](){
                lblStatus->setText("Identité confirmée !");
                lblStatus->setStyleSheet("font-size: 18px; color: #2ecc71; font-weight: bold;");
                
                QTimer::singleShot(500, this, &QDialog::accept);
            });
        });
    }
    
private:
    QStackedWidget *stackedWidget;
    QLineEdit *txtUser;
    QLineEdit *txtPass;
    QLabel *lblStatus;
    QWidget *videoWidget;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Show login dialog first
    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0; // User cancelled
    }
    
    // Login successful, show main window
    MainWindow w;
    w.showMaximized();
    
    return a.exec();
}

#include "main.moc"
