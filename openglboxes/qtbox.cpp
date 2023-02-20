#include "qtbox.h"
#include "roundedbox.h"
#include "glextensions.h"

const qreal ROTATE_SPEED_X = 30.0 / 1000.0;
const qreal ROTATE_SPEED_Y = 20.0 / 1000.0;
const qreal ROTATE_SPEED_Z = 40.0 / 1000.0;
const int MAX_ITEM_SIZE = 512;
const int MIN_ITEM_SIZE = 16;

//============================================================================//
//                                  ItemBase                                  //
//============================================================================//

ItemBase::ItemBase(int size, int x, int y) : m_size(size), m_isResizing(false)
{
	//设置可移动，可选择，可聚焦
    setFlag(QGraphicsItem::ItemIsMovable, true); 
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
    setPos(x, y);
    m_startTime = QTime::currentTime();
}

ItemBase::~ItemBase()
{
}

QRectF ItemBase::boundingRect() const
{
    return QRectF(-m_size / 2, -m_size / 2, m_size, m_size);
}

void ItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	//被选中的时候给对应的提示
    if (option->state & QStyle::State_Selected) {
        painter->setRenderHint(QPainter::Antialiasing, true);
		//如果左键选中
        if (option->state & QStyle::State_HasFocus)
            painter->setPen(Qt::yellow);
        else  //右键选中
            painter->setPen(Qt::blue);
		//组件四周画一个方框
        painter->drawRect(boundingRect());

		//方框右下角画三条线
        painter->drawLine(m_size / 2 - 9, m_size / 2, m_size / 2, m_size / 2 - 9);
        painter->drawLine(m_size / 2 - 6, m_size / 2, m_size / 2, m_size / 2 - 6);
        painter->drawLine(m_size / 2 - 3, m_size / 2, m_size / 2, m_size / 2 - 3);

        painter->setRenderHint(QPainter::Antialiasing, false);
    }
}

void ItemBase::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	//qDebug() << "Enter ItemBase::contextMenuEvent";
    if (!isSelected() && scene()) {
        scene()->clearSelection();
        setSelected(true);
    }

    QMenu menu;
	//添加菜单项
    QAction *delAction = menu.addAction("Delete");
    QAction *newAction = menu.addAction("New");
    QAction *growAction = menu.addAction("Grow");
    QAction *shrinkAction = menu.addAction("Shrink");
	//qDebug() << "ItemBase::contextMenuEvent";

	//同步等待直到选择了执行的项或者取消选择否则函数不会返回，会一直等待
    QAction *selectedAction = menu.exec(event->screenPos());
	//qDebug() << "ItemBase::contextMenuEvent exec";
    if (selectedAction == delAction)
        deleteSelectedItems(scene());   //删除
    else if (selectedAction == newAction)
        duplicateSelectedItems(scene());   //创建新的
    else if (selectedAction == growAction)
        growSelectedItems(scene());   //放大
    else if (selectedAction == shrinkAction)
        shrinkSelectedItems(scene());  //缩小
	//qDebug() << "Leave ItemBase::contextMenuEvent";
}

//复制选中的item
void ItemBase::duplicateSelectedItems(QGraphicsScene *scene)
{
    if (!scene)
        return;

    QList<QGraphicsItem *> selected;
    selected = scene->selectedItems();

    foreach (QGraphicsItem *item, selected) {
        ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
        if (itemBase)
            scene->addItem(itemBase->createNew(itemBase->m_size, itemBase->pos().x() + itemBase->m_size, itemBase->pos().y()));
    }
}

//删除选中的item
void ItemBase::deleteSelectedItems(QGraphicsScene *scene)
{
    if (!scene)
        return;

    QList<QGraphicsItem *> selected;
    selected = scene->selectedItems();

    foreach (QGraphicsItem *item, selected) {
        ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
		if (itemBase)
		{
			scene->removeItem(itemBase);
			delete itemBase;
		}
    }
}

