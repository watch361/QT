#ifndef MYDIAL_H
#define MYDIAL_H

#include <QWidget>
#include<QPainter>
class MyDial : public QWidget
{
    Q_OBJECT

public:
    MyDial(QWidget *parent = 0);
    ~MyDial();

    void setValue(double val);
    void settitle(QString val);
    void setunits(QString val);
    void setmaxValue(double val);
    void setminValue(double val);
protected:
    void paintEvent(QPaintEvent *);

    void drawCrown(QPainter *painter);
    void drawBackground(QPainter *painter);
    void drawScale(QPainter *painter);
    void drawScaleNum(QPainter *painter);
    void drawTitle(QPainter *painter);
    void drawIndicator(QPainter *painter);
    void drawNumericValue(QPainter *painter);

private:
    QColor m_background;
    QColor m_foreground;


    int m_maxValue;
    int m_minValue;
    int m_startAngle;
    int m_endAngle;

    int m_scaleMajor;
    int m_scaleMinor;
    double m_value;
    int m_precision;
    QTimer *m_updateTimer;
    QString m_units;
    QString m_title;

public Q_SLOTS:
    void UpdateAngle();

private:

};

#endif // MYDIAL_H
