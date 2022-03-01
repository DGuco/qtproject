/**
 * https://zhuanlan.zhihu.com/p/99833863
 * https://blog.csdn.net/dcrmg/article/details/53556664
 * https://learnopengl-cn.github.io/01%20Getting%20started/08%20Coordinate%20Systems/
 * https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
 * https://www.cnblogs.com/graphics/archive/2012/07/25/2582119.html
 * https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
 */
#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("cube");
    app.setApplicationVersion("0.1");
    MainWidget widget;
    widget.show();

    return app.exec();
}
