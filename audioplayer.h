// #ifndef AUDIOPLAYER_H
// #define AUDIOPLAYER_H

// #include "m_queue.h"
// #include <QThread>
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
// class Player;
// class AudioPlayer: public QThread
// {
//     Q_OBJECT
// public:
//     AudioPlayer(Player* player);
//     void run();
// public:
//     Player* player;
//     m_queue<AVPacket>apkque;

// };
// void audio_callback(void *userdata, Uint8 *stream, int len);
// int audio_decode_frame(VideoState *is, uint8_t *audio_buf, int buf_size);
// int find_stream_index(AVFormatContext *pformat_ctx, int *video_stream, int *audio_stream);
// #endif // AUDIOPLAYER_H
