#include <QCoreApplication>
#include <QDebug>
#include <QDate>
#include<QTime>
#include<QDateTime>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int value = 0;
    qint8 value8 = 0;
    qint16 value16 = 0;
    qint32 value32 = 0;
    qint64 value64 = 0;
    qintptr valueptr = 0;

    qInfo() << "int: " <<value << sizeof(value);
    qInfo() << "int8: " <<value8 << sizeof(value8);
    qInfo() << "int16: " <<value16 << sizeof(value16);
    qInfo() << "int32: " <<value32  << sizeof(value32);
    qInfo() << "int64: " <<value64  << sizeof(value64);
    qInfo() << "intptr: " <<valueptr  << sizeof(valueptr);

    qInfo() << "Current date: " <<QDate::currentDate();
    qInfo() << "ISO date: " <<QDate::currentDate().toString(Qt::DateFormat::ISODate);
    qInfo() << "RFC2822 date: " <<QDate::currentDate().toString(Qt::DateFormat::RFC2822Date);

    QDateTime current = QDateTime::currentDateTime();
    qInfo() << "current date time: " << current;
    QDateTime expire = current.addDays(45);
    qInfo() << "Expired date: " << expire;

    QString name = "Dhruvi Acharya";
    qInfo() << "String: " << name;
    qInfo() << "Mid:" << name.mid(1,3);
    qInfo() << "Insert: " << name.insert(0,"Ms. ");
    qInfo() << "Split: " << name.split(" ");
    qInfo() << "Index: " << name.indexOf(".");
    qInfo() << "Remove: " << name.remove("a");

    QByteArray buffer(name.toUtf8());
    qInfo() << "ByteArray: " << buffer;
    qInfo() << "Append: " << buffer.append("!!!");
    qInfo() << "At: " <<buffer.at(buffer.length()-1);
    qInfo() << "Right Justified: " << buffer.rightJustified(20,'.');
    qInfo() << "Encoding: " << buffer.toBase64();
    qInfo() << "Encoding: " << buffer.toHex();

    QVariant v = 0;
    qInfo() << "QVariant: " << v;
    v = "HI";
    qInfo() << v;

    QStringList lst = name.split(" ");
    qInfo() << "QStringList: " << lst;

    foreach (QString s, lst) {
        qInfo() << s;
    }

    return a.exec();
}
