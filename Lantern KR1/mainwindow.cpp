#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QLineF>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QBrush brush(Qt::black);
    painter.setBrush(brush);
    QPen pen(Qt::black);

    if(start.x() >= 0 && end.x() >= 0)  {
        QPoint cont = end + (end-start);
        QLineF left(end, cont);
        left.setAngle(left.angle() - 15);
        QLineF right(end, cont);
        right.setAngle(right.angle() + 15);

        pen.setColor(Qt::yellow);
        painter.setPen(pen);
        painter.drawLine(left);
        painter.drawLine(right);

        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawLine(start, end);

    }
    if(start.x() >= 0) painter.drawEllipse(start, 2, 2);
    if(end.x() >= 0) painter.drawEllipse(end, 2, 2);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(start.x() < 0) start = event->pos();
    else if(end.x() < 0) end = event->pos();
    else {
        QString text = QString();

        QPoint cont = end + (end-start);
        QLineF ourVec(end, event->pos());
        double angle = ourVec.angleTo(QLineF(end, cont));
        if(angle <= 15 || 360 - angle <= 15) text += "Попал ";
        else text += "Не попал ";

        QPoint rel = end - start;
        QPoint vec = event->pos() - start;
        double sign = rel.x() * vec.y() - rel.y() * vec.x();
        if(sign < 0) text += "(слева)";
        else if(sign > 0) text += "(справа)";
        else text += "(на линии)";

        QMessageBox::information(this, "Проверка попадания", text);
    }
    repaint();
}
