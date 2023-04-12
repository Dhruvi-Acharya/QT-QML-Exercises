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
#include "ResourceTreeModel.h"
#include "LogSettings.h"
#include "MainViewModel.h"
#include "FilePathConveter.h"
#include <QUuid>
#include <QDebug>
#include <QSet>
#include <QDesktopServices>

ResourceTreeModel::ResourceTreeModel(QObject *parent)
    : BaseTreeModel(parent)
{
    qDebug() << "ResourceTreeModel()";
    m_roleNameMapping[TreeModelRoleName] = "display";

    SetupCategoryTree();

    connect(MainViewModel::getInstance()->Signature(), &SignatureVerifier::verifyFinished,
            this, &ResourceTreeModel::Event_UpdateLocalFileIcon, Qt::QueuedConnection);
}

ResourceTreeModel::~ResourceTreeModel()
{
    qDebug() << "~ResourceTreeModel()";
    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }
}

BaseTreeItemInfo* ResourceTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name)
{
    BaseTreeItemInfo *t;

    t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

void ResourceTreeModel::SetupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    BaseTreeItemInfo* localfilesInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::LocalFileRoot, ItemStatus::Normal, tr("Local Files"));
    m_localFileRootModel = new TreeItemModel(localfilesInfo, rootItem);

    rootItem->appendChild(m_localFileRootModel);

    endResetModel();
}

void ResourceTreeModel::clear()
{
    qDebug() << "ResourceTreeModel::clear()";
    m_treeItemModelMap.clear();

    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }

    SetupCategoryTree();
}

void ResourceTreeModel::initializeTree(QString filePath)
{
    qDebug() << "ResourceTreeModel::initializeTree()";
    // TODO :: std::set<std::string> localfileSet = QCoreServiceManager::Instance().DB()->GetLocalfiles();

    // load local file list
    reloadFileItems(filePath);

    emit expandRoot();
}

void ResourceTreeModel::AddLocalFileItem(const QString& fileUrl)
{
    qDebug() << "ResourceTreeModel::AddLocalFileItem : " << fileUrl;
    QStringList urls = fileUrl.split('/');

    QString fileName = urls.last();

    if(!m_treeItemModelMap.contains(fileUrl)){
        BaseTreeItemInfo* localfileInfo = newTreeItemInfo(fileUrl, ItemType::LocalFile, ItemStatus::Normal, fileName);

        TreeItemModel* localFileModel = new TreeItemModel(localfileInfo, m_localFileRootModel);

        QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);
        beginInsertRows(localfileRootIndex, m_localFileRootModel->childCount(), m_localFileRootModel->childCount());
        m_localFileRootModel->appendChild(localFileModel);
        endInsertRows();

        m_treeItemModelMap.insert(fileUrl, localFileModel);
    }
}

void ResourceTreeModel::RemoveLocalFileItem(const QString& fileUrl)
{
    qDebug() << "ResourceTreeModel::RemoveLocalFileItem : " << fileUrl;
    if(!m_treeItemModelMap.contains(fileUrl))
        return;

    TreeItemModel* treeModel = m_treeItemModelMap.take(fileUrl);

    QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);

    beginRemoveRows(localfileRootIndex, treeModel->row(), treeModel->row());
    m_localFileRootModel->removeChild(treeModel->row());
    endRemoveRows();
}

void ResourceTreeModel::reloadFileItems(QString filePath)
{
    // map에서 로컬파일리스트 초기화
    SPDLOG_DEBUG("ResourceTreeModel::reloadFileItems(), current count={}", m_localFileRootModel->childCount());

    if (m_localFileRootModel->childCount() > 0) {
        auto children = m_localFileRootModel->children();
        for (auto child : children) {
            qDebug() << "ResourceTreeModel::remove child::" << child->getUuid();
            m_treeItemModelMap.remove(child->getUuid());
        }
        QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);
        beginRemoveRows(localfileRootIndex, 0, m_localFileRootModel->childCount()-1);
        m_localFileRootModel->removeAllChild();
        endRemoveRows();
    }

    // 새로 파일리스트를 불러와서 구성한다.
    auto localFiles = GetLocalFiles(filePath);
    for (auto& filePath : localFiles) {
        AddLocalFileItem(filePath);
    }
}

