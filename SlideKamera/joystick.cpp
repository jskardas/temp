#include "joystick.h"

#include <QtDebug>
#include <QtMath>
#include <QMouseEvent>
#include <QPainter>
#include <QPair>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

template<typename T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

Joystick::Joystick(QWidget *parent) : QWidget(parent),
                                      xVal{0}, yVal{0}, maxXVal{50}, maxYVal{50},
                                      recenterAnimation{new QPropertyAnimation(this, "centerParam")}
{
    connect(this, &Joystick::xChanged, this, &Joystick::setX);
    connect(this, &Joystick::yChanged, this, &Joystick::setY);

    recenterAnimation->setStartValue(1.0f);
    recenterAnimation->setEndValue(.0f);
    recenterAnimation->setDuration(250);
    recenterAnimation->setEasingCurve(QEasingCurve::InCirc);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Joystick::setX(const float val)
{
    xVal = (val - joystickBounds.center().x())*(maxXVal/(joystickRadius() - knobRadius()));
}

void Joystick::setY(const float val)
{
    yVal = (joystickBounds.center().y() - val)*(maxYVal/(joystickRadius() - knobRadius()));
}

void Joystick::setXMax(const int32_t val)
{
    maxXVal = val;
}

void Joystick::setYMax(const int32_t val)
{
    maxYVal = val;
}

bool Joystick::pressedInKnobBounds(const QPointF& pressPoint) const
{
    return qSqrt(qPow(pressPoint.x() - knobBounds.center().x(), 2) +
                (qPow(pressPoint.y() - knobBounds.center().y(), 2))) <= knobRadius();
}

bool Joystick::pressedInJoystickBounds(const QPointF& pressPoint) const
{
    return qSqrt(qPow(pressPoint.x() - joystickBounds.center().x(), 2) +
                (qPow(pressPoint.y() - joystickBounds.center().y(), 2))) <= joystickRadius();
}


void Joystick::recenterKnob(const float val)
{
    knobBounds.moveCenter(QPointF(joystickBounds.center().x() + (knobBounds.center().x() - joystickBounds.center().x())*val,
                                  joystickBounds.center().y() + (knobBounds.center().y() - joystickBounds.center().y())*val));
    update();
}

QPointF Joystick::calculateMovedKnobCenter(QPointF mousePos)
{
    QPointF newKnobCenter;
    QPointF mouseToJoystickCenterDiff = mousePos - joystickBounds.center();

    float radiusDiff = joystickRadius() - knobRadius();

    if (pressedInJoystickBounds(mousePos)) {
        QPointF knobCenterToJoystickCenterDiff = knobBounds.center() - joystickBounds.center();
        QPair<qreal, qreal> knobCenterToJoystickCenterDist {
                                   qSqrt(qPow(radiusDiff, 2) - qPow(knobCenterToJoystickCenterDiff.x(), 2)),
                                   qSqrt(qPow(radiusDiff, 2) - qPow(knobCenterToJoystickCenterDiff.y(), 2))};

        newKnobCenter.rx() = clamp(knobBounds.center().x() + (mousePos.x() - prevMousePos.x()),				// current knob position + mouse displacement
                                   joystickBounds.center().x() - knobCenterToJoystickCenterDist.second,
                                   joystickBounds.center().x() + knobCenterToJoystickCenterDist.second);
        newKnobCenter.ry() = clamp(knobBounds.center().y() + (mousePos.y() - prevMousePos.y()),
                                   joystickBounds.center().y() - knobCenterToJoystickCenterDist.first,
                                   joystickBounds.center().y() + knobCenterToJoystickCenterDist.first);
    } else {
        float mouseToJoystickCentreDist = qSqrt(qPow(mouseToJoystickCenterDiff.x(), 2) + qPow(mouseToJoystickCenterDiff.y(), 2));
        newKnobCenter.rx() = clamp(mousePos.x(),
                                   -radiusDiff*qAbs(mouseToJoystickCenterDiff.x())/mouseToJoystickCentreDist + joystickBounds.center().x(),
                                    radiusDiff*qAbs(mouseToJoystickCenterDiff.x())/mouseToJoystickCentreDist + joystickBounds.center().x());
        newKnobCenter.ry() = clamp(mousePos.y(),
                                   -radiusDiff*qAbs(mouseToJoystickCenterDiff.y())/mouseToJoystickCentreDist + joystickBounds.center().y(),
                                    radiusDiff*qAbs(mouseToJoystickCenterDiff.y())/mouseToJoystickCentreDist + joystickBounds.center().y());
    }

    if (mousePos.x() != newKnobCenter.x())
        emit xChanged(newKnobCenter.x());
    if (mousePos.y() != newKnobCenter.y())
        emit yChanged(newKnobCenter.y());

    qDebug("(%+4d, %+4d)", this->x(), this->y());

    return newKnobCenter;
}

void Joystick::mousePressEvent(QMouseEvent *event)
{
    if (pressedInKnobBounds(event->pos())) {
        recenterAnimation->stop();			// joystick grabbed while recentering
        pressed = true;
        prevMousePos = event->pos();
    }
}

void Joystick::mouseReleaseEvent(QMouseEvent*)
{
    if (pressed) {
        pressed = false;
        recenterAnimation->start();
    }
}

void Joystick::mouseMoveEvent(QMouseEvent *event)
{
    if (!pressed) return;
    knobBounds.moveCenter(calculateMovedKnobCenter(event->pos()));
    prevMousePos = event->pos();

    update();
}

void Joystick::resizeEvent(QResizeEvent *event)
{
    float windowMinSize = qMin(event->size().width(), event->size().height());

    joystickBounds.setWidth(windowMinSize*joystickSizeRatio);
    joystickBounds.setHeight(windowMinSize*joystickSizeRatio);
    joystickBounds.moveCenter(QPointF(event->size().width()/2, event->size().height()/2));

    knobBounds.setWidth(windowMinSize*knobSizeRatio);
    knobBounds.setHeight(windowMinSize*knobSizeRatio);
    knobBounds.moveCenter(QPointF(event->size().width()/2, event->size().height()/2));

    arrowWidth = windowMinSize*0.04;
    arrowHeight = arrowWidth*0.4;

    prevMousePos = knobBounds.center();
}

void Joystick::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor lightGreen (126, 195, 82);
    QColor lightGray (64, 64, 64);

    /* arrows */
    QBrush arrowBrush(Qt::BrushStyle::SolidPattern);
    arrowBrush.setColor(lightGreen);
    painter.setBrush(arrowBrush);
    painter.setPen(Qt::NoPen);

    QPolygon arrow;
    arrow << QPoint(joystickBounds.center().x(), joystickBounds.top() - 2*arrowHeight)
          << QPoint(joystickBounds.center().x() - arrowWidth/2, joystickBounds.top() - arrowHeight/2)
          << QPoint(joystickBounds.center().x() + arrowWidth/2, joystickBounds.top() - arrowHeight/2);

    QTransform transform = QTransform().translate(joystickBounds.center().x(),joystickBounds.center().y())
                                       .rotate(90)
                                       .translate(-joystickBounds.center().x(), -joystickBounds.center().y());
    for (int i = 0; i != 4; ++i) {
        painter.drawPolygon(arrow);
        arrow = transform.map(arrow);
    }

    /* crosshair */
    QPen crosshairPen;
    crosshairPen.setColor(lightGray);
    crosshairPen.setWidth(knobBounds.width()*0.04);
    painter.setBrush(Qt::NoBrush);

    // lines
    painter.setPen(crosshairPen);
    painter.drawLine(QPointF(joystickBounds.left(), joystickBounds.center().y()), QPointF(joystickBounds.right(), joystickBounds.center().y()));
    painter.drawLine(QPointF(joystickBounds.center().x(), joystickBounds.bottom()), QPointF(joystickBounds.center().x(), joystickBounds.top()));

    // outer circle
    painter.drawEllipse(joystickBounds);

    // middle circle
    crosshairPen.setWidth(knobBounds.width()*0.03);
    painter.setPen(crosshairPen);
    painter.drawEllipse(QRectF(QPointF(joystickBounds.topLeft().x() + joystickRadius()/4, joystickBounds.topLeft().y() + joystickRadius()/4),
                               QPointF(joystickBounds.bottomRight().x() - joystickRadius()/4, joystickBounds.bottomRight().y() - joystickRadius()/4)));
    // inner circle
    crosshairPen.setWidth(knobBounds.width()*0.02);
    painter.setPen(crosshairPen);
    painter.drawEllipse(QRectF(QPointF(joystickBounds.topLeft().x() + joystickRadius()/2, joystickBounds.topLeft().y() + joystickRadius()/2),
                               QPointF(joystickBounds.bottomRight().x() - joystickRadius()/2, joystickBounds.bottomRight().y() - joystickRadius()/2)));

    /* knob */
    QBrush knobBrush(Qt::BrushStyle::SolidPattern);
    knobBrush.setColor(Qt::gray);
    painter.setBrush(knobBrush);
    painter.setPen(QPen(QBrush(lightGreen), knobBounds.width()* 0.02));
    painter.drawEllipse(knobBounds);
}
