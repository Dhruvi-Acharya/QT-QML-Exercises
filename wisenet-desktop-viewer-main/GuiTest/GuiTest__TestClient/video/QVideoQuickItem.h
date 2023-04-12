#pragma once

#include "QMediaProcessing.h"
#include "OpenGLRenderNode.h"
#include <QQuickItem>
#include <QTimer>
#include "QCoreServiceManager.h"

class QVideoQuickItem : public QQuickItem, public IMediaSink
{
    Q_OBJECT
    QML_ELEMENT

public:
    QVideoQuickItem(QQuickItem *parent = nullptr);
    virtual ~QVideoQuickItem();

    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

    Q_INVOKABLE void openMedia(const QString& path, const bool isYuv);
    Q_INVOKABLE void closeMedia();
    void OnNewMediaFrame(const MediaOutputFrameBaseSharedPtr& mediaOutput) override;

public slots:
    void updateTick();
    void mediaRequestEventTriggered(QMediaRequestEventPtr requestEvent);

private:
    QMediaProcessing *m_mediaProcessing = nullptr;
    OpenGLRenderNode* m_renderNode = nullptr;
    QTimer *m_timer = nullptr;
};

