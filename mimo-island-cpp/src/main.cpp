#include <QApplication>
#include "DynamicIsland.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName("MiMo Island");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MiMo");
    
    DynamicIsland window;
    window.show();
    
    return app.exec();
}
