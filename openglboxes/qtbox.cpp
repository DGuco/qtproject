#include "qtbox.h"
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
	bool ret = arrayBuf.create();
	ret = indexBuf.create();

	//初始化顶点信息
    for (int i = 0; i < 8; ++i) 
	{
        m_vertices[i].setX(i & 1 ? 0.5f : -0.5f);
        m_vertices[i].setY(i & 2 ? 0.5f : -0.5f);
        m_vertices[i].setZ(i & 4 ? 0.5f : -0.5f);
    }
	//初始化纹理坐标
    for (int i = 0; i < 4; ++i) 
	{
        m_texCoords[i].setX(i & 1 ? 1.0f : 0.0f);
        m_texCoords[i].setY(i & 2 ? 1.0f : 0.0f);
    }
	//初始化法线向量
    m_normals[0] = QVector3D(-1.0f, 0.0f, 0.0f);
    m_normals[1] = QVector3D(1.0f, 0.0f, 0.0f);
    m_normals[2] = QVector3D(0.0f, -1.0f, 0.0f);
    m_normals[3] = QVector3D(0.0f, 1.0f, 0.0f);
    m_normals[4] = QVector3D(0.0f, 0.0f, -1.0f);
    m_normals[5] = QVector3D(0.0f, 0.0f, 1.0f);

	int dataindex = 0;
	for (int dir = 0; dir < 3; ++dir)
	{
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				m_renderData[dataindex].normal = m_normals[2 * dir + 0];
				m_renderData[dataindex].texCoord = m_texCoords[(j << 1) | i];
				m_renderData[dataindex].position = m_vertices[(i << ((dir + 2) % 3)) | (j << ((dir + 1) % 3))];
				dataindex++;
			}
		}

		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				m_renderData[dataindex].normal = m_normals[2 * dir + 0];
				m_renderData[dataindex].texCoord = m_texCoords[(j << 1) | i];
				m_renderData[dataindex].position = m_vertices[(i << ((dir + 2) % 3)) | (j << ((dir + 1) % 3))];
				dataindex++;
			}
		}
	}

	VertexData vertices[24] =
	{
		////////////////////////////////CUBE1//////////////////////////////////////
		// 点数4的面
		{ QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f),	QVector3D(0.0f, 0.0f,1.0f) },
		{ QVector3D(1.0f, -1.0f,  1.0f),  QVector2D((1 / 3.0f), 0.0f),QVector3D(0.0f, 0.0f,1.0f) },
		{ QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f,(1 / 2.0f)),QVector3D(0.0f, 0.0f,1.0f) },
		{ QVector3D(1.0f,  1.0f,  1.0f),  QVector2D((1 / 3.0f),(1 / 2.0f)),QVector3D(0.0f, 0.0f,1.0f) },
		// 点数1的面
		{ QVector3D(1.0f, -1.0f,  1.0f),  QVector2D(0.0f,(1 / 2.0f)), QVector3D(1.0f, 0.0f,0.0f) },
		{ QVector3D(1.0f, -1.0f, -1.0f),  QVector2D((1 / 3.0f),(1 / 2.0f)), QVector3D(1.0f, 0.0f,0.0f) },
		{ QVector3D(1.0f,  1.0f,  1.0f),  QVector2D(0.0f, 1.0f), QVector3D(1.0f, 0.0f,0.0f) },
		{ QVector3D(1.0f,  1.0f, -1.0f),  QVector2D((1 / 3.0f), 1.0f), QVector3D(1.0f, 0.0f,0.0f) },
		//点数3的面
		{ QVector3D(1.0f, -1.0f, -1.0f),  QVector2D((2 / 3.0f),(1 / 2.0f)), QVector3D(0.0f, 0.0f,-1.0f) },
		{ QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f,(1 / 2.0f)), QVector3D(0.0f, 0.0f,-1.0f) },
		{ QVector3D(1.0f,  1.0f, -1.0f),  QVector2D((2 / 3.0f), 1.0f), QVector3D(0.0f, 0.0f,-1.0f) },
		{ QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f,-1.0f) },
		//点数6的面
		{ QVector3D(-1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f), 0.0f), QVector3D(-1.0f, 0.0f,0.0f) },
		{ QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f), QVector3D(-1.0f, 0.0f,0.0f) },
		{ QVector3D(-1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f),(1 / 2.0f)), QVector3D(-1.0f, 0.0f,0.0f) },
		{ QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f,(1 / 2.0f)), QVector3D(-1.0f, 0.0f,0.0f) },
		//点数5的面
		{ QVector3D(-1.0f, -1.0f, -1.0f), QVector2D((1 / 3.0f), 0.0f), QVector3D(0.0f, -1.0f,0.0f) },
		{ QVector3D(1.0f, -1.0f, -1.0f), QVector2D((2 / 3.0f), 0.0f), QVector3D(0.0f, -1.0f,0.0f) },
		{ QVector3D(-1.0f, -1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f)), QVector3D(0.0f, -1.0f,0.0f) },
		{ QVector3D(1.0f, -1.0f,  1.0f), QVector2D((2 / 3.0f),(1 / 2.0f)), QVector3D(0.0f, -1.0f,0.0f) },
		//点数2的面
		{ QVector3D(-1.0f,  1.0f,  1.0f), QVector2D((1 / 3.0f),(1 / 2.0f)), QVector3D(0.0f, 1.0f,0.0f) },
		{ QVector3D(1.0f,  1.0f,  1.0f), QVector2D((2 / 3.0f),(1 / 2.0f)), QVector3D(0.0f, 1.0f,0.0f) },
		{ QVector3D(-1.0f,  1.0f, -1.0f), QVector2D((1 / 3.0f), 1.0f), QVector3D(0.0f, 1.0f,0.0f) },
		{ QVector3D(1.0f,  1.0f, -1.0f), QVector2D((2 / 3.0f), 1.0f), QVector3D(0.0f, 1.0f,0.0f) },
	};

	// 绑定顶点数组 VBO 0
	arrayBuf.bind();
	arrayBuf.allocate(vertices, 24 * sizeof(VertexData));

	GLushort indices[36] =
	{
		/**
		* 每两个三角形渲染一个面
		* 注意节点顺序，因为开启了遮挡剔除(glEnable(GL_CULL_FACE)),opengl是根据顶点顺序决定三角形法线方向的,顺时针顺序算出来
		* 三角形是朝里的就不画了,所以 0 3 1 会导致该三角形不显示,后面的三角形同样的道理注意顶点顺序
		*/
		//Face 0
		0,  1 , 3, //triangle1
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
	// 绑定索引数组 VBO 1
 	indexBuf.bind();
 	indexBuf.allocate(indices, 36 * sizeof(GLushort));

	m_texture = new GLTexture2D(":/res/boxes/qt-logo.jpg", 64, 64);
	initGl();
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

	arrayBuf.destroy();
    indexBuf.destroy();
}

