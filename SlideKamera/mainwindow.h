#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>

class Joystick;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createSpinBoxes();

    Joystick* joystick;
    QGroupBox* spinBoxesGroup;

    QLabel* maxXLabel;
    QLabel* maxYLabel;

    QSpinBox* maxXSpinBox;
    QSpinBox* maxYSpinBox;

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:
    void xMaxChanged(float);
    void yMaxChanged(float);
};
#endif // MAINWINDOW_H
