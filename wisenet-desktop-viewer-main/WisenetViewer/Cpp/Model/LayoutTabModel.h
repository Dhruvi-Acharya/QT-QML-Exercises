#pragma once

#include <QObject>
#include <QDebug>

class LayoutTabModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit LayoutTabModel(QObject *parent = nullptr);
    ~LayoutTabModel();

    int itemType(){return m_itemType;}
    QString uuid(){return m_uuid;}
    QString text(){return m_text;}

    void setItemType(int itemType)
    {
        m_itemType = itemType;
        emit itemTypeChanged(itemType);
    }

    void setUuid(QString id)
    {
        m_uuid = id;
        emit uuidChanged(id);
    }

    void setText(QString text)
    {
        m_text = text;
        emit textChanged(text);
    }

signals:
    void itemTypeChanged(int itemType);
    void uuidChanged(QString id);
    void textChanged(QString text);


private:
    int m_itemType;
    QString m_uuid;
    QString m_text;
};


