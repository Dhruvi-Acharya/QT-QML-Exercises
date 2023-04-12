#pragma once

#include <QObject>
#include "QCoreServiceManager.h"
#include "CoreService/CoreServiceStructure.h"

class UserGroupModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userId READ userId NOTIFY userIdChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY isAdminChanged)
    Q_PROPERTY(bool playback READ playback NOTIFY playbackChanged)
    Q_PROPERTY(bool exportVideo READ exportVideo NOTIFY exportVideoChanged)
    Q_PROPERTY(bool ptzControl READ ptzControl NOTIFY ptzControlChanged)
    Q_PROPERTY(bool audio READ audio NOTIFY audioChanged)
    Q_PROPERTY(bool mic READ mic NOTIFY micChanged)

public:
    explicit UserGroupModel(QObject *parent = nullptr);
    static UserGroupModel* getInstance(){
        static UserGroupModel instance;
        return &instance;
    }

    QString userId();
    bool isAdmin();
    bool playback();
    bool exportVideo();
    bool ptzControl();
    bool audio();
    bool mic();

    void loginCompleted();

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void userIdChanged();
    void isAdminChanged();
    void playbackChanged();
    void exportVideoChanged();
    void ptzControlChanged();
    void audioChanged();
    void micChanged();
};

