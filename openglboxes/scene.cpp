#include <QDebug>
#include "scene.h"
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qvector3d.h>
#include <cmath>
#include <typeinfo>

#include "3rdparty/fbm.h"

void checkGLErrors(const QString& prefix)
{
    switch (glGetError()) {
    case GL_NO_ERROR:
        //qDebug() << prefix << tr("No error.");
        break;
    case GL_INVALID_ENUM:
        qDebug() << prefix << QObject::tr("Invalid enum.");
        break;
    case GL_INVALID_VALUE:
        qDebug() << prefix << QObject::tr("Invalid value.");
        break;
    case GL_INVALID_OPERATION:
        qDebug() << prefix << QObject::tr("Invalid operation.");
        break;
    case GL_STACK_OVERFLOW:
        qDebug() << prefix << QObject::tr("Stack overflow.");
        break;
    case GL_STACK_UNDERFLOW:
        qDebug() << prefix << QObject::tr("Stack underflow.");
        break;
    case GL_OUT_OF_MEMORY:
        qDebug() << prefix << QObject::tr("Out of memory.");
        break;
    default:
        qDebug() << prefix << QObject::tr("Unknown error.");
        break;
    }
}

//============================================================================//
//                                  ColorEdit                                 //
//============================================================================//

ColorEdit::ColorEdit(QRgb initialColor, int id)
    : m_color(initialColor), m_id(id)
{
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_lineEdit = new QLineEdit(QString::number(m_color, 16));
    layout->addWidget(m_lineEdit);

    m_button = new QFrame;
    QPalette palette = m_button->palette();
    palette.setColor(QPalette::Window, QColor(m_color));
    m_button->setPalette(palette);
    m_button->setAutoFillBackground(true);
    m_button->setMinimumSize(32, 0);
    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_button->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    layout->addWidget(m_button);

    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editDone()));
}

void ColorEdit::editDone()
{
    bool ok;
    QRgb newColor = m_lineEdit->text().toUInt(&ok, 16);
    if (ok)
        setColor(newColor);
}

void ColorEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QColor color(m_color);
        QColorDialog dialog(color, 0);
        dialog.setOption(QColorDialog::ShowAlphaChannel, true);
// The ifdef block is a workaround for the beta, TODO: remove when bug 238525 is fixed
#if 0 // Used to be included in Qt4 for Q_WS_MAC
        dialog.setOption(QColorDialog::DontUseNativeDialog, true);
#endif
        dialog.move(280, 120);
        if (dialog.exec() == QDialog::Rejected)
            return;
        QRgb newColor = dialog.selectedColor().rgba();
        if (newColor == m_color)
            return;
        setColor(newColor);
    }
}

void ColorEdit::setColor(QRgb color)
{
    m_color = color;
    m_lineEdit->setText(QString::number(m_color, 16)); // "Clean up" text
    QPalette palette = m_button->palette();
    palette.setColor(QPalette::Window, QColor(m_color));
    m_button->setPalette(palette);
    emit colorChanged(m_color, m_id);
}

//============================================================================//
//                                  FloatEdit                                 //
//============================================================================//

FloatEdit::FloatEdit(float initialValue, int id)
    : m_value(initialValue), m_id(id)
{
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    m_lineEdit = new QLineEdit(QString::number(m_value));
    layout->addWidget(m_lineEdit);

    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editDone()));
}

void FloatEdit::editDone()
{
    bool ok;
    float newValue = m_lineEdit->text().toFloat(&ok);
    if (ok) {
        m_value = newValue;
        m_lineEdit->setText(QString::number(m_value)); // "Clean up" text
        emit valueChanged(m_value, m_id);
    }
}

//============================================================================//
//                           TwoSidedGraphicsWidget                           //
//============================================================================//

TwoSidedGraphicsWidget::TwoSidedGraphicsWidget(QGraphicsScene *scene)
    : QObject(scene)
    , m_current(0)
    , m_angle(0)
    , m_delta(0)
	, m_scale(0)
{
    for (int i = 0; i < 2; ++i)
        m_proxyWidgets[i] = 0;
}

void TwoSidedGraphicsWidget::setWidget(int index, QWidget *widget)
{
    if (index < 0 || index >= 2)
    {
        qWarning("TwoSidedGraphicsWidget::setWidget: Index out of bounds, index == %d", index);
        return;
    }

    GraphicsWidget *proxy = new GraphicsWidget;
	//保证ui在场景的最顶层不会被其他item遮住
	proxy->setZValue(100);
	proxy->setWidget(widget);

    if (m_proxyWidgets[index])
        delete m_proxyWidgets[index];
    m_proxyWidgets[index] = proxy;

    proxy->setCacheMode(QGraphicsItem::ItemCoordinateCache);

	//只显示当前的GraphicsWidget
    if (index != m_current)
        proxy->setVisible(false);

	//把widget添加到场景中
    qobject_cast<QGraphicsScene *>(parent())->addItem(proxy);
}

QWidget *TwoSidedGraphicsWidget::widget(int index)
{
    if (index < 0 || index >= 2)
    {
        qWarning("TwoSidedGraphicsWidget::widget: Index out of bounds, index == %d", index);
        return 0;
    }
    return m_proxyWidgets[index]->widget();
}