//放大选中的item
void ItemBase::growSelectedItems(QGraphicsScene *scene)
{
    if (!scene)
        return;

    QList<QGraphicsItem *> selected;
    selected = scene->selectedItems();

    foreach (QGraphicsItem *item, selected) {
        ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
        if (itemBase) {
            itemBase->prepareGeometryChange();
            itemBase->m_size *= 2;
            if (itemBase->m_size > MAX_ITEM_SIZE)
                itemBase->m_size = MAX_ITEM_SIZE;
        }
    }
}

//缩小选中的item
void ItemBase::shrinkSelectedItems(QGraphicsScene *scene)
{
    if (!scene)
        return;

    QList<QGraphicsItem *> selected;
    selected = scene->selectedItems();

    foreach (QGraphicsItem *item, selected) {
        ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
        if (itemBase) {
            itemBase->prepareGeometryChange();
            itemBase->m_size /= 2;
            if (itemBase->m_size < MIN_ITEM_SIZE)
                itemBase->m_size = MIN_ITEM_SIZE;
        }
    }
}

void ItemBase::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isResizing) {
        int dx = int(2.0 * event->pos().x());
        int dy = int(2.0 * event->pos().y());
        prepareGeometryChange();
        m_size = (dx > dy ? dx : dy);
        if (m_size < MIN_ITEM_SIZE)
            m_size = MIN_ITEM_SIZE;
        else if (m_size > MAX_ITEM_SIZE)
            m_size = MAX_ITEM_SIZE;
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void ItemBase::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_isResizing || (isInResizeArea(event->pos()) && isSelected()))
        setCursor(Qt::SizeFDiagCursor);
    else
        setCursor(Qt::ArrowCursor);
    QGraphicsItem::hoverMoveEvent(event);
}

void ItemBase::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    static qreal z = 0.0;
    setZValue(z += 1.0);
    if (event->button() == Qt::LeftButton && isInResizeArea(event->pos())) {
        m_isResizing = true;
    } else {
        QGraphicsItem::mousePressEvent(event);
    }
}

void ItemBase::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isResizing) {
        m_isResizing = false;
    } else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void ItemBase::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	int a = 1;
}

void ItemBase::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        deleteSelectedItems(scene());
        break;
    case Qt::Key_Insert:
        duplicateSelectedItems(scene());
        break;
    case Qt::Key_Plus:
        growSelectedItems(scene());
        break;
    case Qt::Key_Minus:
        shrinkSelectedItems(scene());
        break;
    default:
        QGraphicsItem::keyPressEvent(event);
        break;
    }
}

void ItemBase::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    prepareGeometryChange();
    m_size = int(m_size * qExp(-event->delta() / 600.0));
    if (m_size > MAX_ITEM_SIZE)
        m_size = MAX_ITEM_SIZE;
    else if (m_size < MIN_ITEM_SIZE)
        m_size = MIN_ITEM_SIZE;
}

int ItemBase::type() const
{
    return Type;
}


bool ItemBase::isInResizeArea(const QPointF &pos)
{
    return (-pos.y() < pos.x() - m_size + 9);
}

//============================================================================//
//                                    QtBox                                   //
//============================================================================//

