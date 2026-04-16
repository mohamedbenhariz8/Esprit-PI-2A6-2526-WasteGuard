#include "arduinowidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QTextEdit>
#include <QDial>
#include <QScrollArea>
#include <QRegularExpression>
#include <QDebug>

static const char *kPageStyle = R"(
QWidget#arduinoRoot {
    background-color: #f4f6f9;
}
QLabel#ardTitle {
    color: #0f2b4c;
    font-size: 22px;
    font-weight: 800;
    padding: 2px 0;
}
QLabel#ardSubtitle {
    color: #5a6b82;
    font-size: 13px;
}
QFrame#ardTopBar {
    background-color: #ffffff;
    border: 1px solid #e3e7ee;
    border-radius: 14px;
}
QFrame#ardCard {
    background-color: #ffffff;
    border: 1px solid #e3e7ee;
    border-radius: 14px;
}
QFrame#ardCard QLabel#cardTitle {
    color: #0f2b4c;
    font-size: 14px;
    font-weight: 700;
}
QFrame#ardCard QLabel#cardIcon {
    font-size: 34px;
    color: #0f2b4c;
}
QFrame#ardCard QLabel#cardValue {
    color: #1a4270;
    font-size: 15px;
    font-weight: 700;
}
QLabel#dot {
    min-width: 14px;
    max-width: 14px;
    min-height: 14px;
    max-height: 14px;
    border-radius: 7px;
    background-color: #d0d6de;
    border: 1px solid #c2c9d3;
}
QPushButton#cardBtn {
    background-color: #0f2b4c;
    color: white;
    border: none;
    border-radius: 8px;
    padding: 8px 14px;
    font-weight: 700;
    font-size: 12px;
}
QPushButton#cardBtn:hover { background-color: #1a4270; }
QPushButton#cardBtn:checked {
    background-color: #3498db;
}
QPushButton#reconnectBtn {
    background-color: #3498db;
    color: white;
    border: none;
    border-radius: 8px;
    padding: 8px 16px;
    font-weight: 700;
}
QPushButton#reconnectBtn:hover { background-color: #2d87c4; }

QLabel#connPill {
    border-radius: 10px;
    padding: 5px 12px;
    font-weight: 700;
    font-size: 12px;
}
QLabel#sectionLabel {
    color: #0f2b4c;
    font-size: 16px;
    font-weight: 800;
    padding: 6px 2px;
    border-bottom: 2px solid #e3e7ee;
}
QTextEdit#ardLog {
    background-color: #0f2b4c;
    color: #d6e4f5;
    border: 1px solid #0f2b4c;
    border-radius: 10px;
    font-family: Consolas, "Courier New", monospace;
    font-size: 12px;
}
QDial {
    background-color: #eaf2fb;
}
)";

ArduinoWidget::ArduinoWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("arduinoRoot");
    setStyleSheet(kPageStyle);

    buildUi();

    int ret = m_ard.connect_arduino();
    switch (ret) {
    case 0:
        m_connStatus->setText("CONNECTE");
        m_connStatus->setStyleSheet("background-color:#e6f6ec;color:#1e7e34;");
        m_portLabel->setText(QString("Port : %1").arg(m_ard.getarduino_port_name()));
        break;
    case 1:
        m_connStatus->setText("PORT TROUVE - ECHEC OUVERTURE");
        m_connStatus->setStyleSheet("background-color:#fff3cd;color:#856404;");
        m_portLabel->setText(QString("Port : %1").arg(m_ard.getarduino_port_name()));
        break;
    default:
        m_connStatus->setText("NON CONNECTE");
        m_connStatus->setStyleSheet("background-color:#fdecea;color:#a02620;");
        m_portLabel->setText("Port : (aucun)");
        break;
    }

    if (m_ard.getserial()) {
        connect(m_ard.getserial(), &QSerialPort::readyRead, this, &ArduinoWidget::onSerialData);
    }

    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(150);
    connect(m_refreshTimer, &QTimer::timeout, this, &ArduinoWidget::refreshUiState);
    m_refreshTimer->start();
}

