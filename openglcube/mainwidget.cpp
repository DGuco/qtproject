#include "mainwidget.h"
#include <QMouseEvent>
#include <math.h>
#include <QTime>
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    texture(0),
    angularSpeed(0),
	viewChageDisOld(0),
	viewChageDisNew(0),
	proindex(0)
{
}

MainWidget::~MainWidget()
{
	/*
	Your widget's OpenGL rendering context is made current when paintGL(), resizeGL(), or initializeGL() is called. 
	If you need to call the standard OpenGL API functions from other places (e.g. in your widget's constructor or 
	in your own paint functions), you must call makeCurrent() first.All rendering happens into an OpenGL framebuffer object.
	makeCurrent() ensure that it is bound in the context.
	*/
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    //记录鼠标按下时的位置信息
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    //释放时和按下时的位置差
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
    qreal acc = diff.length() / 100.0;
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    //累加鼠标移动的速度
    angularSpeed += acc;
}

void MainWidget::wheelEvent(QWheelEvent *event)
{
	viewChageDisNew += event->delta();
	if (viewChageDisNew >= 360)
	{
		viewChageDisNew = 360;
	}

	if (viewChageDisNew <= -360)
	{
		viewChageDisNew = -360;
	}
}

void MainWidget::timerEvent(QTimerEvent *event)
{
	//旋转慢慢停下来
    angularSpeed *= 0.99;
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    }

	if (angularSpeed > 0 || viewChageDisOld != viewChageDisNew)
	{
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
		update();
	}
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    initShaders();
    initTextures();
    geometries = new GeometryEngine;
    //启动刷新定时器
    timer.start(1000 / ACTION_FPS, this);
}

//! [3]
void MainWidget::initShaders()
{
    // 编译 筛子 vertex shader
    if (!cubeProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader.vs"))
        close();

    // 编译 筛子 fragment shader
    if (!cubeProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader.fs"))
        close();

	// 编译 lighting vertex shader
	if (!lightinhProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/base_lighting.vs"))
		close();

	// 编译 lighting fragment shader
	if (!lightinhProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/base_lighting.fs"))
		close();

	// Link shader pipeline
	if (!cubeProgram.link())
		close();

	// Link shader pipeline
	if (!lightinhProgram.link())
		close();
}
void MainWidget::initTextures()
{
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h)
{
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = 2.0, zFar = 20.0, fov = 45.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

//opengl渲染函数
void MainWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	//开启遮挡剔除
	glDisable(GL_CULL_FACE);
    texture->bind();
	qreal viewChangeRate = viewChageDisNew / 1000.f;
	viewChageDisOld = viewChageDisNew;
	QMatrix4x4 viewmatrix;
	//计算view矩阵
	viewmatrix.lookAt(QVector3D(0,0,8), QVector3D(0, 0, -20), QVector3D(0, 1, 0));
	//模型坐标转换矩阵坐标
	QMatrix4x4 modelmatrix;
	//平移至左下角
	modelmatrix.translate(-1.6,-0.8,0);
	//鼠标滚动旋转角度
	modelmatrix.rotate(rotation);
	//滚轮缩放
	modelmatrix.scale(1.0 * (1 + viewChangeRate));

	//设置cube着色器变量
	QMatrix4x4 mvpmatrix = projection * viewmatrix * modelmatrix;
	cubeProgram.setUniformValue("mvp_matrix", mvpmatrix);
	cubeProgram.setUniformValue("texture", 0);
	geometries->drawCubeGeometry(&cubeProgram);


	//设置光源cube着色器变量
	QMatrix4x4 lampmodelmatrix;
	lampmodelmatrix.translate(1.0, 0.8, 0);
	//缩放
	lampmodelmatrix.scale(0.6);
	QMatrix4x4 lampmvpmatrix = projection * viewmatrix * lampmodelmatrix;
	lightinhProgram.setUniformValue("mvp_matrix", lampmvpmatrix);
	lightinhProgram.setUniformValue("texture", 0);
	geometries->drawLighting(&lightinhProgram);
}
