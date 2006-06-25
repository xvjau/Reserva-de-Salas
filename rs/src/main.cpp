#include "main.h"
#include "CMainWindow.h"
//#include "res/RS.h"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    g_application = &app;
    
    CMainWindow mainwindow;
    mainwindow.show();
    
    return app.exec();
}
