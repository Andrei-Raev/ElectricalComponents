#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTranslator>




MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{

    if (translator.load(":/i18n/ElectroControl_ru_RU.ts")){
        QApplication::instance() -> installTranslator(&translator);
    }

    ui->setupUi(this);

    connect(ui->actionEnglish, &QAction::triggered, this, [&]() { on_actionEnglish_triggered(); });
    connect(ui->actionRussian, &QAction::triggered, this, [&]() {on_actionRussian_triggered(); });

}

void  MainWindow::on_actionRussian_triggered()
{
    // QApplication::instance()->removeTranslator(&tr_en);
    if (ui -> actionEnglish->isChecked()){
        QApplication::instance() -> installTranslator(&translator);
        ui->setupUi(this);
        ui -> actionEnglish->setChecked(false);
        ui -> actionRussian->setChecked(true);

        connect(ui->actionEnglish, &QAction::triggered, this, [&]() { on_actionEnglish_triggered(); });
        connect(ui->actionRussian, &QAction::triggered, this, [&]() {on_actionRussian_triggered(); });
    }else{
        ui -> actionRussian->setChecked(true);
    }
}

void MainWindow::on_actionEnglish_triggered()
{
    if (ui -> actionRussian->isChecked()){
        QApplication::instance() -> removeTranslator(&translator);
        ui->setupUi(this);
        ui -> actionRussian->setChecked(false);
        ui -> actionEnglish->setChecked(true);

        connect(ui->actionEnglish, &QAction::triggered, this, [&]() { on_actionEnglish_triggered(); });
        connect(ui->actionRussian, &QAction::triggered, this, [&]() {on_actionRussian_triggered(); });

    }else{
        ui -> actionEnglish->setChecked(true);
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}
