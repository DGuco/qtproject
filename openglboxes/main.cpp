//#include <GL/glew.h>
/**
 * https://zhuanlan.zhihu.com/p/346844820
 * https://zhuanlan.zhihu.com/p/22337544
 * https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/
 * https://www.jianshu.com/p/e001aec29976
 * https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/
 * https://baijiahao.baidu.com/s?id=1708089581887590806&wfr=spider&for=pc
 * https://blog.csdn.net/zwj_zny/article/details/123070087
 * https://blog.csdn.net/lwx309025167/article/details/83990802
 */
#include "glextensions.h"

#include "scene.h"

#include <QtWidgets>
#include <QGLWidget>
#include <QVector>

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
    {
        setWindowTitle(tr("Boxes"));
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        if (scene())
            scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        QGraphicsView::resizeEvent(event);
    }
};

inline bool matchString(const char *extensionString, const char *subString)
{
    int subStringLength = strlen(subString);
    return (strncmp(extensionString, subString, subStringLength) == 0)
        && ((extensionString[subStringLength] == ' ') || (extensionString[subStringLength] == '\0'));
}

bool necessaryExtensionsSupported()
{
    const char *extensionString = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
    const char *p = extensionString;

    const int GL_EXT_FBO = 1;
    const int GL_ARB_VS = 2;
    const int GL_ARB_FS = 4;
    const int GL_ARB_SO = 8;
    int extensions = 0;

    while (*p) {
        if (matchString(p, "GL_EXT_framebuffer_object"))
            extensions |= GL_EXT_FBO;
        else if (matchString(p, "GL_ARB_vertex_shader"))
            extensions |= GL_ARB_VS;
        else if (matchString(p, "GL_ARB_fragment_shader"))
            extensions |= GL_ARB_FS;
        else if (matchString(p, "GL_ARB_shader_objects"))
            extensions |= GL_ARB_SO;
        while ((*p != ' ') && (*p != '\0'))
            ++p;
        if (*p == ' ')
            ++p;
    }
    return (extensions == 15);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_3) == 0) {
        QMessageBox::critical(0, "OpenGL features missing",
            "OpenGL version 1.5 or higher is required to run this demo.\n"
            "The program will now exit.");
        return -1;
    }

    int maxTextureSize = 1024;
    QGLWidget *widget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
    widget->makeCurrent();

    if (!necessaryExtensionsSupported()) {
        QMessageBox::critical(0, "OpenGL features missing",
            "The OpenGL extensions required to run this demo are missing.\n"
            "The program will now exit.");
        delete widget;
        return -2;
    }

    // Check if all the necessary functions are resolved.
    if (!getGLExtensionFunctions().resolve(widget->context())) {
        QMessageBox::critical(0, "OpenGL features missing",
            "Failed to resolve OpenGL functions required to run this demo.\n"
            "The program will now exit.");
        delete widget;
        return -3;
    }

//     // TODO: Make conditional for final release
//     QMessageBox::information(0, "For your information",
//         "This demo can be GPU and CPU intensive and may\n"
//         "work poorly or not at all on your system.");

	// The current context must be set before calling Scene's constructor
    widget->makeCurrent();

    Scene scene(1024, 768, maxTextureSize);
    GraphicsView view;
    view.setViewport(widget);
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setScene(&scene);
    view.show();

    return app.exec();
}