void TwoSidedGraphicsWidget::flipwidget()
{
    m_delta = (m_current == 0 ? 5 : -5);
	m_scale = 100;
    animateFlip();
}

void TwoSidedGraphicsWidget::hidewidget()
{
	m_scale = 100;
	m_delta = 720 / 2 / (100 - 6);
	animateHide();
}

void TwoSidedGraphicsWidget::showidget()
{
	if (!m_proxyWidgets[m_current]->isVisible())
	{
		m_delta = (m_current == 0 ? 5 : -5);
		m_scale = 6;
		Scene* pScene = qobject_cast<Scene*>(parent());
		if (pScene)
		{
			int width = pScene->width();
			int high = pScene->height();
			QRectF r = m_proxyWidgets[m_current]->boundingRect();
			//移动至屏幕中央
			m_proxyWidgets[m_current]->setPos(width / 2 - r.width() / 2 , high /2 - r.height() / 2);
			m_proxyWidgets[m_current]->setVisible(true);
			animateShow();
		}
	}
}

void TwoSidedGraphicsWidget::animateFlip()
{
    m_angle += m_delta;
    if (m_angle == 90) {
        int old = m_current;
        m_current ^= 1;
        m_proxyWidgets[old]->setVisible(false);
        m_proxyWidgets[m_current]->setVisible(true);
        m_proxyWidgets[m_current]->setGeometry(m_proxyWidgets[old]->geometry());
    }

    QRectF r = m_proxyWidgets[m_current]->boundingRect();
	QTransform anTransform;
	//把坐标系移动到widget的中心
	anTransform.translate(r.width() / 2, r.height() / 2);
	//围绕y轴旋转
	anTransform.rotate(m_angle - 180 * m_current, Qt::YAxis);
	//还原坐标系
	anTransform.translate(-r.width() / 2, -r.height() / 2);
    m_proxyWidgets[m_current]->setTransform(anTransform);
	if ((m_current == 0 && m_angle > 0) || (m_current == 1 && m_angle < 180))
		QTimer::singleShot(25, this, SLOT(animateFlip()));
}

void TwoSidedGraphicsWidget::animateHide()
{
	m_scale -= 2;
	m_scale < 6 ? 6 : m_scale;
	m_angle += m_delta;
	//缩放到15时隐藏ui，显示新的item
	if(m_scale == 6)
	{
		m_proxyWidgets[m_current]->setVisible(false);
		Scene* pScene = qobject_cast<Scene*>(parent());
		if (pScene)
		{
			QRectF r = m_proxyWidgets[m_current]->boundingRect();
			QWidget* pWidget = m_proxyWidgets[m_current]->widget();
			//添加一个item
			pScene->addItem(new CircleItem(64, pWidget->pos().x() +  r.width() / 2, pWidget->pos().y() + r.height() / 2));
		}
	}
	QRectF r = m_proxyWidgets[m_current]->boundingRect();
	QTransform anTransform;
	anTransform.translate(r.width() / 2, r.height() / 2);
	anTransform.rotate(m_angle, Qt::ZAxis);
	anTransform.scale(m_scale / 100.0f, m_scale / 100.0f);
	anTransform.translate(-r.width() / 2, -r.height() / 2);

	m_proxyWidgets[m_current]->setTransform(anTransform);
	if (m_scale > 6)
	{
		QTimer::singleShot(25, this, SLOT(animateHide()));
	}
}

void TwoSidedGraphicsWidget::animateShow()
{
	m_scale += 2;
	m_angle += m_delta;
	if (m_scale == 100)
	{
		QTransform anTransform;
		m_proxyWidgets[m_current]->setTransform(anTransform);
		return;
	}
	QRectF r = m_proxyWidgets[m_current]->boundingRect();
	QTransform anTransform;
	anTransform.translate(r.width() / 2, r.height() / 2);
	anTransform.rotate(m_angle, Qt::ZAxis);
	anTransform.scale(m_scale / 100.0f, m_scale / 100.0f);
	anTransform.translate(-r.width() / 2, -r.height() / 2);

	m_proxyWidgets[m_current]->setTransform(anTransform);
	if (m_scale < 100)
	{
		QTimer::singleShot(25, this, SLOT(animateShow()));
	}
}

QVariant GraphicsWidget::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QRectF rect = boundingRect();
        QPointF pos = value.toPointF();
        QRectF sceneRect = scene()->sceneRect();
        if (pos.x() + rect.left() < sceneRect.left())
            pos.setX(sceneRect.left() - rect.left());
        else if (pos.x() + rect.right() >= sceneRect.right())
            pos.setX(sceneRect.right() - rect.right());
        if (pos.y() + rect.top() < sceneRect.top())
            pos.setY(sceneRect.top() - rect.top());
        else if (pos.y() + rect.bottom() >= sceneRect.bottom())
            pos.setY(sceneRect.bottom() - rect.bottom());
        return pos;
    }
    return QGraphicsProxyWidget::itemChange(change, value);
}

void GraphicsWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    setCacheMode(QGraphicsItem::NoCache);
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
    QGraphicsProxyWidget::resizeEvent(event);
}

void GraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, false);
    QGraphicsProxyWidget::paint(painter, option, widget);
    //painter->setRenderHint(QPainter::Antialiasing, true);
}

void GraphicsWidget::closeEvent(QCloseEvent* event)
{
	QWidget* pWidget = widget();
	if (pWidget)
	{
		if (pWidget->inherits("RenderOptionsDialog"))
		{
			qobject_cast<RenderOptionsDialog *>(pWidget)->emitHideWidget();
			event->ignore();
		}
		else if ((pWidget->inherits("ItemDialog")))
		{
			qobject_cast<ItemDialog *>(pWidget)->emitHideWidget();
			event->ignore();
		}
	}
}
//============================================================================//
//                             RenderOptionsDialog                            //
//============================================================================//

RenderOptionsDialog::RenderOptionsDialog()
    : QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setWindowOpacity(0.75);
    setWindowTitle(tr("Options (double click to flip)"));
    QGridLayout *layout = new QGridLayout;
    setLayout(layout);
    layout->setColumnStretch(1, 1);

    int row = 0;

    QCheckBox *check = new QCheckBox(tr("Dynamic cube map"));
    check->setCheckState(Qt::Unchecked);
    // Dynamic cube maps are only enabled when multi-texturing and render to texture are available.
    check->setEnabled(glActiveTexture && glGenFramebuffersEXT);
    connect(check, SIGNAL(stateChanged(int)), this, SIGNAL(dynamicCubemapToggled(int)));
    layout->addWidget(check, 0, 0, 1, 2);
    ++row;

    QPalette palette;

    QSet<QByteArray> uniforms;
    QList<QString> filter = QStringList("*.par");
    QList<QFileInfo> files = QDir(":/res/boxes/").entryInfoList(filter, QDir::Files | QDir::Readable);

    foreach (QFileInfo fileInfo, files) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly)) {
            while (!file.atEnd()) {
                QList<QByteArray> tokens = file.readLine().simplified().split(' ');
                QList<QByteArray>::const_iterator it = tokens.begin();
                if (it == tokens.end())
                    continue;
                QByteArray type = *it;
                if (++it == tokens.end())
                    continue;
                QByteArray name = *it;
                bool singleElement = (tokens.size() == 3); // type, name and one value
                char counter[10] = "000000000";
                int counterPos = 8; // position of last digit
                while (++it != tokens.end()) {
                    m_parameterNames << name;
                    if (!singleElement) {
                        m_parameterNames.back() += '[';
                        m_parameterNames.back() += counter + counterPos;
                        m_parameterNames.back() += ']';
                        int j = 8; // position of last digit
                        ++counter[j];
                        while (j > 0 && counter[j] > '9') {
                            counter[j] = '0';
                            ++counter[--j];
                        }
                        if (j < counterPos)
                            counterPos = j;
                    }

                    if (type == "color") {
                        layout->addWidget(new QLabel(m_parameterNames.back()));
                        bool ok;
                        ColorEdit *colorEdit = new ColorEdit(it->toUInt(&ok, 16), m_parameterNames.size() - 1);
                        m_parameterEdits << colorEdit;
                        layout->addWidget(colorEdit);
                        connect(colorEdit, SIGNAL(colorChanged(QRgb,int)), this, SLOT(setColorParameter(QRgb,int)));
                        ++row;
                    } else if (type == "float") {
                        layout->addWidget(new QLabel(m_parameterNames.back()));
                        bool ok;
                        FloatEdit *floatEdit = new FloatEdit(it->toFloat(&ok), m_parameterNames.size() - 1);
                        m_parameterEdits << floatEdit;
                        layout->addWidget(floatEdit);
                        connect(floatEdit, SIGNAL(valueChanged(float,int)), this, SLOT(setFloatParameter(float,int)));
                        ++row;
                    }
                }
            }
            file.close();
        }
    }

    layout->addWidget(new QLabel(tr("Texture:")));
    m_textureCombo = new QComboBox;
    connect(m_textureCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(textureChanged(int)));
    layout->addWidget(m_textureCombo);
    ++row;

    layout->addWidget(new QLabel(tr("Shader:")));
    m_shaderCombo = new QComboBox;
    connect(m_shaderCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(shaderChanged(int)));
    layout->addWidget(m_shaderCombo);
    ++row;

    layout->setRowStretch(row, 1);
}

int RenderOptionsDialog::addTexture(const QString &name)
{
    m_textureCombo->addItem(name);
    return m_textureCombo->count() - 1;
}

int RenderOptionsDialog::addShader(const QString &name)
{
    m_shaderCombo->addItem(name);
    return m_shaderCombo->count() - 1;
}

void RenderOptionsDialog::emitParameterChanged()
{
    foreach (ParameterEdit *edit, m_parameterEdits)
        edit->emitChange();
}

void RenderOptionsDialog::emitHideWidget()
{
	emit widgetHide();
}

void RenderOptionsDialog::setColorParameter(QRgb color, int id)
{
    emit colorParameterChanged(m_parameterNames[id], color);
}

