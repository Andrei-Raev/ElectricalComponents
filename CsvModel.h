#ifndef CSVMODEL_H
#define CSVMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>
#include <QPair>

/**
 * @brief Класс для работы с CSV файлами в качестве модели данных для QTableView.
 */
class CsvModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    bool isValid;
    /**
     * @brief Конструктор класса CsvModel.
     * @param parent Родительский объект.
     */
    explicit CsvModel(QObject *parent = nullptr);

    /**
     * @brief Загружает данные из CSV файла.
     * @param filename Имя файла.
     */
    void loadFromFile(const QString &filename);

    /**
     * @brief Сохраняет данные в CSV файл.
     * @param filename Имя файла.
     */
    void saveToFile(const QString &filename);

    /**
     * @brief Сохраняет данные в последний использованный CSV файл.
     */
    void saveToFile();

    /**
     * @brief Добавляет строку данных.
     * @param rowData Список данных для строки.
     * @return Возвращает true, если добавление прошло успешно.
     */
    bool addRow(const QStringList &rowData);

    /**
     * @brief Удаляет строку данных.
     * @param row Номер строки для удаления.
     * @param parent Индекс родительского элемента (по умолчанию пустой).
     * @return Возвращает true, если удаление прошло успешно.
     */
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    /**
     * @brief Устанавливает значение в ячейке.
     * @param index Индекс ячейки.
     * @param value Новое значение.
     * @param role Роль данных (по умолчанию Qt::EditRole).
     * @return Возвращает true, если установка прошла успешно.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief Проверяет данные на корректность.
     */
    void validateData();

    /**
     * @brief Инициализирует пустой файл.
     * @param filename Имя файла.
     */
    void initializeEmpty(QString &filename);

    /**
     * @brief Сортирует данные по определенной колонке.
     * @param column Номер колонки.
     * @param order Порядок сортировки.
     */
    void sort(int column, Qt::SortOrder order) override;

    /**
     * @brief Устанавливает фильтр для данных.
     * @param filter Строка фильтра.
     */
    void setFilter(const QString &filter);

    /**
     * @brief Очищает установленный фильтр.
     */
    void clearFilter();

    /**
     * @brief Устанавливает автоматическое сохранение.
     * @param st_state Состояние автоматического сохранения.
     */
    void setAutoSave(bool st_state);

    /**
     * @brief Получает текущее состояние автоматического сохранения.
     * @return Возвращает true, если автоматическое сохранение включено.
     */
    bool getAutoSave();

private:
    QStringList headers;
    QList<QStringList> csvData;
    QString fileName;
    bool autosave;

    QSet<QPair<int, int>> invalidCells;
    QString currentFilter;

    QList<QStringList> originalData;
};

#endif // CSVMODEL_H
