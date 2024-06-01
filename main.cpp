#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FlarRepair");
    QCoreApplication::setApplicationName("Count");

    QSettings settings;

    QString filePathWorksDbPath;
    settings.setValue("worksDbPath", filePathWorksDbPath);
    QString filePathMaterialsDbPath;
    settings.setValue("materialsDbPath", filePathMaterialsDbPath);

    MainWindow w;
    w.show();
    return a.exec();
}