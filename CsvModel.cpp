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
    // isValid = true;
    invalidCells.clear();
    for (int row = 0; row < csvData.size(); ++row) {
        QStringList &line = csvData[row];

        if (line == headers) {continue;}

        bool ok;
        // Validate id
        line[0].toInt(&ok);
        if (!ok) {
            line[0] = "";
            // isValid = false;
            invalidCells.insert(qMakePair(row, 0));

            qDebug() << line[0] << "id";
        }

        // Validate quantity
        line[3].toInt(&ok);
        if (!ok) {
            line[3] = "";
            // isValid = false;
            invalidCells.insert(qMakePair(row, 3));

            qDebug() << line[3] << "qua";
        }

        // Validate price
        line[4].toFloat(&ok);
        if (!ok) {
            line[4] = "";
            // isValid = false;
            invalidCells.insert(qMakePair(row, 4));
        }

        // Validate voltage
        line[5].toInt(&ok);
        if (!ok) {
            line[5] = "";
            // isValid = false;
            invalidCells.insert(qMakePair(row, 5));

            qDebug() << line[5] << "volt";
        }

        // Validate power_usage
        line[6].toInt(&ok);
        if (!ok) {
            line[6] = "";
            // isValid = false;
            invalidCells.insert(qMakePair(row, 6));

            qDebug() << line[6] << "pu";
        }

        // Validate in_stock
        if (line[7] != "true" && line[7] != "false") {
            line[7] = "";
            // isValid = false;
            invalidCells.insert(qMakePair(row, 7));

            qDebug() << line[7] << "is_s";
        }
    }
}

// Добавим метод для инициализации пустыми данными
void CsvModel::initializeEmpty(QString &filename)
{
    fileName = filename;
    csvData.clear();

    // Инициализация пустыми строками, добавим одну строку с пустыми значениями
    QStringList emptyRow(headers.size(), "");
    csvData.append(emptyRow);

    // Устанавливаем валидность данных, так как таблица пустая, считаем её валидной
    isValid = true;

    // Обновляем модель
    beginResetModel();
    endResetModel();
}
