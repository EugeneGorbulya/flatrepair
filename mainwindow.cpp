#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTreeWidget>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("FlarRepair", "Count");
    QString worksDb = settings.value("worksDbPath").toString();
    QString materialsDb = settings.value("materialsDbPath").toString();

    if (worksDb.isEmpty() || materialsDb.isEmpty()) {
        worksDb = QFileDialog::getOpenFileName(this,
                                               tr("Выберите базу работ"), "", tr(".csv (BD.work*.csv)"));
        materialsDb = QFileDialog::getOpenFileName(this,
                                                   tr("Выберите базу материалов"), "", tr("csv (BD.main*.csv)"));

        settings.setValue("worksDbPath", worksDb);
        settings.setValue("materialsDbPath", materialsDb);
    }

    openCSV(worksDb);
    openCSV(materialsDb);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openCSV(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox msgBox;
        msgBox.setText("Файл не открылся");
        msgBox.exec();

    }
    bool isTopic = true;
    bool isWork{};
    while(!file.atEnd()){
        QByteArray line = file.readLine();
        QString strLine = QString::fromUtf8(line);
        int quotIndex = strLine.indexOf('"');

        while(quotIndex != -1)
        {
            strLine.remove(quotIndex,1);
            quotIndex = strLine.indexOf('"');
        }

        QStringList lstr = strLine.split(',');

        if(isTopic ){
            if(lstr[1].contains("Type of menial work"))
            {
                isWork = true;
            } else{
                isWork = false;
            }
        }

        if(isWork)
        {
            openCSVwork(lstr);
        }
        else
        {
            openCSVmaterials(lstr);
        }
        isTopic = false;
    }

}

void MainWindow::openCSVwork(QStringList lstr)
{
    int colIndex{};
    for(QString i: lstr){
        if(workDB.size() == colIndex)
        {
            QVector<QString> tmp;
            workDB.insert(colIndex, tmp);
        }
        else
        {
            workDB[colIndex].push_back(i);
        }
        colIndex++;
    }
}

void MainWindow::openCSVmaterials(QStringList lstr)
{
    int colIndex{};
    for(QString i: lstr){
        if(materialsBD.size() == colIndex)
        {
            QVector<QString> tmp;
            materialsBD.insert(colIndex, tmp);
        }
        else
        {
            materialsBD[colIndex].push_back(i);
        }
        colIndex++;
    }
}

QVector<QString> MainWindow::getUniSurfaces(QString room)    // функция для перебора уникальных поверхностей для каждой комнаты (считается автоматически по названию комнаты)
{
    QVector<QString> result{};
    QSet<QString> lastSur{};

    for (int i = 0; i < materialsBD[1].count(); i++)
    {
        if(materialsBD[0][i] == room)
        {
            if(!lastSur.contains(materialsBD[1][i]))
            {
                result.push_back(materialsBD[1][i]);
                lastSur.insert(materialsBD[1][i]);
            }
        }
    }

    return result;
}

QVector<QString> MainWindow::getUniMatirials(QString surface)   // функция для перебора уникальных материалов для каждой поверхности (считается автоматически по поверхности из функции getUniSurfaces )
{
    QVector<QString> result{};
    QSet<QString> lastMaterial{};

    for (int i = 0; i < materialsBD[2].count(); i++)
    {
        if(materialsBD[1][i] == surface)
        {
            if(!lastMaterial.contains(materialsBD[2][i]))
            {
                result.push_back(materialsBD[2][i]);
                lastMaterial.insert(materialsBD[2][i]);
            }
        }
    }

    return result;
}
// zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz вниз


QVector<QString> MainWindow::getUniMenialWork(QString room)
{
    QVector<QString> result1{};
    QSet<QString> lastSur1{};

    for (int i = 0; i < workDB[1].count(); i++)
    {
        if(workDB[0][i] == room)
        {
            if(!lastSur1.contains(workDB[1][i]))
            {
                result1.push_back(workDB[1][i]);
                lastSur1.insert(workDB[1][i]);
            }
        }
    }

    return result1;

}

QVector<QString> MainWindow::getUniWorkType(QString workType)
{
    QVector<QString> result1{};
    QSet<QString> lastSur1{};

    for (int i = 0; i < workDB[2].count(); i++)
    {
        if(workDB[1][i] == workType)
        {
            if(!lastSur1.contains(workDB[2][i]))
            {
                result1.push_back(workDB[2][i]);
                lastSur1.insert(workDB[2][i]);
            }
        }
    }

    return result1;
}

// zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz вверх

void MainWindow::on_pushButton_clicked() {
    int newRowNum = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRowNum);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setItem(newRowNum, 1, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(newRowNum, 4, new QTableWidgetItem("0"));
    ui->tableWidget->setColumnWidth(2, 300);

    QComboBox *cb = new QComboBox(this);
    cb->insertItem(0, "Choose a room:");
    cb->setDuplicatesEnabled(false);
    QString lastItem{};
    for(QString str: materialsBD[0]) {
        if(str != lastItem) {
            cb->insertItem(cb->count(), str);
        }
        lastItem = str;
    }

    ui->tableWidget->setCellWidget(newRowNum, 0, cb);

    // Передаем номер строки через сигнал и слот
    connect(cb, &QComboBox::currentTextChanged, this, [this, newRowNum](const QString &room) {
        int rowNum = newRowNum == 0 ? -1 : newRowNum;
        this->roomChanged(room, rowNum);
    });

    ui->tableWidget->setColumnWidth(3, 300);

    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &MainWindow::recalcForMeters);
}

