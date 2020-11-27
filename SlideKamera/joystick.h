#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPropertyAnimation;
QT_END_NAMESPACE

class Joystick : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float centerParam READ getCenterParam WRITE recenterKnob)

public:
    explicit Joystick(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint()		const override;

    float getCenterParam() const { return centerParam; }

    float knobRadius() 	 const { return knobBounds.width()/2; }
    float knobDiamater() const { return knobBounds.width(); }

    float joystickRadius()   const { return joystickBounds.width()/2; }
    float joystickDiameter() const { return joystickBounds.width(); }


public slots:
    void recenterKnob(float val);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool pressedInKnobBounds(const QPointF&) const;
    QPointF setPosition(QPointF dist);

    QPropertyAnimation* recenterAnimation;

    QRectF joystickBounds;
    QRectF knobBounds;

    float centerParam;
    bool pressed;
    const float knobRatio = 0.1f;
    const float joystickRatio = 0.5f;
};

#endif // JOYSTICK_H
