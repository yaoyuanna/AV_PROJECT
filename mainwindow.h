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
    void SLO_settotaltime(qint64 total);
    void SLO_setcurtime(int clock);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Player m_player;
};
#endif // MAINWINDOW_H
