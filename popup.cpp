#include "popup.h"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

PopUp::PopUp(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint |        // Disable window decoration
                   Qt::Tool |                       // Discard display in a separate window
                   Qt::WindowStaysOnTopHint);       // Set on top of all windows
    setAttribute(Qt::WA_TranslucentBackground);     // Indicates that the background will be transparent
    setAttribute(Qt::WA_ShowWithoutActivating);     // At the show, the widget does not get the focus automatically

    animation.setTargetObject(this);                // Set the target animation
    animation.setPropertyName("popupOpacity");      //

    /*add auxiliary animation >>>>>>>>>>>>>>>>*/
    m_animation_moveup.setTargetObject(this);
    m_animation_moveup.setPropertyName("popupMoveup");
    connect(this, &PopUp::s_moved, this, &PopUp::onMoved);
    /*<<<<<<<<<<<<<<<<<<<<<<< add auxiliary animation*/

    connect(&animation, &QAbstractAnimation::finished, this, &PopUp::hide);

    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label.setStyleSheet("QLabel { color : white; "
                        "margin-top: 6px;"
                        "margin-bottom: 6px;"
                        "margin-left: 10px;"
                        "margin-right: 10px; }");

    layout.addWidget(&label, 0, 0);
    setLayout(&layout);

    timer = new QTimer();
//    connect(timer, &QTimer::timeout, this, &PopUp::hideAnimation);
    connect(timer, &QTimer::timeout, this, &PopUp::deleteLater);
}

void PopUp::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(QColor(0,0,0,180)));
    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(roundedRect, 10, 10);
}

void PopUp::setPopupText(const QString &text)
{
    label.setText(text);    // Set the text in the Label
    adjustSize();           // With the recalculation notice sizes
}

void PopUp::show()
{
    setWindowOpacity(0.0);  // Set the transparency to zero

    animation.setDuration(1000);     // Configuring the duration of the animation
    animation.setStartValue(0.0);   // The start value is 0 (fully transparent widget)
    animation.setEndValue(1.0);     // End - completely opaque widget

    setGeometry(QApplication::desktop()->availableGeometry().width() - 36 - width() + QApplication::desktop() -> availableGeometry().x(),
                QApplication::desktop()->availableGeometry().height() - 36 - height() + QApplication::desktop() -> availableGeometry().y(),
                width(),
                height());
    QWidget::show();

    animation.start();
    timer->start(20000);
}

void PopUp::moveUp(const int x)
{
//    const auto movementAnimation = new QPropertyAnimation(this, "pos");
//    movementAnimation->setEndValue(QPoint(pos().x(), pos().y() - x));
    m_animation_moveup.setStartValue(pos().y());
    m_animation_moveup.setEndValue(pos().y() - x);
    m_animation_moveup.setDuration(3000);
    m_animation_moveup.setEasingCurve(QEasingCurve::InOutQuad);
    m_animation_moveup.start();
//    m_animation_moveup.setStartValue(pos());
//    m_animation_moveup.setEndValue(QPoint(pos().x(), pos().y() - x));
//    m_animation_moveup.setDuration(3000);
//    m_animation_moveup.setEasingCurve(QEasingCurve::InOutQuad);
}

void PopUp::hideAnimation()
{
    timer->stop();
    animation.setDuration(1000);
    animation.setStartValue(1.0);
    animation.setEndValue(0.0);
    animation.start();
}

void PopUp::hide()
{
    // If the widget is transparent, then hide it
    if(getPopupOpacity() == 0.0){
        QWidget::hide();
    }
}

void PopUp::setPopupOpacity(float opacity)
{
    popupOpacity = opacity;

    setWindowOpacity(opacity);
}

float PopUp::getPopupOpacity() const
{
    return popupOpacity;
}
