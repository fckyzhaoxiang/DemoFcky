#include "TimeDisplay.h"
#include "ui_qttimedisplay.h"
#include <QDebug>

QtTimeDisplay::QtTimeDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QtTimeDisplay)
{
    ui->setupUi(this);
    this->setWindowTitle("时间倒计时");
    //CalculationTime();
    fTimer = new QTimer(this);
    fTimer->start();
    fTimer->setInterval(1000);
    connect(fTimer, SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
}

QtTimeDisplay::~QtTimeDisplay()
{
    delete ui;
}

void QtTimeDisplay::on_timer_timeout()
{
    //当前时间
    curDateTime = QDateTime::currentDateTime();
    ui->DateTime->setText(curDateTime.toString("yyyy-MM-dd hh::mm::ss"));

    int second = QDateTime::fromString("2020-12-31 23::59::59","yyyy-MM-dd hh::mm::ss").toTime_t()
            - QDateTime::fromString(curDateTime.toString("yyyy-MM-dd hh::mm::ss"),"yyyy-MM-dd hh::mm::ss").toTime_t();
    int day = second /3600 /24;
    int hour = (second -day*3600*24)/3600;
    int min = (second - day*3600*24 - hour*3600) /60;
    int sec =  second - day*24*3600 - hour*3600 - min*60;
    int weeks = day / 7;
    QString CountDownStr = QString::number(day) + "天 " + QString::number(weeks) + "周 "
            + QString::number(hour) + "::" + QString::number(min) + "::" + QString::number(sec);
    ui->Countdown->setText(CountDownStr);
    qDebug() << "day = " << day << "  hour = " << hour << "min = " <<min << "sec = " << sec;
}

