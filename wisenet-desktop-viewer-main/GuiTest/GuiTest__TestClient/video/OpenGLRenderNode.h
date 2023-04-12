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

#include <QSGRenderNode>
#include <QQuickItem>
#include <mutex>
#include "QMediaProcessing.h"

//#if QT_CONFIG(opengl)

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>


class OpenGLRenderNode : public QSGRenderNode
{
public:
    OpenGLRenderNode();
    virtual ~OpenGLRenderNode();

    void render(const RenderState *state) override;
    void releaseResources() override;
    StateFlags changedStates() const override;
    RenderingFlags flags() const override;
    QRectF rect() const override;

    void sync(QQuickItem *item);
    void updateVideoFrame(const VideoOutputFrameSharedPtr& videoOutput);

private:
    void init();
    void setBindVbo();
    void releaseTexture();
    void checkTexture(const int width, const int height, const bool useYuv);
    void checkVideoFrame();

    int m_width = 0;
    int m_height = 0;
    bool m_sizeUpdated = false;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;

    QOpenGLTexture*         m_videoTexture = nullptr;
    QOpenGLTexture*         m_videoTextureY = nullptr;
    QOpenGLTexture*         m_videoTextureU = nullptr;
    QOpenGLTexture*         m_videoTextureV = nullptr;
    int m_matrixUniform = 0;
    int m_opacityUniform = 0;
    int m_textureUniformRgb = 0;
    int m_textureUniformY = 0;
    int m_textureUniformU = 0;
    int m_textureUniformV = 0;
    int m_useYuvUniform = 0;

    bool m_isYuvTexture = false;

    int m_textureWidth = 0;
    int m_textureHeight = 0;
    std::mutex                  m_videoMutex;
    VideoOutputFrameSharedPtr   m_lastVideoOutput;
};

//#endif // opengl
