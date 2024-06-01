#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTreeWidget>
#include <QSettings>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("FlarRepair", "Count");
    QString worksDb = settings.value("worksDbPath").toString();
    QString materialsDb = settings.value("materialsDbPath").toString();

    if (worksDb.isEmpty() || materialsDb.isEmpty()) {
        worksDb = QFileDialog::getOpenFileName(this, tr("Выберите базу работ"), "", tr(".csv (BD.work*.csv)"));
        materialsDb = QFileDialog::getOpenFileName(this, tr("Выберите базу материалов"), "", tr(".csv (BD.main*.csv)"));

        settings.setValue("worksDbPath", worksDb);
        settings.setValue("materialsDbPath", materialsDb);
    }

    openCSV(worksDb);
    openCSV(materialsDb);

    // Initialize the total cost label
    totalCostLabel = findChild<QLabel*>("totalCostLabel");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openCSV(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Файл не открылся"));
        return;
    }

    bool isTopic = true;
    bool isWork = false;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QString strLine = QString::fromUtf8(line).remove('"');
        QStringList lstr = strLine.split(',');

        if (isTopic) {
            isWork = lstr[1].contains("Type of menial work");
            isTopic = false;
        }

        if (isWork) {
            openCSVwork(lstr);
        } else {
            openCSVmaterials(lstr);
        }
    }
}

void MainWindow::openCSVwork(QStringList lstr)
{
    for (int colIndex = 0; colIndex < lstr.size(); ++colIndex) {
        if (workDB.size() == colIndex) {
            workDB.insert(colIndex, QVector<QString>());
        }
        workDB[colIndex].push_back(lstr[colIndex]);
    }
}

void MainWindow::openCSVmaterials(QStringList lstr)
{
    for (int colIndex = 0; colIndex < lstr.size(); ++colIndex) {
        if (materialsBD.size() == colIndex) {
            materialsBD.insert(colIndex, QVector<QString>());
        }
        materialsBD[colIndex].push_back(lstr[colIndex]);
    }
}

QVector<QString> MainWindow::getUniSurfaces(QString room)
{
    QVector<QString> result;
    QSet<QString> surfaces;

    for (int i = 0; i < materialsBD[1].count(); ++i) {
        if (materialsBD[0][i] == room && !surfaces.contains(materialsBD[1][i])) {
            result.push_back(materialsBD[1][i]);
            surfaces.insert(materialsBD[1][i]);
        }
    }

    return result;
}

QVector<QString> MainWindow::getUniMaterials(QString surface)
{
    QVector<QString> result;
    QSet<QString> materials;

    for (int i = 0; i < materialsBD[2].count(); ++i) {
        if (materialsBD[1][i] == surface && !materials.contains(materialsBD[2][i])) {
            result.push_back(materialsBD[2][i]);
            materials.insert(materialsBD[2][i]);
        }
    }

    return result;
}

QVector<QString> MainWindow::getUniMenialWork(QString room)
{
    QVector<QString> result;
    QSet<QString> works;

    for (int i = 0; i < workDB[1].count(); ++i) {
        if (workDB[0][i] == room && !works.contains(workDB[1][i])) {
            result.push_back(workDB[1][i]);
            works.insert(workDB[1][i]);
        }
    }

    return result;
}

QVector<QString> MainWindow::getUniWorkType(QString workType)
{
    QVector<QString> result;
    QSet<QString> workTypes;

    for (int i = 0; i < workDB[2].count(); ++i) {
        if (workDB[1][i] == workType && !workTypes.contains(workDB[2][i])) {
            result.push_back(workDB[2][i]);
            workTypes.insert(workDB[2][i]);
        }
    }

    return result;
}

