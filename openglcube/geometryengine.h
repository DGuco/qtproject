#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class GeometryEngine : protected QOpenGLFunctions
{
public:
	struct VertexData
	{
		QVector3D position;  //opengl 顶点坐标
		QVector2D texCoord;  //该顶点对应的纹理坐标(cube.png)
		QVector3D normal;	 //法线向量
	};
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawCubeGeometry(QOpenGLShaderProgram *program);
	void drawLighting(QOpenGLShaderProgram *program);

private:
    void initCubeGeometry();

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

#endif // GEOMETRYENGINE_H
