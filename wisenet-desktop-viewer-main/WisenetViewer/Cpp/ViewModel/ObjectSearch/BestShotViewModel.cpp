#include "BestShotViewModel.h"
#include "../../Model/BestShotItemModel.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QTime>
#include <QtConcurrent>
#include <QLocaleManager.h>

BestShotViewModel::BestShotViewModel(QObject *parent)
    : QObject{parent}
{
    //makeTestData();
}

BestShotListModel* BestShotViewModel::bestShotListModel()
{
    return &m_bestShotListModel;
}

int BestShotViewModel::bestShotHeight()
{
    return m_bestShotHeight;
}

void BestShotViewModel::setBestShotHeight(int height)
{
    m_bestShotHeight = height;
    emit bestShotHeightChanged(height);

    setPositions();
}

int BestShotViewModel::viewWidth()
{
    return m_viewWidth;
}

int BestShotViewModel::viewHeight()
{
    return (m_bestShotHeight + 8) * (m_bestShotListModel.getRowCountList());
}

void BestShotViewModel::setViewWidth(int viewWidth)
{
    m_viewWidth = viewWidth;
    emit viewWidthChanged(viewWidth);

    setPositions();
}

int BestShotViewModel::selectedIndex()
{
    return m_selectedIndex;
}

void BestShotViewModel::setSelectedIndex(int selectedIndex)
{
    m_selectedIndex = selectedIndex;
    emit selectedIndexChanged(selectedIndex);

    emit selectedItemChanged(this->m_bestShotListModel.getData().at(selectedIndex));
}

void BestShotViewModel::makeTestData()
{
    qDebug() << "makeTestData()";

    for(int i=0; i<=20; i++)
    {
        BestShotItemModel* bestShotItem = new BestShotItemModel();

        int width = QRandomGenerator::global()->bounded(40,100);
        int height = QRandomGenerator::global()->bounded(100,200);


        bestShotItem->setImageWidth(width);
        bestShotItem->setImageHeight(height);

        bestShotItem->setHeight(m_bestShotHeight);
        int bestShotWidth = (int)((float)m_bestShotHeight * (float)bestShotItem->imageWidth() / (float)bestShotItem->imageHeight());
        bestShotItem->setWidth(bestShotWidth);

        //qDebug() << "makeTestData()" << width << height << m_bestShotHeight << bestShotWidth;


        this->m_bestShotListModel.append(bestShotItem);
    }
}

void BestShotViewModel::setData(std::vector<Wisenet::Device::MetaAttribute>* result)
{
    qDebug() << "BestShotViewModel::setData" << result->size();
    emit openLoading();

    m_isReceivedMeta = true;
    m_requestCount = 0;

    for(int i=0; i<result->size(); i++)
    {
        m_result.append(result->at(i));
    }

    std::map<std::string, std::shared_ptr<Wisenet::Device::DeviceUrlSnapShotRequest>> requestMap;

    int loadCount = 0;
    while(true)
    {
        if(m_result.size() <= 0)
            break;

        if(loadCount >= BESTSHOT_COUNT_FIRST_REQUEST)
            break;

        Wisenet::Device::MetaAttribute metaAttribute = m_result.takeAt(0);
        loadCount++;

        BestShotItemModel* bestShotItem = new BestShotItemModel();
        bestShotItem->setImageWidth(metaAttribute.aiBaseAttribute.imageWidth);
        bestShotItem->setImageHeight(metaAttribute.aiBaseAttribute.imageHeight);
        bestShotItem->setMetaAttribute(metaAttribute);

        bestShotItem->setHeight(m_bestShotHeight);
        int bestShotWidth = (int)((float)m_bestShotHeight * (float)bestShotItem->imageWidth() / (float)bestShotItem->imageHeight());
        bestShotItem->setWidth(bestShotWidth);

        std::shared_ptr<Wisenet::Device::DeviceUrlSnapShotRequest> deviceUrlSnapShotRequest;

        if(requestMap.find(metaAttribute.aiBaseAttribute.deviceID) == requestMap.end())
        {
            deviceUrlSnapShotRequest = std::make_shared<Wisenet::Device::DeviceUrlSnapShotRequest>();
            deviceUrlSnapShotRequest->deviceID = metaAttribute.aiBaseAttribute.deviceID;
            requestMap.emplace(metaAttribute.aiBaseAttribute.deviceID, deviceUrlSnapShotRequest);
        }
        else
        {
            deviceUrlSnapShotRequest = requestMap.at(metaAttribute.aiBaseAttribute.deviceID);
        }

        deviceUrlSnapShotRequest->urlList.push_back(metaAttribute.aiBaseAttribute.imageUrl);

        m_urlMap.insert(QString::fromStdString(metaAttribute.aiBaseAttribute.imageUrl), bestShotItem);
        this->m_bestShotListModel.append(bestShotItem);
    }

    m_requestCount = requestMap.size();
    qDebug() << "BestShotViewModel::setData() bestshot request count" << requestMap.size();
    qDebug() << "BestShotViewModel::setData remain meta count " << m_result.size();

    foreach(auto& request, requestMap)
    {
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceUrlSnapShot,
                    this, requestMap[request.first],
                [=](const QCoreServiceReplyPtr& reply)
        {
            qDebug() << "DeviceUrlSnapShot - Res start";
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceUrlSnapShotResponse>(reply->responseDataPtr);

            m_requestCount--;

            if(!response || response->isFailed()){
                qDebug() << "DeviceUrlSnapShot, isFailed={}" << response->isFailed();
            }
            else if(response->snapShots.empty()){
                qDebug() << "DeviceUrlSnapShot, snapShots is empty";
            }
            else{
                foreach (auto& snapShot, response->snapShots) {
                    if(snapShot.isSuccess)
                    {
                        QString url = QString::fromStdString(snapShot.url);
                        if(m_urlMap.contains(url))
                        {
                            BestShotItemModel* item = m_urlMap[url];
                            QImage image = QImage::fromData(snapShot.snapshotData.data(), snapShot.snapshotData.size(), "JPEG");
                            item->setVisible(true);
                            item->setImage(image);
                        }
                    }
                }
            }

            if(m_requestCount == 0)
            {
                setPositions();
                closeLoading();
            }
        });
    }
}

