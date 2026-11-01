#include "clock_label.h"
#include <QTimer>
#include <QTime>

ClockLabel::ClockLabel(QWidget* p)
    : QLabel(p)
{
    QTimer* tmr = new QTimer(this);
    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime()));
    tmr->start();

    updateTime();
}

void ClockLabel::updateTime()
{
    QTime t = QTime::currentTime();
    setText(t.toString("hh:mm:ss AP"));
}
