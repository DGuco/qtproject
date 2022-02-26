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
        // ����4����
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},{QVector3D( 1.0f, -1.0f,  1.0f), QVector2D((1 / 3.0f), 0.0f)},
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f,(1 / 2.0f))},{QVector3D( 1.0f,  1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))}, 
		// ����1����
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f,(1 / 2.0f))},{QVector3D( 1.0f, -1.0f, -1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))},  
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},{QVector3D( 1.0f,  1.0f, -1.0f), QVector2D((1 / 3.0f), 1.0f)}, 
        //����3����
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},{QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f,(1 / 2.0f))},  
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f), 1.0f)},{QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  
        //����6����
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f), 0.0f)},{QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},   
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},{QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f,(1 / 2.0f))},   
        //����5����
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D((1 / 3.0f), 0.0f)},{QVector3D( 1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f), 0.0f)},  
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f))},{QVector3D( 1.0f, -1.0f,  1.0f), QVector2D((2 / 3.0f),(1 / 2.0f))},  
        //����2����
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
		 * ÿ������������Ⱦһ����
		 * ע��ڵ�˳����Ϊ�������ڵ��޳�(glEnable(GL_CULL_FACE))�� opengl�Ǹ��ݶ���˳����������η��߷����,˳ʱ��˳�������
		 * �������ǳ���ľͲ�����,���� 0 3 1 �ᵼ�¸������β���ʾ,�����������ͬ���ĵ���ע�ⶥ��˳��
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
    // �󶨶������� VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 48 * sizeof(VertexData));

    // ���������� VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 72 * sizeof(GLushort));
}

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program)
{
    quintptr offset = 0;

	//����opengl��ȡ�������껺������ĸ�ʽ:����float,ÿ�����ݴ�Сsizeof(VertexData),
	//����������ÿ�������е�ƫ��0,���ݴ�С3��float,
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

	//����opengl��ȡ�������껺������ĸ�ʽ:����float,ÿ�����ݴ�Сsizeof(VertexData),
	//����������ÿ�������е�ƫ��sizeof(QVector3D),���ݴ�С2��float,
	offset += sizeof(QVector3D);
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

	// ʹ����������������Ⱦ������
    glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);  
}

void GeometryEngine::drawLighting(QOpenGLShaderProgram *program)
{
}