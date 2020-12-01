#include "mainwindow.h"

#include <QtWidgets>
#include <QPainter>

#include "joystick.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Joystick"));
    setStyleSheet("background-color: rgb(51, 51, 51)");
    this->setMinimumSize(QSize(400, 400));

    QGridLayout* layout = new QGridLayout;

    joystick = new Joystick;
    createSpinBoxes();
    spinBoxesGroup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );

    layout->addWidget(joystick, 0, 1);
    layout->addWidget(spinBoxesGroup, 1, 1, Qt::AlignHCenter);
    setLayout(layout);
}

void MainWindow::createSpinBoxes()
{
    spinBoxesGroup = new QGroupBox;
    spinBoxesGroup->setMaximumSize(QSize(this->width()/2, this->height()/5));
    spinBoxesGroup->setStyleSheet(" border: none;"
                                  " background-color: gray;"
                                  " border-radius: 5px; "
                                  " color: rgb(64, 64, 64);");

    maxXLabel = new QLabel(tr("max X"));
    maxYLabel = new QLabel(tr("max Y"));

    maxXSpinBox = new QSpinBox;
    maxXSpinBox->setRange(0, 1000);
    maxXSpinBox->setValue(50);
    maxXSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    maxXSpinBox->setStyleSheet("border: none; background-color: white;");
    maxXSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    maxYSpinBox = new QSpinBox;
    maxYSpinBox->setRange(0, 1000);
    maxYSpinBox->setValue(50);
    maxYSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    maxYSpinBox->setStyleSheet("border: none; background-color: white;");
    maxYSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QFont font("Arial", spinBoxesGroup->height()*0.2);
    maxXLabel->setFont(font);
    maxYLabel->setFont(font);
    maxXSpinBox->setFont(font);
    maxYSpinBox->setFont(font);

    QGridLayout *spinBoxLayout = new QGridLayout;
    spinBoxLayout->addWidget(maxXLabel, 0, 0);
    spinBoxLayout->addWidget(maxYLabel, 1, 0);
    spinBoxLayout->addWidget(maxXSpinBox, 0, 1);
    spinBoxLayout->addWidget(maxYSpinBox, 1, 1);
    spinBoxesGroup->setLayout(spinBoxLayout);

    connect(maxXSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), joystick, &Joystick::setXMax);
    connect(maxYSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), joystick, &Joystick::setYMax);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    spinBoxesGroup->setMaximumSize(QSize(event->size().width()/2, event->size().height()/5));

    QFont font("Lato", spinBoxesGroup->height()*0.2);
    maxXLabel->setFont(font);
    maxYLabel->setFont(font);
    maxXSpinBox->setFont(font);
    maxYSpinBox->setFont(font);
}

MainWindow::~MainWindow()
{
}

