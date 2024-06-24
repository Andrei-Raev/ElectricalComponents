#include "diagramma.h"
#include "ui_diagramma.h"
#include "CsvModel.h"

Diagramma::Diagramma(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Diagramma)
{
    ui->setupUi(this);
}

Diagramma::~Diagramma()
{
    delete ui;
}

void Diagramma::createCharts(QTableView *currentView) {
    createPriceFromPowerChart(currentView);
    createPowerFromVoltageChart(currentView);
    createComponentsInCategoryChart(currentView);
    createComponentsInStockChart(currentView);
}

void Diagramma::createPriceFromPowerChart(QTableView *currentView) {
    CsvModel *model = qobject_cast<CsvModel*>(currentView->model());

    QScatterSeries *series = new QScatterSeries();
    for (int row = 0; row < model->rowCount(); ++row) {
        bool ok;
        float price = model->data(model->index(row, 4)).toFloat(&ok);
        int power = model->data(model->index(row, 6)).toInt(&ok);
        if (ok) {
            series->append(power, price);
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Price from Power");
    chart->createDefaultAxes();

    QChartView *chartView = new QChartView(chart);

    QVBoxLayout *layout = findChild<QVBoxLayout*>("price_power_layout");
    if(layout) {
        layout->addWidget(chartView);
    }
}

void Diagramma::createPowerFromVoltageChart(QTableView *currentView) {
    CsvModel *model = qobject_cast<CsvModel*>(currentView->model());

    QScatterSeries *series = new QScatterSeries();
    for (int row = 0; row < model->rowCount(); ++row) {
        bool ok;
        int voltage = model->data(model->index(row, 5)).toInt(&ok);
        int power = model->data(model->index(row, 6)).toInt(&ok);
        if (ok) {
            series->append(voltage, power);
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Power from Voltage");
    chart->createDefaultAxes();

    // Логика для создания трендовой линии (например, используя метод наименьших квадратов, если вставите)
    // QLineSeries *trendLine = new QLineSeries();
    // chart->addSeries(trendLine);

    QChartView *chartView = new QChartView(chart);

    QVBoxLayout *layout = findChild<QVBoxLayout*>("power_voltage_layout");
    if(layout) {
        layout->addWidget(chartView);
    }
}

void Diagramma::createComponentsInCategoryChart(QTableView *currentView) {
    CsvModel *model = qobject_cast<CsvModel*>(currentView->model());

    QBarSeries *series = new QBarSeries();
    QMap<QString, int> categoryCounts;
    for (int row = 0; row < model->rowCount(); ++row) {
        QString category = model->data(model->index(row, 2)).toString();
        categoryCounts[category]++;
    }

    for (auto it = categoryCounts.begin(); it != categoryCounts.end(); ++it) {
        QBarSet *set = new QBarSet(it.key());
        *set << it.value();
        series->append(set);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Number of Components in Each Category");
    chart->createDefaultAxes();

    QChartView *chartView = new QChartView(chart);

    QVBoxLayout *layout = findChild<QVBoxLayout*>("components_in_category_layout");
    if(layout) {
        layout->addWidget(chartView);
    }
}

void Diagramma::createComponentsInStockChart(QTableView *currentView) {
    CsvModel *model = qobject_cast<CsvModel*>(currentView->model());

    int inStockCount = 0, totalCount = model->rowCount();
    for (int row = 0; row < model->rowCount(); ++row) {
        QString inStock = model->data(model->index(row, 7)).toString();
        if (inStock == "true") {
            inStockCount++;
        }
    }

    QPieSeries *series = new QPieSeries();
    series->append("In Stock", inStockCount);
    series->append("Out of Stock", totalCount - inStockCount);

    QPieSlice *sliceInStock = series->slices().at(0);
    sliceInStock->setLabel(sliceInStock->label() + QStringLiteral(": %1%").arg(100 * sliceInStock->value() / totalCount, 0, 'f', 1));

    QPieSlice *sliceOutOfStock = series->slices().at(1);
    sliceOutOfStock->setLabel(sliceOutOfStock->label() + QStringLiteral(": %1%").arg(100 * sliceOutOfStock->value() / totalCount, 0, 'f', 1));

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Percent of Components in Stock");

    QChartView *chartView = new QChartView(chart);

    QVBoxLayout *layout = findChild<QVBoxLayout*>("is_in_stock_layout");
    if(layout) {
        layout->addWidget(chartView);
    }
}
