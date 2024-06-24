#include "CsvModel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QBrush>

CsvModel::CsvModel(QObject *parent)
    : QAbstractTableModel(parent), isValid(false)
{
    headers = {"id", "name", "category", "quantity", "price", "voltage", "power_usage", "in_stock"};
}

void CsvModel::loadFromFile(const QString &filename)
{
    fileName = filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file" << filename;
        return;
    }

    QTextStream in(&file);
    bool is_headers = false;
    csvData.clear();

    while (!in.atEnd()) {
        QStringList line = in.readLine().split(";");
        if (line == headers) {is_headers = true;continue;}
        if (line.size() != headers.size()) {
            int excess = line.size() - headers.size();
            for (int i = 0; i < excess; ++i)
                line.removeLast();
            while (line.size() < headers.size())
                line.append("");
        }
        csvData.append(line);
    }

    if (is_headers) {isValid = true;}

    file.close();
    validateData();

    originalData = csvData;

    beginResetModel();
    endResetModel();
}

void CsvModel::saveToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file" << filename;
        return;
    }

    fileName = filename;

    QTextStream out(&file);
    out << headers.join(";") << "\n";
    for (const QStringList &row : csvData) {
        out << row.join(";") << "\n";
    }
    file.close();
}

void CsvModel::saveToFile(){

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file" << fileName;
        return;
    }

    QTextStream out(&file);
    out << headers.join(";") << "\n";
    for (const QStringList &row : csvData) {
        out << row.join(";") << "\n";
    }
    file.close();
}

bool CsvModel::addRow(const QStringList &rowData)
{
    if (rowData.size() != headers.size()) {
        return false;
    }

    beginInsertRows(QModelIndex(), csvData.size(), csvData.size());
    csvData.append(rowData);
    endInsertRows();
    return true;
}

bool CsvModel::removeRow(int row, const QModelIndex &parent)
{
    if (row < 0 || row >= csvData.size()) {
        return false;
    }

    beginRemoveRows(parent, row, row);
    csvData.removeAt(row);
    endRemoveRows();
    return true;
}

bool CsvModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        csvData[index.row()][index.column()] = value.toString();
        validateData();  // Переvalidate данные после изменения
        emit dataChanged(index, index, {role});
        originalData = csvData;
        return true;
    }
    return false;
}

QVariant CsvModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return csvData.at(index.row()).at(index.column());
    } else if (role == Qt::BackgroundRole) {
        QPair<int, int> cell(index.row(), index.column());
        if (invalidCells.contains(cell)) {
            return QBrush(Qt::red);  // Красим некорректные ячейки в красный
        }
    }
    return QVariant();
}

int CsvModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return csvData.size();
}

int CsvModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers.size();
}

QVariant CsvModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return headers.at(section);
    else
        return QString::number(section + 1);
}

Qt::ItemFlags CsvModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void CsvModel::validateData()
{
    invalidCells.clear();
    for (int row = 0; row < csvData.size(); ++row) {
        QStringList &line = csvData[row];

        if (line == headers) {continue;}

        bool ok;
        // Validate id
        line[0].toInt(&ok);
        if (!ok) {
            line[0] = "";
            invalidCells.insert(qMakePair(row, 0));
        }

        // Validate quantity
        line[3].toInt(&ok);
        if (!ok) {
            line[3] = "";
            invalidCells.insert(qMakePair(row, 3));
        }

        // Validate price
        line[4].toFloat(&ok);
        if (!ok) {
            line[4] = "";
            invalidCells.insert(qMakePair(row, 4));
        }

        // Validate voltage
        line[5].toInt(&ok);
        if (!ok) {
            line[5] = "";
            invalidCells.insert(qMakePair(row, 5));
        }

        // Validate power_usage
        line[6].toInt(&ok);
        if (!ok) {
            line[6] = "";
            invalidCells.insert(qMakePair(row, 6));
        }

        // Validate in_stock
        if (line[7] != "true" && line[7] != "false") {
            line[7] = "";
            invalidCells.insert(qMakePair(row, 7));
        }
    }
}

void CsvModel::initializeEmpty(QString &filename)
{
    fileName = filename;
    csvData.clear();

    QStringList emptyRow(headers.size(), "");
    csvData.append(emptyRow);

    isValid = true;

    beginResetModel();
    endResetModel();
}

void CsvModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();
    if (order == Qt::AscendingOrder) {
        std::sort(csvData.begin(), csvData.end(),
                  [column](const QStringList &a, const QStringList &b) {
                      return a[column] < b[column];
                  });
    } else {
        std::sort(csvData.begin(), csvData.end(),
                  [column](const QStringList &a, const QStringList &b) {
                      return a[column] > b[column];
                  });
    }
    endResetModel();
}

void CsvModel::setFilter(const QString &filter)
{
    csvData = originalData;

    if (filter.isEmpty()) {
        return;
    }

    beginResetModel();
    csvData.erase(
        std::remove_if(csvData.begin(), csvData.end(),
                       [filter](const QStringList &row) {
                           return std::none_of(row.begin(), row.end(),
                                               [&filter](const QString &cell) {
                                                   return cell.contains(filter, Qt::CaseInsensitive);
                                               });
                       }),
        csvData.end());
    endResetModel();
}

void CsvModel::clearFilter()
{
    beginResetModel();
    csvData = originalData;
    endResetModel();
}
