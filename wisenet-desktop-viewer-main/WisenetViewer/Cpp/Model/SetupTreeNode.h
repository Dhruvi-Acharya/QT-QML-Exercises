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
#include <QQmlListProperty>
#include <QModelIndex>
#include <QDebug>

class SetupTreeNode : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QQmlListProperty<SetupTreeNode> nodes READ nodes)
    Q_CLASSINFO("DefaultProperty", "nodes")
    SetupTreeNode(QObject *parent = Q_NULLPTR);

    QQmlListProperty<SetupTreeNode> nodes();

    void setParentNode(SetupTreeNode *parent);
    SetupTreeNode *ParentNode()const {return m_parentNode;}
    bool insertNode(SetupTreeNode *node, int pos = (-1));
    SetupTreeNode *child(int index) const;
    void clear();
    int row() const;
    QVariant getTreenode() const {return m_treeValue;}

    Q_INVOKABLE int count() const { return m_nodes.size(); }


    static void append_element(QQmlListProperty<SetupTreeNode> *property, SetupTreeNode *value)
    {
        SetupTreeNode *parent = (qobject_cast<SetupTreeNode *>(property->object));
        value->setParentNode(parent);
        parent->insertNode(value, -1);
    }

    static int count_element(QQmlListProperty<SetupTreeNode> *property)
    {
        SetupTreeNode *parent = (qobject_cast<SetupTreeNode *>(property->object));
        return parent->count();
    }

    static SetupTreeNode *at_element(QQmlListProperty<SetupTreeNode> *property, int index)
    {
        SetupTreeNode *parent = (qobject_cast<SetupTreeNode *>(property->object));
        if(index < 0 || index >= parent->count())
            return nullptr;
        return parent->child(index);
    }

    static void clear_element(QQmlListProperty<SetupTreeNode> *property)
    {
        SetupTreeNode *parent = (qobject_cast<SetupTreeNode *>(property->object));
        parent->clear();
    }

private:
    QList<SetupTreeNode *> m_nodes;
    SetupTreeNode *m_parentNode;
    QVariant m_treeValue;
};

