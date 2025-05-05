// #include "audioplayer.h"
// #include "player.h"
// AudioPlayer::AudioPlayer(Player* player) {
//     this->player=player;
// }

// void AudioPlayer::run()
// {

// }
// //13.回调函数中将从队列中取数据, 解码后填充到播放缓冲区.
// void audio_callback(void *userdata, Uint8 *stream, int len)
// {
//     //
//     AVCodecContext *pcodec_ctx   = (AVCodecContext *) userdata;
//     VideoState * is = (VideoState *) userdata;
//     int len1, audio_data_size;
//     //
//     //
//     //
//     /*
// static uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
// static unsigned int audio_buf_size = 0;
// static unsigned int audio_buf_index = 0;
// len 是由SDL传入的SDL缓冲区的大小，如果这个缓冲未满，我们就一直往里填充数据 */
//     /*  audio_buf_index 和 audio_buf_size 标示我们自己用来放置解码出来的数据的缓冲区，*/
//     /*
// 这些数据待copy到SDL缓冲区， 当audio_buf_index >= audio_buf_size的时候意味着我*/
//     /*
// /*
// 们的缓冲为空，没有数据可供copy，这时候需要调用audio_decode_frame来解码出更
// 多的桢数据 */
//     while (len > 0)
//     {
//         if (is->audio_buf_index >= is->audio_buf_size) {
//             audio_data_size = audio_decode_frame( is ,
//                                                  is->audio_buf,sizeof(is->audio_buf));
//             /* audio_data_size < 0 标示没能解码出数据，我们默认播放静音 */
//             if (audio_data_size < 0) {
//                 /* silence */
//                 is->audio_buf_size = 1024;
//                 /* 清零，静音 */
//                 memset(is->audio_buf, 0, is->audio_buf_size);
//             } else {
//                 is->audio_buf_size = audio_data_size;
//             }
//             is->audio_buf_index = 0;
//         }
//         /*  查看stream可用空间，决定一次copy多少数据，剩下的下次继续copy */
//         len1 = is->audio_buf_size - is->audio_buf_index;
//         if (len1 > len) {
//             len1 = len;
//         }
//         memset( stream , 0 , len1);
//         //混音函数 sdl 2.0版本使用该函数 替换SDL_MixAudio
//         SDL_MixAudioFormat(stream, (uint8_t *) is->audio_buf + is->audio_buf_index,
//                            AUDIO_S16SYS,len1,100);
//         len -= len1;
//         stream += len1;
//         is->audio_buf_index += len1;
//     }
// }
// int find_stream_index(AVFormatContext *pformat_ctx, int *video_stream, int *audio_stream)
// {
//     assert(video_stream != NULL || audio_stream != NULL);

//     int i = 0;
//     int audio_index = -1;
//     int video_index = -1;

//     for (i = 0; i < pformat_ctx->nb_streams; i++)
//     {
//         if (pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
//         {
//             video_index = i;
//         }
//         if (pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
//         {
//             audio_index = i;
//         }
//     }

//     //注意以下两个判断有可能返回-1.
//     if (video_stream == NULL)
//     {
//         *audio_stream = audio_index;
//         return *audio_stream;
//     }

//     if (audio_stream == NULL)
//     {
//         *video_stream = video_index;
//     }
//     return *video_stream;
//     *video_stream = video_index;
//     *audio_stream = audio_index;
//     return 0;
// }
// int audio_decode_frame(VideoState *is, uint8_t *audio_buf, int buf_size)
// {
//     AVPacket pkt;
//     uint8_t *audio_pkt_data = NULL;
//     int audio_pkt_size = 0;
//     int len1, data_size;
//     int sampleSize = 0;
//     AVCodecContext *aCodecCtx = is->pAudioCodecCtx;
//     AVFrame *audioFrame = is->audioFrame/*av_frame_alloc()*/;
//     PacketQueue *audioq = is->audioq;
//     AVFrame wanted_frame = is->out_frame;
//     if( !aCodecCtx|| !audioFrame ||!audioq) return -1;
//     /*static*/ struct SwrContext *swr_ctx = NULL;
//     int convert_len;
//     int n = 0;
//     for(;;)
//     {
//         if( is->quit ) return -1;
//         if( !audioq ) return -1;
//         if(packet_queue_get(audioq, &pkt, 0) <= 0) //一定注意
//         {
//             return -1;
//         }
//         audio_pkt_data = pkt.data;
//         audio_pkt_size = pkt.size;
//         while(audio_pkt_size > 0)
//         {
//             if( is->quit ) return -1;
//             int got_picture;
//             memset(audioFrame, 0, sizeof(AVFrame));
//             int ret =avcodec_decode_audio4( aCodecCtx, audioFrame, &got_picture, &pkt);
//             if( ret < 0 ) {
//                 printf("Error in decoding audio frame.\n");
//                 exit(0);
//             }
//             //一帧一个声道读取数据字节数是 nb_samples , channels 为声道数 2 表示 16 位 2 个字节
//                 //data_size = audioFrame->nb_samples * wanted_frame.channels * 2;
//                 switch( is->out_frame.format )
//             {
//             case AV_SAMPLE_FMT_U8:
//                 data_size = audioFrame->nb_samples * is->out_frame.channels * 1;
//                 break;
//             case AV_SAMPLE_FMT_S16:
//                 data_size = audioFrame->nb_samples * is->out_frame.channels * 2;
//                 break;
//             default:
//                 data_size = audioFrame->nb_samples * is->out_frame.channels * 2;
//                 break;
//             }
//             //计算音频时钟
//             if( pkt.pts != AV_NOPTS_VALUE)
//             {
//                 is->audio_clock = pkt.pts *av_q2d( is->audio_st->time_base )*1000000 ;
//                 //取音频时钟
//             }else
//             {
//                 is->audio_clock = (*(uint64_t *)
//                                    audioFrame->opaque)*av_q2d( is->audio_st->time_base )*1000000 ;
//             }

//             if( got_picture )
//             {

//                 swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout,
//                                              (AVSampleFormat)wanted_frame.format,wanted_frame.sample_rate,
//                                              audioFrame->channel_layout,(AVSampleFormat)audioFrame->format,
//                                              audioFrame->sample_rate, 0, NULL);
//                 //初始化
//                 if (swr_ctx == NULL || swr_init(swr_ctx) < 0)
//                 {
//                     printf("swr_init error\n");
//                     break;
//                 }
//                 convert_len = swr_convert(swr_ctx, &audio_buf,
//                                           AVCODEC_MAX_AUDIO_FRAME_SIZE,
//                                           (const uint8_t **)audioFrame->data,
//                                           audioFrame->nb_samples);
//                 swr_free( &swr_ctx );
//             }
//             audio_pkt_size -= ret;
//             if (audioFrame->nb_samples <= 0)
//             {
//                 continue;
//             }
//             av_free_packet(&pkt); //新版考虑使用av_packet_unref() 函数来代替
//             return data_size ;
//         }
//         av_free_packet(&pkt); //新版考虑使用av_packet_unref() 函数来代替
//     }
// }
