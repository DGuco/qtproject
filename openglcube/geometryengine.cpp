#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>

GeometryEngine::GeometryEngine()
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    arrayBuf.create();
    indexBuf.create();
    initCubeGeometry();
}

GeometryEngine::~GeometryEngine()
{
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initCubeGeometry()
{
    VertexData vertices[48] = {
		////////////////////////////////CUBE1//////////////////////////////////////
        // 点数4的面
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},{QVector3D( 1.0f, -1.0f,  1.0f), QVector2D((1 / 3.0f), 0.0f)},
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f,(1 / 2.0f))},{QVector3D( 1.0f,  1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))}, 
		// 点数1的面
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f,(1 / 2.0f))},{QVector3D( 1.0f, -1.0f, -1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))},  
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},{QVector3D( 1.0f,  1.0f, -1.0f), QVector2D((1 / 3.0f), 1.0f)}, 
        //点数3的面
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},{QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f,(1 / 2.0f))},  
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f), 1.0f)},{QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  
        //点数6的面
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f), 0.0f)},{QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},   
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},{QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f,(1 / 2.0f))},   
        //点数5的面
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D((1 / 3.0f), 0.0f)},{QVector3D( 1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f), 0.0f)},  
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))},{QVector3D( 1.0f, -1.0f,  1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},  
        //点数2的面
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))},{QVector3D( 1.0f,  1.0f,  1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},  
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D((1 / 3.0f), 1.0f)},{QVector3D( 1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f), 1.0f)},	
    };

	int tmpIndex = 0;
	for (int index = 24; index < 48; index++)
	{
		vertices[index].position = vertices[tmpIndex].position + QVector3D(3.5F, 3.5F, 0.0f);
		vertices[index].texCoord = vertices[tmpIndex].texCoord;
		tmpIndex++;
	}
	GLushort indices[72] = {
		/**
		 * 每两个三角形渲染一个面
		 * 注意节点顺序，因为开启了遮挡剔除(glEnable(GL_CULL_FACE))， opengl是根据顶点顺序决定三角形法线方向的,顺时针顺序算出来
		 * 三角形是朝里的就不画了,所以 0 3 1 会导致该三角形不显示,后面的三角形同样的道理注意顶点顺序
		 */
		//Face 0
		1,  0 , 3, //triangle1
		2,  0,  3, //triangle2
		//Face 1 
		4,  5,  7, //triangle3
		6,  4,  7, //triangle4
		//Face 2 
		8,  9,  11, //triangle5
		10,  8,  11, //triangle6
		//Face 3 
		12,  13,  15, //triangle7
		14,  12,  15, //triangle8
		//Face 4 
		16,  17,  19, //triangle9
		18,  16,  19, //triangle10
		//Face 5 
		20,  21,  23, //triangle11
		22,  20,  23, //triangle12
	};
	tmpIndex = 0;
	for (int index = 36; index < 72; index++)
	{
		indices[index] = indices[tmpIndex] + 36;
		tmpIndex++;
	}
    // 绑定顶点数组 VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 48 * sizeof(VertexData));

    // 绑定索引数组 VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 72 * sizeof(GLushort));
}

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program)
{
    quintptr offset = 0;

	//告诉opengl读取顶点坐标缓存数组的格式:类型float,每个数据大小sizeof(VertexData),
	//顶点坐标在每个数据中的偏移0,数据大小3个float,
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

	//告诉opengl读取纹理坐标缓存数组的格式:类型float,每个数据大小sizeof(VertexData),
	//纹理坐标在每个数据中的偏移sizeof(QVector3D),数据大小2个float,
	offset += sizeof(QVector3D);
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

	// 使用索引缓冲数组渲染三角形
    glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);  
}

void GeometryEngine::drawLighting(QOpenGLShaderProgram *program)
{
}