void RenderOptionsDialog::setFloatParameter(float value, int id)
{
    emit floatParameterChanged(m_parameterNames[id], value);
}

void RenderOptionsDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit doubleClicked();
}

//============================================================================//
//                                 ItemDialog                                 //
//============================================================================//

ItemDialog::ItemDialog()
    : QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setWindowTitle(tr("Items (double click to flip)"));
    setWindowOpacity(0.75);
    resize(160, 100);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    QPushButton *button;

    button = new QPushButton(tr("Add Qt box"));
    layout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(triggerNewQtBox()));

    button = new QPushButton(tr("Add circle"));
    layout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(triggerNewCircleItem()));

    button = new QPushButton(tr("Add square"));
    layout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(triggerNewSquareItem()));

    layout->addStretch(1);
}

void ItemDialog::emitHideWidget()
{
	emit widgetHide();
}
void ItemDialog::triggerNewQtBox()
{
    emit newItemTriggered(QtBoxItem);
}

void ItemDialog::triggerNewCircleItem()
{
    emit newItemTriggered(CircleItem);
}

void ItemDialog::triggerNewSquareItem()
{
    emit newItemTriggered(SquareItem);
}

void ItemDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit doubleClicked();
}

//============================================================================//
//                                    Scene                                   //
//============================================================================//

Scene::Scene(int width, int height, int maxTextureSize)
    : m_distExp(600)
    , m_frame(0)
    , m_maxTextureSize(maxTextureSize)
    , m_currentShader(0)
    , m_currentTexture(0)
    , m_dynamicCubemap(false)
    , m_updateAllCubemaps(true)
    , m_box(0)
    , m_vertexShader(0)
	, m_environment(0)
    , m_environmentShader(0)
    , m_environmentProgram(0)
{
    setSceneRect(0, 0, width, height);

	m_trackBalls[0] = TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);
	m_trackBalls[1] = TrackBall(0.005f, QVector3D(0, 0, 1), TrackBall::Sphere);
	m_trackBalls[2] = TrackBall(0.0f, QVector3D(0, 1, 0), TrackBall::Plane);

	//渲染设置对话框
    m_renderOptions = new RenderOptionsDialog;
    m_renderOptions->move(20, 120);
    m_renderOptions->resize(m_renderOptions->sizeHint());

    connect(m_renderOptions, SIGNAL(dynamicCubemapToggled(int)), this, SLOT(toggleDynamicCubemap(int)));
    connect(m_renderOptions, SIGNAL(colorParameterChanged(QString,QRgb)), this, SLOT(setColorParameter(QString,QRgb)));
    connect(m_renderOptions, SIGNAL(floatParameterChanged(QString,float)), this, SLOT(setFloatParameter(QString,float)));
    connect(m_renderOptions, SIGNAL(textureChanged(int)), this, SLOT(setTexture(int)));
    connect(m_renderOptions, SIGNAL(shaderChanged(int)), this, SLOT(setShader(int)));

    m_itemDialog = new ItemDialog;
    connect(m_itemDialog, SIGNAL(newItemTriggered(ItemDialog::ItemType)), this, SLOT(newItem(ItemDialog::ItemType)));

	m_twoSidedGraphicsWidget = new TwoSidedGraphicsWidget(this);
	m_twoSidedGraphicsWidget->setWidget(0, m_renderOptions);
	m_twoSidedGraphicsWidget->setWidget(1, m_itemDialog);

    connect(m_renderOptions, SIGNAL(doubleClicked()), m_twoSidedGraphicsWidget, SLOT(flipwidget()));
	connect(m_itemDialog, SIGNAL(doubleClicked()), m_twoSidedGraphicsWidget, SLOT(flipwidget()));
	connect(m_renderOptions, SIGNAL(widgetHide()), m_twoSidedGraphicsWidget, SLOT(hidewidget()));
	connect(m_itemDialog, SIGNAL(widgetHide()), m_twoSidedGraphicsWidget, SLOT(hidewidget()));

    addItem(new QtBox(64, width - 64, height - 64));
    addItem(new QtBox(64, width - 64, 64));
    addItem(new QtBox(64, 64, height - 64));
    addItem(new QtBox(64, 64, 64));

    initGL();

    m_timer = new QTimer(this);
    m_timer->setInterval(20);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start();
}

Scene::~Scene()
{
    if (m_box)
        delete m_box;
    foreach (GLTexture *texture, m_textures)
        if (texture) delete texture;
    if (m_mainCubemap)
        delete m_mainCubemap;
    foreach (QGLShaderProgram *program, m_programs)
        if (program) delete program;
    if (m_vertexShader)
        delete m_vertexShader;
    foreach (QGLShader *shader, m_fragmentShaders)
        if (shader) delete shader;
    foreach (GLRenderTargetCube *rt, m_cubemaps)
        if (rt) delete rt;
    if (m_environmentShader)
        delete m_environmentShader;
    if (m_environmentProgram)
        delete m_environmentProgram;
}

