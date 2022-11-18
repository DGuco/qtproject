#ifndef GLBUFFERS_H
#define GLBUFFERS_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>

#define BUFFER_OFFSET(i) ((char*)0 + (i))
#define SIZE_OF_MEMBER(cls, member) sizeof(static_cast<cls *>(0)->member)

#define GLBUFFERS_ASSERT_OPENGL(prefix, assertion, returnStatement)                         \
if (m_failed || !(assertion)) {                                                             \
    if (!m_failed) qCritical(prefix ": The necessary OpenGL functions are not available."); \
    m_failed = true;                                                                        \
    returnStatement;                                                                        \
}

void qgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

QT_BEGIN_NAMESPACE
class QMatrix4x4;
QT_END_NAMESPACE

class GLTexture
{
public:
    GLTexture();
    virtual ~GLTexture();
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool failed() const {return m_failed;}
protected:
    GLuint m_texture;
    bool m_failed;
};

class GLFrameBufferObject
{
public:
    friend class GLRenderTargetCube;
    // friend class GLRenderTarget2D;

    GLFrameBufferObject(int width, int height);
    virtual ~GLFrameBufferObject();
    bool isComplete();
    virtual bool failed() const {return m_failed;}
protected:
    void setAsRenderTarget(bool state = true);
    GLuint m_fbo;
    GLuint m_depthBuffer;
    int m_width, m_height;
    bool m_failed;
};

/**
 * 2d纹理
 */
class GLTexture2D : public GLTexture
{
public:
    GLTexture2D(int width, int height);
    explicit GLTexture2D(const QString& fileName, int width = 0, int height = 0);
    void load(int width, int height, QRgb *data);
    void bind() override;
    void unbind() override;
};


/**
 * 3dw纹理
 */
class GLTexture3D : public GLTexture
{
public:
    GLTexture3D(int width, int height, int depth);
    // TODO: Implement function below
    //GLTexture3D(const QString& fileName, int width = 0, int height = 0);
    void load(int width, int height, int depth, QRgb *data);
    void bind() override;
    void unbind() override;
};

/**
 * 立方体纹理贴图
 */
class GLTextureCube : public GLTexture
{
public:
    GLTextureCube(int size);
    explicit GLTextureCube(const QStringList& fileNames, int size = 0);
    void load(int size, int face, QRgb *data);
    void bind() override;
    void unbind() override;
};

class GLRenderTargetCube : public GLTextureCube
{
public:
    GLRenderTargetCube(int size);
    // begin rendering to one of the cube's faces. 0 <= face < 6
    void begin(int face);
    // end rendering
    void end();
    bool failed() const override { return m_failed || m_fbo.failed(); }

    static void getViewMatrix(QMatrix4x4& mat, int face);
    static void getProjectionMatrix(QMatrix4x4& mat, float nearZ, float farZ);
private:
    GLFrameBufferObject m_fbo;
};

struct VertexDescription
{
    enum
    {
        Null = 0, // Terminates a VertexDescription array
        Position,
        TexCoord,
        Normal,
        Color,
    };
    int field; // Position, TexCoord, Normal, Color
    int type; // GL_FLOAT, GL_UNSIGNED_BYTE
    int count; // number of elements
    int offset; // field's offset into vertex struct
    int index; // 0 (unused at the moment)
};

template<class T>
class GLVertexBuffer : public QOpenGLFunctions
{
public:
    GLVertexBuffer(int length, const T *data = 0, int mode = GL_STATIC_DRAW)
        : m_length(0)
        , m_mode(mode)
        , m_buffer(0)
        , m_failed(false)
    {
        GLBUFFERS_ASSERT_OPENGL("GLVertexBuffer::GLVertexBuffer", glGenBuffers && glBindBuffer && glBufferData, return)

        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferData(GL_ARRAY_BUFFER, (m_length = length) * sizeof(T), data, mode);

		m_glbuffer.create();
		m_glbuffer.bind();
		m_glbuffer.allocate(data, (m_length = length) * sizeof(T));
	}

    ~GLVertexBuffer()
    {
        GLBUFFERS_ASSERT_OPENGL("GLVertexBuffer::~GLVertexBuffer", glDeleteBuffers, return)

        glDeleteBuffers(1, &m_buffer);
		m_glbuffer.destroy();
    }

