#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void openCSV(QString fileName);
    void openCSVwork(QStringList lstr);
    void openCSVmaterials(QStringList lstr);
    QVector<QString> getUniSurfaces(QString room);
    QVector<QString> getUniMaterials(QString surface);
    QVector<QString> getUniMenialWork(QString room);
    QVector<QString> getUniWorkType(QString workType);

    QMap<int, QVector<QString>> workDB;
    QMap<int, QVector<QString>> materialsBD;

    QVector<int> checktMaterials{};
    QVector<int> checktWorks{};

    QLabel *totalCostLabel;

protected:

private slots:
    void on_pushButton_clicked();
    void roomChanged(const QString &room, int rowNum);
    void materialChecked(QTreeWidgetItem *item, int column);
    void workChecked(QTreeWidgetItem *item, int column);
    void recalcForMeters(QTableWidgetItem *item);
    void updateTotalCost();
};

#endif // MAINWINDOW_H