ArduinoWidget::~ArduinoWidget()
{
    m_ard.close_arduino();
}

QFrame *ArduinoWidget::makeCard(const QString &title, QLabel *&valueLabel, QLabel *&iconLabel,
                                 QPushButton *&toggleBtn, const QString &icon)
{
    auto *card = new QFrame;
    card->setObjectName("ardCard");
    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(16, 14, 16, 14);
    v->setSpacing(10);

    auto *topRow = new QHBoxLayout;
    topRow->setSpacing(10);

    iconLabel = new QLabel(icon);
    iconLabel->setObjectName("cardIcon");

    auto *titleLbl = new QLabel(title);
    titleLbl->setObjectName("cardTitle");

    auto *dot = new QLabel;
    dot->setObjectName("dot");

    topRow->addWidget(iconLabel);
    topRow->addWidget(titleLbl, 1);
    topRow->addWidget(dot);

    valueLabel = new QLabel("--");
    valueLabel->setObjectName("cardValue");

    toggleBtn = new QPushButton("Simuler appui");
    toggleBtn->setObjectName("cardBtn");
    toggleBtn->setCheckable(true);
    toggleBtn->setCursor(Qt::PointingHandCursor);

    v->addLayout(topRow);
    v->addWidget(valueLabel);
    v->addWidget(toggleBtn);

    // Stash the dot pointer in a property so we can retrieve it from caller context.
    card->setProperty("dotPtr", QVariant::fromValue<void*>(dot));
    return card;
}

