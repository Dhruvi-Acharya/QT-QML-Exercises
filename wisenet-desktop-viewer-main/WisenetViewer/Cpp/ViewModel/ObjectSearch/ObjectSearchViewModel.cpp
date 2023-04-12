#include "ObjectSearchViewModel.h"
#include "LogSettings.h"

ObjectSearchViewModel::ObjectSearchViewModel(QObject *parent)
    : QObject{parent}
{
    connect(&m_objectSearchFilterViewModel, &ObjectSearchFilterViewModel::searchRequest, this, &ObjectSearchViewModel::searchRequest);
    connect(&m_objectSearchFilterViewModel, &ObjectSearchFilterViewModel::filterChanged, this, &ObjectSearchViewModel::applyFilter);

    connect(&m_bestshotFilterTooltipModel, &BestshotFilterTooltipModel::filterChanged, &m_bestShotViewModel, &BestShotViewModel::applyFilter);
    connect(&m_bestshotFilterTooltipModel, &BestshotFilterTooltipModel::filterChanged, &m_flagViewModel, &BestShotViewModel::applyFilter);
    connect(&m_bestshotFilterTooltipModel, &BestshotFilterTooltipModel::filterChanged, &m_objectSearchFilterViewModel, &ObjectSearchFilterViewModel::updateFilterChecked);
}

