#pragma once

#include <QObject>

class DummyUserGroupModel : public QObject
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
    QString userId(){return "";}
    bool isAdmin(){return false;}
    bool playback(){return true;}
    bool exportVideo(){return false;}
    bool ptzControl(){return true;}
    bool audio(){return true;}
    bool mic(){return false;}

signals:
    void userIdChanged();
    void isAdminChanged();
    void playbackChanged();
    void exportVideoChanged();
    void ptzControlChanged();
    void audioChanged();
    void micChanged();
};