void ArduinoWidget::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(22, 18, 22, 18);
    root->setSpacing(16);

    // ---- Top bar ----
    auto *topBar = new QFrame;
    topBar->setObjectName("ardTopBar");
    auto *tbl = new QHBoxLayout(topBar);
    tbl->setContentsMargins(20, 14, 20, 14);
    tbl->setSpacing(14);

    auto *titleWrap = new QVBoxLayout;
    titleWrap->setSpacing(2);
    auto *title = new QLabel(QString::fromUtf8("Arduino — Capteurs & Actionneurs"));
    title->setObjectName("ardTitle");
    auto *sub = new QLabel(QString::fromUtf8("Supervision temps réel des boutons, LEDs, servos et du capteur PIR"));
    sub->setObjectName("ardSubtitle");
    titleWrap->addWidget(title);
    titleWrap->addWidget(sub);
    tbl->addLayout(titleWrap, 1);

    m_portLabel = new QLabel("Port : (…)");
    m_portLabel->setStyleSheet("color:#5a6b82;font-weight:600;");
    tbl->addWidget(m_portLabel);

    m_connStatus = new QLabel("…");
    m_connStatus->setObjectName("connPill");
    m_connStatus->setAlignment(Qt::AlignCenter);
    m_connStatus->setMinimumWidth(150);
    tbl->addWidget(m_connStatus);

    m_reconnectBtn = new QPushButton("Reconnecter");
    m_reconnectBtn->setObjectName("reconnectBtn");
    m_reconnectBtn->setCursor(Qt::PointingHandCursor);
    connect(m_reconnectBtn, &QPushButton::clicked, this, &ArduinoWidget::reconnect);
    tbl->addWidget(m_reconnectBtn);

    root->addWidget(topBar);

    // ---- Scroll area so the page stays friendly in small windows ----
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea{background:transparent;}");
    auto *content = new QWidget;
    content->setStyleSheet("background:transparent;");
    auto *contentLay = new QVBoxLayout(content);
    contentLay->setContentsMargins(0, 0, 0, 0);
    contentLay->setSpacing(16);

    // ---- PIR section ----
    {
        auto *pirCard = new QFrame;
        pirCard->setObjectName("ardCard");
        auto *pl = new QHBoxLayout(pirCard);
        pl->setContentsMargins(22, 18, 22, 18);
        pl->setSpacing(18);

        auto *pirIcon = new QLabel(QString::fromUtf8("🛰️"));
        pirIcon->setObjectName("cardIcon");
        pirIcon->setStyleSheet("font-size:48px;");
        pl->addWidget(pirIcon);

        auto *textWrap = new QVBoxLayout;
        auto *pirTitle = new QLabel(QString::fromUtf8("Capteur de mouvement (PIR)"));
        pirTitle->setObjectName("cardTitle");
        pirTitle->setStyleSheet("font-size:16px;");
        m_pirValue = new QLabel(QString::fromUtf8("Aucun mouvement"));
        m_pirValue->setObjectName("cardValue");
        m_pirValue->setStyleSheet("font-size:18px;");
        textWrap->addWidget(pirTitle);
        textWrap->addWidget(m_pirValue);
        pl->addLayout(textWrap, 1);

        m_pirDot = new QLabel;
        m_pirDot->setObjectName("dot");
        m_pirDot->setStyleSheet("min-width:22px;max-width:22px;min-height:22px;max-height:22px;border-radius:11px;background:#d0d6de;border:1px solid #c2c9d3;");
        pl->addWidget(m_pirDot);

        contentLay->addWidget(pirCard);
    }

    // ---- Buttons section ----
    {
        auto *lbl = new QLabel(QString::fromUtf8("Boutons"));
        lbl->setObjectName("sectionLabel");
        contentLay->addWidget(lbl);

        auto *grid = new QGridLayout;
        grid->setSpacing(14);
        const QStringList icons = {"1", "2", "3", "4"};
        for (int i = 0; i < 4; ++i) {
            QLabel *val = nullptr;
            QLabel *ico = nullptr;
            QPushButton *tog = nullptr;
            QFrame *card = makeCard(QString("Bouton %1").arg(i + 1), val, ico, tog, icons[i]);
            ico->setText(QString::fromUtf8("🔘"));
            m_btnValue[i] = val;
            m_btnToggle[i] = tog;
            m_btnDot[i] = static_cast<QLabel*>(card->property("dotPtr").value<void*>());
            const int idx = i;
            connect(tog, &QPushButton::toggled, this, [this, idx](bool on) {
                m_uiBtn[idx] = on;
                // Mirror as if pressed so downstream LEDs/servos react in the UI simulation.
                if (on) m_lastBtn[idx] = QDateTime::currentDateTime();
            });
            grid->addWidget(card, i / 2, i % 2);
        }
        contentLay->addLayout(grid);
    }

    // ---- LEDs section ----
    {
        auto *lbl = new QLabel(QString::fromUtf8("LEDs"));
        lbl->setObjectName("sectionLabel");
        contentLay->addWidget(lbl);

        auto *row = new QHBoxLayout;
        row->setSpacing(14);
        for (int i = 0; i < 2; ++i) {
            auto *card = new QFrame;
            card->setObjectName("ardCard");
            auto *v = new QVBoxLayout(card);
            v->setContentsMargins(18, 16, 18, 16);
            v->setSpacing(8);

            auto *top = new QHBoxLayout;
            auto *ic = new QLabel(QString::fromUtf8("💡"));
            ic->setObjectName("cardIcon");
            auto *t = new QLabel(QString("LED %1").arg(i + 1));
            t->setObjectName("cardTitle");
            auto *dot = new QLabel;
            dot->setObjectName("dot");
            top->addWidget(ic);
            top->addWidget(t, 1);
            top->addWidget(dot);

            auto *val = new QLabel("OFF");
            val->setObjectName("cardValue");
            val->setStyleSheet("font-size:18px;");

            v->addLayout(top);
            v->addWidget(val);

            m_ledDot[i] = dot;
            m_ledValue[i] = val;
            row->addWidget(card);
        }
        contentLay->addLayout(row);
    }

    // ---- Servos section ----
    {
        auto *lbl = new QLabel(QString::fromUtf8("Servo-moteurs"));
        lbl->setObjectName("sectionLabel");
        contentLay->addWidget(lbl);

        auto *row = new QHBoxLayout;
        row->setSpacing(14);
        for (int i = 0; i < 2; ++i) {
            auto *card = new QFrame;
            card->setObjectName("ardCard");
            auto *v = new QVBoxLayout(card);
            v->setContentsMargins(18, 16, 18, 16);
            v->setSpacing(10);

            auto *top = new QHBoxLayout;
            auto *ic = new QLabel(QString::fromUtf8("⚙️"));
            ic->setObjectName("cardIcon");
            auto *t = new QLabel(QString("Servo %1").arg(i + 1));
            t->setObjectName("cardTitle");
            top->addWidget(ic);
            top->addWidget(t, 1);

            auto *dial = new QDial;
            dial->setRange(0, 180);
            dial->setValue(90);
            dial->setNotchesVisible(true);
            dial->setEnabled(false);
            dial->setFixedHeight(110);

            auto *val = new QLabel(QString::fromUtf8("90°"));
            val->setObjectName("cardValue");
            val->setAlignment(Qt::AlignCenter);
            val->setStyleSheet("font-size:20px;");

            v->addLayout(top);
            v->addWidget(dial);
            v->addWidget(val);

            m_servoDial[i] = dial;
            m_servoValue[i] = val;
            row->addWidget(card);
        }
        contentLay->addLayout(row);
    }

    // ---- Log ----
    {
        auto *lbl = new QLabel(QString::fromUtf8("Journal série"));
        lbl->setObjectName("sectionLabel");
        contentLay->addWidget(lbl);
        m_log = new QTextEdit;
        m_log->setObjectName("ardLog");
        m_log->setReadOnly(true);
        m_log->setFixedHeight(150);
        contentLay->addWidget(m_log);
    }

    contentLay->addStretch();
    scroll->setWidget(content);
    root->addWidget(scroll, 1);
}

