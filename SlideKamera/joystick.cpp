#include "joystick.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QPainter>		// QPainter
#include <QtDebug>		// qDebug()
#include <QtMath>		// qPow, qSqrt

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

Joystick::Joystick(QWidget *parent) : QWidget(parent),
                                      recenterAnimation{new QPropertyAnimation(this, "centerParam")}
{
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    recenterAnimation->setStartValue(1.0f);
    recenterAnimation->setEndValue(.0f);
    recenterAnimation->setDuration(250);
    recenterAnimation->setEasingCurve(QEasingCurve::InCirc);
}

QSize Joystick::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Joystick::sizeHint() const
{
    return QSize(200, 200);
}

bool Joystick::pressedInKnobBounds(const QPointF& pressPoint) const
{
    return qSqrt(qPow(pressPoint.x() - knobBounds.center().rx(), 2) +
                (qPow(pressPoint.y() - knobBounds.center().ry(), 2))) <= knobRadius();
}

void Joystick::mousePressEvent(QMouseEvent *event)
{
    if (pressedInKnobBounds(event->pos())) {
        recenterAnimation->stop();			// joystick grabbed while recentering
        pressed = true;
    }
}

void Joystick::recenterKnob(float val)
{
    knobBounds.moveCenter(QPointF(joystickBounds.center().rx() + (knobBounds.center().rx() - joystickBounds.center().rx())*val,
                                  joystickBounds.center().ry() + (knobBounds.center().ry() - joystickBounds.center().ry())*val));
    update();
}

QPointF Joystick::setPosition(QPointF mousePos)
{
    float H = qAbs(mousePos.rx() - joystickBounds.center().rx())/qAbs(mousePos.ry() - joystickBounds.center().ry());
    float newX = clamp(mousePos.x(),
                    -H*joystickRadius()/qSqrt(1 + qPow(H, 2)) + joystickBounds.center().rx(),
                     H*joystickRadius()/qSqrt(1 + qPow(H, 2)) + joystickBounds.center().rx());

    float newY = clamp(mousePos.y(),
                    -joystickRadius()/qSqrt(1 + qPow(H, 2)) + joystickBounds.center().ry(),
                     joystickRadius()/qSqrt(1 + qPow(H, 2)) + joystickBounds.center().ry());
    return QPointF(newX, newY);
}

void Joystick::mouseMoveEvent(QMouseEvent *event)
{
    if (!pressed) return;
    auto pos = setPosition(event->pos());
    knobBounds.moveCenter(pos);

    update();
}

void Joystick::mouseReleaseEvent(QMouseEvent*)
{
    if (pressed) {
       pressed = false;
       recenterAnimation->start();
    }
}

void Joystick::resizeEvent(QResizeEvent *event)
{
    float windowMinSize = qMin(event->size().width(), event->size().height());

    joystickBounds.setWidth(windowMinSize*joystickRatio);
    joystickBounds.setHeight(windowMinSize*joystickRatio);
    joystickBounds.moveCenter(QPointF(event->size().width()/2, event->size().height()/2));

    knobBounds.setWidth(windowMinSize*knobRatio);
    knobBounds.setHeight(windowMinSize*knobRatio);
    knobBounds.moveCenter(QPointF(event->size().width()/2, event->size().height()/2));
}

void Joystick::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QBrush brush(Qt::blue, Qt::BrushStyle::SolidPattern);
    painter.setBrush(brush);
    painter.drawEllipse(joystickBounds);

    QColor red = Qt::red;
    red.setAlphaF(0.7);
    brush.setColor(red);
    painter.setBrush(brush);
    painter.drawEllipse(knobBounds);
}

