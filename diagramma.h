#ifndef DIAGRAMMA_H
#define DIAGRAMMA_H

#include <QWidget>
#include <QtCharts>
#include <QTableView>

// Включает пространство имен для удобства

QT_BEGIN_NAMESPACE
namespace Ui {
    class Diagramma;
}
QT_END_NAMESPACE

class Diagramma : public QWidget
{
    Q_OBJECT

public:
    explicit Diagramma(QWidget *parent = nullptr);
    ~Diagramma();

    void createCharts(QTableView *currentView);

private:
    Ui::Diagramma *ui;

    void createPriceFromPowerChart(QTableView *currentView);
    void createPowerFromVoltageChart(QTableView *currentView);
    void createComponentsInCategoryChart(QTableView *currentView);
    void createComponentsInStockChart(QTableView *currentView);
};

#endif // DIAGRAMMA_H