void ObjectSearchViewModel::searchRequest(const QDateTime &from, const QDateTime &to, QStringList channels, /*Wisenet::Device::AiClassType*/int classType, const bool &allDevice)
{
    qDebug() << "ObjectSearchFilterViewModel::searchRequest() allDevice" << allDevice << " channels" << channels;
    m_bestShotViewModel.clearAll();
    m_flagViewModel.clearAll();
    emit openLoading();
    emit refreshWidth();
    m_result.clear();
    setReceivedMetaCount(0);
    setTotalMetaCount(0);
    m_isCanceled = false;
    m_requestCount = 0;
    m_requestedDeviceIdSet.clear();

    // compose device - channel mapping
    QMap<QString, QList<QString>> deviceChannelMapping;

    if(allDevice){
        std::map<Wisenet::uuid_string, Wisenet::Device::Device> deviceMap = QCoreServiceManager::Instance().DB()->GetDevices();

        foreach(auto& deviceIter, deviceMap){
            Wisenet::Device::Device device = deviceMap[deviceIter.first];

            QString deviceId = QString::fromStdString(deviceIter.first);

            foreach(auto& kv, device.channels)
            {
                if(kv.second.mediaCapabilities.personSearch || kv.second.mediaCapabilities.faceSearch ||
                        kv.second.mediaCapabilities.vehicleSearch || kv.second.mediaCapabilities.ocrSearch)
                {
                    QString channelId = QString::fromStdString(kv.first);
                    if(!deviceChannelMapping.contains(deviceId)){
                        QList<QString> channelList;
                        channelList.append(channelId);
                        deviceChannelMapping.insert(deviceId, channelList);
                    }
                    else
                    {
                        deviceChannelMapping[deviceId].append(channelId);
                    }
                }
            }
        }
    }
    else
    {
        foreach(QString channel, channels)
        {
            QStringList splitIds = channel.split("_");

            if(splitIds.count() != 2)
                continue;

            QString deviceId = splitIds[0];
            QString channelId = splitIds[1];

            if(!deviceChannelMapping.contains(deviceId)){
                QList<QString> channelList;
                channelList.append(channelId);
                deviceChannelMapping.insert(deviceId, channelList);
            }
            else
            {
                deviceChannelMapping[deviceId].append(channelId);
            }
        }
    }

    // Class type, from-to datetime
    Wisenet::Device::AiClassType aiClassType = Wisenet::Device::AiClassType::person;
    if(classType == 0)
        aiClassType = Wisenet::Device::AiClassType::person;
    else if(classType == 1)
        aiClassType = Wisenet::Device::AiClassType::vehicle;
    else if(classType == 2)
        aiClassType = Wisenet::Device::AiClassType::face;
    else if(classType == 3)
        aiClassType = Wisenet::Device::AiClassType::ocr;

    int64_t fromDate = from.toUTC().toMSecsSinceEpoch();
    int64_t toDate = to.toUTC().toMSecsSinceEpoch();

    m_requestCount = deviceChannelMapping.count();

    if(m_requestCount == 0)
    {
        emit closeLoading();
        emit popupNoResult();
        return;
    }

    // request
    foreach(auto& key, deviceChannelMapping.keys()){
        auto deviceMetaAttributeSearchRequest = std::make_shared<Wisenet::Device::DeviceMetaAttributeSearchRequest>();

        deviceMetaAttributeSearchRequest->deviceID = key.toStdString();

        QList<QString> channelList = deviceChannelMapping[key];
        foreach(auto& channel, channelList){
            deviceMetaAttributeSearchRequest->channelIDList.push_back(channel.toStdString());
        }

        deviceMetaAttributeSearchRequest->classType = aiClassType;
        deviceMetaAttributeSearchRequest->fromDate = fromDate;
        deviceMetaAttributeSearchRequest->toDate = toDate;

        qDebug() << "ObjectSearchViewModel::search() classType" << classType;
        qDebug() << "ObjectSearchViewModel::search() deviceID" << key;
        qDebug() << "ObjectSearchViewModel::search() fromDate" << deviceMetaAttributeSearchRequest->fromDate;
        qDebug() << "ObjectSearchViewModel::search() toDate" << deviceMetaAttributeSearchRequest->toDate;
        qDebug() << "ObjectSearchViewModel::search() channelIDList" << channelList;

        Wisenet::Core::memFuncType requestFunction;

        if(aiClassType == Wisenet::Device::AiClassType::ocr)
            requestFunction = &Wisenet::Core::ICoreService::DeviceOcrSearch;
        else
            requestFunction = &Wisenet::Core::ICoreService::DeviceMetaAttributeSearch;

        QCoreServiceManager::Instance().RequestToCoreService(
                    requestFunction,
                    this, deviceMetaAttributeSearchRequest,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceMetaAttributeSearchResponse>(reply->responseDataPtr);

            if(m_isCanceled)
            {
                qDebug() << "DeviceMetaAttributeSearch - CANCELED :" << m_isCanceled << "-- this response will be discarded.";
                return;
            }

            if(!response || response->isFailed())
            {
                SPDLOG_INFO("DeviceMetaAttributeSearch, isFailed={}", response->isFailed());
                m_requestCount--;
            }
            else if(response->results.empty())
            {
                SPDLOG_INFO("DeviceMetaAttributeSearch, results is empty");
                m_requestCount--;
            }
            else
            {
                setReceivedMetaCount(m_receivedMetaCount + (int)response->results.size());

                //  check first response and add total meta count
                if(m_requestedDeviceIdSet.find(deviceMetaAttributeSearchRequest->deviceID) == m_requestedDeviceIdSet.end())
                {
                    m_requestedDeviceIdSet.emplace(response->deviceId);
                    setTotalMetaCount(m_totalMetaCount + response->totalCount);
                }

                m_result.insert(m_result.end(), response->results.begin(), response->results.end());
                qDebug() << "DeviceMetaAttributeSearch - result" << response->results.size() << QDateTime::currentDateTime();

                if(response->isContinue == false)
                {
                    m_requestCount--;
                    qDebug() << "DeviceMetaAttributeSearch - response remains" << m_requestCount;
                }
            }

            if(m_requestCount == 0)
            {
                emit closeLoading();
                qDebug() << "DeviceMetaAttributeSearch - total metadata" << m_result.size();

                if(m_result.size() == 0){
                    qDebug() << "DeviceMetaAttributeSearch - count == 0 return";
                    emit popupNoResult();
                    return;
                }

                std::sort(m_result.begin(), m_result.end(), [](const Wisenet::Device::MetaAttribute& m1, const Wisenet::Device::MetaAttribute& m2) {
                    return m1.aiBaseAttribute.dateTime > m2.aiBaseAttribute.dateTime;
                });

                m_objectSearchFilterViewModel.setVisibleObjectOptionButtonsByResult(&m_result);
                m_bestShotViewModel.setData(&m_result);
                m_gridListViewModel.setData(&m_result);
            }
        });
    }
}

