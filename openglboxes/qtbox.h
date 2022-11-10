#ifndef QTBOX_H
#define QTBOX_H

#include <QtWidgets>

#include <QtGui/qvector3d.h>
#include "glbuffers.h"

class ItemBase : public QGraphicsItem
{
public:
	enum 
	{ 
		ItemBox = UserType + 1,
		ItemCircle = UserType + 2,
		ItemSquare = UserType + 3,

	};

    ItemBase(int size, int x, int y);
    virtual ~ItemBase();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    virtual ItemBase *createNew(int size, int x, int y) = 0;
	//ÓÒ¼ü²Ëµ¥µ¯¿ò´¦Àí
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    int type() const override;
    bool isInResizeArea(const QPointF &pos);

    static void duplicateSelectedItems(QGraphicsScene *scene);
    static void deleteSelectedItems(QGraphicsScene *scene);
    static void growSelectedItems(QGraphicsScene *scene);
    static void shrinkSelectedItems(QGraphicsScene *scene);

    int m_size;
    QTime m_startTime;
    bool m_isResizing;
};

class QtBox : public ItemBase
{
public:
    QtBox(int size, int x, int y);
    virtual ~QtBox();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    ItemBase *createNew(int size, int x, int y) override;
	int type() const override;
private:
    QVector3D m_vertices[8];
    QVector3D m_texCoords[4];
    QVector3D m_normals[6];
    GLTexture *m_texture;
};

class CircleItem : public ItemBase
{
public:
    CircleItem(int size, int x, int y);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    ItemBase *createNew(int size, int x, int y) override;
	int type() const override;

    QColor m_color;
};

class SquareItem : public ItemBase
{
public:
    SquareItem(int size, int x, int y);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    ItemBase *createNew(int size, int x, int y) override;
	int type() const override;

    QPixmap m_image;
};

#endif