void QtBox::initGl()
{
	//编译顶点着色器
	m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	m_vertexShader->compileSourceFile(QLatin1String(":/res/boxes/box_shader.vsh"));
	//编译顶点着色器
	m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
	m_fragmentShader->compileSourceFile(QLatin1String(":/res/boxes/box_shader.fsh"));

	m_cubeProgram = new QOpenGLShaderProgram();
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
    float moveToRectMatrix[] = {
        0.5f * (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f * (bottom - top), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f * (right + left), 0.5f * (bottom + top), 0.0f, 1.0f
    };

    painter->beginNativePainting();
	glClear(GL_COLOR | GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	bool debug = 0;
	if (debug)
	{
		m_texture->bind();
		glEnable(GL_TEXTURE_2D);

		m_cubeProgram->bind();

		QMatrix4x4 projection(moveToRectMatrix);
		//qgluPerspective(projection,60.0, 1.0, 0.01, 10.0);
		QMatrix4x4 modelview;
		modelview.setToIdentity();
		modelview.translate(0.0f, 0.0f, -1.5f);
		modelview.rotate(ROTATE_SPEED_X * m_startTime.msecsTo(QTime::currentTime()), 1.0f, 0.0f, 0.0f);
		modelview.rotate(ROTATE_SPEED_Y * m_startTime.msecsTo(QTime::currentTime()), 0.0f, 1.0f, 0.0f);
		modelview.rotate(ROTATE_SPEED_Z * m_startTime.msecsTo(QTime::currentTime()), 0.0f, 0.0f, 1.0f);
		int dt = m_startTime.msecsTo(QTime::currentTime());
		if (dt < 500)
			modelview.scale(dt / 500.0f, dt / 500.0f, dt / 500.0f);

		QMatrix4x4 mvpmatrix = projection * modelview;
		m_cubeProgram->setUniformValue("mvp_matrix", mvpmatrix);
		//渲染管线只有一个纹理，指定着色器texture为0号纹理
		m_cubeProgram->setUniformValue("texture", m_texture->textureId());

		// 使用索引缓冲数组渲染三角形
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		m_cubeProgram->release();
		m_texture->unbind();
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

		//glEnable(GL_DEPTH_TEST);
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
		for (int dir = 0; dir < 3; ++dir) 
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0);

			glBegin(GL_TRIANGLE_STRIP);
			glNormal3fv(reinterpret_cast<float *>(&m_normals[2 * dir + 0]));
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 2; ++j) {
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