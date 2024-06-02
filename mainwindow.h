#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QVector>
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

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void roomChanged(const QString& room, int rowNum);
    void materialChecked(QTreeWidgetItem *item, int column);
    void workChecked(QTreeWidgetItem *item, int column);
    void recalcForMeters(QTableWidgetItem *item);
    void updateTotalCost();
    void showAbout();

private:
    Ui::MainWindow *ui;
    QLabel *totalCostLabel;
    QVector<QVector<QString>> workDB;
    QVector<QVector<QString>> materialsBD;
    QVector<int> checktMaterials;
    QVector<int> checktWorks;

    void openCSV(QString fileName);
    void openCSVwork(QStringList lstr);
    void openCSVmaterials(QStringList lstr);
    QVector<QString> getUniSurfaces(QString room);
    QVector<QString> getUniMaterials(QString surface);
    QVector<QString> getUniMenialWork(QString room);
    QVector<QString> getUniWorkType(QString workType);
};

#endif // MAINWINDOW_H