void ArduinoWidget::applyIndicatorStyle(QLabel *dot, bool active, const QString &activeColor)
{
    if (!dot) return;
    if (active) {
        dot->setStyleSheet(QString("min-width:14px;max-width:14px;min-height:14px;max-height:14px;"
                                   "border-radius:7px;background:%1;border:1px solid %1;").arg(activeColor));
    } else {
        dot->setStyleSheet("min-width:14px;max-width:14px;min-height:14px;max-height:14px;"
                           "border-radius:7px;background:#d0d6de;border:1px solid #c2c9d3;");
    }
}

void ArduinoWidget::onSerialData()
{
    QByteArray chunk = m_ard.read_from_arduino();
    if (chunk.isEmpty()) return;
    m_buffer.append(chunk);
    int idx;
    while ((idx = m_buffer.indexOf('\n')) != -1) {
        QByteArray line = m_buffer.left(idx);
        m_buffer.remove(0, idx + 1);
        QString s = QString::fromUtf8(line).trimmed();
        if (s.isEmpty()) continue;
        processLine(s);
    }
}

void ArduinoWidget::processLine(const QString &line)
{
    if (m_log) {
        m_log->append(QString("[%1]  %2")
                      .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                      .arg(line));
    }

    QDateTime now = QDateTime::currentDateTime();
    static const QRegularExpression reBtn("BTN(\\d)");
    auto m = reBtn.match(line);
    if (m.hasMatch()) {
        int n = m.captured(1).toInt();
        if (n >= 1 && n <= 4) m_lastBtn[n - 1] = now;
    }
    if (line.contains("PIR", Qt::CaseInsensitive) && line.contains("Motion", Qt::CaseInsensitive)) {
        m_lastPir = now;
    }
}

