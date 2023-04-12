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
#pragma once
#include "BaseTreeModel.h"
#include "DragItemListModel.h"

using namespace WisenetViewerDefine;

class ResourceTreeModel : public BaseTreeModel
{
    Q_OBJECT
public:
    explicit ResourceTreeModel(QObject *parent = nullptr);
    virtual ~ResourceTreeModel();

    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);

    void clear();
    void initializeTree(QString filePath);

public slots:
    void openLocalDirPath(const QString &dirPath);
    void reloadFileItems(QString filePath); // reload file item list
    QString getFirstMediaFileUrl(){return m_firstMediaFileUrl;}
    void verifySignature(const QStringList wnmList);

signals:
    void invalidate();
    void expandRoot();
    void expandSelectedIndex();
    void verifyFinished(QString fileUuid, int verifyResult);

private:
    void SetupCategoryTree();

    void AddLocalFileItem(const QString& fileUrl);
    void RemoveLocalFileItem(const QString& fileUrl);
    void Event_UpdateLocalFileIcon(SignatureVerifyEventPtr event);

    QStringList GetLocalFiles(QString mediaPath);

    TreeItemModel* m_localFileRootModel;

    QMap<QString, TreeItemModel*> m_treeItemModelMap;

    QString m_firstMediaFileUrl = "";
};

