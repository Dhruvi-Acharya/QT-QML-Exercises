#pragma once

#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"
#include "../../Model/SequenceListModel.h"

class SequenceAddViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sequenceName READ sequenceName WRITE setSequenceName NOTIFY sequenceNameChanged)
    Q_PROPERTY(SequenceListModel* sequenceListModel READ sequenceListModel NOTIFY sequenceListModelChanged)
    Q_PROPERTY(QStringList layoutList READ layoutList NOTIFY layoutListChanged)

public:
    explicit SequenceAddViewModel(QObject* parent = nullptr);
    ~SequenceAddViewModel();

    static SequenceAddViewModel* getInstance(){
        static SequenceAddViewModel instance;
        return &instance;
    }

    QString sequenceName() const
    {
        return m_sequenceName;
    }

    void setSequenceName(const QString arg)
    {
        m_sequenceName = arg;
    }

    SequenceListModel* sequenceListModel()
    {
        return &m_sequenceListModel;
    }

    QStringList layoutList()
    {
        return m_layoutList;
    }

    void Request_UpdateSequenceLayout(QString sequenceId);
    void Request_SaveSequenceLayout();

    Q_INVOKABLE int getLayoutCount();

public slots:
    void saveLayout();
    void updateLayout(QString sequenceId);
    void reloadLayout();
    void loadLayouts(QString sequenceName, QString sequenceId);
    void addSequenceItem();
    void removeSequenceItem(int row);
    void swapSequenceItem(int firstIdx, int secondIdx);
    void resetListModel();

signals:
    void sequenceNameChanged(QString name);
    void sequenceListModelChanged(SequenceListModel* arg);
    void layoutListChanged(QStringList arg);
    void sequenceInfoChanged(QString sequenceName, QString sequenceId);

private:
    QString m_sequenceName;
    QStringList m_layoutList;
    QStringList m_layoutIdList;
    SequenceListModel m_sequenceListModel;
};
