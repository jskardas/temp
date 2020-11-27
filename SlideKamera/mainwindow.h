#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class Joystick;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Joystick* joystick;
};
#endif // MAINWINDOW_H
