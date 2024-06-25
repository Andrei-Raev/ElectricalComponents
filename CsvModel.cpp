#include "CsvModel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QBrush>

/**
 * @brief Конструктор класса CsvModel.
 * @param parent Родительский объект.
 */
CsvModel::CsvModel(QObject *parent)
    : QAbstractTableModel(parent), isValid(false), autosave(false)
{
    headers = {"id", "name", "category", "quantity", "price", "voltage", "power_usage", "in_stock"};
}

/**
 * @brief Загружает данные из CSV файла.
 * @param filename Имя файла.
 */
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

/**
 * @brief Сохраняет данные в CSV файл.
 * @param filename Имя файла.
 */
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

/**
 * @brief Сохраняет данные в последний использованный CSV файл.
 */
void CsvModel::saveToFile()
{
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

/**
 * @brief Добавляет строку данных.
 * @param rowData Список данных для строки.
 * @return Возвращает true, если добавление прошло успешно.
 */
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

/**
 * @brief Удаляет строку данных.
 * @param row Номер строки для удаления.
 * @param parent Индекс родительского элемента (по умолчанию пустой).
 * @return Возвращает true, если удаление прошло успешно.
 */
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

/**
 * @brief Устанавливает значение в ячейке.
 * @param index Индекс ячейки.
 * @param value Новое значение.
 * @param role Роль данных (по умолчанию Qt::EditRole).
 * @return Возвращает true, если установка прошла успешно.
 */
bool CsvModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        csvData[index.row()][index.column()] = value.toString();
        validateData();  // Проверить данные после изменения
        emit dataChanged(index, index, {role});
        originalData = csvData;

        if (autosave) { saveToFile(); }

        return true;
    }
    return false;
}

/**
 * @brief Устанавливает состояние автоматического сохранения.
 * @param st_state Состояние автоматического сохранения.
 */
void CsvModel::setAutoSave(bool st_state)
{
    autosave = st_state;
}

/**
 * @brief Получает текущее состояние автоматического сохранения.
 * @return Возвращает true, если автоматическое сохранение включено.
 */
bool CsvModel::getAutoSave()
{
    return autosave;
}

/**
 * @brief Возвращает данные для определённой ячейки.
 * @param index Индекс ячейки.
 * @param role Роль данных.
 * @return Значение в ячейке в зависимости от роли.
 */
QVariant CsvModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return csvData.at(index.row()).at(index.column());
    } else if (role == Qt::BackgroundRole) {
        QPair<int, int> cell(index.row(), index.column());
        if (invalidCells.contains(cell)) {
            return QBrush(Qt::red);
        }
    }
    return QVariant();
}

/**
 * @brief Возвращает количество строк.
 * @param parent Индекс родительского элемента (по умолчанию пустой).
 * @return Количество строк.
 */
int CsvModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return csvData.size();
}

/**
 * @brief Возвращает количество столбцов.
 * @param parent Индекс родительского элемента (по умолчанию пустой).
 * @return Количество столбцов.
 */
int CsvModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers.size();
}

/**
 * @brief Возвращает данные заголовка.
 * @param section Секция заголовка.
 * @param orientation Ориентация заголовка.
 * @param role Роль данных.
 * @return Значение заголовка.
 */
QVariant CsvModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return headers.at(section);
    else
        return QString::number(section + 1);
}

/**
 * @brief Возвращает флаги элемента.
 * @param index Индекс элемента.
 * @return Флаги элемента.
 */
Qt::ItemFlags CsvModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/**
 * @brief Проверяет данные на корректность.
 */
void CsvModel::validateData()
{
    invalidCells.clear();
    for (int row = 0; row < csvData.size(); ++row) {
        QStringList &line = csvData[row];

        if (line == headers) { continue; }

        bool ok;
        // Проверить id
        line[0].toInt(&ok);
        if (!ok) {
            line[0] = "";
            invalidCells.insert(qMakePair(row, 0));
        }

        if (line[1] == "") {
            invalidCells.insert(qMakePair(row, 1));
        }

        if (line[2] == "") {
            invalidCells.insert(qMakePair(row, 2));
        }

        // Проверить quantity
        line[3].toInt(&ok);
        if (!ok) {
            line[3] = "";
            invalidCells.insert(qMakePair(row, 3));
        }

        // Проверить price
        line[4].toFloat(&ok);
        if (!ok) {
            invalidCells.insert(qMakePair(row, 4));
        }

        // Проверить voltage
        line[5].toInt(&ok);
        if (!ok) {
            line[5] = "";
            invalidCells.insert(qMakePair(row, 5));
        }

        // Проверить power_usage
        line[6].toInt(&ok);
        if (!ok) {
            line[6] = "";
            invalidCells.insert(qMakePair(row, 6));
        }

        // Проверить in_stock
        if (line[7] != "true" && line[7] != "false") {
            line[7] = "";
            invalidCells.insert(qMakePair(row, 7));
        }
    }
}

/**
 * @brief Инициализирует пустой файл.
 * @param filename Имя файла.
 */
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

/**
 * @brief Сортирует данные по определенной колонке.
 * @param column Номер колонки.
 * @param order Порядок сортировки.
 */
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

/**
 * @brief Устанавливает фильтр для данных.
 * @param filter Строка фильтра.
 */
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

/**
 * @brief Очищает установленный фильтр.
 */
void CsvModel::clearFilter()
{
    beginResetModel();
    csvData = originalData;
    endResetModel();
}
