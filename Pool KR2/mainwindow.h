#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QVector>
#include <QRect>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Circle {
    QPoint center;
    int radius;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    QRect pool = QRect(100, 100, 500, 300);
    QVector<Circle> circledFlowers;
    QVector<QRect> squaredFlowers;
    int flowerRadius = 15;
    QPoint viewOffset;
    bool moving;
    QPoint lastPos;
};
#endif // MAINWINDOW_H
