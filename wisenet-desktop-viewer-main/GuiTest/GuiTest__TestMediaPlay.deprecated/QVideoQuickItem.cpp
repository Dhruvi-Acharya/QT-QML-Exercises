#include "QVideoQuickItem.h"
#include <QtQuick/qquickwindow.h>
#include <QtCore/QRunnable>
#include "LogSettings.h"

QVideoQuickItem::QVideoQuickItem()
    : m_renderer(nullptr)
    , m_controller(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &QVideoQuickItem::handleWindowChanged);
    m_controller = new MediaController(m_decodeVideoQueue, m_renderVideoQueue);

    // signal to signal
    connect(m_controller, &MediaController::videoInfoChanged, this, &QVideoQuickItem::videoInfoChanged);

    m_sizeChangedTimer = new QTimer(this);

    m_sizeChangedTimer->setSingleShot(true);
    m_sizeChangedTimer->setInterval(200);
    connect(m_sizeChangedTimer, &QTimer::timeout, this, &QVideoQuickItem::handleSizeChangeTimeouted);

}

void QVideoQuickItem::openMediaFile(const QString &path)
{
    auto qurl = QUrl(path);
    auto filePath = qurl.toLocalFile();
    auto nativePath = QDir::toNativeSeparators(filePath);
    SPDLOG_INFO("QVideoQuickItem:: file open = {}, native path={}",
                path.toStdString(), nativePath.toStdString());

    qreal dpi = window()->devicePixelRatio();
    int width = (int) (dpi * window()->width());
    int height = (int) (dpi * window()->height());
    m_controller->setVideoOutSize(width, height);
    m_controller->mediaOpen(nativePath, m_renderer);
}

void QVideoQuickItem::setVideoOutSize(const int width, const int height)
{
    SPDLOG_DEBUG("checkVideoOutSize, width={}, height={}", width, height);
    if (m_sizeChangedTimer->isActive()) {
        m_sizeChangedTimer->stop();
    }
    m_sizeChangedTimer->start();
}

void QVideoQuickItem::handleSizeChangeTimeouted()
{
    SPDLOG_INFO("QVideoQuickItem::handleSizeChangeTimeouted()");
    qreal dpi = window()->devicePixelRatio();
    int width = (int) (dpi * window()->width());
    int height = (int) (dpi * window()->height());
    m_controller->setVideoOutSize(width, height);
}

void QVideoQuickItem::handleWindowChanged(QQuickWindow *win)
{
    SPDLOG_DEBUG("QVideoQuickItem::handleWindowChanged()");
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &QVideoQuickItem::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &QVideoQuickItem::cleanup, Qt::DirectConnection);
        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor(Qt::black);
    }
}

void QVideoQuickItem::sync()
{
    SPDLOG_DEBUG("QVideoQuickItem::sync()");

    if (!m_renderer) {
        m_renderer = new VideoRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &VideoRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &VideoRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());
}

void QVideoQuickItem::cleanup()
{
    if (m_renderer != nullptr) {
        delete m_renderer;
        m_renderer = nullptr;
    }
    if (m_controller != nullptr) {
        delete m_controller;
        m_controller = nullptr;
    }
}


class CleanupJob : public QRunnable
{
public:
    CleanupJob(VideoRenderer *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    VideoRenderer *m_renderer;
};


void QVideoQuickItem::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}
