#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTranslator>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{



    if (translator.load(":/i18n/ElectroControl_ru_RU.ts")){
        QApplication::instance() -> installTranslator(&translator);
    }

    ui->setupUi(this);

    UiSetUp();


}

void MainWindow::UiSetUp(){
    connect(ui->actionEnglish, &QAction::triggered, this, [&]() {on_actionEnglish_triggered(); });
    connect(ui->actionRussian, &QAction::triggered, this, [&]() {on_actionRussian_triggered(); });

    connect(ui->actionAbout_author, &QAction::triggered, this, [&]() {msgBox.setText(tr("Author:\nRaev Andrei Sergeevich, IZTMS 2-3."));msgBox.setIcon(QMessageBox::Information);msgBox.exec();});
}

void  MainWindow::on_actionRussian_triggered()
{
    // QApplication::instance()->removeTranslator(&tr_en);
    if (ui -> actionEnglish->isChecked()){
        QApplication::instance() -> installTranslator(&translator);
        ui->setupUi(this);
        ui -> actionEnglish->setChecked(false);
        ui -> actionRussian->setChecked(true);

        UiSetUp();
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

        UiSetUp();

    }else{
        ui -> actionEnglish->setChecked(true);
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}
