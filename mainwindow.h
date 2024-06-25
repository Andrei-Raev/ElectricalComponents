/**
 * @file mainwindow.h
 * @brief Определение класса MainWindow, отвечающего за основной интерфейс программы.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTranslator>
#include <QTableView>
#include "CsvModel.h"

#include <QDragEnterEvent>
#include <QDropEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Класс MainWindow, отвечающий за управление основным окном приложения.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса MainWindow.
     * @param parent Родительский виджет.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор класса MainWindow.
     */
    ~MainWindow();

    /**
     * @brief Переключение интерфейса на русский язык.
     */
    void on_actionRussian_triggered();

    /**
     * @brief Переключение интерфейса на английский язык.
     */
    void on_actionEnglish_triggered();

protected:
    /**
     * @brief Событие перетаскивания в окно.
     * @param event Указатель на событие перетаскивания.
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * @brief Событие сброса файла в окно.
     * @param event Указатель на событие сброса.
     */
    void dropEvent(QDropEvent *event) override;

private:
    QTranslator translator;
    QMessageBox msgBox;
    Ui::MainWindow *ui;

    /**
     * @brief Настройка интерфейса пользователя.
     */
    void UiSetUp();

    /**
     * @brief Открытие окна статистики.
     */
    void openStatisticWindow();

    /**
     * @brief Открытие файла.
     */
    void openFile();

    /**
     * @brief Сохранение файла.
     */
    void saveFile();

    /**
     * @brief Создать новый файл.
     */
    void newFile();

    /**
     * @brief Закрыть файл.
     */
    void closeFile();

    /**
     * @brief Быстрое сохранение файла.
     */
    void quickSaveFile();

    /**
     * @brief Удаление выбранной строки.
     */
    void removeSelectedRow();

    /**
     * @brief Добавить строку.
     */
    void addRow();

    /**
     * @brief Событие контекстного меню.
     * @param event Указатель на событие контекстного меню.
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

    /**
     * @brief Сохранение действия.
     */
    void saveAction();

    /**
     * @brief Открытие действия.
     */
    void openAction();

    /**
     * @brief Новое действие.
     */
    void newAction();

    /**
     * @brief Создание новой вкладки с таблицей.
     * @param filePath Путь к файлу.
     * @param model Указатель на модель CSV.
     * @return Указатель на QTableView.
     */
    QTableView* createNewTableTab(const QString &filePath, CsvModel *model);

    void saveSettings();
    void loadSettings();

    void changeAutoSave();
    void applyAutoSave(bool state);

private slots:
    /**
     * @brief Обработка нажатия на заголовок столбца.
     * @param logicalIndex Логический индекс столбца.
     */
    void handleHeaderSectionClicked(int logicalIndex);

    /**
     * @brief Обработка действия поиска.
     */
    void handleFindAction();

    /**
     * @brief Обработка действия "Escape".
     */
    void handleEscape();
};

#endif // MAINWINDOW_H
