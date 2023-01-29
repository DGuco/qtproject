#include "glbuffers.h"
#include <QtGui/qmatrix4x4.h>


void qgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    const GLdouble ymax = zNear * tan(fovy * M_PI / 360.0);
    const GLdouble ymin = -ymax;
    const GLdouble xmin = ymin * aspect;
    const GLdouble xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

//============================================================================//
//                                  GLTexture                                 //
//============================================================================//

GLTexture::GLTexture() : m_texture(0), m_failed(false)
{
    glGenTextures(1, &m_texture);
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_texture);
}

//============================================================================//
//                                 GLTexture2D                                //
//============================================================================//

GLTexture2D::GLTexture2D(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, 0);
}


GLTexture2D::GLTexture2D(const QString& fileName, int width, int height)
{
    // TODO: Add error handling.
    QImage image(fileName);

    if (image.isNull()) {
        m_failed = true;
        return;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    //qDebug() << "Image size:" << image.width() << "x" << image.height();
    if (width <= 0)
        width = image.width();
    if (height <= 0)
        height = image.height();
    if (width != image.width() || height != image.height())
        image = image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Works on x86, so probably works on all little-endian systems.
    // Does it work on big-endian systems?
    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width(), image.height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::load(int width, int height, QRgb *data)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glEnable(GL_TEXTURE_2D);
}

void GLTexture2D::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}


//============================================================================//
//                                 GLTexture3D                                //
//============================================================================//

GLTexture3D::GLTexture3D(int width, int height, int depth)
{
    GLBUFFERS_ASSERT_OPENGL("GLTexture3D::GLTexture3D", glTexImage3D, return)

    glBindTexture(GL_TEXTURE_3D, m_texture);
    glTexImage3D(GL_TEXTURE_3D, 0, 4, width, height, depth, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void GLTexture3D::load(int width, int height, int depth, QRgb *data)
{
    GLBUFFERS_ASSERT_OPENGL("GLTexture3D::load", glTexImage3D, return)

    glBindTexture(GL_TEXTURE_3D, m_texture);
    glTexImage3D(GL_TEXTURE_3D, 0, 4, width, height, depth, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void GLTexture3D::bind()
{
    glBindTexture(GL_TEXTURE_3D, m_texture);
    glEnable(GL_TEXTURE_3D);
}

void GLTexture3D::unbind()
{
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
}

//============================================================================//
//                                GLTextureCube                               //
//============================================================================//

GLTextureCube::GLTextureCube(int size)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    for (int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 4, size, size, 0,
            GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLTextureCube::GLTextureCube(const QStringList& fileNames, int size)
{
	//glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    int index = 0;
    foreach (QString file, fileNames) {
        QImage image(file);
        if (image.isNull()) {
            m_failed = true;
            break;
        }

        image = image.convertToFormat(QImage::Format_ARGB32);
        if (size <= 0)
            size = image.width();
        if (size != image.width() || size != image.height())
            image = image.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, 4, image.width(), image.height(), 0,GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

        if (++index == 6)
            break;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GLTextureCube::load(int size, int face, QRgb *data)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 4, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GLTextureCube::bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    glEnable(GL_TEXTURE_CUBE_MAP);
}

void GLTextureCube::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

//============================================================================//
//                            GLFrameBufferObject                             //
//============================================================================//

GLFrameBufferObject::GLFrameBufferObject(int width, int height)
    : m_fbo(0)
    , m_depthBuffer(0)
    , m_width(width)
    , m_height(height)
    , m_failed(false)
{
    GLBUFFERS_ASSERT_OPENGL("GLFrameBufferObject::GLFrameBufferObject",
        glGenFramebuffersEXT && glGenRenderbuffersEXT && glBindRenderbufferEXT && glRenderbufferStorageEXT, return)
	//创建一个帧缓冲对象
    glGenFramebuffersEXT(1, &m_fbo);
	//将它绑定为激活的帧缓冲
    //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	//创建一个渲染缓冲对象
    glGenRenderbuffersEXT(1, &m_depthBuffer);
	//绑定这个渲染缓冲对象
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffer);
	//创建一个深度和模板渲染缓冲对象
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_width, m_height);
	//附加这个渲染缓冲对象
    //glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBuffer);
    //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLFrameBufferObject::~GLFrameBufferObject()
{
    GLBUFFERS_ASSERT_OPENGL("GLFrameBufferObject::~GLFrameBufferObject",
        glDeleteFramebuffersEXT && glDeleteRenderbuffersEXT, return)

    glDeleteFramebuffersEXT(1, &m_fbo);
    glDeleteRenderbuffersEXT(1, &m_depthBuffer);
}

void GLFrameBufferObject::setAsRenderTarget(bool state)
{
    GLBUFFERS_ASSERT_OPENGL("GLFrameBufferObject::setAsRenderTarget", glBindFramebufferEXT, return)

    if (state) 
	{
		//将它绑定为激活的帧缓冲
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, m_width, m_height);
    } else
	{
        glPopAttrib();
		//要保证所有的渲染操作在主窗口中有视觉效果，我们需要再次激活默认帧缓冲，将它绑定到0
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
}

bool GLFrameBufferObject::isComplete()
{
    GLBUFFERS_ASSERT_OPENGL("GLFrameBufferObject::isComplete", glCheckFramebufferStatusEXT, return false)

    return GL_FRAMEBUFFER_COMPLETE_EXT == glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
}

//============================================================================//
//                             GLRenderTargetCube                             //
//============================================================================//

GLRenderTargetCube::GLRenderTargetCube(int size)
    : GLTextureCube(size)
    , m_fbo(size, size)
{
}

void GLRenderTargetCube::begin(int face)
{
    GLBUFFERS_ASSERT_OPENGL("GLRenderTargetCube::begin",
        glFramebufferTexture2DEXT && glFramebufferRenderbufferEXT, return)

    m_fbo.setAsRenderTarget(true);
	//纹理附加到帧缓冲
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_texture, 0);
	//附加这个渲染缓冲对象
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_fbo.m_depthBuffer);
	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		qWarning() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		return;
	}
}

void GLRenderTargetCube::end()
{
    m_fbo.setAsRenderTarget(false);
}

void GLRenderTargetCube::getViewMatrix(QMatrix4x4& mat, int face)
{
    if (face < 0 || face >= 6) {
        qWarning("GLRenderTargetCube::getViewMatrix: 'face' must be in the range [0, 6). (face == %d)", face);
        return;
    }

    static int perm[6][3] = {
        {2, 1, 0},
        {2, 1, 0},
        {0, 2, 1},
        {0, 2, 1},
        {0, 1, 2},
        {0, 1, 2},
    };

    static float signs[6][3] = {
        {-1.0f, -1.0f, -1.0f},
        {+1.0f, -1.0f, +1.0f},
        {+1.0f, +1.0f, -1.0f},
        {+1.0f, -1.0f, +1.0f},
        {+1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, +1.0f},
    };

    mat.fill(0.0f);
    for (int i = 0; i < 3; ++i)
        mat(i, perm[face][i]) = signs[face][i];
    mat(3, 3) = 1.0f;
}

void GLRenderTargetCube::getProjectionMatrix(QMatrix4x4& mat, float nearZ, float farZ)
{
    static const QMatrix4x4 reference(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f);

    mat = reference;
    mat(2, 2) = (nearZ+farZ)/(nearZ-farZ);
    mat(2, 3) = 2.0f*nearZ*farZ/(nearZ-farZ);
}
