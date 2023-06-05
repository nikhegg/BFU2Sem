#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>

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
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    QPoint start = QPoint(-10, -10);
    QPoint end = QPoint(-10, -10);

};
#endif // MAINWINDOW_H
