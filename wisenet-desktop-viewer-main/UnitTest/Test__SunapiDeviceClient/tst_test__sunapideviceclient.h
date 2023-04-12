#pragma once
#include <QtTest>

// add necessary includes here
#include <QUuid>
#include <QDir>
#include <thread>
#include <vector>
#include <chrono>

#include "ThreadPool.h"
#include "LogSettings.h"
#include "SunapiDeviceClient.h"
#include <toml.hpp>
#include "CloudService.h"

using namespace Wisenet;
using namespace Wisenet::Device;

class Test__SunapiDeviceClient : public QObject
{
    Q_OBJECT

public:
    Test__SunapiDeviceClient() : m_threadPool(8), m_streamIoContextPool(8)
    {
        m_cloudService = std::make_shared<Wisenet::Library::CloudService>();
        m_deviceUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
        m_deviceClient = std::make_shared<Wisenet::Device::SunapiDeviceClient>(m_threadPool.ioContext(),
                                                                               m_streamIoContextPool.ioContext(),
                                                                               m_deviceUuid,
                                                                               m_cloudService, 15, false, 1881);
    }
    ~Test__SunapiDeviceClient() {}

private:
    void initConfig();
    bool connectInternal();
    bool disconnectInternal();

private:
    ThreadPool m_threadPool;
    ThreadPool m_streamIoContextPool;
    std::string m_ip;
    int m_port;
    std::string m_user;
    std::string m_password;
    std::shared_ptr<Wisenet::Device::SunapiDeviceClient> m_deviceClient;
    std::string m_deviceUuid;
    std::string m_audioSamplePath;
    std::shared_ptr<Wisenet::Library::CloudService> m_cloudService;

private slots:
//    void init();
//    void cleanup();
    void initTestCase();
    void cleanupTestCase();
    void test_ConnectDisconnect();
    void test_AlarmOutput();
    void test_Snapshot();
    void test_MediaOpenClose();
    void test_DeviceTalk();
    void test_DevicePTZControl();
    void test_DevicePreset();
    void test_EventParse();
    void test_RecordingTimeline();
    void test_OnlyRecordingTimeline();
    void test_ChangeUserPassword();
    void test_ConfigBackup();
    void test_ConfigRestore();
    void test_FirmwareUpdate();
    void test_AreaZoom();
    void test_AutoTracking();
    void test_DigitalAutoTracking();
    void test_Defog();
    void test_AuxControl();
    void test_DPTZControlPanorama();
    void test_DPTZControlQuadView();
    void test_DPTZPreset();
    void test_MetaAttributeSearch();
    void test_OcrSearch();
    void test_SmartSearch();
private:
    void deviceEventHandler(const DeviceEventBaseSharedPtr& event);

private:
    std::condition_variable m_condition;
    std::mutex m_mutex;
    bool m_canContinue = false; //Wiat or Go 여부
    bool m_isSuccess = false;   //비동기 응답 성공 여부
    void InitializeNotify();     // 비동기 명령 호출 전 m_canContinue, m_isSuccess false로 초기화 함.
    void Notify(bool isSuccess); // 비동기 응답 처리가 완료된 경우 응답값의 정상 여부와 함께 호출.
    bool Wait(long timeout = 1800); // true: timeout 시간 안에 Notify(true)가 호출된 경우, false: timeout이 지났거나, Nofity(false)가 호출된 경우.
};
