#ifndef DIAGRAMMA_H
#define DIAGRAMMA_H

#include <QWidget>

namespace Ui {
class Diagramma;
}

class Diagramma : public QWidget
{
    Q_OBJECT

public:
    explicit Diagramma(QWidget *parent = nullptr);
    ~Diagramma();

private:
    Ui::Diagramma *ui;
};

#endif // DIAGRAMMA_H
