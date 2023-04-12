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

#include <QObject>
#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include "IVideoRenderer.h"
#include <mutex>

class VideoRenderer : public QObject, protected QOpenGLFunctions, public IVideoRenderer
{
    Q_OBJECT
public:
    VideoRenderer();
    ~VideoRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

    void updateVideoOut(DecodedVideoDataPtr& videoData) override;

public slots:
    void init();
    void paint();

signals:

private:
    void createObject();
    void recreateTexture(int width, int height);
private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;

    DecodedVideoDataPtr m_lastVideoData;
    std::mutex          m_videoMutex;

    //unsigned int        m_texture;

    QOpenGLBuffer       m_vbo;
    QOpenGLTexture*     m_videoTexture = nullptr;
};

