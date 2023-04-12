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
#include "VideoRenderer.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtCore/QRunnable>
#include <QImage>

#include "LogSettings.h"


static constexpr auto VertexShaderCode = R"xxx(
attribute highp vec4 vertices;
attribute mediump vec4 texCoord;
varying mediump vec4 texc;
void main() {
    gl_Position = vertices;
    texc = texCoord;
}
)xxx";

static constexpr auto FragmentShaderCode = R"xxx(
uniform sampler2D texture;
varying mediump vec4 texc;
void main() {
    gl_FragColor = texture2D(texture, texc.st);
}
)xxx";

VideoRenderer::VideoRenderer()
    : m_program(nullptr)
{
    SPDLOG_DEBUG("VideoRenderer::VideoRenderer()");
}

VideoRenderer::~VideoRenderer()
{
    SPDLOG_DEBUG("VideoRenderer::~VideoRenderer()");
    if (m_program != nullptr)
        delete m_program;
}

void VideoRenderer::init()
{
    if (!m_program) {
        QSGRendererInterface *rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        createObject();

        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, VertexShaderCode);
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, FragmentShaderCode);

        m_program->bindAttributeLocation("vertices", 0);
        m_program->bindAttributeLocation("texCoord", 1);
        m_program->link();
    }
}

void VideoRenderer::createObject()
{
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


    m_videoTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_videoTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    m_videoTexture->create();
    m_videoTexture->setSize(1920, 1080, 1);
    m_videoTexture->setFormat(QOpenGLTexture::RGB8_UNorm);
    m_videoTexture->allocateStorage();

    //m_videoTexture = new QOpenGLTexture(QImage(QString(":/side1.png")).mirrored());
    auto bg = QImage(QString(":/bts.png")).convertToFormat(QImage::Format_RGB888);
    m_videoTexture->setData(0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, bg.constBits(), nullptr);

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
}

void VideoRenderer::recreateTexture(int width, int height)
{
    delete m_videoTexture;
    m_videoTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_videoTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    m_videoTexture->create();
    m_videoTexture->setSize(width, height, 1);
    m_videoTexture->setFormat(QOpenGLTexture::RGB8_UNorm);
    m_videoTexture->allocateStorage();
}


void VideoRenderer::paint()
{
    SPDLOG_DEBUG("VideoRenderer::paint() start");
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();

    m_program->bind();
    m_videoTexture->bind();
    m_vbo.bind();

    m_program->setUniformValue("texture", 0);
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 4 * sizeof(GLfloat));
    m_program->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));

    // update texture
    {
        std::lock_guard<std::mutex> lock_guard(m_videoMutex);
        if (m_lastVideoData) {
            SPDLOG_DEBUG("VideoRenderer:: update video texture!!");

            if (m_videoTexture->width() != m_lastVideoData->width() ||
                m_videoTexture->height() != m_lastVideoData->height()) {
                SPDLOG_INFO("Recreate video texture()!!, width={}, height={}",
                            m_lastVideoData->width(), m_lastVideoData->height());
                recreateTexture(m_lastVideoData->width(), m_lastVideoData->height());
            }

            m_videoTexture->setData(0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8,
                                    m_lastVideoData->data(0), nullptr);

            m_lastVideoData.reset();
        }
    }

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->release();
    m_vbo.release();
    m_videoTexture->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();

    m_window->endExternalCommands();
}


void VideoRenderer::updateVideoOut(DecodedVideoDataPtr& videoData)
{
    std::lock_guard<std::mutex> lock_guard(m_videoMutex);
    SPDLOG_TRACE("VideoRenderer::update()");
    m_lastVideoData = videoData;

    if (m_window)
        m_window->update();
}
