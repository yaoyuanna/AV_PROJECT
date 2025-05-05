#ifndef PLAYER_H
#define PLAYER_H

// #include "videoplayer.h"
// #include "audioplayer.h"
#include <QThread>
#include<QDebug>
#include<QImage>
#include"m_queue.h"
#include "qwaitcondition.h"
#define AVCODEC_MAX_AUDIO_FRAME_SIZE    192000
#define SDL_AUDIO_BUFFER_SIZE           1024
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include <libavutil/imgutils.h>
#include<libavutil/time.h>
//#include <SDL3/SDL.h>
#include<SDL.h>
}


// typedef struct VideoState {
//     AVFormatContext *format_ctx;//相当于视频”文件指针”
//     ///////////////音频///////////////////////////////////
//     AVStream*           audio_stream; //音频流
//     m_queue<AVPacket>*  audio_que;//音频缓冲队列
//     AVCodecContext*     audio_codec_ctx ;//音频解码器信息指针
//     int                 audio_stream_index;//视频音频流索引
//     double              audio_clock; ///<pts of last decoded frame 音频时钟
//     SDL_AudioDeviceID   audioID; //音频 ID
//     AVFrame             out_frame; //设置参数，供音频解码后的swr_alloc_set_opts使用。
//     /// 音频回调函数使用的量
//     uint8_t             audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
//     unsigned int        audio_buf_size = 0;
//     unsigned int        audio_buf_index = 0;
//     AVFrame*            audioFrame;
//     //////////////////////////////////////////////////////
//     ///////////////视频///////////////////////////////////
//     AVStream*           video_stream; //视频流
//     m_queue<AVPacket>*  video_que;//视频队列
//     AVCodecContext*     video_codec_ctx ;//音频解码器信息指针
//     int                 video_stream_index;//视频音频流索引
//     double              video_clock; ///<pts of last decoded frame 视频时钟
//     SDL_Thread*         video_tid;  //视频线程id
//     /////////////////////////////////////////////////////
//     //////////播放控制//////////////////////////////////////
//     int                 quit;
//     //////////////////////////////////////////////////////
//     int64_t             start_time; //单位 微秒
//     VideoState()
//     {
//         audio_clock = video_clock  = start_time = 0;
//     }
//     //VideoPlayer* m_player;//用于调用函数
// } VideoState;



class Player : public QThread
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
signals:
    void SIG_getainmage(QImage img);
public:
    void run();
    void setFileName(const QString &newFileName);
public:
    QString m_fileName;
    AVFormatContext *format_ctx;//相当于视频”文件指针”
    ///////////////音频///////////////////////////////////
    AVStream*           audio_stream; //音频流
    m_queue<AVPacket>*  audio_que;//音频缓冲队列
    AVCodecContext*     audio_codec_ctx ;//音频解码器信息指针
    int                 audio_stream_index;//视频音频流索引
    SDL_AudioDeviceID   audio_devID; //音频 ID
    AVFrame             out_frame; //设置参数，供音频解码后的swr_alloc_set_opts使用。
    SwsContext*         swr_ctx;
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
    //////////播放控制//////////////////////////////////////
    int                 state;/*0为暂停，大于1则为倍速*/
    SDL_TimerID         timer_id;
    double              fps;
    bool                is_running;
};
//视频线程
int video_thread(void *arg);
//错误打印函数
void print_error(const char *msg, int err);
//定时器回调函数
Uint32 SDLCALL timer_callback(Uint32 interval, void *userdata);
void audio_callback(void *userdata, Uint8 *stream, int len);
//解码函数
int audio_decode_frame(AVCodecContext *pcodec_ctx, uint8_t *audio_buf, int buf_size);
//找 auto_stream
int find_stream_index(AVFormatContext *pformat_ctx, int *video_stream, int*audio_stream);
#endif // PLAYER_H
