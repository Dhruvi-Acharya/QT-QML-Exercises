/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "QCoreServiceManager.h"
#include "VideoWidget.h"
#include "LogSettings.h"
#include "TimeUtil.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QPainter>
#include <QDir>
#include <QMimeData>
#include <QOpenGLPixelTransferOptions>
#include <spdlog/fmt/chrono.h>

VideoWidget::VideoWidget(QGraphicsProxyWidget* graphcisItem, bool useYuvTexture)
    : m_graphcisItem(graphcisItem)
    , m_useYuv(useYuvTexture)
{
    if (graphcisItem) {
        graphcisItem->setWidget(this);
    }

    setAcceptDrops(true);
    m_mediaProcessing = new QMediaProcessing(this);

    m_timer = new QTimer(this);
    m_timer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &VideoWidget::videoTick);
    m_timer->start(16);

//    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
//    setFocus();
}

VideoWidget::VideoWidget(QWidget *parent, bool useYuvTexture)
    : QOpenGLWidget(parent)
    , m_graphcisItem(nullptr)
    , m_useYuv(useYuvTexture)
{
    setAcceptDrops(true);
    m_mediaProcessing = new QMediaProcessing(this);

    m_timer = new QTimer(this);
    m_timer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &VideoWidget::videoTick);
    m_timer->start(16);
}


void VideoWidget::videoTick()
{
    //update();
    if (m_graphcisItem) {
        m_graphcisItem->update();
    }
    else {
        update();
    }
}



VideoWidget::~VideoWidget()
{
    SPDLOG_INFO("VideoWidget::~VideoWidget()");
    m_mediaProcessing->Close();
    delete m_mediaProcessing;

    // Make sure the context is current when deleting the texture and the buffers.
    makeCurrent();
    if (m_videoTexture)
        delete m_videoTexture;
    if (m_videoTextureY)
        delete m_videoTextureY;
    if (m_videoTextureU)
        delete m_videoTextureU;
    if (m_videoTextureV)
        delete m_videoTextureV;
    doneCurrent();
}


void VideoWidget::initializeGL()
{
    spdlog::stopwatch sw;

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    // initialize shaders
    //if (!m_program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vert")) {
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vert")) {
        SPDLOG_CRITICAL("addCacheableShaderFromSourceFile() vertex failed.");
        return;
    }
    //if (!m_program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.frag")) {
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.frag")) {
        SPDLOG_CRITICAL("addCacheableShaderFromSourceFile() fragment failed.");
        return;
    }

    SPDLOG_INFO("uniform location = {},{},{},{}",
                m_textureUniformRgb,
                m_textureUniformY,
                m_textureUniformU,
                m_textureUniformV);

    m_program.bindAttributeLocation("vertices", 0);
    m_program.bindAttributeLocation("texCoord", 1);

    if (!m_program.link()) {
        SPDLOG_CRITICAL("line shader program failed.");
        return;
    }

    static float values[4][2] = {
        {+1, -1},
        {-1, -1},
        {-1, +1},
        {+1, +1}
    };

    static float texCoords[4][2] = {
        {1, 1},
        {0, 1},
        {0, 0},
        {1, 0}
    };


    QVector<GLfloat> vertData;
    for (int i = 0; i < 4; ++i) {
        // vertex position
        vertData.append(values[i][0]);
        vertData.append(values[i][1]);

        // texture coordinate
        vertData.append(texCoords[i][0]);
        vertData.append(texCoords[i][1]);
    }

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
    m_vbo.release();

    m_textureUniformRgb = m_program.uniformLocation("texture");
    m_textureUniformY = m_program.uniformLocation("tex_y");
    m_textureUniformU = m_program.uniformLocation("tex_u");
    m_textureUniformV = m_program.uniformLocation("tex_v");
    m_useYuvUniform = m_program.uniformLocation("isYuv");

    recreateTexture(1920, 1080, false);
//    m_videoTexture->bind();
//    auto bg = QImage(QString(":/bts.png")).convertToFormat(QImage::Format_RGB888);
//    m_videoTexture->setData(0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, bg.constBits(), nullptr);


    SPDLOG_INFO("VideoWidget::initializeGL() finished::elapsed={:.3} seconds", sw);
}


