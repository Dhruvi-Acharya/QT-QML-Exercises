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

#include "Test__CommonLibrary.h"

#include <QUuid>
#include <QCryptographicHash>

#include "CloudService.h"
#include "CloudHttps.h"
#include "ThreadPool.h"

void Test__CommonLibrary::test_createUserToCloud()
{
    //creat a uuid.
    QString uuid = QUuid::createUuid().toString(QUuid::Id128);

    SPDLOG_DEBUG("userName = {}", uuid.toStdString());


    uuid = QUuid::createUuid().toString(QUuid::Id128);

    QString pass = uuid.left(13);
    pass += "!!";


    //return Key
    SPDLOG_DEBUG("password = {}", pass.toStdString());

    return;

    std::string json = "{\"links\":{\"self\":\"/v1.0/user/devices\"},\"data\":{\"nextPageNumber\":null,\"devices\":[{\"deviceId\":\"htwdz7xn6nxw2222\",\"firmware\":\"hello3\",\"model\":\"hello1\",\"maxChannels\":2},{\"deviceId\":\"htwdytlxzzvt6p3h\",\"firmware\":\"hello3\",\"model\":\"hello1\",\"maxChannels\":0}]}}";
    Wisenet::Library::CloudDevicesInfo info;

    JsonReader reader(json.c_str());
    reader & info;

    SPDLOG_DEBUG("devices size = {}", info.data.devices.size());
    for(auto device : info.data.devices){
        SPDLOG_DEBUG("Device Info - deviceId = {}", device.deviceId);
        SPDLOG_DEBUG("Device Info - frimware = {}", device.firmware);
        SPDLOG_DEBUG("Device Info - model = {}", device.model);
        SPDLOG_DEBUG("Device Info - maxChannels = {}", device.maxChannels);
    }

    ThreadPool threadPool(1);

    Wisenet::Library::CloudHttpConnectionInfo connectionInfo;
    connectionInfo.api = "api.dev.wra1.wisenetcloud.com";
    connectionInfo.auth = "auth.dev.wra1.wisenetcloud.com";
    connectionInfo.turn = "turn.dev.wra1.wisenetcloud.com";
    connectionInfo.port = "443";

    Wisenet::Library::CloudHttps https(threadPool.ioContext(),connectionInfo);

    Wisenet::Library::CloudUserInfo userInfo;
    userInfo.userName = "hyukjinkwon";
    userInfo.password = "init123!!!";

    //https.CreateUser(userInfo);

    std::string acceessToken, accessTokenIotHub, iotHubAuthName;
    https.GetToken(userInfo,acceessToken, iotHubAuthName, accessTokenIotHub);

    Wisenet::Library::CloudDevicesInfo devicesInfo;
    https.GetDevices(accessTokenIotHub, "", devicesInfo);

    Wisenet::Library::CloudTurnInfo turnInfo;
    https.GetTurnConfig(accessTokenIotHub, turnInfo);

}
