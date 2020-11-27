#include "mainwindow.h"

#include <QtWidgets>
#include <QPainter>

#include "joystick.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout* layout = new QGridLayout;
    joystick = new Joystick;
    layout->addWidget(joystick, 1, 0);

    setLayout(layout);
    setWindowTitle(tr("Joystick"));

}

MainWindow::~MainWindow()
{
}

