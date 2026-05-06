#include "arduino.h"

Arduino::Arduino()
{
    data = "";
    arduino_port_name = "";
    arduino_is_available = false;
    serial = new QSerialPort;
}

Arduino::~Arduino()
{
    if (serial) {
        if (serial->isOpen()) serial->close();
        delete serial;
        serial = nullptr;
    }
}

QString Arduino::getarduino_port_name()
{
    return arduino_port_name;
}

QSerialPort *Arduino::getserial()
{
    return serial;
}

int Arduino::connect_arduino()
{
    arduino_is_available = false;
    arduino_port_name.clear();

    // 1. Try exact Arduino Uno VID/PID.
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "[Arduino] port:" << info.portName()
                 << "desc:" << info.description()
                 << "mfr:" << info.manufacturer()
                 << "vid:" << (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : "-")
                 << "pid:" << (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : "-");
        if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
            if (info.vendorIdentifier() == arduino_uno_vendor_id
                && info.productIdentifier() == arduino_uno_producy_id) {
                arduino_is_available = true;
                arduino_port_name = info.portName();
                break;
            }
        }
    }

    // 2. Heuristic: match description/manufacturer for clones (CH340, FTDI, etc.)
    //    but skip Bluetooth-style virtual ports that pollute Windows COM list.
    if (!arduino_is_available) {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            const QString desc = info.description().toLower();
            const QString mfr = info.manufacturer().toLower();
            if (desc.contains("bluetooth") || mfr.contains("bluetooth")) continue;
            if (desc.contains("arduino") || desc.contains("ch340")
                || desc.contains("ch341") || desc.contains("usb-serial")
                || desc.contains("ftdi") || desc.contains("cp210")
                || mfr.contains("arduino") || mfr.contains("ftdi")
                || mfr.contains("silicon labs") || mfr.contains("wch")) {
                arduino_is_available = true;
                arduino_port_name = info.portName();
                break;
            }
        }
    }

    // 3. Last-chance fallback: first non-Bluetooth port.
    if (!arduino_is_available) {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            const QString desc = info.description().toLower();
            const QString mfr = info.manufacturer().toLower();
            if (desc.contains("bluetooth") || mfr.contains("bluetooth")) continue;
            arduino_is_available = true;
            arduino_port_name = info.portName();
            break;
        }
    }

    qInfo() << "[Arduino] Selected port:" << arduino_port_name
            << "(available=" << arduino_is_available << ")";

    if (arduino_is_available) {
        if (serial->isOpen()) serial->close();
        serial->setPortName(arduino_port_name);
        if (serial->open(QSerialPort::ReadWrite)) {
            serial->setBaudRate(QSerialPort::Baud9600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            qInfo() << "[Arduino] Serial port opened at 9600 baud.";
            return 0;
        }
        qWarning() << "[Arduino] Failed to open port" << arduino_port_name
                   << ":" << serial->errorString();
        return 1;
    }
    return -1;
}

int Arduino::connect_arduino_excluding(const QStringList &excludePorts)
{
    arduino_is_available = false;
    arduino_port_name.clear();

    auto isExcluded = [&](const QString &name) {
        return excludePorts.contains(name, Qt::CaseInsensitive);
    };

    // 1. VID/PID match, skipping excluded ports.
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (isExcluded(info.portName())) continue;
        if (info.hasVendorIdentifier() && info.hasProductIdentifier()
            && info.vendorIdentifier()  == arduino_uno_vendor_id
            && info.productIdentifier() == arduino_uno_producy_id) {
            arduino_is_available = true;
            arduino_port_name    = info.portName();
            break;
        }
    }

    // 2. Description / manufacturer heuristic for clones.
    if (!arduino_is_available) {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            if (isExcluded(info.portName())) continue;
            const QString desc = info.description().toLower();
            const QString mfr  = info.manufacturer().toLower();
            if (desc.contains("bluetooth") || mfr.contains("bluetooth")) continue;
            if (desc.contains("arduino") || desc.contains("ch340")
                || desc.contains("ch341") || desc.contains("usb-serial")
                || desc.contains("ftdi") || desc.contains("cp210")
                || mfr.contains("arduino") || mfr.contains("ftdi")
                || mfr.contains("silicon labs") || mfr.contains("wch")) {
                arduino_is_available = true;
                arduino_port_name    = info.portName();
                break;
            }
        }
    }

    qInfo() << "[Arduino:second] Selected port:" << arduino_port_name
            << "(available=" << arduino_is_available
            << ", excluded=" << excludePorts << ")";

    if (arduino_is_available) {
        if (serial->isOpen()) serial->close();
        serial->setPortName(arduino_port_name);
        if (serial->open(QSerialPort::ReadWrite)) {
            serial->setBaudRate(QSerialPort::Baud9600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            return 0;
        }
        qWarning() << "[Arduino:second] Failed to open" << arduino_port_name
                   << ":" << serial->errorString();
        return 1;
    }
    return -1;
}

int Arduino::open_specific_port(const QString &portName)
{
    if (portName.isEmpty()) return -1;
    if (serial->isOpen()) serial->close();
    serial->setPortName(portName);
    if (!serial->open(QSerialPort::ReadWrite)) {
        qWarning() << "[Arduino] open_specific_port" << portName
                   << "failed:" << serial->errorString();
        arduino_is_available = false;
        arduino_port_name.clear();
        return 1;
    }
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    arduino_is_available = true;
    arduino_port_name = portName;
    qInfo() << "[Arduino] open_specific_port" << portName << "OK";
    return 0;
}

int Arduino::close_arduino()
{
    if (serial && serial->isOpen()) {
        serial->close();
        return 0;
    }
    return 1;
}

QByteArray Arduino::read_from_arduino()
{
    if (serial && serial->isReadable()) {
        data = serial->readAll();
        return data;
    }
    return QByteArray();
}

void Arduino::write_to_arduino(QByteArray d)
{
    if (!serial || !serial->isOpen()) {
        qDebug() << "write_to_arduino: serial port not open!";
        return;
    }
    if (serial->isWritable()) {
        qint64 written = serial->write(d);
        serial->waitForBytesWritten(30);
        qDebug() << "write_to_arduino: sent" << written << "bytes ->" << d.trimmed();
    } else {
        qDebug() << "Couldn't write to serial!";
    }
}
