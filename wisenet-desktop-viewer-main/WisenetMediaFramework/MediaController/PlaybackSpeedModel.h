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
#pragma once

#include <QObject>
#include "QCoreServiceManager.h"
#include "MediaControlModel.h"

class PlaybackSpeedModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maximumForwardSpeed READ maximumForwardSpeed NOTIFY maximumForwardSpeedChanged)
    Q_PROPERTY(int maximumBackwardSpeed READ maximumBackwardSpeed NOTIFY maximumBackwardSpeedChanged)

public:
    PlaybackSpeedModel();

    Q_INVOKABLE void connectMediaControlModelSignals(MediaControlModel* sender);
    Q_INVOKABLE QVariantList getSupportedPlaybackSpeeds(bool forwardSpeeds);
    Q_INVOKABLE int speedToIndex(float speed);
    Q_INVOKABLE float indexToSpeed(int index);

    int maximumBackwardSpeed() {return m_maximumBackwardSpeed;}
    int maximumForwardSpeed() {return m_maximumForwardSpeed;}

public slots:
    void onSelectedChannelChanged(const QString& deviceID, const QString& channelID);

signals:
    void selectedDeviceChanged();
    void maximumForwardSpeedChanged();
    void maximumBackwardSpeedChanged();

private:
    QString m_selectedDeviceID = "";
    QMap<float, QString> m_devicePlaybackSpeeds;

    QVariantList m_supportedPlaybackSpeeds;
    float m_maximumBackwardSpeed = 0;
    float m_maximumForwardSpeed = 0;

    QString convertSpeedToString(float speed);
    void refreshDevicePlaybackSpeeds(bool forwardSpeeds);

};