void VideoWidget::recreateTexture(int width, int height, bool useYuv)
{
    SPDLOG_INFO("Recreate video texture()!!, width={}, height={}, isYuv={}",
                width, height, useYuv);

    if (m_videoTexture) {
        delete m_videoTexture;
        m_videoTexture = nullptr;
    }
    if (m_videoTextureY) {
        delete m_videoTextureY;
        m_videoTextureY = nullptr;
    }
    if (m_videoTextureU) {
        delete m_videoTextureU;
        m_videoTextureU = nullptr;
    }
    if (m_videoTextureV) {
        delete m_videoTextureV;
        m_videoTextureV = nullptr;
    }

    if (useYuv) {
        m_videoTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_videoTextureY->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        m_videoTextureY->create();
        m_videoTextureY->setSize(width, height, 1);
        m_videoTextureY->setFormat(QOpenGLTexture::R8_UNorm);
        m_videoTextureY->allocateStorage();

        m_videoTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_videoTextureU->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        m_videoTextureU->create();
        m_videoTextureU->setSize(width/2, height/2, 1);
        m_videoTextureU->setFormat(QOpenGLTexture::R8_UNorm);
        m_videoTextureU->allocateStorage();

        m_videoTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_videoTextureV->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        m_videoTextureV->create();
        m_videoTextureV->setSize(width/2, height/2, 1);
        m_videoTextureV->setFormat(QOpenGLTexture::R8_UNorm);
        m_videoTextureV->allocateStorage();

        SPDLOG_INFO("YUV TEXTURE ID={},{},{}",
                    m_videoTextureY->textureId(),
                    m_videoTextureU->textureId(),
                    m_videoTextureV->textureId());
    }
    else {
        m_videoTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_videoTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        m_videoTexture->create();
        m_videoTexture->setSize(width, height, 1);
        m_videoTexture->setFormat(QOpenGLTexture::RGB8_UNorm);
        m_videoTexture->allocateStorage();
    }

    m_videoWidth = width;
    m_videoHeight = height;
    m_isYuvTexture = useYuv;
}


void VideoWidget::resizeGL(int w, int h)
{
    SPDLOG_INFO("VideoWidget::resizeGL()::w={}, h={}", w, h);
    m_width = w;
    m_height = h;
}

void VideoWidget::paintGL()
{
    //SPDLOG_DEBUG("VideoWidget::paintGL()");

    m_program.bind();
    m_vbo.bind();

    // update texture
    {
        std::lock_guard<std::mutex> lock_guard(m_videoMutex);
        if (m_lastVideoOutput) {
            SPDLOG_TRACE("VideoWidget:: update video texture!!");

            bool useYuv = m_lastVideoOutput->format() == AVPixelFormat::AV_PIX_FMT_YUV420P;

            if (m_videoWidth != m_lastVideoOutput->width() ||
                    m_videoHeight != m_lastVideoOutput->height() ||
                    m_isYuvTexture != useYuv) {

                recreateTexture(m_lastVideoOutput->width(),
                                m_lastVideoOutput->height(),
                                m_lastVideoOutput->format() == AVPixelFormat::AV_PIX_FMT_YUV420P);
            }

            if (useYuv) {
                QOpenGLPixelTransferOptions options;
                options.setAlignment(8);
                m_videoTextureY->bind();
                m_videoTextureY->setData(0, QOpenGLTexture::Red, QOpenGLTexture::UInt8,
                                        m_lastVideoOutput->data(0), &options);
                m_videoTextureU->bind();
                m_videoTextureU->setData(0, QOpenGLTexture::Red, QOpenGLTexture::UInt8,
                                        m_lastVideoOutput->data(1), &options);
                m_videoTextureV->bind();
                m_videoTextureV->setData(0, QOpenGLTexture::Red, QOpenGLTexture::UInt8,
                                        m_lastVideoOutput->data(2), &options);
            }
            else {
                m_videoTexture->bind();
                m_videoTexture->setData(0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8,
                                        m_lastVideoOutput->data(0), nullptr);
            }

            m_lastVideoOutput.reset();
        }
    }
    if (m_isYuvTexture) {
        /*
        m_videoTextureY->bind();
        m_videoTextureU->bind();
        m_videoTextureV->bind();
        */

        m_program.setUniformValue(m_useYuvUniform, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_videoTextureY->textureId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_videoTextureU->textureId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_videoTextureV->textureId());
        m_program.setUniformValue(m_textureUniformY, 0);
        m_program.setUniformValue(m_textureUniformU, 1);
        m_program.setUniformValue(m_textureUniformV, 2);
    }
    else {
        m_videoTexture->bind();
        m_program.setUniformValue(m_useYuvUniform, 0);
        m_program.setUniformValue(m_textureUniformRgb, 0);
    }

    m_program.enableAttributeArray(0);
    m_program.enableAttributeArray(1);
    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 3, 4 * sizeof(GLfloat));
    m_program.setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));


    glViewport(0, 0, m_width, m_height);

    glDisable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    m_program.disableAttributeArray(0);
    m_program.disableAttributeArray(1);
    m_program.release();
    m_vbo.release();

    if (m_videoTexture)     m_videoTexture->release();
    if (m_videoTextureY)    m_videoTextureY->release();
    if (m_videoTextureU)    m_videoTextureU->release();
    if (m_videoTextureV)    m_videoTextureV->release();
}

