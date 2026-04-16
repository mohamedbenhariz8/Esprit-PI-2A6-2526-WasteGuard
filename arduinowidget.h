#ifndef ARDUINOWIDGET_H
#define ARDUINOWIDGET_H

#include <QWidget>
#include <QByteArray>
#include <QString>
#include <QDateTime>

#include "arduino.h"

class QLabel;
class QPushButton;
class QFrame;
class QTimer;
class QTextEdit;
class QDial;

class ArduinoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArduinoWidget(QWidget *parent = nullptr);
    ~ArduinoWidget();

private slots:
    void onSerialData();
    void refreshUiState();
    void reconnect();

private:
    void buildUi();
    QFrame *makeCard(const QString &title, QLabel *&valueLabel, QLabel *&iconLabel,
                     QPushButton *&toggleBtn, const QString &icon);
    void processLine(const QString &line);
    void applyIndicatorStyle(QLabel *dot, bool active, const QString &activeColor);

    Arduino m_ard;
    QByteArray m_buffer;

    // Physical timestamps (when we last saw activity from the Arduino)
    QDateTime m_lastBtn[4];
    QDateTime m_lastPir;

    // UI-simulated toggles (when user clicks in the UI)
    bool m_uiBtn[4] = {false, false, false, false};

    // UI widgets
    QLabel *m_connStatus = nullptr;
    QLabel *m_portLabel = nullptr;
    QPushButton *m_reconnectBtn = nullptr;

    QLabel *m_pirDot = nullptr;
    QLabel *m_pirValue = nullptr;

    QLabel *m_btnDot[4] = {nullptr, nullptr, nullptr, nullptr};
    QLabel *m_btnValue[4] = {nullptr, nullptr, nullptr, nullptr};
    QPushButton *m_btnToggle[4] = {nullptr, nullptr, nullptr, nullptr};

    QLabel *m_ledDot[2] = {nullptr, nullptr};
    QLabel *m_ledValue[2] = {nullptr, nullptr};

    QLabel *m_servoValue[2] = {nullptr, nullptr};
    QDial  *m_servoDial[2] = {nullptr, nullptr};

    QTextEdit *m_log = nullptr;

    QTimer *m_refreshTimer = nullptr;
};

#endif // ARDUINOWIDGET_H
