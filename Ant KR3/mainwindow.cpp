#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QRect>
#include <QPoint>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QDebug>
#include <QMessageBox>
#include <QVector>
#include <QMap>
#include <math.h>

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

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QBrush brush(Qt::black);
    QPen pen(Qt::black);
    for(int i = 0; i < MainWindow::height(); i+=cellSize) {
        for(int j = 0; j < MainWindow::width(); j+=cellSize) {
            painter.drawRect(QRect(j, i, cellSize, cellSize));
        }
    }

    if(fly.x() >= 0) {
        brush.setColor(QColor(112,59,33));
        painter.setBrush(brush);
        painter.drawEllipse(fly, 5, 5);
    }
    if(ant.x() >= 0) {
        brush.setColor(QColor(242,131,75));
        painter.setBrush(brush);
        painter.drawEllipse(ant, 5, 5);
    }
    if(tree.x() >= 0) {
        brush.setColor(QColor(112,59,33));
        painter.setBrush(brush);
        painter.drawEllipse(tree, treeRadius, treeRadius);

        // Текстура пня
        int drawRadius = treeRadius - 5;
        for(int i = 0; drawRadius > 0; drawRadius -= 3) {
            if(i%2 == 0) brush.setColor(QColor(207,191,147));
            else brush.setColor(QColor(202, 187, 142));
            painter.setBrush(brush);
            painter.drawEllipse(tree, drawRadius, drawRadius);
            i++;
        }

    }
    if(route.length() == 0) return;
    pen.setColor(Qt::blue);
    pen.setWidth(3);
    painter.setPen(pen);
    for(int i = 0; i < route.length() - 1; i++) painter.drawLine(route[i], route[i+1]);
}

QPoint approximate(QPoint toApprox, int cellSize) {
    int xIters = 0, yIters = 0;
    while(toApprox.x() > cellSize) {
        toApprox -= QPoint(cellSize, 0);
        xIters++;
    }
    while(toApprox.y() > cellSize) {
        toApprox -= QPoint(0, cellSize);
        yIters++;
    }
    if(toApprox.x() >= cellSize/2) toApprox.setX(cellSize);
    else toApprox.setX(0);
    if(toApprox.y() >= cellSize/2) toApprox.setY(cellSize);
    else toApprox.setY(0);
    toApprox += QPoint(xIters * cellSize, yIters * cellSize);
    return toApprox;
}

bool isInTree(QPoint animal, QPoint tree, int treeSize) {
    if(tree.x() < 0) return false;
    QPoint prod = tree - animal;
    return (QPoint::dotProduct(prod, prod) <= treeSize * treeSize);
}

void MainWindow::calcRoute() {
    if(ant.x() < 0 || fly.x() < 0) return;
    route.clear();
    route.append(ant);
    QPoint mover = ant;

    // Take ant out of bounds of tree
    QRect treeSquare(tree, QSize(treeRadius*2, treeRadius*2));
    treeSquare.moveCenter(tree);
    if(treeSquare.contains(ant)) {
        QPoint hExitDirection, vExitDirection;
        QPoint altMover = ant;
        hExitDirection = (mover.x() < tree.x()) ? (QPoint(-20, 0)) : (QPoint(20, 0));
        vExitDirection = (mover.y() < tree.y()) ? (QPoint(0, -20)) : (QPoint(0, 20));

        // Try to exit X first
        while(treeSquare.contains(mover)) mover += hExitDirection;
        // Then check Y exit
        while(treeSquare.contains(altMover)) altMover += vExitDirection;

        QPoint exit;
        // If altMover is closer to fly then it's better
        int horDistance = QPoint::dotProduct(fly-mover, fly-mover);
        int verDistance = QPoint::dotProduct(fly-altMover, fly-altMover);
        exit = hExitDirection;
        if(horDistance > verDistance) {
            mover = altMover;
            exit = vExitDirection;
        } else exit = hExitDirection;

        // Restore path & add it to route
        altMover = ant;
        while(altMover != mover) {
            altMover += exit;
            route.append(altMover);
        }
    }

    int debug = 0;
    while(mover != fly) {
        if(debug >= 1000) return;
        QPoint vertical, horizontal;
        if(mover.x() < fly.x()) horizontal = QPoint(cellSize, 0);
        else if(mover.x() > fly.x()) horizontal = QPoint(-cellSize, 0);
        if(mover.y() < fly.y()) vertical = QPoint(0, cellSize);
        else if(mover.y() > fly.y()) vertical = QPoint(0, -cellSize);

        // If it's previous direction then ignore this idea
        if(route.length() >= 2 && route[route.length()-2] == mover + horizontal) horizontal = QPoint(0, 0);
        if(route.length() >= 2 && route[route.length()-2] ==  mover + vertical) vertical = QPoint(0, 0);

        // Check if any of these ways intersects a tree
        if(isInTree(mover + horizontal, tree, treeRadius)) horizontal = QPoint(0, 0);
        if(isInTree(mover+vertical, tree, treeRadius)) vertical = QPoint(0, 0);

        // Find best solution for moving
        if(vertical.isNull()) mover += horizontal;
        else if(horizontal.isNull()) mover += vertical;
        else if((fly - vertical).manhattanLength() > (fly-horizontal).manhattanLength()) mover += vertical;
        else mover += horizontal;
        debug++;
        route.append(mover);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if(Qt::ShiftModifier & event->modifiers()) { // Place tree
        QPoint potentialTree = event->pos();
        tree = potentialTree;
        if(!ant.isNull() && isInTree(ant, potentialTree, treeRadius)) {
            QMessageBox::information(this, "Муравей погиб", "Поздравляем! Вы убили муравья. Вселенная не может существовать без него");
            MainWindow::close();
        } if(!fly.isNull() && isInTree(fly, potentialTree, treeRadius)) {
            QMessageBox::information(this, "Муха умерла", "Вы придавили муху деревом. Потеря не велика, но муравей расстроен. Теперь он не хочет никуда идти");
            MainWindow::close();
        }
    } else if(event->button() & Qt::LeftButton) { // Place ant
        QPoint potentialAnt = approximate(event->pos(), cellSize);
        if(!tree.isNull() && isInTree(potentialAnt, tree, treeRadius)) return;
        ant = potentialAnt;
    } else if(event->button() & Qt::RightButton) { // Place fly
        QPoint potentialFly = approximate(event->pos(), cellSize);
        if(!tree.isNull() && isInTree(potentialFly, tree, treeRadius)) return;
        fly = potentialFly;
    }
    calcRoute();
    repaint();
}
