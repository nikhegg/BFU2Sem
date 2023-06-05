#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QBrush>
#include <QMessageBox>
#include <math.h>
#include <QImage>
#include <QMargins>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMessageBox::information(this, "Инструкция", "ЛКМ - поставить круглую клумбу\nПКМ - поставить квадратную клумбу\nЗажать ЛКМ и двигать - перемещение карты");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool contains(Circle c, QPoint p) {
    QPoint raz = c.center - p;
    return (QPoint::dotProduct(raz, raz) <= c.radius * c.radius);
}

bool isClumbaOk(Circle cir, QRect rect) {
    int anglesIn = 0;
    if(contains(cir, rect.topLeft())) anglesIn++;
    if(contains(cir, rect.topRight())) anglesIn++;
    if(contains(cir, rect.bottomLeft())) anglesIn++;
    if(contains(cir, rect.bottomRight())) anglesIn++;
    if(anglesIn == 4) return true;
    if(anglesIn !=  0) return false;
    return !rect.marginsAdded(QMargins(cir.radius, cir.radius, cir.radius, cir.radius)).contains(cir.center);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QBrush brush(Qt::green);

    // Draw grass
    QImage grass(":/src/textures/grass.jpg");
    brush.setTextureImage(grass);
    painter.setBrush(brush);
    painter.drawRect(0, 0, MainWindow::width(), MainWindow::height());

    // Draw water
    QImage water(":/src/textures/water.png");
    brush.setTextureImage(water);
    painter.setBrush(brush);
    QRect poolCopy = pool;
    poolCopy.setTopLeft(poolCopy.topLeft() + viewOffset);
    poolCopy.setBottomRight(poolCopy.bottomRight() + viewOffset);
    painter.drawRect(poolCopy);

    // Draw flowers
    QImage flowersTexture(":/src/textures/flower.png");
    brush.setTextureImage(flowersTexture);
    painter.setBrush(brush);
    for(int i = 0; i < circledFlowers.length(); i++) {
        painter.drawEllipse(circledFlowers[i].center + viewOffset, circledFlowers[i].radius, circledFlowers[i].radius);
    }
    for(int i = 0; i < squaredFlowers.length(); i++) {
        QRect flowerCopy = squaredFlowers[i];
        flowerCopy.moveCenter(squaredFlowers[i].center() + viewOffset);
        painter.drawRect(flowerCopy);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    moving = false;
    lastPos = event->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    moving = true;
    int speed = 3;
    if(lastPos.x() < event->x()) viewOffset.setX(viewOffset.x() + speed);
    else if(lastPos.x() > event->x()) viewOffset.setX(viewOffset.x() - speed);
    if(lastPos.y() < event->y()) viewOffset.setY(viewOffset.y() + speed);
    else if(lastPos.y() > event->y()) viewOffset.setY(viewOffset.y() - speed);
    lastPos = event->pos();
    repaint();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if(!moving) {
        if(event->button() & Qt::LeftButton) {
            Circle cir = {event->pos() - viewOffset, flowerRadius};

            // Check intersection with the pool
            if(pool.contains(event->pos() - viewOffset)) return;

            // Check intersections with other circled flowers
            for(Circle c : circledFlowers) {
               QPoint diff = cir.center - c.center - viewOffset;
               double length = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
               if(length < cir.radius + c.radius) return;
            }

            // Check intersections with all squared flowers
            for(QRect rect : squaredFlowers) if(!isClumbaOk(cir, rect)) return;

            circledFlowers.append(cir);
        } else if(event->button() & Qt::RightButton) {
            QRect flower(0,0, flowerRadius, flowerRadius);
            flower.moveCenter(event->pos() - viewOffset);

            // Check intersection with the pool
            if(pool.contains(flower.center())) return;

            // Check intersections with other squared flowers
            for(QRect square : squaredFlowers) if(square.intersects(flower)) return;

            // Check intersections with all circled flowers
            for(Circle c : circledFlowers) if(!isClumbaOk(c, flower)) return;

            // Everything is fine then
            squaredFlowers.append(flower);
        }
    }
    repaint();
}

