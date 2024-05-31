#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QTableWidgetItem>

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
    QVector<QString> getUniMenialWork(QString room);  //zzzzzzzzzzz
    QVector<QString> getUniWorkType(QString workType);   //zzzzzzzzzzz
    QMap<int, QVector<QString>> workDB;
    void openCSVwork(QStringList lstr);
    void openCSVmaterials(QStringList lstr);
    QVector<QString> getUniSurfaces(QString room);
    QMap<int, QVector<QString>> materialsBD;

    QVector<int> checktMaterials{};

    QVector<QString> getUniMatirials(QString surface);

protected:

private slots:
    void on_pushButton_clicked();
    void roomChanged(const QString &room, int rowNum);
    void materialChecked(QTreeWidgetItem *item, int column);
    void workChecked(QTreeWidgetItem *item, int column);
    void recalcForMeters(QTableWidgetItem *item);
};
#endif // MAINWINDOW_H




