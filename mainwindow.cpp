#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"player.h"
void MainWindow::SLO_getaimage(QImage img)
{
    QPixmap pixmax=QPixmap::fromImage(img.scaled(ui->lb_show->size(),Qt::KeepAspectRatio));
    ui->lb_show->setPixmap(pixmax);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),m_player()
{
    ui->setupUi(this);
    connect(&m_player,&Player::SIG_getainmage,this,&MainWindow::SLO_getaimage);
    m_player.setFileName("C:\\Users\\86139\\Videos\\Captures\\魔法少女与恶曾是敌人。 第01集-4K动漫 和另外 1 个页面 - 个人 - Microsoft​ Edge 2024-08-20 16-07-36.mp4");
    //m_player.setFileName("C:\\Users\\86139\\Desktop\\video.mp4");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug()<<m_player.is_running;
    if(!m_player.is_running){
        m_player.start();
    }else{
        if(m_player.state==0){
            m_player.state=1;
            //m_player.timer_id = SDL_AddTimer((1/ m_player.fps)*2000,&timer_callback,(void*)&m_player);
        }else{
            m_player.state=0;
        }
    }
}

