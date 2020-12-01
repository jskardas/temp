#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPropertyAnimation;
QT_END_NAMESPACE

class Joystick : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float x 			 READ x 			 WRITE setX)
    Q_PROPERTY(float y 		 	 READ y 			 WRITE setY)
    Q_PROPERTY(float maxXVal 	 READ maxX 	 		 WRITE setXMax)
    Q_PROPERTY(float maxYVal     READ maxY 			 WRITE setXMax)
    Q_PROPERTY(float centerParam READ getCenterParam WRITE recenterKnob)

public:

    explicit Joystick(QWidget *parent = nullptr);

    int32_t   x()  const { return xVal; }
    int32_t& rx()        { return xVal; }

    int32_t   y()  const { return yVal; }
    int32_t& ry() 	  	 { return yVal; }

    int32_t maxX() const { return maxXVal; }
    int32_t maxY() const { return maxYVal; }

    float getCenterParam() const { return centerParam; }

    float knobRadius() 	   const { return knobBounds.width()/2; }
    float joystickRadius() const { return joystickBounds.width()/2; }

signals:
    void xChanged(float);
    void yChanged(float);

public slots:
    void setX(const float);
    void setY(const float);
    void setXMax(const int32_t);
    void setYMax(const int32_t);
    void recenterKnob(const float);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    bool pressedInKnobBounds(const QPointF&) const;
    bool pressedInJoystickBounds(const QPointF&) const;
    QPointF calculateMovedKnobCenter(QPointF);

    int32_t xVal;
    int32_t yVal;

    int32_t maxXVal;
    int32_t maxYVal;

    QRectF joystickBounds;
    QRectF knobBounds;

    QPointF prevMousePos;

private:
    bool pressed;

    const float knobSizeRatio = 0.1f;
    const float joystickSizeRatio = 0.5f;

    float arrowWidth;
    float arrowHeight;

    float centerParam;

    QPropertyAnimation* recenterAnimation;
};

#endif // JOYSTICK_H
