#include "MainViewModel.h"
#include <sstream>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <xlsxdocument.h>
#include <QDateTime>
#include <QDesktopServices>
#include "cryptoadapter.h"

MainViewModel::MainViewModel(QObject *parent)
    : QObject{parent}
{    
}

void MainViewModel::loadJson()
{
    m_countryInfo.clear();
    m_osInfo.clear();

    CryptoAdapter cryptoAdapter;
    cryptoAdapter.Init("statistics");

    QFile jsonFile(m_path);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);

    // Read the contents of the file
    QByteArray jsonData = jsonFile.readAll();

    // Parse the JSON document
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    QXlsx::Document doc;

    doc.addSheet("data");
    doc.addSheet("analysis");

    doc.selectSheet("data");

    int column = 1;
    int row = 1;

    doc.setColumnWidth(column, 39);
    doc.write(row, column++, "PC MAC");
    doc.setColumnWidth(column, 20);
    doc.write(row, column++, "Country");
    doc.setColumnWidth(column, 34);
    doc.write(row, column++, "Average media response time(msec)");
    doc.setColumnWidth(column, 19);
    doc.write(row, column++, "Installed date");
    doc.setColumnWidth(column, 18);
    doc.write(row, column++, "Updated date");
    doc.setColumnWidth(column, 18);
    doc.write(row, column++, "Execution count");
    doc.setColumnWidth(column, 18);
    doc.write(row, column++, "Execution minutes");
    doc.setColumnWidth(column, 19);
    doc.write(row, column++, "Layout opened count");
    doc.setColumnWidth(column, 25);
    doc.write(row, column++, "Opened channels on layout");
    doc.setColumnWidth(column, 27);
    doc.write(row, column++, "OS");
    doc.setColumnWidth(column, 8);
    doc.write(row, column++, "Version");
    doc.setColumnWidth(column, 8);
    doc.write(row, column++, "License");
    doc.setColumnWidth(column, 16);
    doc.write(row, column++, "Devices - MAC");
    doc.setColumnWidth(column, 14);
    doc.write(row, column++, "Model name");
    doc.setColumnWidth(column, 14);
    doc.write(row, column++, "Channels");
    doc.setColumnWidth(column, 14);
    doc.write(row, column++, "Used channels");
    doc.setColumnWidth(column, 10);
    doc.write(row, column++, "Method");
    doc.setColumnWidth(column, 22);
    doc.write(row, column++, "Registration time(msec)");


    // Get the root object of the document
    QJsonObject jsonObj = jsonDoc.object();
    int deviceRow = 1;

    for (auto machineKey : jsonObj.keys()) {
        row = deviceRow + 1;

        qDebug() << machineKey << ":" << jsonObj[machineKey];
        doc.write(row, (int)ColumnNumber::Guid, cryptoAdapter.Decrypt(QByteArray::fromHex(machineKey.toLatin1())));

        if(jsonObj[machineKey].isObject())
        {
            QJsonObject subObj = jsonObj[machineKey].toObject();

            for (auto attributesKey : subObj.keys())
            {
                qDebug() << attributesKey << ":" << subObj[attributesKey];
                int attributeColumnNumber = 0;

                if(attributesKey == "country")
                {
                    attributeColumnNumber = (int)ColumnNumber::Country;
                    QString country = subObj[attributesKey].toString();
                    if(m_countryInfo.contains(country))
                    {
                        m_countryInfo[country] = m_countryInfo[country] + 1;
                    }
                    else
                        m_countryInfo.insert(country, 1);
                }
                else if(attributesKey == "average_media_response_msec")
                {
                    attributeColumnNumber = (int)ColumnNumber::MediaResponseMsec;
                }
                else if(attributesKey == "installed_date")
                {
                    attributeColumnNumber = (int)ColumnNumber::InstalledDate;
                }
                else if(attributesKey == "last_updated_date")
                {
                    attributeColumnNumber = (int)ColumnNumber::LastUpdatedDate;
                }
                else if(attributesKey == "execution_count")
                {
                    attributeColumnNumber = (int)ColumnNumber::ExecutionCount;
                }
                else if(attributesKey == "execution_minutes")
                {
                    attributeColumnNumber = (int)ColumnNumber::ExecutionMinutes;
                }
                else if(attributesKey == "layout_open_count")
                {
                    attributeColumnNumber = (int)ColumnNumber::LayoutOpenCount;
                }
                else if(attributesKey == "layout_channel_count")
                {
                    attributeColumnNumber = (int)ColumnNumber::LayoutChannelCount;
                }
                else if(attributesKey == "os")
                {
                    attributeColumnNumber = (int)ColumnNumber::OS;
                    QString os = subObj[attributesKey].toString();
                    if(m_osInfo.contains(os))
                    {
                        m_osInfo[os] = m_osInfo[os] + 1;
                    }
                    else
                        m_osInfo.insert(os, 1);
                }
                else if(attributesKey == "version")
                {
                    attributeColumnNumber = (int)ColumnNumber::Version;
                }
                else if(attributesKey == "license")
                {
                    attributeColumnNumber = (int)ColumnNumber::License;
                }
                else if(attributesKey == "devices")
                {
                    attributeColumnNumber = (int)ColumnNumber::Devices;
                    deviceRow = row + 1;

                    QJsonObject deviceListObj = subObj[attributesKey].toObject();

                    for (auto devicesKey : deviceListObj.keys())
                    {
                        attributeColumnNumber = (int)ColumnNumber::Devices;
                        doc.write(deviceRow, attributeColumnNumber + (int) DeviceColumnNumber::Mac, cryptoAdapter.Decrypt(QByteArray::fromHex(devicesKey.toLatin1())));
                        QJsonObject deviceObj = deviceListObj[devicesKey].toObject();

                        int deviceInfoColumnNumber = 0;

                        for(auto deviceInfoKey: deviceObj.keys())
                        {
                            if(deviceInfoKey == "model")
                                deviceInfoColumnNumber = attributeColumnNumber + (int) DeviceColumnNumber::Model;
                            else if(deviceInfoKey == "channels")
                                deviceInfoColumnNumber = attributeColumnNumber + (int) DeviceColumnNumber::Channels;
                            else if(deviceInfoKey == "channels_used")
                                deviceInfoColumnNumber = attributeColumnNumber + (int) DeviceColumnNumber::ChannelsUsed;
                            else if(deviceInfoKey == "method")
                                deviceInfoColumnNumber = attributeColumnNumber + (int) DeviceColumnNumber::Method;
                            else if(deviceInfoKey == "registration_time")
                                deviceInfoColumnNumber = attributeColumnNumber + (int) DeviceColumnNumber::RegistrationTime;

                            if(deviceObj[deviceInfoKey].isString())
                                doc.write(deviceRow, deviceInfoColumnNumber, deviceObj[deviceInfoKey].toString());
                            else
                                doc.write(deviceRow, deviceInfoColumnNumber, deviceObj[deviceInfoKey].toInt());
                        }
                        deviceRow++;
                    }
                }

                if(attributeColumnNumber != 0 && attributeColumnNumber < (int)ColumnNumber::Devices)
                {
                    if(attributesKey == "installed_date" || attributesKey == "last_updated_date")
                    {
                        doc.write(row, attributeColumnNumber, QDateTime::fromSecsSinceEpoch(subObj[attributesKey].toInt()).toString("yyyy-MM-dd hh:mm:ss"));
                    }
                    else if(subObj[attributesKey].isString())
                        doc.write(row, attributeColumnNumber, subObj[attributesKey].toString());
                    else
                        doc.write(row, attributeColumnNumber, subObj[attributesKey].toInt());
                }
            }
        }
    }

    qDebug() << m_countryInfo;
    qDebug() << m_osInfo;


    doc.selectSheet("analysis");

    row=1;
    column=1;

    doc.setColumnWidth(column, 26);
    doc.write(row, column, "Country");
    for (QMap<QString, int>::const_iterator it = m_countryInfo.constBegin(); it != m_countryInfo.constEnd(); ++it) {
        row++;
        column=1;
        QString key = it.key();
        int value = it.value();

        doc.write(row, 1, key);
        doc.write(row, 2, value);
    }

    row +=2;
    doc.write(row, column, "OS");

    for (QMap<QString, int>::const_iterator it = m_osInfo.constBegin(); it != m_osInfo.constEnd(); ++it) {
        row++;
        column=1;
        QString key = it.key();
        int value = it.value();

        doc.write(row, 1, key);
        doc.write(row, 2, value);
    }

    if(doc.saveAs("result.xlsx"))
        emit generatedSuccessfully();
}

void MainViewModel::openFolder()
{
    QUrl url = QUrl::fromLocalFile(QDir::current().absolutePath());
    QDesktopServices::openUrl(url);
}

std::string MainViewModel::QStringToStdString(QString str)
{
    std::string ret = str.toStdString();

    return ret;
}

QString MainViewModel::StdStringToQString(std::string str)
{
    QString ret = QString::fromStdString(str);

    return ret;
}
