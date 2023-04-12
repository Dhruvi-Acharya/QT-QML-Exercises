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
#include "OpenGLRenderNode.h"
#include <QQuickItem>


#include "LogSettings.h"

//#if QT_CONFIG(opengl)
#include <QOpenGLPixelTransferOptions>

static constexpr int VERTEX_SIZE = 8 * sizeof(GLfloat);

static constexpr auto VSHADER_SOURCE = R"x(
attribute highp vec4 vertices;
attribute mediump vec4 texCoord;
varying mediump vec4 texc;
uniform highp mat4 matrix;
void main() {
    gl_Position = matrix * vertices;
    texc = texCoord;
}
)x";


static constexpr auto FSHADER_SOURCE = R"x(
uniform sampler2D texture_rgb;
uniform sampler2D texture_y;
uniform sampler2D texture_u;
uniform sampler2D texture_v;
uniform bool isYuv;
uniform lowp float opacity;
varying mediump vec4 texc;
void main() {
    mediump vec3 yuv;
    mediump vec3 rgb;
    if (isYuv) {
        yuv.x = texture2D(texture_y, texc.st).r;
        yuv.y = texture2D(texture_u, texc.st).r - 0.5;
        yuv.z = texture2D(texture_v, texc.st).r - 0.5;

        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;

        gl_FragColor = vec4(rgb, 1);
    }
    else {
        rgb  = texture2D(texture_rgb, texc.st).rgb;
        gl_FragColor = vec4(rgb, 1);
    }
}
)x";

OpenGLRenderNode::OpenGLRenderNode()
{
    SPDLOG_DEBUG("OpenGLRenderNode::OpenGLRenderNode()");
}


OpenGLRenderNode::~OpenGLRenderNode()
{
    SPDLOG_DEBUG("OpenGLRenderNode::~OpenGLRenderNode()");
    releaseResources();
}

void OpenGLRenderNode::releaseResources()
{
    if (m_program) {
        delete m_program;
        m_program = nullptr;
    }
    if (m_vbo) {
        delete m_vbo;
        m_vbo = nullptr;
    }
    releaseTexture();
}

void OpenGLRenderNode::releaseTexture()
{
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
}

void OpenGLRenderNode::checkTexture(const int width, const int height, const bool useYuv)
{
    if (m_textureWidth == width && m_textureHeight == height && m_isYuvTexture == useYuv)
        return;

    SPDLOG_DEBUG("OpenGLRenderNode::checkTexture, width={}, height={}, useYuv={}",
                 width, height, useYuv);

    releaseTexture();

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
    }
    else {
        m_videoTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_videoTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        m_videoTexture->create();
        m_videoTexture->setSize(width, height, 1);
        m_videoTexture->setFormat(QOpenGLTexture::RGB8_UNorm);
        m_videoTexture->allocateStorage();
    }

    m_textureWidth = width;
    m_textureHeight = height;
    m_isYuvTexture = useYuv;
}

void OpenGLRenderNode::init()
{
    SPDLOG_DEBUG("OpenGLRenderNode::init()");

    m_program = new QOpenGLShaderProgram;

    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, VSHADER_SOURCE);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, FSHADER_SOURCE);

    m_program->bindAttributeLocation("vertices", 0);
    m_program->bindAttributeLocation("texCoord", 1);
    if (!m_program->link()) {
        SPDLOG_CRITICAL("line shader program failed.");
        return;
    }

    m_matrixUniform = m_program->uniformLocation("matrix");
    m_opacityUniform = m_program->uniformLocation("opacity");

    m_textureUniformRgb = m_program->uniformLocation("texture_rgb");
    m_textureUniformY = m_program->uniformLocation("texture_y");
    m_textureUniformU = m_program->uniformLocation("texture_u");
    m_textureUniformV = m_program->uniformLocation("texture_v");
    m_useYuvUniform = m_program->uniformLocation("isYuv");

    checkTexture(64, 64, false);

    static GLfloat texCoords[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    m_vbo = new QOpenGLBuffer;
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(VERTEX_SIZE + sizeof(texCoords));

    m_vbo->write(VERTEX_SIZE, texCoords, sizeof(texCoords));
    m_vbo->release();
    m_sizeUpdated = true;
}


