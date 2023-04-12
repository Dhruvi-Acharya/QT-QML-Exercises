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
#include <set>
#include "QCoreServiceManager.h"

class DeviceListModel : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void connectionChange(QVector<QString> deviceList, bool connection);
    DeviceListModel(QObject* parent = nullptr);

    void connectDevice(std::vector<Wisenet::Device::Device> devices);
    std::set<std::string> m_devices;

};