void ObjectSearchViewModel::clear()
{
    m_bestShotViewModel.clearAll();
    m_flagViewModel.clearAll();
    emit refreshWidth();
    m_result.clear();
    setReceivedMetaCount(0);
    setTotalMetaCount(0);
    m_isCanceled = false;
    m_requestCount = 0;
    m_requestedDeviceIdSet.clear();
    m_gridListViewModel.clearData();
}

void ObjectSearchViewModel::cancel()
{
    m_isCanceled = true;
    emit closeLoading();

    foreach(auto& deviceId, m_requestedDeviceIdSet)
    {
        auto deviceRequest = std::make_shared<Wisenet::Device::DeviceRequestBase>();
        deviceRequest->deviceID = deviceId;
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceAiSearchCancel,
                    this, deviceRequest,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

            if(!response || response->isFailed())
            {
                SPDLOG_INFO("DeviceAiSearchCancel, isFailed={}", response->isFailed());
            }

            qDebug() << "DeviceAiSearchCancel() sucess";
        });
    }


    if(m_result.size() > 0)
    {
        qDebug() << "ObjectSearchViewModel::cancel() loadBestShot" << m_result.size();
        std::sort(m_result.begin(), m_result.end(), [](const Wisenet::Device::MetaAttribute& m1, const Wisenet::Device::MetaAttribute& m2) {
            return m1.aiBaseAttribute.dateTime > m2.aiBaseAttribute.dateTime;
        });

        m_objectSearchFilterViewModel.setVisibleObjectOptionButtonsByResult(&m_result);
        m_bestShotViewModel.setData(&m_result);
        m_gridListViewModel.setData(&m_result);
    }
}

