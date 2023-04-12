#pragma once

#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"

class MainViewModel : public QObject
{
    Q_OBJECT
public:
    explicit MainViewModel(QObject* parent = nullptr);
    ~MainViewModel();

    Q_PROPERTY (bool LoginStatus READ GetLoginStatus WRITE SetLoginStatus NOTIFY LoginStatusChangedEvent)
    Q_PROPERTY (ViewerMode viewerMode READ viewerMode WRITE setViewerMode NOTIFY viewerModeChanged)
    Q_PROPERTY (bool displayOsd READ displayOsd WRITE setDisplayOsd NOTIFY displayOsdChanged)
    Q_PROPERTY (bool displayVideoStatus READ displayVideoStatus WRITE setDisplayVideoStatus NOTIFY displayVideoStatusChanged)
    Q_PROPERTY (QList<bool> panelFlipList READ panelFlipList WRITE setPanelFlipList NOTIFY panelFlipListChanged)
    Q_PROPERTY (QString buildDate READ buildDate CONSTANT)
    Q_PROPERTY(bool isLinkedMode READ isLinkedMode WRITE setIsLinkedMode NOTIFY isLinkedModeChanged)
    Q_PROPERTY(bool isThumbnailMode READ isThumbnailMode WRITE setIsThumbnailMode NOTIFY isThumbnailModeChanged)
    Q_PROPERTY(bool isMultiTimelineMode READ isMultiTimelineMode WRITE setIsMultiTimelineMode NOTIFY isMultiTimelineModeChanged)
    Q_PROPERTY(float timelineHeight READ timelineHeight WRITE setTimelineHeight NOTIFY timelineHeightChanged)

    enum ViewerMode
    {
        Windowed = 0,
        Maximized,
        Minimized,
        FullScreen
    };
    Q_ENUM(ViewerMode)

    void setBuildDate(const QString& dateString)
    {
        _buildDate = dateString;
    }

    QString buildDate() const
    {
        return _buildDate;
    }

    bool GetLoginStatus() const
    {
        return _loginStatus;
    }

    void SetLoginStatus(const bool arg)
    {
        _loginStatus = arg;
        emit LoginStatusChangedEvent(_loginStatus);
    }

    ViewerMode viewerMode() const
    {
        return _viewerMode;
    }

    void setViewerMode(const ViewerMode mode)
    {
        qDebug() << "MainViewModel::setViewerMode()" << _viewerMode << mode;
        if (_viewerMode != mode) {
            _viewerMode = mode;
            emit viewerModeChanged(mode);
        }
    }

    void setDisplayOsd(bool osd)
    {
        if (m_displayOsd != osd) {
            m_displayOsd = osd;
            emit displayOsdChanged(osd);
        }
    }
    void setDisplayVideoStatus(bool videoStatus)
    {
        if (m_displayVideoStatus != videoStatus) {
            m_displayVideoStatus = videoStatus;
            emit displayVideoStatusChanged(videoStatus);
        }
    }

    void setPanelFlipList(QList<bool> panelFlipList)
    {
        bool isListChanged = (panelFlipList.size() != m_panelFlipList.size());
        if(!isListChanged)
        {
            for(int i=0; i<panelFlipList.size(); i++)
            {
                if(panelFlipList[i] != m_panelFlipList[i])
                {
                    isListChanged = true;
                    break;
                }
            }
        }

        if (isListChanged) {
            m_panelFlipList = panelFlipList;
            emit panelFlipListChanged(panelFlipList);
        }
    }

    QStringList getScreenInfo()
    {
        return m_screenInfo;
    }

    QString getPositionInfo()
    {
        QString position = QString("%1:%2:%3:%4:%5").arg(m_x).arg(m_y).arg((int)_viewerMode).arg(m_width).arg(m_height);
        return position;
    }

    int x(){
        return m_x;
    }

    int y(){
        return m_y;
    }

    bool displayOsd(){
        return m_displayOsd;
    }
    bool displayVideoStatus(){
        return m_displayVideoStatus;
    }
    QList<bool> panelFlipList(){
        return m_panelFlipList;
    }

    bool isLinkedMode() {
        return m_isLinkedMode;
    }
    bool isThumbnailMode() {
        return m_isThumbnailMode;
    }
    bool isMultiTimelineMode() {
        return m_isMultiTimelineMode;
    }
    float timelineHeight() {
        return m_timelineHeight;
    }

    void setIsLinkedMode(bool linkedMode) {
        if(m_isLinkedMode != linkedMode) {
            m_isLinkedMode = linkedMode;
            emit isLinkedModeChanged();
        }
    }
    void setIsThumbnailMode(bool thumbnailMode) {
        if(m_isThumbnailMode != thumbnailMode) {
            m_isThumbnailMode = thumbnailMode;
            emit isThumbnailModeChanged();
        }
    }
    void setIsMultiTimelineMode(bool multiTimelineMode) {
        if(m_isMultiTimelineMode != multiTimelineMode) {
            m_isMultiTimelineMode = multiTimelineMode;
            emit isMultiTimelineModeChanged();
        }
    }
    void setTimelineHeight(float height) {
        if(m_timelineHeight != height) {
            m_timelineHeight = height;
            emit timelineHeightChanged();
        }
    }

signals:
    void LoginStatusChangedEvent(bool arg);
    void viewerModeChanged(ViewerMode mode);
    void displayOsdChanged(bool osd);
    void displayVideoStatusChanged(bool videoStatus);
    void panelFlipListChanged(QList<bool> panelFlipList);
    void close();
    void refreshScreen();
    void refreshWindowPosition();
    void setPosition(int x, int y, int width, int height);
    void isLinkedModeChanged();
    bool isThumbnailModeChanged();
    bool isMultiTimelineModeChanged();
    float timelineHeightChanged();

public slots:
    void getServiceInformation();
    void setLicenseAgreed();
    void setCollectingAgreed(const bool collectingAgreed);
    void setScreen(QStringList info);
    void setWindowPosition(int x, int y, int viewerMode);
    void setWidth(int width);
    void setHeight(int height);
    void openManual();

signals:
    void showPasswordChangeView();
    void showLoginView();
    void showMonitoringView();
    void showLicenseAgreementView();
    void initializeTree();
    void focusWisenetViewer(const bool onOff);

private:
    bool _loginStatus = false;
    ViewerMode _viewerMode = ViewerMode::Windowed;
    QString _screenName; /* TODO */

    QString _buildDate;

    int m_x = 0;
    int m_y = 0;
    int m_width = 0;
    int m_height = 0;
    bool m_displayOsd = true;
    bool m_displayVideoStatus = false;
    QList<bool> m_panelFlipList = {false, false, false}; // left, right, bottom
    QStringList m_screenInfo;
    bool m_isLinkedMode = true;         // 동기화 재생 모드
    bool m_isThumbnailMode = false;     // 타임라인 썸네일 표시 모드
    bool m_isMultiTimelineMode = false; // 멀티 타임라인 표시 모드
    float m_timelineHeight = 100;       // showHide 및 isThumbnailMode, isMultiTimelineMode on/off에 따라 복귀 할 컨트롤의 height
};