bool BestShotViewModel::filter(const Wisenet::Device::MetaAttribute& metaAttribute)
{
    bool ret = true;
    bool hasAttr = false;

    if(metaAttribute.attributeType == Wisenet::Device::AiClassType::person)
    {
        if(metaAttribute.personGender.size() > 0 && m_filter.personGender.size() > 0)
        {
            for(int i=0; i<metaAttribute.personGender.size(); i++)
            {
                if(m_filter.personGender.find(metaAttribute.personGender.at(i)) != m_filter.personGender.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.personClothingTopsColor.size() > 0 && m_filter.personClothingTopsColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.personClothingTopsColor.size(); i++)
            {
                if(m_filter.personClothingTopsColor.find(metaAttribute.personClothingTopsColor.at(i)) != m_filter.personClothingTopsColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.personClothingBottomsColor.size() > 0 && m_filter.personClothingBottomsColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.personClothingBottomsColor.size(); i++)
            {
                if(m_filter.personClothingBottomsColor.find(metaAttribute.personClothingBottomsColor.at(i)) != m_filter.personClothingBottomsColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.personBelongingBag.size() > 0 && m_filter.personBelongingBag.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.personBelongingBag.size(); i++)
            {
                if(m_filter.personBelongingBag.find(metaAttribute.personBelongingBag.at(i)) != m_filter.personBelongingBag.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }
    }
    else if(metaAttribute.attributeType == Wisenet::Device::AiClassType::vehicle)
    {
        if(metaAttribute.vehicleType.size() > 0 && m_filter.vehicleType.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.vehicleType.size(); i++)
            {
                if(m_filter.vehicleType.find(metaAttribute.vehicleType.at(i)) != m_filter.vehicleType.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.vehicleColor.size() > 0 && m_filter.vehicleColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.vehicleColor.size(); i++)
            {
                if(m_filter.vehicleColor.find(metaAttribute.vehicleColor.at(i)) != m_filter.vehicleColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }
    }
    else if(metaAttribute.attributeType == Wisenet::Device::AiClassType::face)
    {
        if(metaAttribute.faceGender.size() > 0 && m_filter.faceGender.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.faceGender.size(); i++)
            {
                if(m_filter.faceGender.find(metaAttribute.faceGender.at(i)) != m_filter.faceGender.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.faceAgeType.size() > 0 && m_filter.faceAgeType.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.faceAgeType.size(); i++)
            {
                if(m_filter.faceAgeType.find(metaAttribute.faceAgeType.at(i)) != m_filter.faceAgeType.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.faceOpticals.size() > 0 && m_filter.faceOpticals.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.faceOpticals.size(); i++)
            {
                if(m_filter.faceOpticals.find(metaAttribute.faceOpticals.at(i)) != m_filter.faceOpticals.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }

        if(metaAttribute.faceMask.size() > 0 && m_filter.faceMask.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<metaAttribute.faceMask.size(); i++)
            {
                if(m_filter.faceMask.find(metaAttribute.faceMask.at(i)) != m_filter.faceMask.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                ret = false;
                return ret;
            }
        }
    }
    else if(metaAttribute.attributeType == Wisenet::Device::AiClassType::ocr)
    {
        QString source = QString::fromUtf8(metaAttribute.ocrText.c_str());
        QString filterText = QString::fromUtf8(m_filter.ocrText.c_str());

        if(!filterText.contains('*'))
            filterText = "*" + filterText + "*";

        //qDebug() << "OCR filterText"<< filterText;

        QRegExp exp(filterText);
        exp.setPatternSyntax(QRegExp::Wildcard);

        if(!exp.exactMatch(source))
        {
            ret = false;
            return ret;
        }
    }

    return ret;
}

void BestShotViewModel::loadMoreBestShots(bool isScrolled)
{
    if(m_isReceivedMeta == false)
    {
        qDebug() << "BestShotViewModel::loadMoreBestShots not received first meta - return";
        return;
    }

    if(m_result.size() == 0)
    {
        qDebug() << "BestShotViewModel::loadMoreBestShots size 0 - return";
        return;
    }

    emit openLoading();

    int totalCount = this->bestShotListModel()->rowCount();
    QList<int> rowItemCountList = this->m_bestShotListModel.getItemCountList();

    /*
    int lastRowItemCount = rowItemCountList.last();

    //int startIndex = totalCount - lastRowItemCount;
    // visible 기준으로 계산
    int startIndex = this->m_bestShotListModel.getLastRowVisibleStartIndex(lastRowItemCount);
    int startRow = this->m_bestShotListModel.getRowCountList() - 1;

    qDebug() << "loadMoreBestShots()" << totalCount << lastRowItemCount << startIndex << startRow;
    */

    std::map<std::string, std::shared_ptr<Wisenet::Device::DeviceUrlSnapShotRequest>> requestMap;

    int loadCount = 0;
    int loadIndex = 0;
    while(true)
    {
        if(m_result.size() <= 0 || m_result.size() <= loadIndex)
            break;

        if(loadCount >= BESTSHOT_COUNT_PER_REQUEST)
            break;

        // Filter 값이 있을 경우 확인.
        if(!filter(m_result.at(loadIndex)))
        {
            loadIndex++;
            continue;
        }

        Wisenet::Device::MetaAttribute metaAttribute = m_result.takeAt(loadIndex);
        loadCount++;

        BestShotItemModel* bestShotItem = new BestShotItemModel();
        bestShotItem->setImageWidth(metaAttribute.aiBaseAttribute.imageWidth);
        bestShotItem->setImageHeight(metaAttribute.aiBaseAttribute.imageHeight);
        bestShotItem->setMetaAttribute(metaAttribute);

        bestShotItem->setHeight(m_bestShotHeight);
        int bestShotWidth = (int)((float)m_bestShotHeight * (float)bestShotItem->imageWidth() / (float)bestShotItem->imageHeight());
        bestShotItem->setWidth(bestShotWidth);

        std::shared_ptr<Wisenet::Device::DeviceUrlSnapShotRequest> deviceUrlSnapShotRequest;

        if(requestMap.find(metaAttribute.aiBaseAttribute.deviceID) == requestMap.end())
        {
            deviceUrlSnapShotRequest = std::make_shared<Wisenet::Device::DeviceUrlSnapShotRequest>();
            deviceUrlSnapShotRequest->deviceID = metaAttribute.aiBaseAttribute.deviceID;
            requestMap.emplace(metaAttribute.aiBaseAttribute.deviceID, deviceUrlSnapShotRequest);
        }
        else
        {
            deviceUrlSnapShotRequest = requestMap.at(metaAttribute.aiBaseAttribute.deviceID);
        }

        deviceUrlSnapShotRequest->urlList.push_back(metaAttribute.aiBaseAttribute.imageUrl);

        m_urlMap.insert(QString::fromStdString(metaAttribute.aiBaseAttribute.imageUrl), bestShotItem);
        //bestShotItem->applyFilter(m_filter);
        bestShotItem->setVisible(true);
        this->m_bestShotListModel.append(bestShotItem);
    }

    qDebug() << "loadMoreBestShots() remain meta count " << m_result.size();

    m_requestCount = requestMap.size();

    if(m_requestCount == 0)
        emit closeLoading();

    foreach(auto& request, requestMap)
    {
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceUrlSnapShot,
                    this, requestMap[request.first],
                [=](const QCoreServiceReplyPtr& reply)
        {
            m_requestCount--;

            qDebug() << "DeviceUrlSnapShot - Res start";
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceUrlSnapShotResponse>(reply->responseDataPtr);
            if(!response || response->isFailed()){
                qDebug() << "DeviceUrlSnapShot, isFailed={}" << response->isFailed();
            }
            else if(response->snapShots.empty()){
                qDebug() << "DeviceUrlSnapShot, snapShots is empty";
            }
            else{
                foreach (auto& snapShot, response->snapShots) {
                    if(snapShot.isSuccess)
                    {
                        QString url = QString::fromStdString(snapShot.url);
                        if(m_urlMap.contains(url))
                        {
                            BestShotItemModel* item = m_urlMap[url];
                            QImage image = QImage::fromData(snapShot.snapshotData.data(), snapShot.snapshotData.size(), "JPEG");
                            item->setVisible(true);
                            item->setImage(image);
                        }
                    }
                }
            }

            if(m_requestCount == 0)
            {
                if(isScrolled){
                    this->m_bestShotListModel.setPositions(m_viewWidth, m_bestShotHeight);
                    this->m_bestShotListModel.refresh();
                    emit viewHeightChanged(viewHeight());
                    emit bestShotCountChanged(bestShotCount());
                }
                else
                {
                    setPositions();
                }

                emit closeLoading();
            }
        });
    }
}

void BestShotViewModel::clearAll()
{
    qDebug() << "BestShotViewModel::clearAll()";
    m_isReceivedMeta = false;
    emit viewHeightChanged(0);
    this->m_bestShotListModel.clear();
    this->m_urlMap.clear();
    this->m_result.clear();
    Wisenet::Device::MetaFilter filter;
    this->m_filter = filter;
}

void BestShotViewModel::addFlagedItem(BestShotViewModel* arg)
{
    qDebug() << "addFlagedItem()";
    BestShotListModel* listModel = arg->bestShotListModel();

    QList<BestShotItemModel*>& itemList = listModel->getData();

    for(int i=0; i< itemList.count(); i++)
    {
        BestShotItemModel* item = itemList.at(i);

        //qDebug() << "addFlagedItem() flagChecked " << item->flagChecked() << i << item;
        if(item->flagChecked())
        {
            BestShotItemModel* copyItem = new BestShotItemModel();

            copyItem->setImageWidth(item->imageWidth());
            copyItem->setImageHeight(item->imageHeight());
            copyItem->setX(item->x());
            copyItem->setY(item->y());
            copyItem->setWidth(item->width());
            copyItem->setHeight(item->height());
            copyItem->setRowNumber(item->rowNumber());
            copyItem->setSequenceOfRow(item->sequenceOfRow());
            copyItem->setVisible(item->visible());
            copyItem->setFlagChecked(item->flagChecked());
            copyItem->setImage(item->image());

            copyItem->setOrigin(item);
            copyItem->setMetaAttribute(item->metaAttribute());

            this->m_bestShotListModel.append(copyItem);
        }
    }

    this->m_bestShotListModel.setPositions(m_viewWidth, m_bestShotHeight);
    this->m_bestShotListModel.refresh();
    emit viewHeightChanged(viewHeight());
    emit bestShotCountChanged(bestShotCount());
}

void BestShotViewModel::exportImage(QString path)
{
    qDebug() << "BestShotViewModel::exportImage()" << path;
    QList<BestShotItemModel*>& data = m_bestShotListModel.getData();

    auto furture = QtConcurrent::run([=](const QList<BestShotItemModel*>& items){

        QMap<QString, QImage> imageMap;

        // 이미지 저장이 오래걸려 비동기로 변경함.
        // 저장 중에 검색 요청을 하면 items가 변경되어 비정상 종료 문제 발생 가능하여 2 step으로 나눔.
        // Copy
        qDebug() << "BestShotViewModel::exportImage() copy start" << QDateTime::currentDateTime();
        foreach(BestShotItemModel* item, items)
        {
            if(item->visible())
            {
                QString cameraName = item->cameraName();
                QString displayTime = item->displayTime();
                QString currentTime = QLocaleManager::Instance()->getDateTime(QDateTime::currentDateTime());
                displayTime = displayTime.replace('/', '-');
                displayTime = displayTime.replace(':', '_');
                currentTime = currentTime.replace('/', '-');
                currentTime = currentTime.replace(':', '_');

                QString fileName = cameraName + "_" + displayTime + "_" + QString::number(item->metaAttribute().objectId) + "_" + currentTime + ".png";
                QString filePath = path + "/" + fileName;

                //qDebug() << "BestShotViewModel::exportImage() [" << fileName << "]";


                imageMap.insert(filePath, item->image());
                //item->image().save(filePath);
            }
        }

        qDebug() << "BestShotViewModel::exportImage() copy end  " << QDateTime::currentDateTime();

        // Save
        foreach(auto& key, imageMap.keys())
        {
            imageMap[key].save(key);
            qDebug() << "BestShotViewModel::exportImage() [" << key << "]";
        }

    }, data);
}

void BestShotViewModel::setPositions()
{
    //qDebug() << "setPositions() m_viewWidth" << m_viewWidth;

    //QTime time;
    //time.start();

    if (m_filter.personGender.size() != 0 || m_filter.personClothingTopsColor.size() != 0 ||
            m_filter.personClothingBottomsColor.size() != 0 || m_filter.personBelongingBag.size() != 0 ||
            m_filter.faceGender.size() != 0 || m_filter.faceAgeType.size() != 0 ||
            m_filter.faceHat.size() != 0 || m_filter.faceOpticals.size() != 0 ||
            m_filter.faceMask.size() != 0 ||
            m_filter.vehicleType.size() != 0 || m_filter.vehicleColor.size() != 0 ||
            m_filter.ocrText != "")
    {
        this->m_bestShotListModel.applyFilter(m_filter);
    }
    else
    {
        this->m_bestShotListModel.setVisibleAll(true);
    }

    //int nElapsedTime = time.elapsed();
    //qDebug() << "applyFilter() time" << nElapsedTime << "msec";

    //time.start();
    this->m_bestShotListModel.setPositions(m_viewWidth, m_bestShotHeight);
    //nElapsedTime = time.elapsed();
    //qDebug() << "setPositions() time" << nElapsedTime << "msec";

    //time.start();
    this->m_bestShotListModel.refresh();
    //nElapsedTime = time.elapsed();
    //qDebug() << "refresh() time" << nElapsedTime << "msec";

    emit viewHeightChanged(viewHeight());
    emit bestShotCountChanged(bestShotCount());
}

int BestShotViewModel::bestShotCount()
{
    return this->m_bestShotListModel.getVisibleCount();
}

void BestShotViewModel::applyFilter(Wisenet::Device::MetaFilter& filter)
{
    qDebug() << "BestShotViewModel::applyFilter";
    m_filter = filter;

    /*
    for (std::set<std::string>::iterator it = m_filter.personGender.begin(); it != m_filter.personGender.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter personGender" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.personClothingTopsColor.begin(); it != m_filter.personClothingTopsColor.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter personClothingTopsColor" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.personClothingBottomsColor.begin(); it != m_filter.personClothingBottomsColor.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter personClothingBottomsColor" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.personBelongingBag.begin(); it != m_filter.personBelongingBag.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter personBelongingBag" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.faceGender.begin(); it != m_filter.faceGender.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter faceGender" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.faceAgeType.begin(); it != m_filter.faceAgeType.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter faceAgeType" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.faceOpticals.begin(); it != m_filter.faceOpticals.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter faceOpticals" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.faceMask.begin(); it != m_filter.faceMask.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter faceMask" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.vehicleType.begin(); it != m_filter.vehicleType.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter vehicleType" << QString::fromStdString(*it);
    }

    for (std::set<std::string>::iterator it = m_filter.vehicleColor.begin(); it != m_filter.vehicleColor.end(); ++it) {
        qDebug() << "BestShotViewModel::applyFilter vehicleColor" << QString::fromStdString(*it);
    }*/

    setPositions();
}