void MainWindow::on_pushButton_clicked()
{
    int newRowNum = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRowNum);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setItem(newRowNum, 1, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(newRowNum, 4, new QTableWidgetItem("0"));
    ui->tableWidget->setColumnWidth(2, 300);

    QComboBox *cb = new QComboBox(this);
    cb->insertItem(0, tr("Choose a room:"));
    cb->setDuplicatesEnabled(false);

    QString lastItem = "Room type";
    for (const QString& str : materialsBD[0]) {
        if (str != lastItem) {
            cb->insertItem(cb->count(), str);
        }
        lastItem = str;
    }

    ui->tableWidget->setCellWidget(newRowNum, 0, cb);

    connect(cb, &QComboBox::currentTextChanged, this, [this, newRowNum](const QString &room) {
        this->roomChanged(room, newRowNum);
    });
    qDebug() << "IAMALIVE15";
    ui->tableWidget->setColumnWidth(3, 300);
    qDebug() << "IAMALIVE16";
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &MainWindow::recalcForMeters);
    qDebug() << "IAMALIVE17";

}

void MainWindow::roomChanged(const QString& room, int rowNum)
{
    QTableWidget *tw = ui->tableWidget;

    qDebug() << "Room changed: " << room << ", row: " << rowNum;

    tw->item(rowNum, 1)->setText("0");
    tw->item(rowNum, 4)->setText("0");

    // Удаляем галочки с чекбоксов в виджете в ячейке 2
    QTreeWidget *twMaterials = qobject_cast<QTreeWidget*>(tw->cellWidget(rowNum, 2));
    if (twMaterials) {
        // Удаляем все отмеченные элементы
        QList<QTreeWidgetItem *> checkedItems = twMaterials->findItems(QString(), Qt::MatchExactly | Qt::MatchRecursive, Qt::CheckStateRole);
        for (QTreeWidgetItem *item : checkedItems) {
            item->setCheckState(0, Qt::Unchecked);
        }
    }
    // Удаляем галочки с чекбоксов в виджете в ячейке 3
    QTreeWidget *tw1 = qobject_cast<QTreeWidget*>(tw->cellWidget(rowNum, 3));
    if (tw1) {
        // Удаляем все отмеченные элементы
        QList<QTreeWidgetItem *> checkedItems = tw1->findItems(QString(), Qt::MatchExactly | Qt::MatchRecursive, Qt::CheckStateRole);
        for (QTreeWidgetItem *item : checkedItems) {
            item->setCheckState(0, Qt::Unchecked);
        }
    }
    // Обновление данных в существующем виджете в ячейке 2
    if (twMaterials) {
        twMaterials->clear(); // Очищаем старые данные
        twMaterials->setHeaderHidden(true);
        QVector<QString> surfaces = getUniSurfaces(room);

        for (const QString& surface : surfaces) {
            QVector<QString> materialDb = getUniMaterials(surface);
            QTreeWidgetItem *twi = new QTreeWidgetItem(twMaterials);
            twMaterials->addTopLevelItem(twi);
            twi->setText(0, surface);

            for (const QString& material : materialDb) {
                QTreeWidgetItem *twim = new QTreeWidgetItem(twi);
                twim->setCheckState(0, Qt::Unchecked);
                twim->setText(0, material);
            }
        }
    } else {
        QTreeWidget *twMaterials = new QTreeWidget(this);
        twMaterials->setHeaderHidden(true);
        QVector<QString> surfaces = getUniSurfaces(room);

        for (const QString& surface : surfaces) {
            QVector<QString> materialDb = getUniMaterials(surface);
            QTreeWidgetItem *twi = new QTreeWidgetItem(twMaterials);
            twMaterials->addTopLevelItem(twi);
            twi->setText(0, surface);

            for (const QString& material : materialDb) {
                QTreeWidgetItem *twim = new QTreeWidgetItem(twi);
                twim->setCheckState(0, Qt::Unchecked);
                twim->setText(0, material);
            }
        }

        tw->setCellWidget(rowNum, 2, twMaterials);
        tw->setRowHeight(rowNum, 100);
        connect(twMaterials, &QTreeWidget::itemClicked, this, &MainWindow::materialChecked);
    }
    // Обновление данных в существующем виджете в ячейке 3
    if (tw1) {
        tw1->clear(); // Очищаем старые данные
        tw1->setHeaderHidden(true);
        QVector<QString> workTypes = getUniMenialWork(room);

        for (const QString& workType : workTypes) {
            QVector<QString> workDb = getUniWorkType(workType);
            QTreeWidgetItem *twi1 = new QTreeWidgetItem(tw1);
            tw1->addTopLevelItem(twi1);
            twi1->setText(0, workType);

            for (const QString& workT : workDb) {
                QTreeWidgetItem *twim1 = new QTreeWidgetItem(twi1);
                twim1->setCheckState(0, Qt::Unchecked);
                twim1->setText(0, workT);
            }
        }
    } else {
        QTreeWidget *tw1 = new QTreeWidget(this);
        tw1->setHeaderHidden(true);

        QVector<QString> workTypes = getUniMenialWork(room);

        for (const QString& workType : workTypes) {
            QVector<QString> workDb = getUniWorkType(workType);
            QTreeWidgetItem *twi1 = new QTreeWidgetItem(tw1);
            tw1->addTopLevelItem(twi1);
            twi1->setText(0, workType);

            for (const QString& workT : workDb) {
                QTreeWidgetItem *twim1 = new QTreeWidgetItem(twi1);
                twim1->setCheckState(0, Qt::Unchecked);
                twim1->setText(0, workT);
            }
        }

        tw->setCellWidget(rowNum, 3, tw1);
        tw->setRowHeight(rowNum, 100);
        connect(tw1, &QTreeWidget::itemClicked, this, &MainWindow::workChecked);
    }

    updateTotalCost();
}