void Scene::initGL()
{
	//生成有圆角的立方体的顶点数组和索引数组并绑定
    m_box = new GLRoundedBox(0.25f/*圆角大小*/, 1.0f/*缩放比*/, 20/*每一个圆角的定点数，顶点数越多越圆润*/);
	//编译顶点着色器
    m_vertexShader = new QGLShader(QGLShader::Vertex);
    m_vertexShader->compileSourceFile(QLatin1String(":/res/boxes/basic.vsh"));

	//天空盒渲染程序准备
	{
		QStringList list;
		list << ":/res/boxes/cubemap_posx.jpg" << ":/res/boxes/cubemap_negx.jpg" << ":/res/boxes/cubemap_posy.jpg"
			<< ":/res/boxes/cubemap_negy.jpg" << ":/res/boxes/cubemap_posz.jpg" << ":/res/boxes/cubemap_negz.jpg";
		//初始化立方体贴图
		m_environment = new GLTextureCube(list, qMin(1024, m_maxTextureSize));
		//编译天空盒着色器
		m_environmentShader = new QGLShader(QGLShader::Fragment);
		m_environmentShader->compileSourceFile(QLatin1String(":/res/boxes/skybox.fsh"));
		//创建天空盒可编程渲染管线程序
		m_environmentProgram = new QGLShaderProgram;
		//顶点着色器
		m_environmentProgram->addShader(m_vertexShader);
		//像素着色器
		m_environmentProgram->addShader(m_environmentShader);
		//链接程序
		m_environmentProgram->link();
	}

	//噪声数据
	{
		const int NOISE_SIZE = 128;
		m_noise = new GLTexture3D(NOISE_SIZE, NOISE_SIZE, NOISE_SIZE);
		QRgb *data = new QRgb[NOISE_SIZE * NOISE_SIZE * NOISE_SIZE];
		memset(data, 0, NOISE_SIZE * NOISE_SIZE * NOISE_SIZE * sizeof(QRgb));
		QRgb *p = data;
		float pos[3];
		for (int k = 0; k < NOISE_SIZE; ++k) {
			pos[2] = k * (0x20 / (float)NOISE_SIZE);
			for (int j = 0; j < NOISE_SIZE; ++j) {
				for (int i = 0; i < NOISE_SIZE; ++i) {
					for (int byte = 0; byte < 4; ++byte) {
						pos[0] = (i + (byte & 1) * 16) * (0x20 / (float)NOISE_SIZE);
						pos[1] = (j + (byte & 2) * 8) * (0x20 / (float)NOISE_SIZE);
						*p |= (int)(128.0f * (noise3(pos) + 1.0f)) << (byte * 8);
					}
					++p;
				}
			}
		}
		m_noise->load(NOISE_SIZE, NOISE_SIZE, NOISE_SIZE, data);
		delete[] data;
	}

	//所有的boxes设置
	{
		m_mainCubemap = new GLRenderTargetCube(512);
		QStringList filter;
		QList<QFileInfo> files;
		//加载所有的png图片
		m_currentTexture = 0;
		filter = QStringList("*.png");
		files = QDir(":/res/boxes/").entryInfoList(filter, QDir::Files | QDir::Readable);
		foreach(QFileInfo file, files) {
			GLTexture *texture = new GLTexture2D(file.absoluteFilePath(), qMin(256, m_maxTextureSize), qMin(256, m_maxTextureSize));
			if (texture->failed()) {
				delete texture;
				continue;
			}
			m_textures << texture;
			m_renderOptions->addTexture(file.baseName());
		}

		//加载所有的像素着色器程序
		m_currentShader = 0;
		filter = QStringList("*.fsh");
		files = QDir(":/res/boxes/").entryInfoList(filter, QDir::Files | QDir::Readable);
		foreach(QFileInfo file, files) {
			//排除天空壳
			if(file.fileName().indexOf("skybox") != -1)
			{
				continue;
			}
			QGLShaderProgram *program = new QGLShaderProgram;
			QGLShader* shader = new QGLShader(QGLShader::Fragment);
			shader->compileSourceFile(file.absoluteFilePath());
			// The program does not take ownership over the shaders, so store them in a vector so they can be deleted afterwards.
			program->addShader(m_vertexShader);
			program->addShader(shader);
			if (!program->link()) {
				qWarning("Failed to compile and link shader program");
				qWarning("Vertex shader log:");
				qWarning() << m_vertexShader->log();
				qWarning() << "Fragment shader log ( file =" << file.absoluteFilePath() << "):";
				qWarning() << shader->log();
				qWarning("Shader program log:");
				qWarning() << program->log();

				delete shader;
				delete program;
				continue;
			}

			m_fragmentShaders << shader;
			m_programs << program;
			m_renderOptions->addShader(file.baseName());

			program->bind();
			m_cubemaps << ((program->uniformLocation("env") != -1) ? new GLRenderTargetCube(qMin(256, m_maxTextureSize)) : 0);
			program->release();
		}

		m_renderOptions->emitParameterChanged();
	}
}

static void loadMatrix(const QMatrix4x4& m)
{
    // static to prevent glLoadMatrixf to fail on certain drivers
    static GLfloat mat[16];
    const float *data = m.constData();
    for (int index = 0; index < 16; ++index)
        mat[index] = data[index];
    glLoadMatrixf(mat);
}