QtBox::QtBox(int size, int x, int y) : ItemBase(size, x, y), m_texture(0)
{
	m_texture = NULL;
	m_vertexShader = NULL;
	m_fragmentShader = NULL;
	m_cubeProgram = NULL;
	m_box = NULL;
	m_arrayBuf = NULL;
	m_indexBuf = NULL;
	for (int i = 0; i < 8; ++i) 
	{
		m_vertices[i].setX(i & 1 ? 0.5f : -0.5f);
		m_vertices[i].setY(i & 2 ? 0.5f : -0.5f);
		m_vertices[i].setZ(i & 4 ? 0.5f : -0.5f);
	}
	
	for (int i = 0; i < 4; ++i) 
	{
		m_texCoords[i].setX(i & 1 ? 1.0f : 0.0f);
		m_texCoords[i].setY(i & 2 ? 1.0f : 0.0f);
	}
	m_normals[0] = QVector3D(-1.0f, 0.0f, 0.0f);
	m_normals[1] = QVector3D(1.0f, 0.0f, 0.0f);
	m_normals[2] = QVector3D(0.0f, -1.0f, 0.0f);
	m_normals[3] = QVector3D(0.0f, 1.0f, 0.0f);
	m_normals[4] = QVector3D(0.0f, 0.0f, -1.0f);
	m_normals[5] = QVector3D(0.0f, 0.0f, 1.0f);

	m_bDebug = true;
	if (m_bDebug)
	{
		//生成有圆角的立方体的顶点数组和索引数组并绑定
		//m_box = new GLRoundedBox(0.25f/*圆角大小*/, 1.0f/*缩放比*/, 20/*每一个圆角的顶点数，顶点数越多越圆润*/);
		m_arrayBuf = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		m_arrayBuf->create();
		m_indexBuf = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		m_indexBuf->create();

		//编译顶点着色器
		m_vertexShader = new QGLShader(QGLShader::Vertex);
		m_vertexShader->compileSourceFile(QLatin1String(":/res/boxes/box_shader.vsh"));
		//编译顶点着色器
		m_fragmentShader = new QGLShader(QGLShader::Fragment);
		m_fragmentShader->compileSourceFile(QLatin1String(":/res/boxes/box_shader.fsh"));

		m_cubeProgram = new QGLShaderProgram();
		m_cubeProgram->addShader(m_vertexShader);
		m_cubeProgram->addShader(m_fragmentShader);

		if (!m_cubeProgram->link())
		{
			qWarning("Failed to compile and link shader program");
			qWarning("Vertex shader log:");
			qWarning() << m_vertexShader->log();
			qWarning() << "Fragment shader log:";
			qWarning() << m_fragmentShader->log();
			qWarning("Shader program log:");
			qWarning() << m_cubeProgram->log();
			exit(1);
		}

		// 绑定顶点数组 VBO 0
		m_arrayBuf->bind();
		//申请空间
		m_arrayBuf->allocate(NULL, 4 * sizeof(VertexData));

		// 绑定索引数组 VBO 1
		m_indexBuf->bind();
		//申请空间
		m_indexBuf->allocate(NULL, 6 * sizeof(GLushort));
	}

	m_texture = new GLTexture2D(":/res/boxes/qt-logo.jpg", 64, 64);
}

QtBox::~QtBox()
{
    if (m_texture)
        delete m_texture;

	if (m_vertexShader)
		delete m_vertexShader;

	if (m_fragmentShader)
		delete m_fragmentShader;

	if (m_cubeProgram)
		delete m_cubeProgram;

	if (m_arrayBuf)
	{
		delete m_arrayBuf;
	}

	if (m_indexBuf)
	{
		delete m_indexBuf;
	}
}

