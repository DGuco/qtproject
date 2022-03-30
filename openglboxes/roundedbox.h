#ifndef ROUNDEDBOX_H
#define ROUNDEDBOX_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtWidgets>
#include <QtOpenGL>

#include "gltrianglemesh.h"
#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>
#include "glbuffers.h"

struct P3T2N3Vertex
{
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
	QVector4D color;
    static VertexDescription description[];
};

#define VERTEX_COLOR QVector4D(1.0f,1.0f,1.0f,0.0f)

class GLRoundedBox : public GLTriangleMesh<P3T2N3Vertex, unsigned short>
{
public:
    // 0 < r < 0.5, 0 <= n <= 125
    explicit GLRoundedBox(float r = 0.25f, float scale = 1.0f, int n = 10);
};


#endif
