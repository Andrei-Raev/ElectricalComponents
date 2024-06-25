// diagramma.h

#ifndef DIAGRAMMA_H
#define DIAGRAMMA_H

#include <QWidget>
#include <QtCharts>
#include <QTableView>

QT_BEGIN_NAMESPACE
namespace Ui {
class Diagramma;
}
QT_END_NAMESPACE

/**
 * @brief Класс для отображения и создания диаграмм.
 */
class Diagramma : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса Diagramma.
     * @param parent Указатель на родительский виджет.
     */
    explicit Diagramma(QWidget *parent = nullptr);

    /**
     * @brief Деструктор класса Diagramma.
     */
    ~Diagramma();

    /**
     * @brief Создает диаграммы на основе данных из представления таблицы.
     * @param currentView Указатель на текущее представление таблицы.
     */
    void createCharts(QTableView *currentView);

private:
    Ui::Diagramma *ui;

    /**
     * @brief Создает диаграмму зависимости цены от мощности.
     * @param currentView Указатель на текущее представление таблицы.
     */
    void createPriceFromPowerChart(QTableView *currentView);

    /**
     * @brief Создает диаграмму зависимости мощности от напряжения.
     * @param currentView Указатель на текущее представление таблицы.
     */
    void createPowerFromVoltageChart(QTableView *currentView);

    /**
     * @brief Создает диаграмму количества компонентов в каждой категории.
     * @param currentView Указатель на текущее представление таблицы.
     */
    void createComponentsInCategoryChart(QTableView *currentView);

    /**
     * @brief Создает диаграмму наличия компонентов на складе.
     * @param currentView Указатель на текущее представление таблицы.
     */
    void createComponentsInStockChart(QTableView *currentView);
};

#endif // DIAGRAMMA_H