// If one of the boxes should not be rendered, set excludeBox to its index.
// If the main box should not be rendered, set excludeBox to -1.
void Scene::renderBoxes(const QMatrix4x4 &projection_mat, const QMatrix4x4 &view_mat, const QMatrix4x4 &model_mat, int excludeBox)
{
	QMatrix4x4 lightview = view_mat * model_mat;
    QMatrix4x4 invView = (view_mat * model_mat).inverted();
    // If multi-texturing is supported, use three saplers.
    if (glActiveTexture) {
		//设置texture0当前选择的纹理
        glActiveTexture(GL_TEXTURE0);
        m_textures[m_currentTexture]->bind();
		//设置texture2为3d噪声
        glActiveTexture(GL_TEXTURE2);
        m_noise->bind();
        glActiveTexture(GL_TEXTURE1);
    } else {
        m_textures[m_currentTexture]->bind();
    }

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

    QMatrix4x4 viewRotation(view_mat * model_mat);
    viewRotation(3, 0) = viewRotation(3, 1) = viewRotation(3, 2) = 0.0f;
    viewRotation(0, 3) = viewRotation(1, 3) = viewRotation(2, 3) = 0.0f;
    viewRotation(3, 3) = 1.0f;
	//viewRotation.setToIdentity();
    loadMatrix(viewRotation);
    glScalef(20.0f, 20.0f, 20.0f);

    // Don't render the environment if the environment texture can't be set for the correct sampler.
    if (glActiveTexture) {
		QMatrix4x4 skymodel_mat(model_mat);
		skymodel_mat.scale(20.0f, 20.0f, 20.0f);
		QMatrix3x3 normal_mat = (view_mat * skymodel_mat).normalMatrix();

		m_environment->bind();
        m_environmentProgram->bind();
        m_environmentProgram->setUniformValue("env", GLint(1));
		m_environmentProgram->setUniformValue("projection_mat", projection_mat);
		m_environmentProgram->setUniformValue("view_mat", view_mat);
		m_environmentProgram->setUniformValue("model_mat", skymodel_mat);
		m_environmentProgram->setUniformValue("normal_mat", normal_mat);
		m_environmentProgram->setUniformValue("lightview", lightview);
		setLights(m_environmentProgram);
        m_box->draw(m_environmentProgram);
        m_environmentProgram->release();
        m_environment->unbind();
    }

    loadMatrix(view_mat * model_mat);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    for (int i = 0; i < m_programs.size(); ++i) {
        if (i == excludeBox)
            continue;

        glPushMatrix();
        QMatrix4x4 m;
        m.rotate(m_trackBalls[1].rotation());
        glMultMatrixf(m.constData());

        glRotatef(360.0f * i / m_programs.size(), 0.0f, 0.0f, 1.0f);
        glTranslatef(2.0f, 0.0f, 0.0f);
        glScalef(0.3f, 0.6f, 0.6f);

		QMatrix4x4 cubemodel_mat;
		cubemodel_mat.rotate(m_trackBalls[1].rotation());
		cubemodel_mat = model_mat * cubemodel_mat;
		cubemodel_mat.rotate(360.0f * i / m_programs.size(), 0.0f, 0.0f, 1.0f);
		cubemodel_mat.translate(2.0f, 0.0f, 0.0f);
		cubemodel_mat.scale(0.3f, 0.6f, 0.6f);
		//法线矩阵
		QMatrix3x3 normal_mat = (view_mat * cubemodel_mat).normalMatrix();

        if (glActiveTexture) {
            if (m_dynamicCubemap && m_cubemaps[i])
                m_cubemaps[i]->bind();
            else
                m_environment->bind();
        }
        m_programs[i]->bind();
        m_programs[i]->setUniformValue("tex", GLint(0));
        m_programs[i]->setUniformValue("env", GLint(1));
        m_programs[i]->setUniformValue("noise", GLint(2));
        m_programs[i]->setUniformValue("invView", invView);
		m_programs[i]->setUniformValue("projection_mat", projection_mat);
		m_programs[i]->setUniformValue("view_mat", view_mat);
		m_programs[i]->setUniformValue("model_mat", cubemodel_mat);
		m_programs[i]->setUniformValue("normal_mat", normal_mat);
		m_programs[i]->setUniformValue("lightview", lightview);
		setLights(m_programs[i]);

        m_box->draw(m_programs[i]);
        m_programs[i]->release();

        if (glActiveTexture) {
            if (m_dynamicCubemap && m_cubemaps[i])
                m_cubemaps[i]->unbind();
            else
                m_environment->unbind();
        }
        glPopMatrix();
    }

    if (-1 != excludeBox) {
        QMatrix4x4 m;
        m.rotate(m_trackBalls[0].rotation());
        glMultMatrixf(m.constData());

		QMatrix4x4 cubemodel_mat;
		cubemodel_mat.rotate(m_trackBalls[0].rotation());
		cubemodel_mat = model_mat * cubemodel_mat;
		QMatrix3x3 normal_mat = (view_mat * cubemodel_mat).normalMatrix();

        if (glActiveTexture) {
            if (m_dynamicCubemap)
                m_mainCubemap->bind();
            else
                m_environment->bind();
        }

        m_programs[m_currentShader]->bind();
        m_programs[m_currentShader]->setUniformValue("tex", GLint(0));
        m_programs[m_currentShader]->setUniformValue("env", GLint(1));
        m_programs[m_currentShader]->setUniformValue("noise", GLint(2));
        m_programs[m_currentShader]->setUniformValue("invView", invView);
		m_programs[m_currentShader]->setUniformValue("projection_mat", projection_mat);
		m_programs[m_currentShader]->setUniformValue("view_mat", view_mat);
		m_programs[m_currentShader]->setUniformValue("model_mat", cubemodel_mat);
		m_programs[m_currentShader]->setUniformValue("lightview", lightview);
		m_programs[m_currentShader]->setUniformValue("normal_mat", normal_mat);
		setLights(m_programs[m_currentShader]);

        m_box->draw(m_programs[m_currentShader]);
        m_programs[m_currentShader]->release();

        if (glActiveTexture) {
            if (m_dynamicCubemap)
                m_mainCubemap->unbind();
            else
                m_environment->unbind();
        }
    }

    if (glActiveTexture) {
        glActiveTexture(GL_TEXTURE2);
        m_noise->unbind();
        glActiveTexture(GL_TEXTURE0);
    }
    m_textures[m_currentTexture]->unbind();
}

