#include "SetupTreeNode.h"

SetupTreeNode::SetupTreeNode(QObject *parent):
    QObject(parent), m_parentNode(nullptr)
{
    m_treeValue.setValue(this);
}

QQmlListProperty<SetupTreeNode> SetupTreeNode::nodes()
{
    QQmlListProperty<SetupTreeNode> list(this,
                                      0,
                                      &append_element,
                                      &count_element,
                                      &at_element,
                                      &clear_element);
    return list;
}

void SetupTreeNode::setParentNode(SetupTreeNode *parent)
{
    m_parentNode = parent;
}

bool SetupTreeNode::insertNode(SetupTreeNode *node, int pos)
{
    if(pos > m_nodes.count())
        return false;
    if(pos < 0)
        pos = m_nodes.count();
    m_nodes.insert(pos, node);

    return true;
}

SetupTreeNode *SetupTreeNode::child(int index) const
{
    if(index < 0 || index >= m_nodes.length())
        return nullptr;
    return m_nodes.at(index);
}

void SetupTreeNode::clear()
{
    qDeleteAll(m_nodes);
    m_nodes.clear();
}

int SetupTreeNode::row() const
{
    if (m_parentNode)
    {
        int retIndex = m_parentNode->m_nodes.indexOf(const_cast<SetupTreeNode*>(this));
        return retIndex;
    }

    return 0;
}