void QtBox::initGl(int dir, int index)
{
	int dataindex = 0;
	int iindex = 0;
	// 绑定顶点数组 VBO 0
	m_arrayBuf->bind();
	VertexData* vertices = (VertexData*)m_arrayBuf->map(QOpenGLBuffer::ReadWrite);
	// 绑定索引数组 VBO 1
	m_indexBuf->bind();
	GLushort* indices = (GLushort*)m_indexBuf->map(QOpenGLBuffer::ReadWrite);
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			vertices[dataindex].normal = m_normals[2 * dir + index];
			vertices[dataindex].texCoord = m_texCoords[(j << 1) | i];
			if (index == 0)
				vertices[dataindex].position = m_vertices[(i << ((dir + 2) % 3)) | (j << ((dir + 1) % 3))];
			else
				vertices[dataindex].position = m_vertices[(1 << dir) | (i << ((dir + 1) % 3)) | (j << ((dir + 2) % 3))];
			indices[iindex++] = i * 2 + j;
			dataindex++;
		}
	}
	//解除显存映射
	m_arrayBuf->unmap();
	m_indexBuf->unmap();
	
	quintptr offset = 0;
	//告诉opengl读取顶点坐标缓存数组的格式:类型float,每个数据大小sizeof(VertexData),
	//顶点坐标在每个数据中的偏移0,数据大小3个float,
	int vertexLocation = m_cubeProgram->attributeLocation("a_position");
	m_cubeProgram->enableAttributeArray(vertexLocation);
	m_cubeProgram->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

	//告诉opengl读取纹理坐标缓存数组的格式:类型float,每个数据大小sizeof(VertexData),
	//纹理坐标在每个数据中的偏移sizeof(QVector3D),数据大小2个float,
	offset += sizeof(QVector3D);
	int texcoordLocation = m_cubeProgram->attributeLocation("a_texcoord");
	m_cubeProgram->enableAttributeArray(texcoordLocation);
	m_cubeProgram->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

	offset += sizeof(QVector2D);
	int normalLocation = m_cubeProgram->attributeLocation("a_normal");
	m_cubeProgram->enableAttributeArray(normalLocation);
	m_cubeProgram->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
}

ItemBase *QtBox::createNew(int size, int x, int y)
{
    return new QtBox(size, x, y);
}

int QtBox::type() const
{
	return ItemBox;
}

void QtBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect().translated(pos());
    float width = float(painter->device()->width());
    float height = float(painter->device()->height());

    float left = 2.0f * float(rect.left()) / width - 1.0f;
    float right = 2.0f * float(rect.right()) / width - 1.0f;
    float top = 1.0f - 2.0f * float(rect.top()) / height;
    float bottom = 1.0f - 2.0f * float(rect.bottom()) / height;

	/*
	在OpenGL中，矩阵是以列优先的方式(column-major order)存储的，而一般的数学书上是以行优
	先的方式(row-major order)存储的。QMatrix4x4也是以列优先(column-major order)
	列优先：						行优先：
	m0     m4     m8     m12		m0     m1     m2     m3
	m1     m5     m9     m13		m4     m5     m6     m7
	m2     m6     m10   m14			m8     m9     m10    m11
	m3     m7     m11   m15			m12    m13	  m14	 m15
	*/
    float moveToRectMatrix[] = {
        0.5f * (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f * (bottom - top), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f * (right + left), 0.5f * (bottom + top), 0.0f, 1.0f
    };

    painter->beginNativePainting();
	glClear(GL_COLOR | GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	if (m_bDebug)
	{
		//左右，前后，上下，三个面
		for (int dir = 0; dir < 3; ++dir) 
		{
			//每个面两个正方形
			for (int index = 0; index < 2; index++)
			{
				initGl(dir, index);
				m_texture->bind();
				glEnable(GL_TEXTURE_2D);
				m_cubeProgram->bind();

				//投影矩阵
				QMatrix4x4 projection;
				projection.perspective(60.0, 1.0, 0.01, 10.0);
				//qgluPerspective(projection, 60.0, 1.0, 0.01, 10.0);
				
				//观察矩阵(眼睛的位置)
				QMatrix4x4 view;
				//计算view矩阵
				view.lookAt(QVector3D(0.0, 0.0, 0.5f), QVector3D(0, 0, -10), QVector3D(0, 1, 0));

				//模型变换矩阵
				QMatrix4x4 model;
				model.setToIdentity();
				model.translate(0.45f * (right + left), 0.45f * (bottom + top), -1.5f);
				model.scale(0.5f * (right - left), 0.5f * (bottom - top), 0);
				int dt = m_startTime.msecsTo(QTime::currentTime());
				if (dt < 500)
					model.scale(dt / 500.0f, dt / 500.0f, dt / 500.0f);
				model.rotate(ROTATE_SPEED_X * m_startTime.msecsTo(QTime::currentTime()), 1.0f, 0.0f, 0.0f);
				model.rotate(ROTATE_SPEED_Y * m_startTime.msecsTo(QTime::currentTime()), 0.0f, 1.0f, 0.0f);
				model.rotate(ROTATE_SPEED_Z * m_startTime.msecsTo(QTime::currentTime()), 0.0f, 0.0f, 1.0f);

				QMatrix4x4 mvpmatrix = projection * model;
				m_cubeProgram->setUniformValue("mvp_matrix", mvpmatrix);
				//渲染管线只有一个纹理，指定着色器texture为0号纹理
				m_cubeProgram->setUniformValue("texture", 0);

				//m_box->draw(m_cubeProgram);
				m_arrayBuf->bind();
				m_indexBuf->bind();
				glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

				m_cubeProgram->release();
				m_texture->unbind();
			}
		}
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf(moveToRectMatrix);
		QMatrix4x4 modelView;
		qgluPerspective(modelView,60.0, 1.0, 0.01, 10.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_NORMALIZE);

		m_texture->bind();
		glEnable(GL_TEXTURE_2D);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		float lightColour[] = {1.0f, 1.0f, 1.0f, 1.0f};
		float lightDir[] = {0.0f, 0.0f, 1.0f, 0.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColour);
		glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
		glEnable(GL_LIGHT0);

		glTranslatef(0.0f, 0.0f, -1.5f);
		glRotatef(ROTATE_SPEED_X * m_startTime.msecsTo(QTime::currentTime()), 1.0f, 0.0f, 0.0f);
		glRotatef(ROTATE_SPEED_Y * m_startTime.msecsTo(QTime::currentTime()), 0.0f, 1.0f, 0.0f);
		glRotatef(ROTATE_SPEED_Z * m_startTime.msecsTo(QTime::currentTime()), 0.0f, 0.0f, 1.0f);
		{
			int dt = m_startTime.msecsTo(QTime::currentTime());
			if (dt < 500)
				glScalef(dt / 500.0f, dt / 500.0f, dt / 500.0f);
		}

		int dataindex = 0;
		VertexData vertices[24] = { QVector3D(0,0,0)};
		for (int dir = 0; dir < 3; ++dir) 
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0);

			glBegin(GL_TRIANGLE_STRIP);
			glNormal3fv(reinterpret_cast<float *>(&m_normals[2 * dir + 0]));
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
					vertices[dataindex].normal = m_normals[2 * dir + 0];
					vertices[dataindex].texCoord = m_texCoords[(j << 1) | i];
					vertices[dataindex].position = m_vertices[(i << ((dir + 2) % 3)) | (j << ((dir + 1) % 3))];
					dataindex++;

					glTexCoord2fv(reinterpret_cast<float *>(&m_texCoords[(j << 1) | i]));
					glVertex3fv(reinterpret_cast<float *>(&m_vertices[(i << ((dir + 2) % 3)) | (j << ((dir + 1) % 3))]));
				}
			}
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
			glNormal3fv(reinterpret_cast<float *>(&m_normals[2 * dir + 1]));
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
					glTexCoord2fv(reinterpret_cast<float *>(&m_texCoords[(j << 1) | i]));
					glVertex3fv(reinterpret_cast<float *>(&m_vertices[(1 << dir) | (i << ((dir + 1) % 3)) | (j << ((dir + 2) % 3))]));
					vertices[dataindex].normal = m_normals[2 * dir + 1];
					vertices[dataindex].texCoord = m_texCoords[(j << 1) | i];
					vertices[dataindex].position = m_vertices[(1 << dir) | (i << ((dir + 1) % 3)) | (j << ((dir + 2) % 3))];
					dataindex++;
				}
			}
			glEnd();
		}
		m_texture->unbind();

		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHT0);
		glDisable(GL_NORMALIZE);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

    painter->endNativePainting();
	ItemBase::paint(painter, option, widget);
	return;
}

