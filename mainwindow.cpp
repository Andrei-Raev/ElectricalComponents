/**
 * @file mainwindow.cpp
 * @brief Реализация методов класса MainWindow.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "diagramma.h"
#include <QFileDialog>
#include <QMimeData>
#include <QFileInfo>
#include <QDebug>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QShortcut>
#include <QInputDialog>
#include <QHeaderView>

/**
 * @brief Конструктор класса MainWindow.
 * @param parent Родительский виджет.
 */
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
    loadSettings();
}

/**
 * @brief Настройка интерфейса пользователя.
 */
void MainWindow::UiSetUp()
{
    connect(ui->actionEnglish, &QAction::triggered, this, [&]() { on_actionEnglish_triggered(); });
    connect(ui->actionRussian, &QAction::triggered, this, [&]() { on_actionRussian_triggered(); });
    connect(ui->actionOpen, &QAction::triggered, this, [&]() { openFile(); });
    connect(ui->actionSave, &QAction::triggered, this, [&]() { saveFile(); });
    connect(ui->actionNew, &QAction::triggered, this, [&]() { newFile(); });
    connect(ui->actionClose, &QAction::triggered, this, [&]() { closeFile(); });

    QShortcut *shortcutSave = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcutSave, &QShortcut::activated, this, &MainWindow::saveFile);
    QShortcut *shortcutOpen = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(shortcutOpen, &QShortcut::activated, this, &MainWindow::openFile);
    QShortcut *shortcutNew = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(shortcutNew, &QShortcut::activated, this, &MainWindow::newFile);
    QShortcut *shortcutDelete = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcutDelete, &QShortcut::activated, this, &MainWindow::removeSelectedRow);
    QShortcut *shortcutFind = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(shortcutFind, &QShortcut::activated, this, &MainWindow::handleFindAction);
    QShortcut *shortcutEscape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(shortcutEscape, &QShortcut::activated, this, &MainWindow::handleEscape);

    connect(ui->actionFind, &QAction::triggered, this, [&]() { handleFindAction(); });
    connect(ui->actionExit_Find, &QAction::triggered, this, [&]() { handleEscape(); });
    connect(ui->actionAddRow, &QAction::triggered, this, [&]() { addRow(); });
    connect(ui->actionStatistics_open, &QAction::triggered, this, [&]() { openStatisticWindow(); });
    connect(ui->actionRemoveRow, &QAction::triggered, this, [&]() { removeSelectedRow(); });
    connect(ui->actionAbout_author, &QAction::triggered, this, [&]() {
        msgBox.setText(tr("Author:\nRaev Andrei Sergeevich, IZTMS 2-3."));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    });



    if (ui->tabWidget->widget(0)->objectName() == QString("empty")) {
        ui->tabWidget->removeTab(0);
    }
}

/**
 * @brief Переключение интерфейса на русский язык.
 */
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

/**
 * @brief Переключение интерфейса на английский язык.
 */
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

/**
 * @brief Открытие окна статистики.
 */
void MainWindow::openStatisticWindow()
    {
        QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());

        if (!currentView) {
            QMessageBox::warning(this, tr("No Table Selected"), tr("Please select a table to perform the action."));
            return;
        }
        Diagramma *diagrammaWindow = new Diagramma();
        diagrammaWindow->createCharts(currentView);
        diagrammaWindow->show();
    }



/**
 * @brief Обработка нажатия на заголовок столбца.
 * @param logicalIndex Логический индекс столбца.
 */
void MainWindow::handleHeaderSectionClicked(int logicalIndex)
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (!currentView) {
        QMessageBox::warning(this, tr("No Table Selected"), tr("Please select a table to perform the action."));
        return;
    }

    CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
    if (model) {
        Qt::SortOrder currentOrder = currentView->horizontalHeader()->sortIndicatorOrder();
        model->sort(logicalIndex, currentOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder);
        currentView->horizontalHeader()->setSortIndicator(logicalIndex, currentOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder);
    }
}

/**
 * @brief Событие контекстного меню.
 * @param event Указатель на событие контекстного меню.
 */
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction actionRemoveRow("Удалить строку", this);
    connect(&actionRemoveRow, &QAction::triggered, this, &MainWindow::removeSelectedRow);
    contextMenu.addAction(&actionRemoveRow);

    QAction actionAddRow("Добавить строку", this);
    connect(&actionAddRow, &QAction::triggered, this, &MainWindow::addRow);
    contextMenu.addAction(&actionAddRow);

    contextMenu.exec(event->globalPos());
}

/**
 * @brief Обработка действия поиска.
 */
void MainWindow::handleFindAction()
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (!currentView) {
        QMessageBox::warning(this, tr("No Table Selected"), tr("Please select a table to search in."));
        return;
    }

    bool ok;
    QString searchText = QInputDialog::getText(this, tr("Find"),
                                               tr("Enter search term:"),
                                               QLineEdit::Normal,
                                               QString(), &ok);

    if (ok && !searchText.isEmpty()) {
        CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
        if (model) {
            model->setFilter(searchText);
        }
    }

    ui->actionExit_Find->setEnabled(true);
}