    void bind(QGLShaderProgram *program)
    {
        GLBUFFERS_ASSERT_OPENGL("GLVertexBuffer::bind", glBindBuffer, return)
        
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        for (VertexDescription *desc = T::description; desc->field != VertexDescription::Null; ++desc) 
		{
            switch (desc->field) {
            case VertexDescription::Position:
                glVertexPointer(desc->count, desc->type, sizeof(T), BUFFER_OFFSET(desc->offset));
                glEnableClientState(GL_VERTEX_ARRAY);
                break;
            case VertexDescription::TexCoord:
                glTexCoordPointer(desc->count, desc->type, sizeof(T), BUFFER_OFFSET(desc->offset));
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                break;
            case VertexDescription::Normal:
                glNormalPointer(desc->type, sizeof(T), BUFFER_OFFSET(desc->offset));
                glEnableClientState(GL_NORMAL_ARRAY);
                break;
            case VertexDescription::Color:
                glColorPointer(desc->count, desc->type, sizeof(T), BUFFER_OFFSET(desc->offset));
                glEnableClientState(GL_COLOR_ARRAY);
                break;
            default:
                break;
            }
        }

		int offset = 0;
		int vertexLocation = program->attributeLocation("a_position");
		program->enableAttributeArray(vertexLocation);
		program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(T));

		offset += sizeof(QVector3D);
		int texcoordLocation = program->attributeLocation("a_texcoord");
		program->enableAttributeArray(texcoordLocation);
		program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(T));

		offset += sizeof(QVector2D);
		int normalLocation = program->attributeLocation("a_normal");
		program->enableAttributeArray(normalLocation);
		program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(T));

		offset += sizeof(QVector3D);
		int colorLocation = program->attributeLocation("a_color");
		program->enableAttributeArray(colorLocation);
		program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(T));
    }

    void unbind(QGLShaderProgram *program)
    {
        GLBUFFERS_ASSERT_OPENGL("GLVertexBuffer::unbind", glBindBuffer, return)

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        for (VertexDescription *desc = T::description; desc->field != VertexDescription::Null; ++desc) {
            switch (desc->field) {
            case VertexDescription::Position:
                glDisableClientState(GL_VERTEX_ARRAY);
                break;
            case VertexDescription::TexCoord:
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                break;
            case VertexDescription::Normal:
                glDisableClientState(GL_NORMAL_ARRAY);
                break;
            case VertexDescription::Color:
                glDisableClientState(GL_COLOR_ARRAY);
                break;
            default:
                break;
            }
        }
    }

    int length() const {return m_length;}

    T *lock()
    {
        GLBUFFERS_ASSERT_OPENGL("GLVertexBuffer::lock", glBindBuffer && glMapBuffer, return 0)

        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        //glBufferData(GL_ARRAY_BUFFER, m_length, NULL, m_mode);
        GLvoid* buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        m_failed = (buffer == 0);
        return reinterpret_cast<T *>(buffer);
    }

    void unlock()
    {
        GLBUFFERS_ASSERT_OPENGL("GLVertexBuffer::unlock", glBindBuffer && glUnmapBuffer, return)

        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    bool failed()
    {
        return m_failed;
    }

private:
	QOpenGLBuffer m_glbuffer;
    int m_length, m_mode;
    GLuint m_buffer;
    bool m_failed;
};

template<class T>
class GLIndexBuffer : public QOpenGLFunctions
{
public:
    GLIndexBuffer(int length, const T *data = 0, int mode = GL_STATIC_DRAW)
        : m_length(0)
        , m_mode(mode)
        , m_buffer(0)
        , m_failed(false)
    {
        GLBUFFERS_ASSERT_OPENGL("GLIndexBuffer::GLIndexBuffer", glGenBuffers && glBindBuffer && glBufferData, return)

        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (m_length = length) * sizeof(T), data, mode);

		m_glbuffer.create();
		m_glbuffer.bind();
		m_glbuffer.allocate(data, (m_length = length) * sizeof(T));
	}

    ~GLIndexBuffer()
    {
        GLBUFFERS_ASSERT_OPENGL("GLIndexBuffer::~GLIndexBuffer", glDeleteBuffers, return)

        glDeleteBuffers(1, &m_buffer);
		m_glbuffer.destroy();
    }

    void bind()
    {
        GLBUFFERS_ASSERT_OPENGL("GLIndexBuffer::bind", glBindBuffer, return)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
    }

    void unbind()
    {
        GLBUFFERS_ASSERT_OPENGL("GLIndexBuffer::unbind", glBindBuffer, return)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    int length() const {return m_length;}

    T *lock()
    {
        GLBUFFERS_ASSERT_OPENGL("GLIndexBuffer::lock", glBindBuffer && glMapBuffer, return 0)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
        GLvoid* buffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
        m_failed = (buffer == 0);
        return reinterpret_cast<T *>(buffer);
    }

    void unlock()
    {
        GLBUFFERS_ASSERT_OPENGL("GLIndexBuffer::unlock", glBindBuffer && glUnmapBuffer, return)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    bool failed()
    {
        return m_failed;
    }

private:
	QOpenGLBuffer m_glbuffer;
    int m_length, m_mode;
    GLuint m_buffer;
    bool m_failed;
};

#endif
