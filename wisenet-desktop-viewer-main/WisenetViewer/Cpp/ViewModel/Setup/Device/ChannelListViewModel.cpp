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
#include "ChannelListViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include "FisheyeUtil.h"

ChannelListViewModel::ChannelListViewModel(QObject* parent)
{
    Q_UNUSED(parent);

    m_roles[ProfileNumberRole] = "profileNumber";
    m_roles[ProfileNameRole] = "profileName";
    m_roles[CodecRole] = "codec";
    m_roles[ResolutionRole] = "resolution";
    m_roles[FrameRateRole] = "framerate";
    m_roles[BitrateRole] = "bitrate";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &ChannelListViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

void ChannelListViewModel::setChannelName(QString name)
{
    if(name != m_channelName){
        m_channelName = name;

        emit this->channelNameChanged();
    }
}

void ChannelListViewModel::setHighProfile(const QString value){
    if(m_highProfile != value){
        m_highProfile = value;
        emit highProfileChanged(value);
    }
}
void ChannelListViewModel::setLowProfile(const QString value){
    if(m_lowProfile != value){
        m_lowProfile = value;
        emit lowProfileChanged(value);
    }
}

void ChannelListViewModel::setUseChannel(const bool value){
    if(m_useChannel != value){
        m_useChannel = value;
        emit useChannelChanged();
    }
}
void ChannelListViewModel::setUsePtz(const bool value){
    if(m_usePtz != value){
        m_usePtz = value;
        emit usePtzChanged();
    }
}

void ChannelListViewModel::setUseDewarping(const bool value){
    if(m_useDewarping != value){
        m_useDewarping = value;
        emit useDewarpingChanged();
    }
}
void ChannelListViewModel::setFisheyeMount(const FisheyeLensLocation value){
    if(m_fisheyeMount != value){
        m_fisheyeMount = value;
        emit fisheyeMountChanged();
    }
}

void ChannelListViewModel::setFisheyeLensType(const QString value){
    if(m_fisheyeLensType != value){
        m_fisheyeLensType = value;
        emit fisheyeLensTypeChanged();
    }
}

int ChannelListViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_datas.count();
}

int ChannelListViewModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_roles.count();
}

QVariant ChannelListViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_datas.count())
        return QVariant();

    const DeviceListProfile &profile = m_datas[index.row()];
    switch (role) {
    case ProfileNumberRole: return profile.profileNumber;
    case ProfileNameRole: return profile.profileName;
    case CodecRole: return profile.codec;
    case ResolutionRole: return profile.resolution;
    case FrameRateRole: return profile.frameRate;
    case BitrateRole: return profile.bitrate;
    default:return QVariant();
    }
}

QHash<int, QByteArray> ChannelListViewModel::roleNames() const
{
    return m_roles;
}


QVariantMap ChannelListViewModel::get(int row) {
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}

QString ChannelListViewModel::getProfileId(QString profileName)
{
    for(auto& profile : m_datas){
        if(profile.profileName == profileName)
            return QString::number(profile.profileNumber);
    }
    return QString("");
}

void ChannelListViewModel::changeChannelInfo(bool use,bool dewarping, FisheyeLensLocation mount, QString lensType,
                                             QString high, QString low, QString newName)
{
    auto stdHigh = high.toStdString();
    auto stdLow = low.toStdString();

    auto request = std::make_shared<Wisenet::Core::SaveChannelsRequest>();

    for(auto& guid : m_channelGuidList){
        Wisenet::Device::Device::Channel channel;
        auto items = guid.split('_');
        if(items.size() ==2 && QCoreServiceManager::Instance().DB()->FindChannel(items.at(0), items.at(1), channel)){

            SPDLOG_DEBUG("ChannelListViewModel::changeChannelInfo {} -- {} --- {} -- {}", items.at(1).toStdString(), dewarping, mount, lensType.toStdString());


            channel.use = use;
            channel.fisheyeSettings.fisheyeEnable = dewarping;
            if(channel.fisheyeSettings.fisheyeEnable){
                if(mount == FisheyeLensLocation::L_Ceiling)
                    channel.fisheyeSettings.fisheyeLensLocation = Wisenet::FisheyeLensLocation::Ceiling;
                else if(mount == FisheyeLensLocation::L_Ground)
                    channel.fisheyeSettings.fisheyeLensLocation = Wisenet::FisheyeLensLocation::Ground;
                else if(mount == FisheyeLensLocation::L_Wall)
                    channel.fisheyeSettings.fisheyeLensLocation = Wisenet::FisheyeLensLocation::Wall;
                channel.fisheyeSettings.fisheyeLensType = lensType.toStdString();
            }
            if(stdHigh!="")
                channel.highProfile = stdHigh;
            if(stdLow!="")
                channel.lowProfile = stdLow;

            if(m_channelGuidList.size() == 1)
                channel.name = newName.toStdString();

            request->channels.push_back(channel);
        }
    }

    if(request->channels.size() > 0){
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::SaveChannels,
                    this, request,
                    [this](const QCoreServiceReplyPtr& reply)
        {
            auto response = reply->responseDataPtr;
            SPDLOG_DEBUG("ChannelListViewModel::changeChannelInfo() result={}", response->isSuccess());
            emit applyResult(response->isSuccess());
        });
    }

}