/**
 * @brief Обработка действия "Escape".
 */
void MainWindow::handleEscape()
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (currentView) {
        CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
        if (model) {
            model->clearFilter();
        }
    }

    ui->actionExit_Find->setEnabled(false);
}

void MainWindow::changeAutoSave()
{
    bool state = ui->actionAuto_saving->isChecked();
    applyAutoSave(state);
}

void MainWindow::applyAutoSave(bool state)
{
    int tabCount = ui->tabWidget->count();

    if (tabCount == 0) {
        QMessageBox::information(this, tr("No Models Opened"), tr("There are no models open to apply the auto-save setting."));
        return;
    }

    for (int i = 0; i < tabCount; ++i) {
        QTableView* view = qobject_cast<QTableView*>(ui->tabWidget->widget(i));
        if (view) {
            CsvModel* model = qobject_cast<CsvModel*>(view->model());
            if (model) {
                model->setAutoSave(state);
            }
        }
    }
}


/**
 * @brief Удаление выбранной строки.
 */
void MainWindow::removeSelectedRow()
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (currentView) {
        QItemSelectionModel *select = currentView->selectionModel();
        CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
        if (model) {
            QModelIndexList selectedRows = select->selectedRows();
            if (!selectedRows.isEmpty()) {
                model->removeRow(selectedRows.first().row());
            } else {
                QMessageBox::warning(this, tr("No Row Selected"), tr("Please select a row to remove."));
            }
        }
    }
}

/**
 * @brief Добавить строку.
 */
void MainWindow::addRow()
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (currentView) {
        CsvModel *model = qobject_cast<CsvModel*>(currentView->model());
        if (model) {
            model->addRow(QStringList(model->columnCount(), ""));
        }
    }
}

/**
 * @brief Открытие файла.
 */
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

/**
 * @brief Сохранение файла.
 */
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

/**
 * @brief Быстрое сохранение файла.
 */
void MainWindow::quickSaveFile()
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

    model->saveToFile();
}

/**
 * @brief Создание нового файла.
 */
void MainWindow::newFile()
{
    CsvModel *model = new CsvModel(this);
    QString fileName = "Untitled.csv";
    model->initializeEmpty(fileName);
    createNewTableTab(fileName, model);
}

/**
 * @brief Закрытие файла.
 */
void MainWindow::closeFile()
{
    QTableView *currentView = qobject_cast<QTableView*>(ui->tabWidget->currentWidget());
    if (!currentView) {
        QMessageBox::warning(this, tr("No Tab Selected"), tr("Please select a tab to close."));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Save File"), tr("Do you want to save changes before closing?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        quickSaveFile();
        int currentIndex = ui->tabWidget->currentIndex();
        ui->tabWidget->removeTab(currentIndex);
    } else if (reply == QMessageBox::No) {
        int currentIndex = ui->tabWidget->currentIndex();
        ui->tabWidget->removeTab(currentIndex);
    }
}

/**
 * @brief Создание новой вкладки с таблицей.
 * @param filePath Путь к файлу.
 * @param model Указатель на модель CSV.
 * @return Указатель на QTableView.
 */
QTableView* MainWindow::createNewTableTab(const QString &filePath, CsvModel *model)
{
    QTableView *tableView = new QTableView;
    tableView->setModel(model);

    tableView->setSortingEnabled(true);
    tableView->horizontalHeader()->setSortIndicatorShown(true);

    QString tabName = QFileInfo(filePath).fileName();
    ui->tabWidget->addTab(tableView, tabName);
    ui->tabWidget->setCurrentWidget(tableView);

    model->setAutoSave(ui->actionAuto_saving->isChecked());

    return tableView;
}

/**
 * @brief Событие перетаскивания в окно.
 * @param event Указатель на событие перетаскивания.
 */
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

/**
 * @brief Событие сброса файла в окно.
 * @param event Указатель на событие сброса.
 */
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

void MainWindow::saveSettings()
{
    QSettings settings("Nerrow inc.", "ElectroControl");

    settings.setValue("autoSaving", ui->actionAuto_saving->isChecked());
    settings.setValue("english", ui->actionEnglish->isChecked());
    settings.setValue("russian", ui->actionRussian->isChecked());
}

void MainWindow::loadSettings()
{
    QSettings settings("Nerrow inc.", "ElectroControl");

    ui->actionAuto_saving->setChecked(settings.value("autoSaving", true).toBool());

    ui->actionEnglish->setChecked(settings.value("english", false).toBool());
    ui->actionRussian->setChecked(settings.value("russian", true).toBool());

    if (settings.value("english", false).toBool()) {
        qApp->removeTranslator(&translator);
        ui->retranslateUi(this);
    }
}

/**
 * @brief Деструктор класса MainWindow.
 */
MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}
