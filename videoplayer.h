// #ifndef VIDEOPLAYER_H
// #define VIDEOPLAYER_H

// extern "C"
// {
// #include "libavcodec/avcodec.h"
// #include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
// #include "libavdevice/avdevice.h"
// #include <libavutil/imgutils.h>
// #include<libavutil/time.h>
// #include <SDL3/SDL.h>
// }
// #include <QWaitCondition>
// #include"m_queue.h"
// #include<QDebug>
// #include <QThread>
// #include<QImage>
// class Player;
// class VideoPlayer : public QThread
// {
//     Q_OBJECT
// signals:
//     void SIG_getainmage(QImage img);
// public:
//     VideoPlayer(Player* player);
//     void run();
// public:
//     bool work;
//     QWaitCondition cond;
//     QMutex lock;
//     Player* player;
//     m_queue<AVPacket>apkque;
// };
// void print_error(const char *msg, int err);
// #endif // VIDEOPLAYER_H