Wisenet::Device::MetaFilter ObjectSearchViewModel::makeMetaFilter()
{
    Wisenet::Device::MetaFilter metaFilter;

    // Person Gender
    if(m_objectSearchFilterViewModel.isPersonGenderMaleButtonChecked())
    {
        metaFilter.personGender.insert("Male");
    }
    if(m_objectSearchFilterViewModel.isPersonGenderFemaleButtonChecked())
    {
        metaFilter.personGender.insert("Female");
    }

    // Person Clothing Top
    if(m_objectSearchFilterViewModel.isPersonClothingTopBlackButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Black");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopGrayButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Gray");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopWhiteButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("White");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopRedButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Red");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopOrangeButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Orange");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopYellowButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Yellow");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopGreenButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Green");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopBlueButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Blue");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingTopPurpleButtonChecked())
    {
        metaFilter.personClothingTopsColor.insert("Purple");
    }

    // Person Clothing Bottom
    if(m_objectSearchFilterViewModel.isPersonClothingBottomBlackButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Black");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomGrayButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Gray");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomWhiteButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("White");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomRedButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Red");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomOrangeButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Orange");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomYellowButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Yellow");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomGreenButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Green");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomBlueButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Blue");
    }
    if(m_objectSearchFilterViewModel.isPersonClothingBottomPurpleButtonChecked())
    {
        metaFilter.personClothingBottomsColor.insert("Purple");
    }

    // Person Bag
    if(m_objectSearchFilterViewModel.isPersonBagWearButtonChecked())
    {
        metaFilter.personBelongingBag.insert("Wear");
    }
    if(m_objectSearchFilterViewModel.isPersonBagNoWearButtonChecked())
    {
        metaFilter.personBelongingBag.insert("No");
    }

    // Face Gender
    if(m_objectSearchFilterViewModel.isFaceGenderMaleButtonChecked())
    {
        metaFilter.faceGender.insert("Male");
    }
    if(m_objectSearchFilterViewModel.isFaceGenderFemaleButtonChecked())
    {
        metaFilter.faceGender.insert("Female");
    }

    // Face Age Type
    if(m_objectSearchFilterViewModel.isFaceAgeTypeYoungButtonChecked())
    {
        metaFilter.faceAgeType.insert("Young");
    }
    if(m_objectSearchFilterViewModel.isFaceAgeTypeAdultButtonChecked())
    {
        metaFilter.faceAgeType.insert("Adult");
    }
    if(m_objectSearchFilterViewModel.isFaceAgeTypeMiddleButtonChecked())
    {
        metaFilter.faceAgeType.insert("Middle");
    }
    if(m_objectSearchFilterViewModel.isFaceAgeTypeSeniorButtonChecked())
    {
        metaFilter.faceAgeType.insert("Senior");
    }

    // Face Opticals
    if(m_objectSearchFilterViewModel.isFaceOpticalsWearButtonChecked())
    {
        metaFilter.faceOpticals.insert("Wear");
    }
    if(m_objectSearchFilterViewModel.isFaceOpticalsNoWearButtonChecked())
    {
        metaFilter.faceOpticals.insert("No");
    }

    // Face Mask
    if(m_objectSearchFilterViewModel.isFaceMaskWearButtonChecked())
    {
        metaFilter.faceMask.insert("Wear");
    }
    if(m_objectSearchFilterViewModel.isFaceMaskNoWearButtonChecked())
    {
        metaFilter.faceMask.insert("No");
    }

    // Vehicle Type
    if(m_objectSearchFilterViewModel.isVehicleTypeCarButtonChecked())
    {
        metaFilter.vehicleType.insert("Car");
    }
    if(m_objectSearchFilterViewModel.isVehicleTypeBusButtonChecked())
    {
        metaFilter.vehicleType.insert("Bus");
    }
    if(m_objectSearchFilterViewModel.isVehicleTypeTruckButtonChecked())
    {
        metaFilter.vehicleType.insert("Truck");
    }
    if(m_objectSearchFilterViewModel.isVehicleTypeMotorcycleButtonChecked())
    {
        metaFilter.vehicleType.insert("Motorcycle");
    }
    if(m_objectSearchFilterViewModel.isVehicleTypeBicycleButtonChecked())
    {
        metaFilter.vehicleType.insert("Bicycle");
    }

    // Vehicle Type
    if(m_objectSearchFilterViewModel.isVehicleColorBlackButtonChecked())
    {
        metaFilter.vehicleColor.insert("Black");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorGrayButtonChecked())
    {
        metaFilter.vehicleColor.insert("Gray");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorWhiteButtonChecked())
    {
        metaFilter.vehicleColor.insert("White");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorRedButtonChecked())
    {
        metaFilter.vehicleColor.insert("Red");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorOrangeButtonChecked())
    {
        metaFilter.vehicleColor.insert("Orange");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorYellowButtonChecked())
    {
        metaFilter.vehicleColor.insert("Yellow");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorGreenButtonChecked())
    {
        metaFilter.vehicleColor.insert("Green");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorBlueButtonChecked())
    {
        metaFilter.vehicleColor.insert("Blue");
    }
    if(m_objectSearchFilterViewModel.isVehicleColorPurpleButtonChecked())
    {
        metaFilter.vehicleColor.insert("Purple");
    }

    // LP
    metaFilter.ocrText = m_objectSearchFilterViewModel.getOcrLicensePlateText().toUtf8().constData();

    return metaFilter;
}

// 필터링 뷰의 조건 검색 아이콘이 체크/해제시, ObjectSearchViewModel에 현재 선택된 아이콘의 정보 세팅
void ObjectSearchViewModel::applyFilter()
{
    qDebug() << "ObjectSearchViewModel::applyFilter()";
    Wisenet::Device::MetaFilter metaFilter = makeMetaFilter();

    m_flagViewModel.applyFilter(metaFilter);
    m_bestShotViewModel.applyFilter(metaFilter);
    m_gridListViewModel.applyFilter(metaFilter);
}

void ObjectSearchViewModel::updateBestshotTooltipChecked()
{
    qDebug() << "ObjectSearchViewModel::updateBestshotTooltipChecked()";
    Wisenet::Device::MetaFilter metaFilter = makeMetaFilter();

    m_bestshotFilterTooltipModel.setFilterChecked(metaFilter);
}
