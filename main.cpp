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

    a.setStyleSheet(
        "QMainWindow { background-color: #F0F8FF; }"  // AliceBlue background for the main window
        "QPushButton {"
        "   background-color: #1E90FF;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;" /* Rounded corners */
        "   padding: 10px 20px;"
        "   text-align: center;"
        "   text-decoration: none;"
        "   font-size: 16px;"
        "   margin: 4px 2px;"
        "   cursor: pointer;"
        "}"
        "QPushButton:hover { background-color: #4169E1; }"
        "QLabel { font-size: 16px; color: #483D8B; }"  // DarkSlateBlue text for labels
        "QTableWidget {"
        "   gridline-color: #F0F8FF;"  // AliceBlue lines for grid
        "   font-size: 16px;"
        "   background-color: #E6E6FA;"  // Lavender background inside table
        "   color: #483D8B;"  // DarkSlateBlue text inside table
        "   border: 1px solid #F0F8FF;"  // AliceBlue frame
        "}"
        "QHeaderView::section {"
        "   background-color: #E6E6FA;"  // Lavender background for header sections
        "   padding: 4px;"
        "   border: 1px solid #F0F8FF;"  // AliceBlue border for header sections
        "   color: #483D8B;"  // DarkSlateBlue text for header sections
        "}"
        "QTableWidget QTableCornerButton::section {"
        "   background-color: #E6E6FA;"  // Lavender background for corner button
        "   border: 1px solid #F0F8FF;"  // AliceBlue border for corner button
        "   color: #483D8B;"  // DarkSlateBlue text for corner button
        "}"
        "QTableWidget::item {"
        "   border: 1px solid #F0F8FF;"  // AliceBlue border for items
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #6A5ACD;"  // SlateBlue background for selected items
        "   color: #FFFFFF;"  // White text for selected items
        "}"
        "QTableWidget::item:focus {"
        "   border: 1px solid #F0F8FF;"  // AliceBlue border for focused item
        "   background-color: #E6E6FA;"  // Lavender background for focused item
        "}"
        "QTableWidget::item:focus:selected {"
        "   background-color: #6A5ACD;"  // SlateBlue background for focused and selected item
        "   color: #FFFFFF;"  // White text for focused and selected item
        "}"
        "QTableWidget::item:focus:hover {"
        "   background-color: #E6E6FA;"  // Lavender background on hover
        "   color: #483D8B;"  // DarkSlateBlue text on hover
        "}"
        "QTreeWidget { background-color: #E6E6FA; border: 1px solid #F0F8FF; }"  // Lavender background for tree widget
        "QTreeWidget::item { background-color: #E6E6FA; border: 1px solid #F0F8FF; color: #483D8B; }"  // DarkSlateBlue text for tree widget items
        "QTreeWidget::item:selected { background-color: #6A5ACD; color: #FFFFFF; }"  // SlateBlue background and white text for selected items
        "QTreeWidget::item:focus { border: 1px solid #F0F8FF; background-color: #E6E6FA; color: #483D8B; }"  // Lavender background and DarkSlateBlue text for focused items
        "QTreeWidget::item:focus:selected { background-color: #6A5ACD; color: #FFFFFF; }"  // SlateBlue background and white text for focused and selected items
        "QTreeWidget::item:focus:hover { background-color: #E6E6FA; color: #483D8B; }"  // Lavender background and DarkSlateBlue text on hover
        "QTreeWidget::item:hover { background-color: #E6E6FA; color: #483D8B; }"  // Lavender background and DarkSlateBlue text on hover
        "QMenuBar { background-color: #4169E1; color: white; }"
        "QMenuBar::item { background-color: #4169E1; color: white; }"
        "QMenuBar::item:selected { background-color: #483D8B; }"
        "QStatusBar { background-color: #4169E1; color: white; }"
        "QComboBox { color: #000000; background-color: #E6E6FA; }"  // Lavender background for combo box
        "QComboBox QAbstractItemView { color: #000000; background-color: #E6E6FA; }"
        );

    MainWindow w;
    w.show();
    return a.exec();
}
