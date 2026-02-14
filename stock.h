#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /* Qt's 'on_widget_signal' pattern requires these to be
       under 'private slots' to work automatically.
    */
    void on_btnNew_clicked();
    void on_btnCancel_add_clicked();
    void on_btnCancel_mod_clicked();

private:
    Ui::MainWindow *ui;
    void setupTableData();
    void setupLogo();
};
#endif // MAINWINDOW_H
