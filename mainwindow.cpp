#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTranslator>
#include <QMessageBox>
#include <QFileDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent),
      ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    if (translator.load(":/i18n/ElectroControl_ru_RU.ts")){
        qApp -> installTranslator(&translator);
    }
 ui->retranslateUi(this);


    UiSetUp();


}

void MainWindow::UiSetUp(){
    connect(ui->actionEnglish, &QAction::triggered, this, [&]() {on_actionEnglish_triggered(); });
    connect(ui->actionRussian, &QAction::triggered, this, [&]() {on_actionRussian_triggered(); });

    connect(ui -> actionOpen, &QAction::triggered, this, [&]() {openFile(ui -> tabWidget); });

    connect(ui->actionAbout_author, &QAction::triggered, this, [&]() {msgBox.setText(tr("Author:\nRaev Andrei Sergeevich, IZTMS 2-3."));msgBox.setIcon(QMessageBox::Information);msgBox.exec();});
}

void  MainWindow::on_actionRussian_triggered()
{
    if (ui -> actionEnglish->isChecked()){
        qApp -> installTranslator(&translator);
        ui->retranslateUi(this);
        ui -> actionEnglish->setChecked(false);
        ui -> actionRussian->setChecked(true);
    }else{
        ui -> actionRussian->setChecked(true);
    }
}

void MainWindow::on_actionEnglish_triggered()
{
    if (ui -> actionRussian->isChecked()){
        qApp -> removeTranslator(&translator);
        ui->retranslateUi(this);
        ui -> actionRussian->setChecked(false);
        ui -> actionEnglish->setChecked(true);
    }else{
        ui -> actionEnglish->setChecked(true);
    }
}


QVector<QStringList> MainWindow::readCSVTable(const QString& filename) {
    QVector<QStringList> data;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file" << filename;
        return data;
    }

    QTextStream in(&file);
    bool firstLine = true;
    while (!in.atEnd()) {
        QStringList line = in.readLine().split(";");

        if (firstLine) {
            firstLine = false;
            if (line.size() != 8) {
                qDebug() << "Error: Incorrect number of columns in the header";
                return data;
            }
            continue; // Skip processing the header line
        }

        if (line.size() != 8) {
            qDebug() << "Error: Incorrect number of columns in data";
            return data;
        }

        bool ok;
        line[0].toInt(&ok);
        if (!ok) {
            qDebug() << "Error: Invalid id format";
            return data;
        }

        // Add more data integrity checks for other columns here
//        delete line[0];
        data.append(line);
    }

    file.close();
    return data;
}


void MainWindow::openFile(QTabWidget* tabWidget) {
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Select CSV File", "", "CSV files (*.csv)");

    if (filePath.isEmpty()) {
        qDebug() << "Error: No file selected";
        return;
    }

    QVector<QStringList> data = readCSVTable(filePath);

    if (data.isEmpty()) {
        qDebug() << "Error: Failed to load data from file";
        return;
    }

    QTableWidget* tableWidget = new QTableWidget();
    int numRows = data.size();
    int numCols = data[0].size();

    tableWidget->setRowCount(numRows);
    tableWidget->setColumnCount(numCols);

    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            tableWidget->setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }

    tableWidget->setHorizontalHeaderLabels(QStringList() << "Id" << "Name"<<"Category"<<"Quantity"<<"Price"<<"Voltage"<<"Power usage"<<"In stock");
    tableWidget->setSortingEnabled(true);
    tableWidget-> setColumnHidden(0, true);

    //tabWidget -> removeTab(0);
    tabWidget->addTab(tableWidget, filePath.split("/")[filePath.split("/").length()]);
    tabWidget->setEnabled(true);

}



MainWindow::~MainWindow()
{
    delete ui;
}