void Scene::initOpenGLParams()
{
    //glClearColor(0.25f, 0.25f, 0.5f, 1.0f);

	//初始化openl功能开关
    glEnable(GL_DEPTH_TEST);	//开启深度测试
    glEnable(GL_CULL_FACE);		//开启遮挡剔除
    glEnable(GL_LIGHTING);		//开启灯源
    glEnable(GL_COLOR_MATERIAL);//开启图形（材料）根据光线的照耀进行反射。
    glEnable(GL_TEXTURE_2D);	//开启二维文理
    glEnable(GL_NORMALIZE);		//开启法向量

	float materialSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}

void Scene::setLights(QGLShaderProgram* program)
{
	if (program)
	{
		QVector4D lightDir(0.0f, 0.0f, 1.0f, 0.0f );
		QVector4D lightam(0.2,0.2,0.2,1.0 );
		QVector4D lightdif(0.8,0.8,0.8,1.0 );
		QVector4D lightspe(1.0,1.0,1.0,1.0f );
		QVector4D materialSpecular(0.5f, 0.5f, 0.5f, 1.0f);
		//光照位置
		program->setUniformValue("light_position", lightDir);
		//环境光颜色
		program->setUniformValue("light_ambient", lightam);
		//漫反射光颜色
		program->setUniformValue("light_diffuse", lightdif);
		//镜面反射光颜色
		program->setUniformValue("light_specular", lightspe);
		//材质反射光颜色
		program->setUniformValue("material_specular", materialSpecular);
		//材质镜面反射指数[0,128]
		program->setUniformValue("material_shininess", 32.0f);
	}
	else
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		//float lightColour[] = {1.0f, 1.0f, 1.0f, 1.0f};
		float lightDir[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColour);
		//glLightfv(GL_LIGHT0, GL_SPECULAR, lightColour);
		glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);
		glEnable(GL_LIGHT0);
	}
}

void Scene::defaultStates()
{
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    //glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);

	//设置投影矩阵为当前矩阵
    //glMatrixMode(GL_MODELVIEW);
	//弹出一个矩阵
    glPopMatrix();

	//设置视图矩阵为当前矩阵
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0f);
    float defaultMaterialSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultMaterialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void Scene::renderCubemaps()
{
    // To speed things up, only update the cubemaps for the small cubes every N frames.
    const int N = (m_updateAllCubemaps ? 1 : 3);

    QMatrix4x4 mat;
    GLRenderTargetCube::getProjectionMatrix(mat, 0.1f, 100.0f);
	QMatrix4x4 projection(mat);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    loadMatrix(mat);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    QVector3D center;

    for (int i = m_frame % N; i < m_cubemaps.size(); i += N) {
        if (0 == m_cubemaps[i])
            continue;

        float angle = 2.0f * PI * i / m_cubemaps.size();

        center = m_trackBalls[1].rotation().rotatedVector(QVector3D(std::cos(angle), std::sin(angle), 0.0f));

        for (int face = 0; face < 6; ++face) {
            m_cubemaps[i]->begin(face);

            GLRenderTargetCube::getViewMatrix(mat, face);
            QVector4D v = QVector4D(-center.x(), -center.y(), -center.z(), 1.0);
            mat.setColumn(3, mat * v);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderBoxes(projection,mat, QMatrix4x4(), i);

            m_cubemaps[i]->end();
        }
    }

    for (int face = 0; face < 6; ++face) {
        m_mainCubemap->begin(face);
        GLRenderTargetCube::getViewMatrix(mat, face);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderBoxes(projection, mat, QMatrix4x4(), -1);

        m_mainCubemap->end();
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    m_updateAllCubemaps = false;
}

void Scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    float width = float(painter->device()->width());
    float height = float(painter->device()->height());

	//使用opengl绘制3d背景
	{
		//准备绘制
		painter->beginNativePainting();
		//初始化opengl参数
		initOpenGLParams();

		if (m_dynamicCubemap);
			renderCubemaps();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//投影变换矩阵
		QMatrix4x4 projection;
		projection.perspective(60.0, width / height, 0.01, 20.0);

		//观察变换矩阵
		QMatrix4x4 view;
		view.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0, 0, -20), QVector3D(0, 1, 0));

		//模型变换矩阵
		QMatrix4x4 model;
		view.rotate(m_trackBalls[2].rotation());
		view.translate(QVector3D(0, 0, -2.0f * std::exp(m_distExp / 1200.0f)));
		//view(2, 3) -= 2.0f * std::exp(m_distExp / 1200.0f);

		renderBoxes(projection, view, model);

		//恢复渲染前的opengl坐标状态(清除setStates中的设置)
		defaultStates();
		++m_frame;
		//结束绘制
		painter->endNativePainting();
	}
  }

