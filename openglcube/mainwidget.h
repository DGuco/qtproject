#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "geometryengine.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
	void wheelEvent(QWheelEvent *event) override;
    void timerEvent(QTimerEvent *e) override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
	void paintGL() override;
    void initShaders();
    void initTextures();
private:
    QBasicTimer timer;
    QOpenGLShaderProgram cubeProgram;
	QOpenGLShaderProgram lightinhProgram;
    GeometryEngine *geometries;
    QOpenGLTexture *texture;
    QMatrix4x4 projection;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
	qreal viewChageDisOld;
	qreal viewChageDisNew;
    QQuaternion rotation;
};

#endif // MAINWIDGET_H