QString videoCodecType(Wisenet::Media::VideoCodecType type){
    switch ((Wisenet::Media::VideoCodecType)type) {
    case Wisenet::Media::VideoCodecType::H265 : return QString("H265");
    case Wisenet::Media::VideoCodecType::H264 : return QString("H264");
    case Wisenet::Media::VideoCodecType::MJPEG : return QString("MJPEG");
    case Wisenet::Media::VideoCodecType::MPEG4 : return QString("MPEG4");
    case Wisenet::Media::VideoCodecType::VP8 : return QString("VP8");
    case Wisenet::Media::VideoCodecType::VP9 : return QString("VP9");
    case Wisenet::Media::VideoCodecType::AV1 : return QString("AV1");
    default: return QString("UNKNOWN");
    }
}

void ChannelListViewModel::reset()
{
    m_channelGuidList.clear();
}

void ChannelListViewModel::GetDeviceProfile()
{
    auto request = std::make_shared<Wisenet::Core::GetDeviceProfileRequest>();
    request->deviceID = m_deviceId.toStdString();


    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetDeviceProfile,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        resGetDeviceProfile(reply);
    });
}

void ChannelListViewModel::selectChannels(std::vector<QString> guidList)
{
    QSet<QString> channelSet;
    reset();

    m_channelGuidList.clear();

    for(auto &id : guidList){
        SPDLOG_DEBUG("ChannelListViewModel::selectChannels id={}", id.toStdString());

        auto items = id.split('_');
        Wisenet::Device::Device::Channel channel;
        if(items.size() ==2 && QCoreServiceManager::Instance().DB()->FindChannel(items.at(0), items.at(1), channel)){

            if(!channelSet.contains(id)){
                channelSet.insert(id);
                m_channelGuidList.push_back(id);
            }
        }
    }
    if(m_channelGuidList.size()>0){
        //setChannelGuid(m_channelGuidList.first());
        if(m_channelGuidList.size() == 1){
            auto items = m_channelGuidList.at(0).split('_');
            Wisenet::Device::Device::Channel channel;
            if(items.size() ==2 && QCoreServiceManager::Instance().DB()->FindChannel(items.at(0), items.at(1), channel)){
                m_deviceId = items.at(0);
                m_channelId = items.at(1);

                m_useChannel = channel.use;
                m_usePtz = channel.ptzEnable;
                m_useDewarping = channel.fisheyeSettings.fisheyeEnable;
                SPDLOG_DEBUG("ChannelListViewModel::selectChannels {} m_useDewarping={} fisheyeLensLocation={} --{}", m_channelId.toStdString(), m_useDewarping , (int)channel.fisheyeSettings.fisheyeLensLocation, channel.fisheyeSettings.fisheyeLensType);

                if(channel.fisheyeSettings.fisheyeLensLocation == Wisenet::FisheyeLensLocation::Ceiling)
                    setFisheyeMount(FisheyeLensLocation::L_Ceiling);
                else if(channel.fisheyeSettings.fisheyeLensLocation == Wisenet::FisheyeLensLocation::Ground)
                    setFisheyeMount(FisheyeLensLocation::L_Ground);
                else if(channel.fisheyeSettings.fisheyeLensLocation == Wisenet::FisheyeLensLocation::Wall)
                    setFisheyeMount(FisheyeLensLocation::L_Wall);

                if(channel.fisheyeSettings.fisheyeLensType == ""){
                    Wisenet::Common::GetFisheyeType(channel.channelModelName, channel.fisheyeSettings.fisheyeLensType);
                    SPDLOG_DEBUG("ChannelListViewModel::selectChannels model={} fisheyeLensType={} ", channel.channelModelName, channel.fisheyeSettings.fisheyeLensType);
                }

                setFisheyeLensType(QString::fromStdString(channel.fisheyeSettings.fisheyeLensType));
                setChannelName(QString::fromStdString(channel.name));
                setHighProfile(QString::fromStdString(channel.highProfile));
                setLowProfile(QString::fromStdString(channel.lowProfile));

                GetDeviceProfile();
            }
        }
        else{
            setChannelName(QString::number(m_channelGuidList.size()) + " channels");
        }
        emit channelGuidListChanged();
    }
    emit channelCountChanged();
}

void ChannelListViewModel::resGetDeviceProfile(const QCoreServiceReplyPtr& reply)
{
    auto response = std::static_pointer_cast<Wisenet::Core::GetDeviceProfileResponse>(reply->responseDataPtr);

    m_mapData.clear();

    auto channel = response->profileInfo.videoProfiles.find(m_channelId.toStdString());
    if(channel != response->profileInfo.videoProfiles.end()){
        for(auto& profileItem : channel->second){
            //for(auto& profileItem : item.second..second){
            auto &p = profileItem.second;

            DeviceListProfile profile;
            int profileNumber = std::stoi(p.aliasID);
            profile.profileNumber = profileNumber;
            profile.profileName = QString::fromStdString(p.aliasName);
            profile.codec = videoCodecType(p.codecType);
            profile.resolution = QString::number(p.resolution.width) + QString("X") + QString::number(p.resolution.height);
            profile.frameRate = QString::number(p.framerate);
            profile.bitrate = QString::number(p.bitrate);
            m_mapData.emplace(profileNumber, profile);
        }
    }
    m_datas.clear();
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    for(const auto &kv : m_mapData){
        m_datas << kv.second;
    }

    endInsertRows();
    emit highProfileChanged(m_highProfile);
    emit lowProfileChanged(m_lowProfile);
}

void ChannelListViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId())
    {
    case Wisenet::Core::UpdateDevicesEventType:
    case Wisenet::Core::SaveDevicesCredentialEventType: break;
    case Wisenet::Device::DeviceEventType::DeviceProfileInfoEventType :
    {
        GetDeviceProfile();
        break;
    }
    }
}