QPointF Scene::pixelPosToViewPos(const QPointF& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if (event->isAccepted())
        return;

    if (event->buttons() & Qt::LeftButton) {
        m_trackBalls[0].move(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
        event->accept();
    } else {
        m_trackBalls[0].release(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
    }

    if (event->buttons() & Qt::RightButton) {
        m_trackBalls[1].move(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
        event->accept();
    } else {
        m_trackBalls[1].release(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
    }

    if (event->buttons() & Qt::MidButton) {
        m_trackBalls[2].move(pixelPosToViewPos(event->scenePos()), QQuaternion());
        event->accept();
    } else {
        m_trackBalls[2].release(pixelPosToViewPos(event->scenePos()), QQuaternion());
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted())
        return;

    if (event->buttons() & Qt::LeftButton) {
        m_trackBalls[0].push(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
        event->accept();
    }

    if (event->buttons() & Qt::RightButton) {
        m_trackBalls[1].push(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
        event->accept();
    }

    if (event->buttons() & Qt::MidButton) {
        m_trackBalls[2].push(pixelPosToViewPos(event->scenePos()), QQuaternion());
        event->accept();
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->isAccepted())
        return;

    if (event->button() == Qt::LeftButton) {
        m_trackBalls[0].release(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
        event->accept();
    }

    if (event->button() == Qt::RightButton) {
        m_trackBalls[1].release(pixelPosToViewPos(event->scenePos()), m_trackBalls[2].rotation().conjugate());
        event->accept();
    }

    if (event->button() == Qt::MidButton) {
        m_trackBalls[2].release(pixelPosToViewPos(event->scenePos()), QQuaternion());
        event->accept();
    }
}

void Scene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
    QGraphicsScene::wheelEvent(event);
    if (!event->isAccepted()) 
	{
        m_distExp -= event->delta();
        if (m_distExp < -8 * 120)
            m_distExp = -8 * 120;
        if (m_distExp > 10 * 120)
            m_distExp = 10 * 120;
        event->accept();
    }
}

void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseDoubleClickEvent(event);
	//获取双击的item
	QGraphicsItem* item = mouseGrabberItem();
	if (item)
	{
		//如果圆环
		if (item->type() == ItemBase::ItemCircle)
		{
			m_twoSidedGraphicsWidget->showidget();
			delete item;
			item = NULL;
		}
	}
}

void Scene::setShader(int index)
{
    if (index >= 0 && index < m_fragmentShaders.size())
        m_currentShader = index;
}

void Scene::setTexture(int index)
{
    if (index >= 0 && index < m_textures.size())
        m_currentTexture = index;
}

void Scene::toggleDynamicCubemap(int state)
{
    if ((m_dynamicCubemap = (state == Qt::Checked)))
        m_updateAllCubemaps = true;
}

void Scene::setColorParameter(const QString &name, QRgb color)
{
    // set the color in all programs
    foreach (QGLShaderProgram *program, m_programs) {
        program->bind();
        program->setUniformValue(program->uniformLocation(name), QColor(color));
        program->release();
    }
}

void Scene::setFloatParameter(const QString &name, float value)
{
    // set the color in all programs
    foreach (QGLShaderProgram *program, m_programs) {
        program->bind();
        program->setUniformValue(program->uniformLocation(name), value);
        program->release();
    }
}

void Scene::newItem(ItemDialog::ItemType type)
{
    QSize size = sceneRect().size().toSize();
    switch (type) {
    case ItemDialog::QtBoxItem:
        addItem(new QtBox(64, rand() % (size.width() - 64) + 32, rand() % (size.height() - 64) + 32));
        break;
    case ItemDialog::CircleItem:
        addItem(new CircleItem(64, rand() % (size.width() - 64) + 32, rand() % (size.height() - 64) + 32));
        break;
    case ItemDialog::SquareItem:
        addItem(new SquareItem(64, rand() % (size.width() - 64) + 32, rand() % (size.height() - 64) + 32));
        break;
    default:
        break;
    }
}