void OpenGLRenderNode::checkVideoFrame()
{
    std::lock_guard<std::mutex> lock_guard(m_videoMutex);
    if (m_lastVideoOutput) {
        //SPDLOG_DEBUG("OpenGLRenderNode::updte VideoFrame");

        bool useYuv = m_lastVideoOutput->format() == AVPixelFormat::AV_PIX_FMT_YUV420P;
        checkTexture(m_lastVideoOutput->width(), m_lastVideoOutput->height(), useYuv);

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

void OpenGLRenderNode::setBindVbo()
{
    m_vbo->bind();
    if (m_sizeUpdated) {
        m_sizeUpdated = false;

        SPDLOG_DEBUG("OpenGLRenderNode::resetVbo(), width={}, height={}", m_width, m_height);
        QPointF p0(m_width - 2, 0);
        QPointF p1(0, 0);
        QPointF p2(0, m_height - 2);
        QPointF p3(m_width - 2, m_height - 2);

        GLfloat vertices[8] = { GLfloat(p0.x()), GLfloat(p0.y()),
                                GLfloat(p1.x()), GLfloat(p1.y()),
                                GLfloat(p2.x()), GLfloat(p2.y()),
                                GLfloat(p3.x()), GLfloat(p3.y())};
        m_vbo->write(0, vertices, sizeof(vertices));
    }
}

void OpenGLRenderNode::render(const RenderState *state)
{
    if (!m_program)
        init();

    //SPDLOG_DEBUG("OpenGLRenderNode::render() Entered");
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    m_program->bind();
    m_program->setUniformValue(m_matrixUniform, *state->projectionMatrix() * *matrix());
    m_program->setUniformValue(m_opacityUniform, float(inheritedOpacity()));

    setBindVbo();
    checkVideoFrame();


    if (m_isYuvTexture) {
        m_program->setUniformValue(m_useYuvUniform, 1);

//        f->glActiveTexture(GL_TEXTURE0);
//        f->glBindTexture(GL_TEXTURE_2D, m_videoTextureY->textureId());
//        f->glActiveTexture(GL_TEXTURE1);
//        f->glBindTexture(GL_TEXTURE_2D, m_videoTextureU->textureId());
//        f->glActiveTexture(GL_TEXTURE2);
//        f->glBindTexture(GL_TEXTURE_2D, m_videoTextureV->textureId());
//        f->glUniform1i(m_textureUniformY, 0);
//        f->glUniform1i(m_textureUniformU, 1);
//        f->glUniform1i(m_textureUniformV, 2);
//
        m_videoTextureY->bind(0);
        m_videoTextureU->bind(1);
        m_videoTextureV->bind(2);
        m_program->setUniformValue(m_textureUniformY, 0);
        m_program->setUniformValue(m_textureUniformU, 1);
        m_program->setUniformValue(m_textureUniformV, 2);
    }
    else {
        m_program->setUniformValue(m_useYuvUniform, 0);

        m_videoTexture->bind();
        m_program->setUniformValue(m_useYuvUniform, 0);
        m_program->setUniformValue(m_textureUniformRgb, 0);
    }

    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 2);
    m_program->setAttributeBuffer(1, GL_FLOAT, VERTEX_SIZE, 2);
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);

    // We are prepared both for the legacy (direct OpenGL) and the modern
    // (abstracted by RHI) OpenGL scenegraph. So set all the states that are
    // important to us.

    f->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (state->scissorEnabled()) {
        SPDLOG_DEBUG("ScissorEnabled..");
        f->glEnable(GL_SCISSOR_TEST);
        const QRect r = state->scissorRect(); // already bottom-up
        f->glScissor(r.x(), r.y(), r.width(), r.height());
    }
    if (state->stencilEnabled()) {
        SPDLOG_DEBUG("StencilEnabled..");
        f->glEnable(GL_STENCIL_TEST);
        f->glStencilFunc(GL_EQUAL, state->stencilValue(), 0xFF);
        f->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    //f->glDisable(GL_DEPTH_TEST); // test code

    f->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    if (m_isYuvTexture) {
        m_videoTextureY->release();
        m_videoTextureU->release();
        m_videoTextureV->release();
    }
    else {
        m_videoTexture->release();
    }
    m_vbo->release();
    m_program->release();
}

QSGRenderNode::StateFlags OpenGLRenderNode::changedStates() const
{
    return BlendState | ScissorState | StencilState;
}

QSGRenderNode::RenderingFlags OpenGLRenderNode::flags() const
{
    return BoundedRectRendering | DepthAwareRendering;
}

QRectF OpenGLRenderNode::rect() const
{
    return QRect(0, 0, m_width, m_height);
}

void OpenGLRenderNode::sync(QQuickItem *item)
{
    if (item->width() != m_width || item->height() != m_height)  {
        m_width = item->width();
        m_height = item->height();
        m_sizeUpdated = true;
    }
}

void OpenGLRenderNode::updateVideoFrame(const VideoOutputFrameSharedPtr &videoOutput)
{
    {
        std::lock_guard<std::mutex> lock_guard(m_videoMutex);
        m_lastVideoOutput = videoOutput;
    }
}

//#endif // opengl