void MainWindow::materialChecked(QTreeWidgetItem *item, int column)
{
    int matRowNum = 0;
    int currRowNum = ui->tableWidget->currentRow();
    QTableWidget *tw = ui->tableWidget;

    for (; matRowNum < materialsBD[2].count(); ++matRowNum) {
        QString room = static_cast<QComboBox*>(tw->cellWidget(currRowNum, 0))->currentText();
        if (materialsBD[2][matRowNum].contains(item->text(0)) && materialsBD[0][matRowNum].contains(room) && materialsBD[1][matRowNum].contains(item->parent()->text(0))) {
            break;
        }
    }

    if (matRowNum >= materialsBD[2].count()) {
        return;
    }

    double materialPrice = materialsBD[4][matRowNum].toDouble();
    tw->blockSignals(true);

    if (item->checkState(column)) {
        checktMaterials.push_back(matRowNum);

        if (materialsBD[3][matRowNum].toInt()) {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(materialPrice * tw->item(currRowNum, 1)->text().toDouble() + tw->item(currRowNum, 4)->text().toDouble())));
        } else {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(materialPrice + tw->item(currRowNum, 4)->text().toDouble())));
        }
    } else {
        checktMaterials.removeOne(matRowNum);

        if (materialsBD[3][matRowNum].toInt()) {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(tw->item(currRowNum, 4)->text().toDouble() - materialPrice * tw->item(currRowNum, 1)->text().toDouble())));
        } else {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(tw->item(currRowNum, 4)->text().toDouble() - materialPrice)));
        }
    }

    double newValue = tw->item(currRowNum, 4)->text().toDouble();
    newValue = qFabs(newValue) < 0.000001 ? 0.0 : newValue;
    tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(newValue)));

    tw->blockSignals(false);
    updateTotalCost(); // Update total cost whenever a material is checked or unchecked
}

