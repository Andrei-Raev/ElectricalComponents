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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void on_actionRussian_triggered();
    void on_actionEnglish_triggered();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QTranslator translator;
    QMessageBox msgBox;
    Ui::MainWindow *ui;

    void UiSetUp();
    void openFile();
    void saveFile();
    void newFile();

    void saveAction();
    void openAction();
    void newAction();

    QTableView* createNewTableTab(const QString &filePath, CsvModel *model);
};
#endif // MAINWINDOW_H
