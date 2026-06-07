#ifndef ARDUINO_H
#define ARDUINO_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include <QString>
#include <QDebug>

class Arduino
{
public:
    Arduino();
    ~Arduino();

    int connect_arduino();
    // Connect to the first available Arduino-class port whose name is
    // not in the excludePorts list. Used to attach the second Arduino
    // (servo controller) without grabbing the main Arduino's port.
    int connect_arduino_excluding(const QStringList &excludePorts);
    // Open a specific COM port directly (no auto-detection). Used when
    // the app already knows which port belongs to which Arduino role,
    // e.g. after an IDENT-line auto-swap.
    int open_specific_port(const QString &portName);
    int close_arduino();
    void write_to_arduino(QByteArray data);
    QByteArray read_from_arduino();
    QSerialPort *getserial();
    QString getarduino_port_name();
    bool isAvailable() const { return arduino_is_available; }

private:
    QSerialPort *serial;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_producy_id = 67;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray data;
};

#endif // ARDUINO_H
