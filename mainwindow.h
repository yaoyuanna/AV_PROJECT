#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"player.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void SLO_getaimage(QImage img);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Player m_player;
};
#endif // MAINWINDOW_H
