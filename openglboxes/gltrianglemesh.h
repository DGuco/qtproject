#ifndef GLTRIANGLEMESH_H
#define GLTRIANGLEMESH_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLBuffer>

#include "glbuffers.h"

template<class TVertex, class TIndex>
class GLTriangleMesh
{
public:
    GLTriangleMesh(int vertexCount, int indexCount) : m_vb(vertexCount), m_ib(indexCount)
    {
    }

    virtual ~GLTriangleMesh()
    {
    }

    virtual void draw(QGLShaderProgram *program)
    {
        if (failed())
            return;

        int type = GL_UNSIGNED_INT;
        if (sizeof(TIndex) == sizeof(char)) type = GL_UNSIGNED_BYTE;
        if (sizeof(TIndex) == sizeof(short)) type = GL_UNSIGNED_SHORT;

        m_vb.bind(program);
        m_ib.bind();
        glDrawElements(GL_TRIANGLES, m_ib.length(), type, BUFFER_OFFSET(0));
        m_vb.unbind(program);
        m_ib.unbind();
    }

    bool failed()
    {
        return m_vb.failed() || m_ib.failed();
    }
protected:
    GLVertexBuffer<TVertex> m_vb;
    GLIndexBuffer<TIndex> m_ib;
};


#endif