void ArduinoWidget::refreshUiState()
{
    QDateTime now = QDateTime::currentDateTime();
    const qint64 pressedMs = 400; // considered pressed if activity within this window

    // Buttons (physical OR UI-simulated)
    bool btnState[4];
    for (int i = 0; i < 4; ++i) {
        bool phys = m_lastBtn[i].isValid() && m_lastBtn[i].msecsTo(now) < pressedMs;
        btnState[i] = phys || m_uiBtn[i];
        applyIndicatorStyle(m_btnDot[i], btnState[i], "#28a745");
        if (m_btnValue[i]) m_btnValue[i]->setText(btnState[i] ? QString::fromUtf8("Appuyé (LOW)") : QString::fromUtf8("Relâché (HIGH)"));
        if (m_btnToggle[i]) {
            m_btnToggle[i]->setText(btnState[i] ? QString::fromUtf8("Appuyé") : QString::fromUtf8("Simuler appui"));
        }
    }

    // PIR
    bool pir = m_lastPir.isValid() && m_lastPir.msecsTo(now) < 1200;
    applyIndicatorStyle(m_pirDot, pir, "#e67e22");
    if (m_pirDot) {
        if (pir) {
            m_pirDot->setStyleSheet("min-width:22px;max-width:22px;min-height:22px;max-height:22px;"
                                    "border-radius:11px;background:#e67e22;border:1px solid #e67e22;");
        } else {
            m_pirDot->setStyleSheet("min-width:22px;max-width:22px;min-height:22px;max-height:22px;"
                                    "border-radius:11px;background:#d0d6de;border:1px solid #c2c9d3;");
        }
    }
    if (m_pirValue) m_pirValue->setText(pir ? QString::fromUtf8("MOUVEMENT DETECTE") : QString::fromUtf8("Aucun mouvement"));

    // LEDs (mirror Arduino logic: LED1 on if BTN1 OR PIR, LED2 on if BTN2 OR PIR)
    bool led1 = btnState[0] || pir;
    bool led2 = btnState[1] || pir;
    const bool leds[2] = { led1, led2 };
    for (int i = 0; i < 2; ++i) {
        applyIndicatorStyle(m_ledDot[i], leds[i], "#f1c40f");
        if (m_ledValue[i]) {
            m_ledValue[i]->setText(leds[i] ? "ON" : "OFF");
            m_ledValue[i]->setStyleSheet(leds[i]
                ? "font-size:18px;color:#b8860b;font-weight:800;"
                : "font-size:18px;color:#7a8699;font-weight:700;");
        }
    }

    // Servos (mirror Arduino logic: BTN3 => servo1=0 else 90 ; BTN4 => servo2=180 else 90)
    int s1 = btnState[2] ? 0   : 90;
    int s2 = btnState[3] ? 180 : 90;
    if (m_servoDial[0]) m_servoDial[0]->setValue(s1);
    if (m_servoDial[1]) m_servoDial[1]->setValue(s2);
    if (m_servoValue[0]) m_servoValue[0]->setText(QString::fromUtf8("%1°").arg(s1));
    if (m_servoValue[1]) m_servoValue[1]->setText(QString::fromUtf8("%1°").arg(s2));
}

void ArduinoWidget::reconnect()
{
    m_ard.close_arduino();
    int ret = m_ard.connect_arduino();
    if (ret == 0) {
        m_connStatus->setText("CONNECTE");
        m_connStatus->setStyleSheet("background-color:#e6f6ec;color:#1e7e34;");
        m_portLabel->setText(QString("Port : %1").arg(m_ard.getarduino_port_name()));
        if (m_ard.getserial()) {
            connect(m_ard.getserial(), &QSerialPort::readyRead, this, &ArduinoWidget::onSerialData,
                    Qt::UniqueConnection);
        }
        if (m_log) m_log->append(QString::fromUtf8("[info] Connecté sur ") + m_ard.getarduino_port_name());
    } else {
        m_connStatus->setText("NON CONNECTE");
        m_connStatus->setStyleSheet("background-color:#fdecea;color:#a02620;");
        if (m_log) m_log->append(QString::fromUtf8("[erreur] Echec de la connexion (code ") + QString::number(ret) + ")");
    }
}
