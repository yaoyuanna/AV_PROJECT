#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"player.h"
void MainWindow::SLO_getaimage(QImage img)
{
    // // 获取 QLabel 的大小
    // QSize labelSize = ui->lb_show->size();
    // // 获取图片的大小
    // QSize imageSize = img.size();

    // // 计算水平和垂直方向的缩放比例
    // qreal scaleX = (qreal)labelSize.width() / imageSize.width();
    // qreal scaleY = (qreal)labelSize.height() / imageSize.height();

    // // 选取较小的缩放比例，以保证等比例缩放且不超出 QLabel
    // qreal scale = qMin(scaleX, scaleY);

    // // 根据计算出的缩放比例对图片进行缩放，保持宽高比
    // QImage scaledImage = img.scaled(imageSize * scale, Qt::KeepAspectRatio);

    // // 将缩放后的 QImage 转换为 QPixmap
    // QPixmap pixmap = QPixmap::fromImage(scaledImage);

    // // 设置 QLabel 的对齐方式为居中对齐
    // ui->lb_show->setAlignment(Qt::AlignCenter);
    // // 关闭 QLabel 自动缩放内容的功能
    // ui->lb_show->setScaledContents(false);
    // // 在 QLabel 上显示缩放后的图片
    // ui->lb_show->setPixmap(pixmap);
    ui->widget_show->slot_setImage(img);
}

void MainWindow::SLO_settotaltime(qint64 total)
{
    int second=(int)total/1000000;
    ui->horizontalSlider->setRange(0,second);
    QString h=QString("00%1").arg(second/(60*60));
    QString m=QString("00%1").arg(second/60);
    QString s=QString("00%1").arg(second%60);
    QString totaltime=QString("%1:%2:%3").arg(h.right(2), m.right(2), s.right(2));
    ui->lb_timeend->setText(totaltime);
}

void MainWindow::SLO_setcurtime(int clock)
{
    QString h=QString("00%1").arg(clock/(60*60));
    QString m=QString("00%1").arg(clock/60);
    QString s=QString("00%1").arg(clock%60);
    QString time=QString("%1:%2:%3").arg(h.right(2), m.right(2), s.right(2));
    ui->horizontalSlider->setValue(clock);
    ui->lb_timecurrunt->setText(time);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),m_player(this)
{
    ui->setupUi(this);
    connect(&m_player,&Player::SIG_getainmage,this,&MainWindow::SLO_getaimage);
    connect(&m_player,&Player::SIG_settotaltime,this,&MainWindow::SLO_settotaltime);
    connect(&m_player,&Player::SIG_setcurtime,this,&MainWindow::SLO_setcurtime);
    ui->horizontalSlider->installEventFilter(this);
    //m_player.setFileName("C:\\Users\\86139\\Videos\\Captures\\1111.mp4");
    //m_player.setFileName("C:\\Users\\86139\\Videos\\4k测试短片\\4k测试短片.mp4");
    //m_player.setFileName("https://upos-sz-mirrorhw.bilivideo.com/upgcxcode/63/19/29611963/29611963_p1-1-208.mp4?e=ig8euxZM2rNcNbNj7zdVhwdlhbTahwdVhoNvNC8BqJIzNbfqXBvEuENvNC8aNEVEtEvE9IMvXBvE2ENvNCImNEVEIj0Y2J_aug859r1qXg8gNEVE5XREto8z5JZC2X2gkX5L5F1eTX1jkXlsTXHeux_f2o859IMvNC8xNbLEkF6MuwLStj8fqJ0EkX1ftx7Sqr_aio8_&ua=tvproj&uipk=5&nbs=1&deadline=1746688150&gen=playurlv2&os=bcache&oi=1879385489&trid=000027c71889dd464511b681a7135e81adb8T&mid=0&og=hw&upsig=80849005a9bf6d1e3e739c4ab337a2b8&uparams=e,ua,uipk,nbs,deadline,gen,os,oi,trid,mid,og&cdnid=10214&bvc=vod&nettype=0&bw=267954&orderid=0,3&buvid=&build=2040100&mobi_app=android_i&f=T_0_0&logo=80000000");
    m_player.setFileName("rtmp://192.168.74.131:1935/vod//1000008923.mp4");
    //m_player.setFileName("C:\\Users\\86139\\Desktop\\Video_70636308449980.mp4");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->horizontalSlider)
    {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            int min = ui->horizontalSlider->minimum();
            int max = ui->horizontalSlider->maximum();
            int value = QStyle::sliderValueFromPosition(min, max, mouseEvent->pos().x(), ui->horizontalSlider->width());
            SDL_RemoveTimer(m_player.seek_timer_id);
            ui->horizontalSlider->setValue(value);
            m_player.seek=value; //value 秒
            m_player.is_seek=true;
            qDebug()<<"seek:"<<value;
            m_player.seek_timer_id = SDL_AddTimer(1000,&timer_callback1,&m_player);
            return true;
        }
        else
        {
            return false;
        }
    }
    // pass the event on to the parent class
    return MainWindow::eventFilter(obj, event);
}

void MainWindow::on_pushButton_clicked()
{
    qDebug()<<m_player.is_running;
    if(!m_player.is_running){
        m_player.start();
    }else{
        if(m_player.state==0){
            m_player.state=1;
            m_player.timer_id = SDL_AddTimer((1/ m_player.fps)*1000,&timer_callback,(void*)&m_player);
            m_player.seek_timer_id = SDL_AddTimer(1000,&timer_callback1,(void*)&m_player);
        }else{
            m_player.state=0;
        }
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    m_player.is_running=false;
    SDL_Delay(1000);
    m_player.start();
}

