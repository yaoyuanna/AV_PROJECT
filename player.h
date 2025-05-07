#ifndef PLAYER_H
#define PLAYER_H

#include <QThread>
#include<QDebug>
#include<QImage>
#include"m_queue.h"
#include "qwaitcondition.h"
#define AVCODEC_MAX_AUDIO_FRAME_SIZE    192000 //一秒钟音频帧最大值
#define SDL_AUDIO_BUFFER_SIZE           1024//
#define FLUSH_DATA "FLUSH"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libswresample/swresample.h>
#include "libavdevice/avdevice.h"
#include <libavutil/imgutils.h>
#include<libavutil/time.h>
//#include <SDL3/SDL.h>
#include<SDL.h>
}

class MainWindow;
class Player : public QThread
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
signals:
    void SIG_getainmage(QImage img);
    void SIG_settotaltime(qint64 total);
    void SIG_setcurtime(int time);
public:
    void run();
    void setFileName(const QString &newFileName);
public:
    MainWindow *p;
    QString m_fileName;
    AVFormatContext *format_ctx;//相当于视频”文件指针”
    ///////////////音频///////////////////////////////////
    AVStream*           audio_stream; //音频流
    m_queue<AVPacket>*  audio_que;//音频缓冲队列
    AVCodecContext*     audio_codec_ctx ;//音频解码器信息指针
    int                 audio_stream_index;//视频音频流索引
    SDL_AudioDeviceID   audio_devID; //音频 ID
    SDL_AudioSpec       wanted_spec; //SDL 音频设置
    SDL_AudioSpec       spec ; //SDL 音频设置
    AVFrame             out_frame; //设置参数，供音频解码后的swr_alloc_set_opts使用。
    SwsContext*         swr_ctx;
    AVFrame             wanted_frame;
    double              audio_clock;
    /// 音频回调函数使用的量
    uint8_t             audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int        audio_buf_size = 0;
    unsigned int        audio_buf_index = 0;
    AVFrame*            audioFrame;
    ///////////////视频///////////////////////////////////
    AVStream*           video_stream; //视频流
    m_queue<AVPacket>*  video_que;//视频队列
    AVCodecContext*     video_codec_ctx ;//音频解码器信息指针
    int                 video_stream_index;//视频音频流索引
    SDL_Thread*         video_tid;  //视频线程id
    bool                video_work;
    QWaitCondition      video_cond;
    QMutex              lock;
    double              video_clock;
    //////////播放控制//////////////////////////////////////
    int                 state;/*0为暂停，大于1则为倍速*/
    SDL_TimerID         timer_id;
    Uint32              time_last;
    SDL_TimerID         seek_timer_id;
    double              fps;
    bool                is_running;
    bool                ais_running;
    bool                vis_running;
    QImage              defaultimage;
    bool                is_seek;
    int                 seek;
    void    seek_to();
};
//视频线程
int video_thread(void *arg);
//错误打印函数
void print_error(const char *msg, int err);
//定时器回调函数
Uint32 SDLCALL timer_callback1(Uint32 interval, void *userdata);
Uint32 SDLCALL timer_callback(Uint32 interval, void *userdata);
void audio_callback(void *userdata, Uint8 *stream, int len);
//解码函数
int audio_decode_frame(Player* player, uint8_t *audio_buf, int buf_size);
//找 auto_stream
int find_stream_index(AVFormatContext *pformat_ctx, int *video_stream, int*audio_stream);
#endif // PLAYER_H