QStringList ResourceTreeModel::GetLocalFiles(QString mediaPath)
{
    qDebug() << "ResourceTreeModel::GetLocalFiles()" << mediaPath;
    QDirIterator mediaFileIt(mediaPath, {"*.mp4", "*.mkv", "*.avi", "*.wnm", "*.sec", "*.png", "*.jpg", "*.jpeg", "*.bmp"}, QDir::Files);

    m_firstMediaFileUrl == "";
    QStringList res;
    while (mediaFileIt.hasNext()) {
        mediaFileIt.next();
        QString filePath = QUrl::fromLocalFile(mediaFileIt.filePath()).toString();
        qDebug() << "ResourceTreeModel::GetLocalFiles() Media file : " << filePath;
        res << filePath;

        if(m_firstMediaFileUrl == "")
            m_firstMediaFileUrl = filePath;
    }
    return res;
}

void ResourceTreeModel::openLocalDirPath(const QString &dirPath)
{
    QString dir = dirPath.left(dirPath.lastIndexOf('/'));

    QString urlString = dir.replace("file://", "");

    QUrl url = QUrl::fromLocalFile(urlString);

    qDebug() << "ResourceTreeModel::openLocalDirPath " << dir << urlString << url;
    QDesktopServices::openUrl(url);
}

void ResourceTreeModel::verifySignature(const QStringList wnmList)
{
    // result <- 서명 검증 성공 : 4, 서명 검증 실패 : 5, 서명 없음 : 6, publicKey 오픈 에러 : 0
    for(auto& fileUuid : wnmList) {
        int extensionIndex = fileUuid.lastIndexOf(".");
        QString extension = fileUuid.right(fileUuid.length() - extensionIndex - 1);
        if(extension.toLower()!="wnm")
            continue;
        else {
            if(m_treeItemModelMap.contains(fileUuid)) {
                m_treeItemModelMap[fileUuid]->setStatus(ItemStatus::Verifying);
            }
            QString filePath = FilePathConveter::getInstance()->getPathByUrl(fileUuid);

            auto signature = MainViewModel::getInstance()->Signature();
            if (signature != nullptr) {
                signature->verify(filePath);
            }
        }
    }
}

void ResourceTreeModel::Event_UpdateLocalFileIcon(SignatureVerifyEventPtr event)
{
    auto updateVerifyResultEvent = std::static_pointer_cast<Wisenet::Core::UpdateVerifyResultEvent>(event->eventDataPtr);

    QString fileUuid = QString::fromStdString(updateVerifyResultEvent->fileUuid);
    int verifyResult = updateVerifyResultEvent->verifyResult;
    qDebug() << "ResourceTreeModel::Event_UpdateLocalFileIcon fileUuid ->" << fileUuid;

    if (verifyResult > 0) {
        if(m_treeItemModelMap.contains(fileUuid)) {
            m_treeItemModelMap[fileUuid]->setStatus(verifyResult);
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Event_UpdateLocalFileIcon - m_treeItemModelMap doesn't contains this file.");
    }
    else { // 검증 완수 못했을 경우 아이콘 원복
        qDebug() << "ResourceTreeModel::Event_UpdateLocalFileIcon - Resotre the icon.";

        auto signature = MainViewModel::getInstance()->Signature();
        if (signature != nullptr) {
            signature->removeVerifyResult(fileUuid);
            m_treeItemModelMap[fileUuid]->setStatus(1);
        }
    }

    emit verifyFinished(fileUuid, verifyResult);
}
