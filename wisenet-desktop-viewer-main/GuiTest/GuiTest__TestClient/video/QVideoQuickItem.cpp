#include "QVideoQuickItem.h"
#include <QDir>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include "LogSettings.h"

QVideoQuickItem::QVideoQuickItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    SPDLOG_DEBUG("QVideoQuickItem::QVideoQuickItem()");

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::mediaRequestEventTriggered,
            this, &QVideoQuickItem::mediaRequestEventTriggered, Qt::QueuedConnection);

    // Our item shows something so set the flag.
    setFlag(ItemHasContents);

    m_mediaProcessing = new QMediaProcessing(this, this);

    m_timer = new QTimer(this);
    m_timer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &QVideoQuickItem::updateTick);
    m_timer->start(16);
}

void QVideoQuickItem::updateTick()
{
    if (window()) {
        window()->update();
    }
}

QVideoQuickItem::~QVideoQuickItem()
{
    SPDLOG_DEBUG("QVideoQuickItem::~QVideoQuickItem()");
}
QSGNode *QVideoQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    SPDLOG_DEBUG("QVideoQuickItem::updatePaintNode()");

    QSGRenderNode *n = static_cast<QSGRenderNode *>(node);

    QSGRendererInterface *ri = window()->rendererInterface();
    if (!ri)
        return nullptr;

    switch (ri->graphicsApi()) {
    case QSGRendererInterface::OpenGL:
        Q_FALLTHROUGH();
    case QSGRendererInterface::OpenGLRhi:
#if QT_CONFIG(opengl)
        if (!n) {
            n = new OpenGLRenderNode;
            m_renderNode = static_cast<OpenGLRenderNode *>(n);
        }
        m_renderNode->sync(this);
#endif
        break;

    default:
        break;
    }

    if (!n)
        SPDLOG_ERROR("QVideoQuickItem reports unknown graphics API {}", ri->graphicsApi());

    window()->update(); // ensure getting to beforeRendering() at some point
    return n;
}

void QVideoQuickItem::openMedia(const QString &path, const bool isYuv)
{
    if (m_mediaProcessing->IsOpen())
        m_mediaProcessing->Close();

    auto qurl = QUrl(path);
    auto filePath = qurl.toLocalFile();
    auto nativePath = QDir::toNativeSeparators(filePath);
    auto itemSize = size() * window()->devicePixelRatio();

    SPDLOG_INFO("QVideoQuickItem::openMedia(), itemSize={}x{}",
                itemSize.width(), itemSize.height());
    QVideoOutParam outParam;
    outParam.outWidth = itemSize.width();
    outParam.outHeight = itemSize.height();
    outParam.pixelFormat = isYuv?VideoOutPixelFormat::YUV420P:VideoOutPixelFormat::BGRA;

    m_mediaProcessing->OpenFileStream(nativePath, outParam);
}
void QVideoQuickItem::closeMedia()
{
    SPDLOG_INFO("QVideoQuickItem::closeMedia()");
    if (m_mediaProcessing->IsOpen()) {
        SPDLOG_INFO("HERE!!");
        m_mediaProcessing->Close();
    }
    else
        SPDLOG_INFO("MUST NOT HERE!!");
}

void QVideoQuickItem::OnNewMediaFrame(const MediaOutputFrameBaseSharedPtr &mediaOutput)
{
    //SPDLOG_DEBUG("QVideoQuickItem::OnNewMediaFrame()");
    if (m_renderNode) {
        auto videoOutput = std::static_pointer_cast<VideoOutputFrame>(mediaOutput);
        m_renderNode->updateVideoFrame(videoOutput);
    }
}

void QVideoQuickItem::mediaRequestEventTriggered(QMediaRequestEventPtr requestEvent)
{
    SPDLOG_DEBUG("QVideoQuickItem::mediaRequestEventTriggered!!");
    if (m_mediaProcessing->IsOpen())
        m_mediaProcessing->Close();

    auto itemSize = size() * window()->devicePixelRatio();
    QVideoOutParam outParam;
    outParam.outWidth = itemSize.width();
    outParam.outHeight = itemSize.height();
    outParam.pixelFormat = VideoOutPixelFormat::BGRA;

    auto mediaOpenRequest = std::make_shared<Wisenet::Device::DeviceMediaOpenRequest>();
    mediaOpenRequest->deviceID = requestEvent->deviceUUID.toStdString();
    mediaOpenRequest->channelID = std::to_string(requestEvent->channelNumber);
    mediaOpenRequest->mediaID =  QCoreServiceManager::Instance().CreateNewUUidString();
    mediaOpenRequest->mediaRequestInfo.streamType = Wisenet::Device::StreamType::live;    
    mediaOpenRequest->mediaRequestInfo.profileId = "2";

    m_mediaProcessing->OpenDeviceStream(mediaOpenRequest, outParam);
}
