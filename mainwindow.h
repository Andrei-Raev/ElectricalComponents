#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QMessageBox>

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

private:
    QTranslator translator;
    QMessageBox msgBox;
    Ui::MainWindow *ui;
    void UiSetUp();
    void openFile(QTabWidget* tabWidget);
    QVector<QStringList> readCSVTable(const QString& filename);
};
#endif // MAINWINDOW_H
