/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "QCoreServiceManager.h"
#include "MainWindow.h"
#include "LogSettings.h"
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent, bool useYuv, int tileCount)
    : QMainWindow(parent)
{
    const int n = (int)sqrt(tileCount);

    QGroupBox * groupBox = new QGroupBox(this);
    setCentralWidget(groupBox);

    m_layout = new QGridLayout(groupBox);
    for (int x = 0 ; x < n ; x++) {
        for (int y = 0 ; y < n ; y++) {
            auto glwidget = new VideoWidget(groupBox, useYuv);
            m_glWidgets << glwidget;
            m_layout->addWidget(glwidget, x, y);
            SPDLOG_INFO("ADD glWidget on layout::{}, {}", x, y);
        }
    }

    m_btnBackward = new QPushButton(groupBox);
    m_btnBackward->resize(40,40);
    m_btnBackward->move(10,10);
    m_btnBackward->setText("<<");
    connect(m_btnBackward, &QPushButton::clicked, this, &MainWindow::btnBackwardClicked);

    m_btnPause = new QPushButton(groupBox);
    m_btnPause->resize(40,40);
    m_btnPause->move(50,10);
    m_btnPause->setText("||");
    connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::btnPauseClicked);

    m_btnPlay = new QPushButton(groupBox);
    m_btnPlay->resize(40,40);
    m_btnPlay->move(90,10);
    m_btnPlay->setText("[>");
    connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::btnPlayClicked);

    m_btnForward = new QPushButton(groupBox);
    m_btnForward->resize(40,40);
    m_btnForward->move(130,10);
    m_btnForward->setText(">>");
    connect(m_btnForward, &QPushButton::clicked, this, &MainWindow::btnForwardClicked);
}

void MainWindow::useDevice(QString &ip, QString &pw, int channelIndex, int profileIndex)
{
    auto setupRequest = std::make_shared<Wisenet::Core::SetupServiceRequest>();

    setupRequest->userName = "admin";
    setupRequest->newPassword = "5tkatjd!";
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SetupService,
                this, setupRequest,
                [this, ip, pw, channelIndex, profileIndex](const QCoreServiceReplyPtr& reply)
    {

        QCoreServiceManager::Instance().Login(
                    "admin", "5tkatjd!", this,
                    [this, ip, pw, channelIndex, profileIndex](const QCoreServiceReplyPtr& reply)
        {
            auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
            request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
            request->connectionInfo.host = ip.toStdString();
            request->connectionInfo.port = 80;
            request->connectionInfo.user = "admin";
            request->connectionInfo.password = pw.toStdString();

            SPDLOG_INFO("Add sunapi device={}", request->connectionInfo.host);
            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::AddDevice,
                        this, request,
                        [this, request, channelIndex, profileIndex](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

                if (response->isFailed()) {
                    SPDLOG_ERROR("Failed to add sunapi device={}", request->connectionInfo.host);
                    return;
                }
                SPDLOG_INFO("Success to add sunapi device={}", request->connectionInfo.host);

                for (auto &widget : m_glWidgets) {
                    m_deviceUUID = request->deviceID;
                    m_mediaUUID = widget->openCameraStream(request->deviceID, channelIndex, profileIndex);
                }

            });
        });
    });

}

void MainWindow::btnPauseClicked()
{
    Wisenet::Device::MediaControlInfo mediaControlInfo;
    mediaControlInfo.controlType = Wisenet::Device::ControlType::pause;
    mediaControlInfo.speed = m_speed = 0;

    for (auto &widget : m_glWidgets) {
        widget->MediaControl(m_deviceUUID, m_mediaUUID, mediaControlInfo);
    }
}

void MainWindow::btnPlayClicked()
{
    Wisenet::Device::MediaControlInfo mediaControlInfo;
    mediaControlInfo.controlType = Wisenet::Device::ControlType::play;
    mediaControlInfo.speed = m_speed = 1;

    for (auto &widget : m_glWidgets) {
        widget->MediaControl(m_deviceUUID, m_mediaUUID, mediaControlInfo);
    }
}
void MainWindow::btnForwardClicked()
{
    Wisenet::Device::MediaControlInfo mediaControlInfo;
    if(m_speed == 0)
    {
        mediaControlInfo.controlType = Wisenet::Device::ControlType::stepForward;
    }
    else
    {
        mediaControlInfo.controlType = Wisenet::Device::ControlType::play;
        if(m_speed < 0 || m_speed == 64)
            m_speed = 1;
        else
            m_speed *= 2;
        mediaControlInfo.speed = m_speed;
    }

    for (auto &widget : m_glWidgets) {
        widget->MediaControl(m_deviceUUID, m_mediaUUID, mediaControlInfo);
    }
}

void MainWindow::btnBackwardClicked()
{
    Wisenet::Device::MediaControlInfo mediaControlInfo;
    if(m_speed == 0)
    {
        mediaControlInfo.controlType = Wisenet::Device::ControlType::stepBackward;
    }
    else
    {
        mediaControlInfo.controlType = Wisenet::Device::ControlType::play;
        if(m_speed > 0 || m_speed == -64)
            m_speed = -1;
        else
            m_speed *= 2;
        mediaControlInfo.speed = m_speed;
    }

    for (auto &widget : m_glWidgets) {
        widget->MediaControl(m_deviceUUID, m_mediaUUID, mediaControlInfo);
    }
}
