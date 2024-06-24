#ifndef CSVMODEL_H
#define CSVMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include <QSet>
#include <QPair>

class CsvModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CsvModel(QObject *parent = nullptr);
    bool isValid;

    void loadFromFile(const QString &filename);
    void saveToFile(const QString &filename);
    void saveToFile();
    bool addRow(const QStringList &rowData);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role) const override; // одна реализация метода data
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void initializeEmpty(QString &filename);

private:
    QVector<QStringList> csvData;
    QStringList headers;
    void validateData();
    QString fileName;
    QSet<QPair<int, int>> invalidCells;  // Набор для хранения координат некорректных ячеек
};

#endif // CSVMODEL_H
