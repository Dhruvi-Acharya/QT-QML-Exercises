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
#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <mutex>
#include "QMediaProcessing.h"


class VideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IMediaSink
{
    Q_OBJECT
public:
    VideoWidget(QGraphicsProxyWidget* graphcisItem = nullptr, bool useYuvTexture = false);
    VideoWidget(QWidget* parent = nullptr, bool useYuvTexture = false);
    ~VideoWidget();

    void OnNewMediaFrame(const MediaOutputFrameBaseSharedPtr& mediaOutput) override;

    std::string openCameraStream(const std::string &deviceUUID, const int channelIndex, const int profileIndex);
    void MediaControl(const std::string &deviceUUID, const std::string &mediaUUID, Wisenet::Device::MediaControlInfo mediaControlInfo);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void openMediaFile(QString url);
    void videoTick();

signals:

private:
    QOpenGLShaderProgram    m_program;
    QOpenGLBuffer           m_vbo;
    QOpenGLTexture*         m_videoTexture = nullptr;
    QOpenGLTexture*         m_videoTextureY = nullptr;
    QOpenGLTexture*         m_videoTextureU = nullptr;
    QOpenGLTexture*         m_videoTextureV = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_textureUniformRgb = 0;
    int m_textureUniformY = 0;
    int m_textureUniformU = 0;
    int m_textureUniformV = 0;
    int m_useYuvUniform = 0;

    bool m_isYuvTexture = false;
    int m_videoWidth = 0;
    int m_videoHeight = 0;

    bool m_useYuv = false;

    QTimer *m_timer = nullptr;

private:
    QGraphicsProxyWidget* m_graphcisItem;
    QMediaProcessing *m_mediaProcessing;
    VideoOutputFrameSharedPtr m_lastVideoOutput;
    std::mutex          m_videoMutex;
    int64_t  m_lastVideoPts;
    void recreateTexture(int width, int height, bool useYuv);
};

