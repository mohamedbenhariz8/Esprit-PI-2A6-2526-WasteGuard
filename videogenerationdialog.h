#ifndef VIDEOGENERATIONDIALOG_H
#define VIDEOGENERATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QTimer>
#include <QPropertyAnimation>
#include <QProcess>
#include <QPlainTextEdit>

class VideoGenerationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VideoGenerationDialog(const QString &productName, 
                                   const QString &reference, 
                                   int capacity, 
                                   int battery, 
                                   const QStringList &features, 
                                   const QString &imagePath,
                                   QWidget *parent = nullptr);
    ~VideoGenerationDialog();

private slots:
    void onGenerateClicked();
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void closeDialog();

private:
    void setupUi();

    QString m_productName;
    QString m_reference;
    int m_capacity;
    int m_battery;
    QStringList m_features;
    QString m_imagePath;

    QLabel *m_lblTitle;
    QLabel *m_lblStatus;
    QLabel *m_lblProductInfo;
    QProgressBar *m_progressBar;
    QPlainTextEdit *m_logConsole;
    QPushButton *m_btnClose;
    QPushButton *m_btnGenerate;
    
    QProcess *m_nodeProcess;
};

#endif // VIDEOGENERATIONDIALOG_H