void MainWindow::workChecked(QTreeWidgetItem *item, int column)
{
    int workRowNum = 0;
    int currRowNum = ui->tableWidget->currentRow();
    QTableWidget *tw = ui->tableWidget;

    for (; workRowNum < workDB[2].count(); ++workRowNum) {
        QString room = static_cast<QComboBox*>(tw->cellWidget(currRowNum, 0))->currentText();
        if (workDB[2][workRowNum].contains(item->text(0)) && workDB[0][workRowNum].contains(room) && workDB[1][workRowNum].contains(item->parent()->text(0))) {
            break;
        }
    }

    if (workRowNum >= workDB[2].count()) {
        return;
    }

    double workPrice = workDB[4][workRowNum].toDouble();
    tw->blockSignals(true);

    if (item->checkState(column)) {
        checktWorks.push_back(workRowNum);

        if (workDB[3][workRowNum].toInt()) {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(workPrice * tw->item(currRowNum, 1)->text().toDouble() + tw->item(currRowNum, 4)->text().toDouble())));
        } else {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(workPrice + tw->item(currRowNum, 4)->text().toDouble())));
        }
    } else {
        checktWorks.removeOne(workRowNum);

        if (workDB[3][workRowNum].toInt()) {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(tw->item(currRowNum, 4)->text().toDouble() - workPrice * tw->item(currRowNum, 1)->text().toDouble())));
        } else {
            tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(tw->item(currRowNum, 4)->text().toDouble() - workPrice)));
        }
    }

    double newValue = tw->item(currRowNum, 4)->text().toDouble();
    newValue = qFabs(newValue) < 0.000001 ? 0.0 : newValue;
    tw->setItem(currRowNum, 4, new QTableWidgetItem(QString::number(newValue)));

    tw->blockSignals(false);
    updateTotalCost(); // Update total cost whenever a work is checked or unchecked
}


void MainWindow::recalcForMeters(QTableWidgetItem *item)
{
    qDebug() << "Entering recalcForMeters";
    QTableWidget *tw = ui->tableWidget;

    int currRowNum = tw->currentRow();
    double totalMaterialCost = 0.0; // Общая стоимость материалов для текущей строки
    double totalWorkCost = 0.0;
    // Пересчитываем стоимость для всех выбранных материалов
    for (int matRowNum : checktMaterials) {
        tw->blockSignals(true);
        double materialPrice = materialsBD[4][matRowNum].toDouble();
        QString room = static_cast<QComboBox*>(tw->cellWidget(currRowNum, 0))->currentText();
        QString materialSurface = materialsBD[1][matRowNum];
        QString materialRoom = materialsBD[0][matRowNum];

        // Проверяем, что материал соответствует текущему типу комнаты
        if (materialRoom == room) {
            // Пересчитываем стоимость материала в зависимости от нового значения метража
            double meters = tw->item(currRowNum, 1)->text().toDouble();
            double materialCost = materialsBD[3][matRowNum].toInt() ? materialPrice * meters : materialPrice;
            totalMaterialCost += materialCost;
        }

        tw->blockSignals(false);
    }

    for (int workRowNum : checktWorks) {
        tw->blockSignals(true);
        double workPrice = workDB[4][workRowNum].toDouble();
        QString room = static_cast<QComboBox*>(tw->cellWidget(currRowNum, 0))->currentText();
        QString workSurface = workDB[1][workRowNum];
        QString workRoom = workDB[0][workRowNum];

        // Проверяем, что работа соответствует текущему типу комнаты
        if (workRoom == room) {
            // Пересчитываем стоимость работы в зависимости от нового значения метража
            double meters = tw->item(currRowNum, 1)->text().toDouble();
            double workCost = workDB[3][workRowNum].toInt() ? workPrice * meters : workPrice;
            totalWorkCost += workCost;
        }

        tw->blockSignals(false);
    }

    // Устанавливаем новую стоимость для текущей строки
    tw->item(currRowNum, 4)->setText(QString::number(totalMaterialCost + totalWorkCost));

    updateTotalCost(); // Обновляем общую стоимость
    qDebug() << "Exiting recalcForMeters";
}

//

void MainWindow::updateTotalCost()
{
    qDebug() << "IAMALIVE10";
    double totalCost = 0.0;
    QTableWidget *tw = ui->tableWidget;
    qDebug() << "IAMALIVE11";
    for (int row = 0; row < tw->rowCount(); ++row) {
        if (tw->item(row, 4)) {
            totalCost += tw->item(row, 4)->text().toDouble();
        }
    }
    qDebug() << "IAMALIVE12";
    totalCostLabel->setText(QString("Total Cost: %1").arg(totalCost));
    qDebug() << "IAMALIVE13";
}
