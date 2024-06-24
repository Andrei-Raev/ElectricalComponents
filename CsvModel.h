#ifndef CSVMODEL_H
#define CSVMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>
#include <QPair>

class CsvModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    bool isValid;
    explicit CsvModel(QObject *parent = nullptr);

    void loadFromFile(const QString &filename);
    void saveToFile(const QString &filename);
    void saveToFile();
    bool addRow(const QStringList &rowData);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void validateData();

    void initializeEmpty(QString &filename);

    void sort(int column, Qt::SortOrder order) override;
    void setFilter(const QString &filter);
    void clearFilter();

private:
    QStringList headers;
    QList<QStringList> csvData;
    QString fileName;

    QSet<QPair<int, int>> invalidCells;
    QString currentFilter;

    QList<QStringList> originalData;

};

#endif // CSVMODEL_H
