#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMimeData>
#include <QFileInfo>
#include <QDebug>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    if (translator.load(":/i18n/ElectroControl_ru_RU.ts")) {
        qApp->installTranslator(&translator);
    }
    ui->retranslateUi(this);

    UiSetUp();
}

void MainWindow::UiSetUp()
{
    connect(ui->actionEnglish, &QAction::triggered, this, [&]() { on_actionEnglish_triggered(); });
    connect(ui->actionRussian, &QAction::triggered, this, [&]() { on_actionRussian_triggered(); });

    connect(ui->actionOpen, &QAction::triggered, this, [&]() { openFile(); });
    connect(ui->actionSave, &QAction::triggered, this, [&]() { saveFile(); });

    QShortcut *shortcutSave = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcutSave, &QShortcut::activated, this, &MainWindow::saveFile);

    QShortcut *shortcutOpen = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(shortcutOpen, &QShortcut::activated, this, &MainWindow::openFile);

    QShortcut *shortcutNew = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(shortcutNew, &QShortcut::activated, this, &MainWindow::newFile);

    connect(ui->actionAddRow, &QAction::triggered, this, [&]() {
        QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
        if (currentView) {
            CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
            if (model) {
                model->addRow(QStringList(model->columnCount(), ""));
            }
        }
    });

    connect(ui->actionRemoveRow, &QAction::triggered, this, [&]() {
        QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
        if (currentView) {
            QItemSelectionModel *select = currentView->selectionModel();
            CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
            if (model && select->hasSelection()) {
                model->removeRow(select->selectedRows().first().row());
            }
        }
    });

    connect(ui->actionAbout_author, &QAction::triggered, this, [&]() {
        msgBox.setText(tr("Author:\nRaev Andrei Sergeevich, IZTMS 2-3."));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    });
}

void MainWindow::on_actionRussian_triggered()
{
    if (ui->actionEnglish->isChecked()) {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        ui->actionEnglish->setChecked(false);
        ui->actionRussian->setChecked(true);
    } else {
        ui->actionRussian->setChecked(true);
    }
}

void MainWindow::on_actionEnglish_triggered()
{
    if (ui->actionRussian->isChecked()) {
        qApp->removeTranslator(&translator);
        ui->retranslateUi(this);
        ui->actionRussian->setChecked(false);
        ui->actionEnglish->setChecked(true);
    } else {
        ui->actionEnglish->setChecked(true);
    }
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr,
                                                    "Select CSV File",
                                                    "",
                                                    "CSV files (*.csv)");

    if (filePath.isEmpty()) {
        qDebug() << "Error: No file selected";
        return;
    }

    CsvModel *model = new CsvModel(this);
    model->loadFromFile(filePath);

    if (!model->isValid) {
        QMessageBox::warning(this, tr("Invalid File"), tr("The CSV file is invalid."));
        delete model;
        return;
    }

    createNewTableTab(filePath, model);
}

void MainWindow::saveFile()
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (!currentView) {
        QMessageBox::warning(this, tr("No Tab Selected"), tr("Please select a tab to save."));
        return;
    }

    CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
    if (!model) {
        qDebug() << "Error: No model found";
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(nullptr,
                                                    "Save CSV File",
                                                    "",
                                                    "CSV files (*.csv)");

    if (filePath.isEmpty()) {
        qDebug() << "Error: No file selected";
        return;
    }

    model->saveToFile(filePath);
}

void MainWindow::newFile()
{
    // Создаем новый объект CsvModel
    CsvModel *model = new CsvModel(this);

    // Инициализируем его пустыми данными
    model->initializeEmpty();

    // Создаем новую вкладку для этого файла
    createNewTableTab("Untitled.csv", model);
}




QTableView* MainWindow::createNewTableTab(const QString &filePath, CsvModel *model)
{
    QTableView *tableView = new QTableView;
    tableView->setModel(model);

    QString tabName = QFileInfo(filePath).fileName();
    ui->tabWidget->addTab(tableView, tabName);
    ui->tabWidget->setCurrentWidget(tableView);

    return tableView;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        if (!urlList.isEmpty() && QFileInfo(urlList.first().toLocalFile()).suffix() == "csv") {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            QFileInfo fileInfo(filePath);

            if (fileInfo.suffix() == "csv") {
                CsvModel *model = new CsvModel(this);
                model->loadFromFile(filePath);

                if (!model->isValid) {
                    QMessageBox::warning(this, tr("Invalid File"), tr("The CSV file is invalid."));
                    delete model;
                    event->ignore();
                    return;
                }

                createNewTableTab(filePath, model);
                event->acceptProposedAction();
            } else {
                QMessageBox::warning(this, tr("Unsupported File"),
                                     tr("Only .csv files are supported"));
                event->ignore();
            }
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
