#ifndef TIMEDISPLAY_H
#define TIMEDISPLAY_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>

namespace Ui {
class QtTimeDisplay;
}

class QtTimeDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit QtTimeDisplay(QWidget *parent = 0);
    ~QtTimeDisplay();

private slots:
    void on_timer_timeout();
private:
    Ui::QtTimeDisplay *ui;

    QTimer *fTimer;
    QDateTime curDateTime;
};

#endif // TIMEDISPLAY_H
