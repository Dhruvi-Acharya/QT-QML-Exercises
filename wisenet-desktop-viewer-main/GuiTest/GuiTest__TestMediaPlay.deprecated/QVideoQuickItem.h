#pragma once

#include "MediaController.h"
#include "VideoRenderer.h"
#include <QQuickItem>

class QVideoQuickItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QVideoQuickItem();

    Q_INVOKABLE void openMediaFile(const QString& path);
    Q_INVOKABLE void setVideoOutSize(const int width, const int height);

signals:
    void videoInfoChanged(const QString message);

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void handleSizeChangeTimeouted();

private:
    void releaseResources() override;
    VideoRenderer *m_renderer;
    MediaController *m_controller;

    ThreadSafeQueue<RawVideoDataPtr> m_decodeVideoQueue;
    ThreadSafeQueue<DecodedVideoDataPtr> m_renderVideoQueue;

    QTimer *m_sizeChangedTimer;
};

