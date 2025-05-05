// #include "player.h"
// static int current_sine_sample = 0;
// Player::Player(QObject *parent): QThread{parent},out_frame(),video_cond(),lock()
// {
//     /*-------------------文件--------------------*/
//     QString m_fileName;             ///文件路径
//     format_ctx=nullptr;             ///文件上下文
//     /*----------------音频相关参数------------------*/
//     audio_stream=nullptr; //音频流
//     audio_que=new m_queue<AVPacket>;///音频包队列
//     audio_codec_ctx=nullptr;        ///音频解码器信息指针
//     audio_stream_index=-1;          ///视频音频流索引
//     audio_devID=-1;                     ///音频 ID
//     //// 音频回调函数使用的量
//     audio_buf_size = 0;             ///拷贝缓冲区大小
//     audio_buf_index = 0;            ///拷贝缓冲区索引
//     audioFrame=nullptr;
//     /*----------------视频相关参数------------------*/
//     video_stream=nullptr;           ///视频流
//     video_que=new m_queue<AVPacket>;///视频队列
//     video_codec_ctx=nullptr;        ///音频解码器信息指针
//     video_stream_index=-1;          ///视频音频流索引
//     video_tid=nullptr;              ///视频线程id
//     video_work=false;
//     /*---------控制--------------*/
//     state=1;                        ///播放速度初始为1倍速
//     timer_id=-1;
//     fps=-1;
//     is_running=false;
// }
// void Player::setFileName(const QString &newFileName)
// {
//     m_fileName = newFileName;
// }
// Uint32 SDLCALL timer_callback(void *userdata, SDL_TimerID timerID, Uint32 interval){
//     qDebug()<<"timer:"<<timerID<<"wake vplayer";
//     Player* player=(Player*)userdata;
//     player->lock.lock();
//     player->video_work=true;
//     player->lock.unlock();
//     player->video_cond.wakeOne();
//     if(player->state==0){
//         player->timer_id=-1;
//         return 0;
//     }
//     return ((double)1/player->state)*interval;
// }
// int video_thread(void *arg){
//     Player* player=(Player*)arg;
//     /*------------------初始化数据-----------------*/
//     AVFrame *pFrameRGB =  av_frame_alloc();
//     AVFrame *frame = av_frame_alloc();
//     if (!frame) {
//         fprintf(stderr, "Could not allocate frame\n");
//         return -1;
//     }
//     /*------------------创建YUV->RGB32的转换器--------------------------*/
//     struct SwsContext *img_convert_ctx;
//     img_convert_ctx = sws_getContext(player->video_codec_ctx->width, player->video_codec_ctx->height,
//                                      player->video_codec_ctx->pix_fmt, player->video_codec_ctx->width,
//                                      player->video_codec_ctx->height,
//                                      AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
//     if (!img_convert_ctx) {
//         fprintf(stderr, "Failed to create SwsContext\n");
//         return -1;
//     }
//     /*------------------填充pFrameRGB->data----------------------*/
//     auto numBytes = av_image_get_buffer_size (AV_PIX_FMT_RGB32,player->video_codec_ctx->width ,player->video_codec_ctx->height,1);
//     uint8_t *  out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
//     int ret = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGB32, player->video_codec_ctx->width, player->video_codec_ctx->height, 1);
//     if (ret < 0) {
//         // 处理填充失败的情况
//         fprintf(stderr, "av_image_fill_arrays fail\n");
//         av_free(out_buffer);
//         av_frame_free(&frame);
//         return -1;
//     }
//     /*----------------开始从队列中取任务工作--------------*/
//     while(!player->video_que->que.empty()){
//         player->lock.lock();
//         ///若任务队列不为空且定时器已经将video_work设置为true则工作
//         while(player->video_que->que.empty()||!player->video_work){
//             //qDebug()<<"consumer is sleep";
//             player->video_cond.wait(&player->lock);
//         }
//         //qDebug()<<"consumer is wake";
//         AVPacket packet=player->video_que->front();
//         player->video_que->pop();
//         player->lock.unlock();
//         ///已将视频包取出开始进行解码与显示
//         ret = avcodec_send_packet(player->video_codec_ctx, &packet);
//         if (ret < 0) {
//             print_error("Error sending packet to decoder", ret);
//             continue;
//         }
//         ///从解码器接收帧
//         while (ret >= 0) {
//             ret = avcodec_receive_frame(player->video_codec_ctx, frame);
//             if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                 break;
//             } else if (ret < 0) {
//                 print_error("Error receiving frame from decoder", ret);
//                 break;
//             }
//             frame->pts = frame->best_effort_timestamp;
//             int scale_ret = sws_scale(img_convert_ctx,
//                                       (uint8_t const * const *) frame->data,
//                                       frame->linesize, 0, player->video_codec_ctx->height, pFrameRGB->data,
//                                       pFrameRGB->linesize);
//             if (scale_ret < 0) {
//                 fprintf(stderr, "sws_scale failed with error code: %d\n", scale_ret);
//             }
//             QImage tmpImg((uchar *)out_buffer,player->video_codec_ctx->width,player->video_codec_ctx->height,QImage::Format_RGB32);
//             ///发送帧图片去显示
//             emit player->SIG_getainmage(tmpImg);
//             //qDebug()<<"consumer work over";
//         }
//         av_packet_unref(&packet);
//         player->lock.lock();
//         player->video_work=false;
//         player->lock.unlock();
//     }
//     return 0;
// }
// void print_error(const char *msg, int err) {
//     char errbuf[AV_ERROR_MAX_STRING_SIZE];
//     av_strerror(err, errbuf, sizeof(errbuf));
//     fprintf(stderr, "%s: %s\n", msg, errbuf);
// }
// void Player::run(){
//     is_running=true;
//     /*---------------------------初始化文件------------------------------*/
//     std::string path = m_fileName.toStdString();
//     const char* input_filename = path.c_str();
//     format_ctx = avformat_alloc_context();
//     /*音频库SDL初始化*/
//     /*新版本视频库ffmpeg无需初始化，会自动进行加载*/
//     if (!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER))
//     {
//         fprintf(stderr, "Couldn't init SDL,%d:%s\n", SDL_GetError());
//         exit(-1);
//     }
//     // 打开输入文件
//     int ret = avformat_open_input(&format_ctx, input_filename, NULL, NULL);
//     if (ret < 0) {
//         print_error("Could not open input file", ret);
//         return;
//     }
//     // 查找流信息
//     ret = avformat_find_stream_info(format_ctx, NULL);
//     if (ret < 0) {
//         print_error("Could not find stream information", ret);
//         avformat_close_input(&format_ctx);
//         return;
//     }
//     /*---------------------------------------------------------------------*/
//     /*---------------------查找视频流与音频流--------------------------------*/
//     video_stream_index = -1;
//     audio_stream_index = -1;
//     AVCodecParameters* video_codec_params = NULL;
//     AVCodecParameters* audio_codec_params = NULL;
//     for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
//         if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//             video_stream_index = i;
//             video_codec_params = format_ctx->streams[i]->codecpar;
//         }
//         if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//             audio_stream_index = i;
//             audio_codec_params = format_ctx->streams[i]->codecpar;
//         }
//     }
// }
