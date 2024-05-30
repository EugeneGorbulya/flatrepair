#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FlarRepair");
    QCoreApplication::setApplicationName("Count");

    QSettings settings;

    QString filePathWorksDbPath = "data/BD.main.parametrs.project1.csv";
    settings.setValue("worksDbPath", filePathWorksDbPath);
    QString filePathMaterialsDbPath= "data/BD.work.for.project1.csv";
    settings.setValue("materialsDbPath", filePathMaterialsDbPath);

    MainWindow w;
    w.show();
    return a.exec();
}
