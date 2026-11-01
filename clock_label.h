#ifndef CLOCK_LABEL_H
#define CLOCK_LABEL_H

#include <QLabel>

class ClockLabel : public QLabel
{
    Q_OBJECT
public:
    ClockLabel(QWidget* p = nullptr);

private slots:
    void updateTime();
};

#endif // CLOCK_LABEL_H
