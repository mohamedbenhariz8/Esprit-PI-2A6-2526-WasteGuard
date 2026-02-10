#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void openAjoutPopup();
    void openModifierPopup();
    void closePopup();
    void openStatsPopup();

    void handleEditClicked();
    void handleDeleteClicked();

private:
    Ui::MainWindow *ui;

    void showPopup(int pageIndex); // 0=Ajout, 1=Modifier
    void refreshActionButtons();
    void buildStatsCharts();

    void addExampleRow();
    void installActionButtonsForRow(int row);
};

#endif // MAINWINDOW_H
