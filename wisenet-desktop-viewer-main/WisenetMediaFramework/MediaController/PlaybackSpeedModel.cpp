/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/

#include "PlaybackSpeedModel.h"

PlaybackSpeedModel::PlaybackSpeedModel()
{
    m_supportedPlaybackSpeeds = QVariantList({" x256", " x128", " x64", " x32", " x16", " x8", " x4", " x2", " x1", " x1/2", " x1/4", " x1/8"});
    m_maximumBackwardSpeed = -256;
    m_maximumForwardSpeed = 256;
}

void PlaybackSpeedModel::connectMediaControlModelSignals(MediaControlModel* sender)
{
    connect(sender, &MediaControlModel::selectedChannelChanged, this, &PlaybackSpeedModel::onSelectedChannelChanged);
}

void PlaybackSpeedModel::onSelectedChannelChanged(const QString& deviceID, const QString& channelID)
{
    if(m_selectedDeviceID == deviceID)
        return;

    m_selectedDeviceID = deviceID;

    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    if(QCoreServiceManager::Instance().DB()->FindDevice(deviceID.toStdString(), device))
    {
        m_devicePlaybackSpeeds.clear();
        m_maximumBackwardSpeed = -1;
        m_maximumForwardSpeed = 1;

        for(auto& speed : device.deviceCapabilities.playbackSpeeds)
        {
            if(m_maximumBackwardSpeed > speed)
                m_maximumBackwardSpeed = speed;
            if(m_maximumForwardSpeed < speed)
                m_maximumForwardSpeed = speed;

            m_devicePlaybackSpeeds[speed] = convertSpeedToString(speed);
        }
    }
    else
#endif
    {
        m_maximumBackwardSpeed = -256;
        m_maximumForwardSpeed = 256;
    }

    emit selectedDeviceChanged();
    emit maximumBackwardSpeedChanged();
    emit maximumForwardSpeedChanged();
}

QString PlaybackSpeedModel::convertSpeedToString(float speed)
{
    QString speedString = "";

    float absSpeed = std::fabs(speed);
    if(absSpeed == 0.125)
        speedString = "1/8";
    else if(absSpeed == 0.25)
        speedString = "1/4";
    else if(absSpeed == 0.5)
        speedString = "1/2";
    else
        speedString = QString::number(absSpeed);

    if(speed < 0)
        return "-x" + speedString;
    else
        return " x" + speedString;
}

QVariantList PlaybackSpeedModel::getSupportedPlaybackSpeeds(bool forwardSpeeds)
{
    if(m_selectedDeviceID == "")
    {
        if(forwardSpeeds)
            m_supportedPlaybackSpeeds = QVariantList({" x256", " x128", " x64", " x32", " x16", " x8", " x4", " x2", " x1", " x1/2", " x1/4", " x1/8"});
        else
            m_supportedPlaybackSpeeds = QVariantList({"-x256", "-x128", "-x64", "-x32", "-x16", "-x8", "-x4", "-x2", "-x1", "-x1/2", "-x1/4", "-x1/8"});
    }
    else
    {
        refreshDevicePlaybackSpeeds(forwardSpeeds);
    }

    return m_supportedPlaybackSpeeds;
}

void PlaybackSpeedModel::refreshDevicePlaybackSpeeds(bool forwardSpeeds)
{
    m_supportedPlaybackSpeeds.clear();

    QMapIterator<float, QString> iter(m_devicePlaybackSpeeds);
    while (iter.hasNext())
    {
        iter.next();
        if(forwardSpeeds)
        {
            if(iter.key() < 0)
                continue;
            m_supportedPlaybackSpeeds.push_front(iter.value());
        }
        else
        {
            if(iter.key() > 0)
                continue;
            m_supportedPlaybackSpeeds.push_back(iter.value());
        }
    }
}

int PlaybackSpeedModel::speedToIndex(float speed)
{
    QString speedString = convertSpeedToString(speed);
    for(int i=0 ; i<m_supportedPlaybackSpeeds.count() ; i++)
    {
        if(m_supportedPlaybackSpeeds[i] == speedString)
            return i;
    }

    return -1;
}

float PlaybackSpeedModel::indexToSpeed(int index)
{
    float speed = 0;

    if(index >= m_supportedPlaybackSpeeds.count())
        return 0;

    QString speedString = m_supportedPlaybackSpeeds[index].toString();
    bool isForward = speedString.at(0) != '-';
    speedString = speedString.mid(2);

    if(speedString == "1/8")
        speed = 0.125;
    else if(speedString == "1/4")
        speed = 0.25;
    else if(speedString == "1/2")
        speed = 0.5;
    else
        speed = speedString.toFloat();

    if(isForward)
        return speed;
    else
        return -speed;
}