void VideoWidget::openMediaFile(QString filePath)
{
    auto nativePath = QDir::toNativeSeparators(filePath);
    QVideoOutParam outParam;
    outParam.outWidth = m_width;
    outParam.outHeight = m_height;
    outParam.pixelFormat = m_useYuv ? VideoOutPixelFormat::Yuv420p : VideoOutPixelFormat::RGB24;

    m_mediaProcessing->OpenFileStream(nativePath, outParam);
}

void VideoWidget::OnNewMediaFrame(const MediaOutputFrameBaseSharedPtr &mediaOutput)
{
    //SPDLOG_DEBUG("VideoWidget::OnNewMediaFrame()");
    {
        std::lock_guard<std::mutex> lock_guard(m_videoMutex);
        m_lastVideoOutput = std::static_pointer_cast<VideoOutputFrame>(mediaOutput);
        m_lastVideoPts = mediaOutput->ptsTimeMsec;
    }
}

std::string VideoWidget::openCameraStream(const std::string &deviceUUID, const int channelIndex, const int profileIndex)
{
    SPDLOG_DEBUG("openCameraStream(), uuid={}, channel={}, profile={}", deviceUUID, channelIndex, profileIndex);
    QVideoOutParam outParam;
    outParam.outWidth = m_width;
    outParam.outHeight = m_height;
    outParam.pixelFormat = m_useYuv ? VideoOutPixelFormat::Yuv420p : VideoOutPixelFormat::RGB24;

    auto mediaOpenRequest = std::make_shared<Wisenet::Device::DeviceMediaOpenRequest>();
    mediaOpenRequest->deviceID = deviceUUID;
    mediaOpenRequest->channelID = std::to_string(channelIndex);
    mediaOpenRequest->mediaID = QCoreServiceManager::Instance().CreateNewUUidString();
    mediaOpenRequest->mediaRequestInfo.streamType = Wisenet::Device::StreamType::live;
    mediaOpenRequest->mediaRequestInfo.profileId = std::to_string(profileIndex);
    //mediaOpenRequest->mediaRequestInfo.playbackSessionId = QCoreServiceManager::Instance().CreateNewUUidString();
    mediaOpenRequest->mediaRequestInfo.startTime = Wisenet::Common::utcMsecsFromIsoString("2021-03-25T09:00:00Z");
    mediaOpenRequest->mediaRequestInfo.trackId = 100;
    m_mediaProcessing->OpenDeviceStream(mediaOpenRequest, outParam);

    return mediaOpenRequest->mediaID;
}

void VideoWidget::MediaControl(const std::string &deviceUUID, const std::string &mediaUUID, Wisenet::Device::MediaControlInfo mediaControlInfo)
{
    auto mediaControlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    mediaControlRequest->deviceID = deviceUUID;
    mediaControlRequest->mediaID = mediaUUID;
    mediaControlInfo.time = m_lastVideoPts;
    mediaControlRequest->mediaControlInfo = mediaControlInfo;

    m_mediaProcessing->DeviceMediaControl(mediaControlRequest);
}


void VideoWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "VideoWidget::dragEnterEvent() ::" << event->dropAction();
    event->acceptProposedAction();

}

void VideoWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void VideoWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "VideoWidget::dropEvent::" << event->dropAction();
    if (event->mimeData()->hasUrls()) {
        auto url = event->mimeData()->urls()[0].toLocalFile();
        openMediaFile(url);
    }
}

void VideoWidget::keyPressEvent(QKeyEvent *event)
{
    SPDLOG_INFO("VideoWidget::keyPressEvent");
}
