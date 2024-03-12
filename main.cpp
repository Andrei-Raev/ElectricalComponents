#include "mainwindow.h"

#include <QApplication>
#include <QLocale>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ElectroControl_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }*/
    MainWindow w;
    // w.connect(ui->actionEnglish, &QAction::triggered, w, &on_actionEnglish_triggered);
    w.show();
    return a.exec();
}
