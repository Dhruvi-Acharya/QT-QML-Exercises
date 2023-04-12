#include "CameraSettingModel.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlApplicationEngine>

CameraSettingModel::CameraSettingModel(QQmlApplicationEngine * view, QObject *parent) : QObject(parent)
  , m_id("admin"), m_password("5tkatjd!")
  , m_vendor("Hanwha Techwin"), m_name("SNB-6004"), m_model("SNB-6004"), m_firmware("4.00_210101")
  , m_ipAddress("192.168.15.181"), m_webPage("http://" + m_ipAddress)
  , m_macAddress("00-09-18-99-03"), m_cameraID("eb4e957b-ab1f-aa10-ceed-669929a67a51")
  ,m_primaryStream("rtsp://192.168.15.181:554/profile6/media.smp")
  ,m_secondaryStream("rtsp://192.168.15.181:554/profile7/media.smp")
{
    qDebug() << "CameraSettingModel()";

    m_primary.setCodec(QString("MJPEG"));
    m_primary.setBitrateControlType(QString("VBR"));
    m_secondary.setBitrateControlType(QString("CBR"));

    m_view = view;
    m_view->rootContext()->setContextProperty("cameraSettingModel", this);
    m_view->rootContext()->setContextProperty("primaryModel", &(this->m_primary));
    m_view->rootContext()->setContextProperty("secondaryModel", &(this->m_secondary));


    m_view->load(QUrl(QStringLiteral("qrc:/CameraSetting.qml")));
}

CameraSettingModel::~CameraSettingModel()
{
    qDebug() << "~CameraSettingModel()";
}

void CameraSettingModel::setName(const QString &name)
{
    if(name != m_name){
        m_name = name;
        emit this->nameChanged();
    }
}

void CameraSettingModel::setID(const QString &id)
{
    if(id != m_id){
        m_id = id;
        emit this->idChanged();
    }
}

void CameraSettingModel::setPassword(const QString &pw)
{
    if(pw != m_password){
        m_password = pw;
        emit this->passwordChanged();
    }
}

