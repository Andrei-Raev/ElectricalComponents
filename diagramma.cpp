#include "diagramma.h"
#include "ui_diagramma.h"

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
