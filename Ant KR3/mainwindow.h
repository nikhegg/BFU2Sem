#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPoint>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    void calcRoute();
    int cellSize = 20;
    int treeRadius = 50;
    QPoint ant = QPoint(-1, -1);
    QPoint fly = QPoint(-1, -1);
    QPoint tree = QPoint(-treeRadius, -treeRadius);
    QVector<QPoint> route;
};
#endif // MAINWINDOW_H
