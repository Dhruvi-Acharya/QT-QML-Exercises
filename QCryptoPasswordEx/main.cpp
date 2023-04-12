#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QSettings>
#include <QCryptographicHash>

QString createHash(QString data)
{
    QCryptographicHash hash(QCryptographicHash::Sha3_256);
    hash.addData(data.toUtf8());
    return hash.result().toBase64();
}

void setPassword()
{
    QTextStream stream(stdin);
    qInfo() << "Enter a password: ";
    QString data1 = stream.readLine();

    qInfo() << "Confirm the password: ";
    QString data2 = stream.readLine();

    if(data1 == data2)
    {
        QString hashed = createHash(data1);
        qInfo() << "Hashed: " << hashed;

        QSettings settings;
        settings.setValue("password",hashed);
    }
    else
    {
        qInfo() << "They don't match";
        setPassword();
    }
}

void checkPassword()
{
    qInfo() << "Please enter the password you previously entered...";
    QTextStream stream(stdin);
    QSettings settings;
    QString data = stream.readLine();
    QString dataHash = createHash(data);

    QString stored = settings.value("password").toString();

    qInfo() << "Hashed: " << dataHash;
    qInfo() << "Stored: " << stored;

    if(stored == dataHash)
    {
        qInfo() << "Correct!";
    }
    else{
        qInfo() << "Sorry, try again!";
        checkPassword();
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setPassword();
    checkPassword();

    return a.exec();
}
