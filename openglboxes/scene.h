#ifndef SCENE_H
#define SCENE_H

//#include <GL/glew.h>
#include "glextensions.h"

#include <QtWidgets>
#include <QtOpenGL>

#include "roundedbox.h"
#include "gltrianglemesh.h"
#include "trackball.h"
#include "glbuffers.h"
#include "qtbox.h"

#define PI 3.14159265358979

QT_BEGIN_NAMESPACE
class QMatrix4x4;
QT_END_NAMESPACE

class ParameterEdit : public QWidget
{
public:
    virtual void emitChange() = 0;
};

class ColorEdit : public ParameterEdit
{
    Q_OBJECT
public:
    ColorEdit(QRgb initialColor, int id);
    QRgb color() const {return m_color;}
    void emitChange() override { emit colorChanged(m_color, m_id); }
public slots:
    void editDone();
signals:
    void colorChanged(QRgb color, int id);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void setColor(QRgb color); // also emits colorChanged()
private:
    QGraphicsScene *m_dialogParentScene;
    QLineEdit *m_lineEdit;
    QFrame *m_button;
    QRgb m_color;
    int m_id;
};

class FloatEdit : public ParameterEdit
{
    Q_OBJECT
public:
    FloatEdit(float initialValue, int id);
    float value() const {return m_value;}
    void emitChange() override { emit valueChanged(m_value, m_id); }
public slots:
    void editDone();
signals:
    void valueChanged(float value, int id);
private:
    QGraphicsScene *m_dialogParentScene;
    QLineEdit *m_lineEdit;
    float m_value;
    int m_id;
};

class GraphicsWidget : public QGraphicsProxyWidget
{
public:
    GraphicsWidget() : QGraphicsProxyWidget(0, Qt::Window) {}
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void resizeEvent(QGraphicsSceneResizeEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	void closeEvent(QCloseEvent* event) override;
};

class TwoSidedGraphicsWidget : public QObject
{
    Q_OBJECT
public:
    TwoSidedGraphicsWidget(QGraphicsScene *scene);
    void setWidget(int index, QWidget *widget);
    QWidget *widget(int index);
public slots:
    void flipwidget();
	void hidewidget();
	void showidget();
protected slots:
	//翻转动画
    void animateFlip();
	//隐藏动画
	void animateHide();
	//显示动画
	void animateShow();
private:
    GraphicsWidget *m_proxyWidgets[2];
    int m_current;
    int m_angle; // angle in degrees
    int m_delta;
	int m_scale;
};

class RenderOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    RenderOptionsDialog();
    int addTexture(const QString &name);
    int addShader(const QString &name);
    void emitParameterChanged();
	void emitHideWidget();
protected slots:
    void setColorParameter(QRgb color, int id);
    void setFloatParameter(float value, int id);
signals:
    void colorParameterChanged(const QString &, QRgb);
    void floatParameterChanged(const QString &, float);
    void textureChanged(int);
    void shaderChanged(int);
    void doubleClicked();
	void widgetHide();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    QVector<QByteArray> m_parameterNames;
    QComboBox *m_textureCombo;
    QComboBox *m_shaderCombo;
    QVector<ParameterEdit *> m_parameterEdits;
};

class ItemDialog : public QDialog
{
    Q_OBJECT
public:
    enum ItemType {
        QtBoxItem,
        CircleItem,
        SquareItem,
    };

    ItemDialog();
public:
	void emitHideWidget();
public slots:
    void triggerNewQtBox();
    void triggerNewCircleItem();
    void triggerNewSquareItem();
signals:
    void doubleClicked();
    void newItemTriggered(ItemDialog::ItemType type);
	void widgetHide();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene(int width, int height, int maxTextureSize);
    ~Scene();
    void drawBackground(QPainter *painter, const QRectF &rect) override;

public slots:
    void setShader(int index);
    void setTexture(int index);
    void setColorParameter(const QString &name, QRgb color);
    void setFloatParameter(const QString &name, float value);
    void newItem(ItemDialog::ItemType type);
protected:
    void renderBoxes(const QMatrix4x4 &projection_mat, const QMatrix4x4 &view_mat, const QMatrix4x4 &model_mat, int excludeBox = -2);
    void initOpenGLParams();
    void setLights(QGLShaderProgram* program);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent * event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
private:
    void initGL();
    QPointF pixelPosToViewPos(const QPointF& p);

    int m_lastTime;
    int m_mouseEventTime;
    int m_distExp;
    int m_frame;
    int m_maxTextureSize;

    int m_currentShader;
    int m_currentTexture;

    RenderOptionsDialog *m_renderOptions;
	TwoSidedGraphicsWidget *m_twoSidedGraphicsWidget;
    ItemDialog *m_itemDialog;
    QTimer *m_timer;
    GLRoundedBox *m_box;
    TrackBall m_trackBalls[3];
    QVector<GLTexture *> m_textures;
    GLTexture3D *m_noise;
    GLRenderTargetCube *m_mainCubemap;
    QVector<QGLShaderProgram *> m_programs;
    QGLShader *m_vertexShader;
    QVector<QGLShader *> m_fragmentShaders;
	GLTextureCube *m_environment;    //立方体贴图
    QGLShader *m_environmentShader;  //天空壳shader
    QGLShaderProgram *m_environmentProgram;//天空壳shader program
};

#endif