void MainWindow::roomChanged(const QString &room, int rowNum) {
    QTableWidget *tw = ui->tableWidget;

    // Debugging output
    qDebug() << "Room changed: " << room << ", row: " << rowNum;

    if (rowNum == -1) {
        // Handle the case where the first room is currently selected
        rowNum = 0;
    }

    // Remove existing widgets if present
    QWidget *existingWidget2 = tw->cellWidget(rowNum, 2);
    if (existingWidget2) {
        delete existingWidget2;
    }

    QTreeWidget *twMaterials = new QTreeWidget(this);
    twMaterials->setHeaderHidden(true);
    QVector<QString> surfaces = getUniSurfaces(room);

    for(QString surface: surfaces) {
        QVector<QString> materialDb = getUniMatirials(surface);
        QTreeWidgetItem *twi = new QTreeWidgetItem(twMaterials);
        twMaterials->addTopLevelItem(twi);
        twi->setText(0, surface);

        for(QString material: materialDb) {
            QTreeWidgetItem *twim = new QTreeWidgetItem(twi);
            twim->setCheckState(0, Qt::Unchecked);
            twim->setText(0, material);
        }
    }

    tw->setCellWidget(rowNum, 2, twMaterials);
    tw->setRowHeight(rowNum, 100);
    connect(twMaterials, &QTreeWidget::itemClicked, this, &MainWindow::materialChecked);

    // Processing work types similarly
    int rowNum1 = ui->tableWidget->currentRow();
    if (rowNum1 == -1) {
        // Handle the case where the first room is currently selected
        rowNum1 = 0;
    }
    // zzzzz
    if(ui->tableWidget->cellWidget(rowNum1, 3))              // zzzzz
        delete ui->tableWidget->cellWidget(rowNum1, 3);
    QTreeWidget *tw1 = new QTreeWidget(this);
    tw1->setHeaderHidden(true);

    QVector<QString> workTypes = getUniMenialWork(room);

    for(QString workType: workTypes)
    {
        QVector<QString> workDb = getUniWorkType(workType);

        QTreeWidgetItem * twi1 = new QTreeWidgetItem(tw1);
        tw1 -> addTopLevelItem(twi1);

        twi1->setText(0,workType);


        ui->tableWidget->setCellWidget(rowNum1, 3, tw1);
        ui->tableWidget->setRowHeight(rowNum1, 100);

        for(QString workT: workDb)
        {
            QTreeWidgetItem *twim1 = new QTreeWidgetItem(twi1);
            twim1->setCheckState(0,Qt::Unchecked);
            twim1->setText(0, workT);
        }
    }
    connect(tw1, &QTreeWidget::itemClicked, this, &MainWindow::workChecked);
}



void MainWindow::materialChecked(QTreeWidgetItem *item, int column)
{
    int matRowNum{0};
    int currRowNum = ui->tableWidget->currentRow();
    QTableWidget * tw = ui->tableWidget;
    for(matRowNum; matRowNum < materialsBD[2].count(); matRowNum++)
    {
        QString room = static_cast<QComboBox*>(tw->cellWidget(currRowNum,0))->currentText();
        if(materialsBD[2][matRowNum].contains(item->text(0)) && materialsBD[0][matRowNum].contains(room) && materialsBD[1][matRowNum].contains(item->parent()->text(0)))
        {
            break;
        }
    }
    if(matRowNum >= materialsBD[2].count())
        return;
    double materialPrice = materialsBD[4][matRowNum].toDouble();


    tw->blockSignals(true);
    if(item->checkState(column))
    {
        checktMaterials.push_back(matRowNum);

        if(materialsBD[3][matRowNum].toInt())
        {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(materialPrice * tw->item(currRowNum, 1)->text().toDouble() + tw->item(currRowNum, 4)->text().toDouble())));
        }
        else
        {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(materialPrice + tw->item(currRowNum, 4)->text().toDouble())));
        }
    }
    else
    {
        checktMaterials.remove(checktMaterials.indexOf(matRowNum),1);
        if(materialsBD[3][matRowNum].toInt())
        {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(tw->item(currRowNum, 4)->text().toDouble() - materialPrice * tw->item(currRowNum, 1)->text().toDouble())));
        }
        else
        {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(tw->item(currRowNum, 4)->text().toDouble() - materialPrice)));
        }
    }
    tw->blockSignals(false);
}

void MainWindow::workChecked(QTreeWidgetItem *item, int column)
{

}

void MainWindow::recalcForMeters(QTableWidgetItem *item)
{
    QTableWidget * tw = ui->tableWidget;

    int currRowNum = tw->currentRow();
    tw->item(currRowNum, 4)->setText("0");
    for(int matRowNum:checktMaterials)
    {
        tw->blockSignals(true);
        double materialPrice = materialsBD[4][matRowNum].toDouble();
        if(materialsBD[3][matRowNum].toInt())
        {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(materialPrice * tw->item(currRowNum, 1)->text().toDouble() + tw->item(currRowNum, 4)->text().toDouble())));
        }
        else
        {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(materialPrice + tw->item(currRowNum, 4)->text().toDouble())));
        }
        tw->blockSignals(false);
    }
}