//============================================================================//
//                                 CircleItem                                 //
//============================================================================//

CircleItem::CircleItem(int size, int x, int y) : ItemBase(size, x, y)
{
     m_color = QColor::fromHsv(rand() % 360, 255, 255);
}

void CircleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int dt = m_startTime.msecsTo(QTime::currentTime());
	qreal speedFix = 1 / 1000.0F;
	//圆1的半径
    qreal r0 = 0.5 * m_size * (1.0 - qExp(-speedFix * ((dt + 3000) % 4000)));
	//圆2的半径
    qreal r1 = 0.5 * m_size * (1.0 - qExp(-speedFix * ((dt + 0) % 4000)));
	//圆3的半径
	qreal r2 = 0.5 * m_size * (1.0 - qExp(-speedFix * ((dt + 1000) % 4000)));
	//圆4的半径
	qreal r3 = 0.5 * m_size * (1.0 - qExp(-speedFix * ((dt + 2000) % 4000)));

    if (r0 > r1)
        r0 = 0.0;
    if (r2 > r3)
        r2 = 0.0;

	QPainterPath path;
	//移动到(r1, 0.0)
	path.lineTo(r1, 0.0);
	//以(r1, 0.0)为起点逆时针画一个半径为r1得圆
	path.arcTo(-r1, -r1, 2 * r1, 2 * r1, 0.0, 360.0);
 	path.lineTo(r0, 0.0);
 	path.arcTo(-r0, -r0, 2 * r0, 2 * r0, 0.0, -360.0);
    path.closeSubpath();
    path.lineTo(r3, 0.0);
    path.arcTo(-r3, -r3, 2 * r3, 2 * r3, 0.0, 360.0);
    path.lineTo(r0, 0.0);
    path.arcTo(-r2, -r2, 2 * r2, 2 * r2, 0.0, -360.0);
    path.closeSubpath();

	//抗锯齿
    painter->setRenderHint(QPainter::Antialiasing, true);
	//设置画刷样式
    painter->setBrush(QBrush(m_color, Qt::Dense5Pattern));
	//去掉边缘的线
    painter->setPen(Qt::NoPen);
    painter->drawPath(path);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::SolidLine);
    painter->setRenderHint(QPainter::Antialiasing, false);

    ItemBase::paint(painter, option, widget);
}

ItemBase *CircleItem::createNew(int size, int x, int y)
{
    return new CircleItem(size, x, y);
}

int CircleItem::type() const
{
	return ItemCircle;
}
//============================================================================//
//                                 SquareItem                                 //
//============================================================================//

SquareItem::SquareItem(int size, int x, int y) : ItemBase(size, x, y)
{
	//背景图片
    m_image = QPixmap(":/res/boxes/square.jpg");
}

void SquareItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int dt = m_startTime.msecsTo(QTime::currentTime());
	//获取当前item的世界变换矩阵
    QTransform oldTransform = painter->worldTransform();
    int dtMod = dt % 2000;
	//旋转参数
    qreal amp = 0.002 * (dtMod < 1000 ? dtMod : 2000 - dtMod) - 1.0;
	//缩放参数
    qreal scale = 0.6 + 0.2 * amp * amp;
	//设置世界转换矩阵=oldTransform * QTransform().rotate(15.0 * amp).scale(scale, scale)，旋转+缩放
    painter->setWorldTransform(QTransform().rotate(15.0 * amp).scale(scale, scale), true);
	//绘制图片
    painter->drawPixmap(-m_size / 2, -m_size / 2, m_size, m_size, m_image);
	//还原老的世界转换矩阵(四周的线框不做一上的变换)
    painter->setWorldTransform(oldTransform, false);
    ItemBase::paint(painter, option, widget);
}

ItemBase *SquareItem::createNew(int size, int x, int y)
{
    return new SquareItem(size, x, y);
}

int SquareItem::type() const
{
	return ItemSquare;
}