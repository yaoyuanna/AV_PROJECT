#include "player.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
Player::Player(QObject *parent): QThread{parent},p((MainWindow*)parent),out_frame(),video_cond(),lock(),
    defaultimage("C:\\Users\\86139\\Documents\\Qt_project\\av\\picture\\defaultimage.png")
{
    /*-------------------文件--------------------*/
    QString m_fileName;             ///文件路径
    format_ctx=nullptr;             ///文件上下文
    /*----------------音频相关参数------------------*/
    audio_stream=nullptr; //音频流
    audio_que=new m_queue<AVPacket>;///音频包队列
    audio_codec_ctx=nullptr;        ///音频解码器信息指针
    audio_stream_index=-1;          ///视频音频流索引
    audio_devID=-1;                     ///音频 ID
    //// 音频回调函数使用的量
    audio_buf_size = 0;             ///拷贝缓冲区大小
    audio_buf_index = 0;            ///拷贝缓冲区索引
    audioFrame=nullptr;
    /*----------------视频相关参数------------------*/
    video_stream=nullptr;           ///视频流
    video_que=new m_queue<AVPacket>;///视频队列
    video_codec_ctx=nullptr;        ///音频解码器信息指针
    video_stream_index=-1;          ///视频音频流索引
    video_tid=nullptr;              ///视频线程id
    video_work=false;
    /*---------控制--------------*/
    state=1;                        ///播放速度初始为1倍速
    timer_id=-1;
    time_last=0;
    seek_timer_id=-1;
    fps=-1;
    is_running=false;
    ais_running=false;
    vis_running=false;
    is_seek=false;
    seek=-1;
    audio_clock=0;
    video_clock=0;
}
void Player::run()
{
    qDebug()<<"start run";
    is_running=true;
    /*---------------------------初始化文件------------------------------*/
    std::string path = m_fileName.toStdString();
    const char* input_filename = path.c_str();
    format_ctx = avformat_alloc_context();
    qDebug()<<"aaa";
    /*音频库SDL初始化*/
    av_register_all();
    /*新版本视频库ffmpeg无需初始化，会自动进行加载*/
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO| SDL_INIT_TIMER))
    {
        qDebug()<<"SDL INIT fail";
        fprintf(stderr, "Couldn't init SDL,%d:%s\n", SDL_GetError());
        exit(-1);
    }else
    qDebug()<<"SDL INIT ok";
    // 打开输入文件
    int ret = avformat_open_input(&format_ctx, input_filename, NULL, NULL);
    if (ret < 0) {
        print_error("Could not open input file", ret);
        return;
    }
    qDebug()<<"open file ok";
    // 查找流信息
    ret = avformat_find_stream_info(format_ctx, NULL);
    if (ret < 0) {
        print_error("Could not find stream information", ret);
        avformat_close_input(&format_ctx);
        return;
    }
    /*---------------------------------------------------------------------*/
    /*---------------------查找视频流与音频流--------------------------------*/
    video_stream_index = -1;
    audio_stream_index = -1;
    AVCodecParameters* video_codec_params = NULL;
    AVCodecParameters* audio_codec_params = NULL;
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            video_codec_params = format_ctx->streams[i]->codecpar;
        }
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            audio_codec_params = format_ctx->streams[i]->codecpar;
        }
    }
    video_stream=format_ctx->streams[video_stream_index];
    audio_stream=format_ctx->streams[audio_stream_index];
    /*------------------------------------------------------------------------*/

    /*---------------------------查找解码器--------------------------------------*/
    if(video_stream_index!=-1){
        video_codec_params = format_ctx->streams[video_stream_index]->codecpar;
        auto video_codec = avcodec_find_decoder(video_codec_params->codec_id);
        if (!video_codec) {
            fprintf(stderr, "Could not find video codec\n");
        }
        video_codec_ctx = avcodec_alloc_context3(video_codec);
        if (!video_codec_ctx) {
            fprintf(stderr, "Could not allocate video codec context\n");
        }
        ret = avcodec_parameters_to_context(video_codec_ctx, video_codec_params);
        if (ret < 0) {
            print_error("Could not copy codec parameters to video codec context", ret);
            avcodec_free_context(&video_codec_ctx);
        }
        ret = avcodec_open2(video_codec_ctx, video_codec, NULL);
        if (ret < 0) {
            print_error("Could not open codec", ret);
            avcodec_free_context(&video_codec_ctx);
            return;
        }
    }
    if(audio_stream_index!=-1){
        audio_codec_params = format_ctx->streams[audio_stream_index]->codecpar;
        auto audio_codec = avcodec_find_decoder(audio_codec_params->codec_id);
        if (!audio_codec) {
            fprintf(stderr, "Could not find audio codec\n");
        }
        audio_codec_ctx = avcodec_alloc_context3(audio_codec);
        if (!audio_codec_ctx) {
            fprintf(stderr, "Could not allocate audio codec context\n");
        }
        ret = avcodec_parameters_to_context(audio_codec_ctx, audio_codec_params);
        if (ret < 0) {
            print_error("Could not copy codec parameters to audio codec context", ret);
            avcodec_free_context(&audio_codec_ctx);
        }
        ret = avcodec_open2(audio_codec_ctx, audio_codec, NULL);
        if (ret < 0) {
            print_error("Could not open codec", ret);
            avcodec_free_context(&audio_codec_ctx);
            return;
        }
        /*-----------------------------对音频播放进行设置--------------------------------*/
        wanted_spec.freq = audio_codec_ctx->sample_rate; //采样频率
        wanted_spec.format = AUDIO_S16SYS; //精度
        wanted_spec.channels = audio_codec_ctx->channels; //通道数
        wanted_spec.silence = 0; //设置静音值
        wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE; //采样点
        wanted_spec.callback = audio_callback;//回调函数
        wanted_spec.userdata = this;//回调函数参数
        audio_devID = SDL_OpenAudioDevice(NULL,0,&wanted_spec, &spec,0);
        if( audio_devID < 0 ) //第二次打开 audio 会返回-1
        {
            qDebug()<< "Couldn't open Audio: " << SDL_GetError() ;
            return;
        }
        wanted_frame.format = AV_SAMPLE_FMT_S16;
        wanted_frame.sample_rate = spec.freq;
        wanted_frame.channel_layout = av_get_default_channel_layout(spec.channels);
        wanted_frame.channels = spec.channels;
    }
    qDebug()<<"decode ok";
    /*-----------------------------根据流的情况判断开始播放音频还是视频-----------------*/
    if (video_stream_index != -1) {
        vis_running=true;
        video_tid = SDL_CreateThread(video_thread,"video_thread",this);
    }
    if (audio_stream_index != -1) {
        ais_running=true;
        SDL_PauseAudioDevice(audio_devID,0);
    }
    if (video_stream_index == -1&&audio_stream_index == -1) {
        fprintf(stderr, "Could not find stream\n");
        avformat_close_input(&format_ctx);
        return;
    }
    qDebug()<<"thread start ok";
    /*---------------------------------------------------------------------------*/
    /*-----------------------------计算视频帧率确定定时器------------------------------*/
    fps = av_q2d(format_ctx->streams[video_stream_index]->r_frame_rate);
    timer_id = SDL_AddTimer((1/ fps)*1000,&timer_callback,(void*)this);
    seek_timer_id = SDL_AddTimer(1000,&timer_callback1,(void*)this);
    qDebug()<<fps;
    qDebug()<<"timer ok";
    /*---------------------------------------------------------------------------*/
    emit SIG_settotaltime(format_ctx->duration);
    /*-----------------------开始投放数据--------------------------*/

    AVPacket *packet = av_packet_alloc();
    // 读取数据包
    qDebug()<<"producer start work";
    while (ais_running||vis_running) {
        if(!is_running)video_cond.wakeOne();
        //qDebug()<<ais_running<<vis_running;
        if(is_seek){
            seek_to();
        }
        if(av_read_frame(format_ctx, packet) >= 0){
            if (packet->stream_index == video_stream_index) {
                qDebug()<<"producer add a video packet";
                /*使用深拷贝*/
                video_que->push(*av_packet_clone(packet));
            }else if(packet->stream_index == audio_stream_index){
                qDebug()<<"producer add a audio packet";
                /*使用深拷贝*/
                audio_que->push(*av_packet_clone(packet));
            }
            av_packet_unref(packet);
            /*根据队列中的任务数量来延迟，超过100个包了就延迟*/
            while((audio_que->size()>100||video_que->size()>100)&&is_running&&!is_seek){
                qDebug()<<audio_que->size()<<video_que->size();
                SDL_Delay(1000);
            }
        }else SDL_Delay(100);
    }
    qDebug()<<"player is over";
    is_running=false;
    // 释放资源
    av_packet_free(&packet);
    avformat_close_input(&format_ctx);
    SDL_CloseAudioDevice(audio_devID);
    video_que->clear();
    audio_que->clear();
    avcodec_free_context(&audio_codec_ctx);
    audio_codec_ctx = nullptr;
    state=1;
    audio_clock=0;
    video_clock=0;
    is_seek=false;
    seek=-1;
    return;
}

void Player::setFileName(const QString &newFileName)
{
    m_fileName = newFileName;
}

void Player::seek_to()
{
    int stream_index = video_stream_index!=-1?video_stream_index:audio_stream_index;
    int64_t seek_target = av_rescale_q(seek*1000000, {1, AV_TIME_BASE},format_ctx->streams[stream_index]->time_base);
    if (av_seek_frame(format_ctx, stream_index, seek_target,AVSEEK_FLAG_BACKWARD) < 0)
        fprintf(stderr, "%s: error while seeking",format_ctx->filename);

    AVPacket* temppkt=(AVPacket *) malloc(sizeof(AVPacket));
    av_new_packet(temppkt, 10);
    strcpy((char*)temppkt->data,FLUSH_DATA);

    if(audio_stream_index!=-1)audio_que->push_flush(*av_packet_clone(temppkt));
    if(video_stream_index!=-1)video_que->push_flush(*av_packet_clone(temppkt));

    qDebug()<<"video que:"<<video_que->size()<<"   audio que:"<<audio_que->size();

    is_seek=false;
}
Uint32 SDLCALL timer_callback(Uint32 interval, void *userdata){
    Player* player=(Player*)userdata;
    if(player->state==0||!player->vis_running){
        player->timer_id=-1;
        qDebug()<<"timmer stop";
        return 0;
    }
    //qDebug()<<"timer:"<<"wake vplayer"<<interval<<SDL_GetTicks()-player->time_last;
    player->time_last=SDL_GetTicks();
    player->lock.lock();
    player->video_work=true;
    player->lock.unlock();
    player->video_cond.wakeOne();
    return (((double)1/player->state)*interval);
}
Uint32 SDLCALL timer_callback1(Uint32 interval, void *userdata){
    Player* player=(Player*)userdata;
    if(player->state==0||!player->vis_running){
        player->timer_id=-1;
        qDebug()<<"timmer stop";
        return 0;
    }
    emit player->SIG_setcurtime(player->audio_clock);
    return (1/player->state)*interval;
}
int video_thread(void *arg){
    qDebug()<<"consumer run";
    Player* player=(Player*)arg;
    /*------------------初始化数据-----------------*/
    AVFrame *pFrameRGB =  av_frame_alloc();
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        return -1;
    }
    /*------------------创建YUV->RGB32的转换器--------------------------*/
    struct SwsContext *img_convert_ctx;
    /*转换为显示窗大小*/
    // img_convert_ctx = sws_getContext(player->video_codec_ctx->width, player->video_codec_ctx->height,
    //                                  player->video_codec_ctx->pix_fmt, player->p->ui->lb_show->width(),
    //                                  player->p->ui->lb_show->height(),
    //                                  AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    /*转换为图片大小*/
    img_convert_ctx = sws_getContext(player->video_codec_ctx->width, player->video_codec_ctx->height,
                                     player->video_codec_ctx->pix_fmt, player->video_codec_ctx->width,
                                     player->video_codec_ctx->height,
                                     AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    if (!img_convert_ctx) {
        fprintf(stderr, "Failed to create SwsContext\n");
        return -1;
    }
    /*------------------填充pFrameRGB->data----------------------*/
    /*转换为图片大小*/
    auto numBytes = av_image_get_buffer_size (AV_PIX_FMT_RGB32,player->video_codec_ctx->width ,player->video_codec_ctx->height,1);
    /*转换为显示窗大小*/
    //auto numBytes = av_image_get_buffer_size (AV_PIX_FMT_RGB32,player->p->ui->lb_show->width() ,player->p->ui->lb_show->height(),1);
    uint8_t *  out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    /*转换为图片大小*/
    int ret = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGB32, player->video_codec_ctx->width ,player->video_codec_ctx->height, 1);
    /*转换为显示窗大小*/
    //int ret = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGB32, player->p->ui->lb_show->width(), player->p->ui->lb_show->height(), 1);
    if (ret < 0) {
        // 处理填充失败的情况
        fprintf(stderr, "av_image_fill_arrays fail\n");
        av_free(out_buffer);
        av_frame_free(&frame);
        return -1;
    }
    /*----------------开始从队列中取任务工作--------------*/
    while(1){
        player->lock.lock();
        ///若任务队列不为空且定时器已经将video_work设置为true则工作
        while(player->video_que->empty()||!player->video_work){
            //qDebug()<<"consumer is sleep";
            if(player->video_que->empty()||player->is_running==false){
                if(player->is_running==false){
                    player->vis_running=false;
                }
                else{
                    SDL_Delay(1000);
                    if(player->video_que->empty())player->vis_running=false;
                }
                if(player->vis_running==false){
                    emit player->SIG_getainmage(player->defaultimage);
                    qDebug()<<"video over";
                    player->lock.unlock();
                    return 0;
                }
            }
            player->video_cond.wait(&player->lock);
        }
        //qDebug()<<"consumer is wake";
        AVPacket packet=player->video_que->front();
        player->video_que->pop();
        while(strcmp((char*)packet.data,FLUSH_DATA) == 0)
        {
            avcodec_flush_buffers(player->audio_codec_ctx);
            av_free_packet(&packet); //很关键 , 不清空 向左跳转, 视频帧会等待音频帧
            packet=player->video_que->front();
            player->video_que->pop();
        }
        player->video_clock=packet.pts*av_q2d(player->video_stream->time_base);
        //qDebug()<<"video clock:"<<player->video_clock;
        while(player->video_clock>player->audio_clock){
            //qDebug()<<"-----------------------------------------------------------";
            SDL_Delay(10);
        }
        //qDebug()<<"video que:"<<player->video_que->size();
        player->lock.unlock();
        ///已将视频包取出开始进行解码与显示
        ret = avcodec_send_packet(player->video_codec_ctx, &packet);
        if (ret < 0) {
            print_error("Error sending packet to decoder", ret);
            continue;
        }
        ///从解码器接收帧
        while (ret >= 0) {
            ret = avcodec_receive_frame(player->video_codec_ctx, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                print_error("Error receiving frame from decoder", ret);
                break;
            }
            frame->pts = frame->best_effort_timestamp;

            int scale_ret = sws_scale(img_convert_ctx,
                                      (uint8_t const * const *) frame->data,
                                      frame->linesize, 0, player->video_codec_ctx->height, pFrameRGB->data,
                                      pFrameRGB->linesize);
            if (scale_ret < 0) {
                fprintf(stderr, "sws_scale failed with error code: %d\n", scale_ret);
            }
            /*转换为显示窗大小*/
            //QImage tmpImg((uchar *)out_buffer,player->p->ui->lb_show->width(),player->p->ui->lb_show->height(),QImage::Format_RGB32);
            /*转换为图片大小*/
            QImage tmpImg((uchar *)out_buffer,player->video_codec_ctx->width ,player->video_codec_ctx->height,QImage::Format_RGB32);
            ///发送帧图片去显示
            emit player->SIG_getainmage(tmpImg);
            //qDebug()<<"consumer work over";
        }
        av_packet_unref(&packet);
        player->lock.lock();
        player->video_work=false;
        player->lock.unlock();
    }
    qDebug()<<"video error over";
    return 0;
}
void print_error(const char *msg, int err) {
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(err, errbuf, sizeof(errbuf));
    fprintf(stderr, "%s: %s\n", msg, errbuf);
}
void audio_callback(void *userdata, Uint8 *stream, int len)
{
    Player* player = (Player *) userdata;
    int len1, audio_data_size;
    player->audio_buf_size = 0;
    player->audio_buf_index = 0;
    /* len 是由 SDL 传入的 SDL 缓冲区的大小，如果这个缓冲未满，我们就一直往里填充数据 */
    /* audio_buf_index 和 audio_buf_size 标示我们自己用来放置解码出来的数据的缓冲区，*/
    /* 这些数据待 copy 到 SDL 缓冲区， 当 audio_buf_index >= audio_buf_size 的时候意味着我*/
    /* 们的缓冲为空，没有数据可供 copy，这时候需要调用 audio_decode_frame 来解码出更
/* 多的桢数据 */
    while (len > 0)
    {
        if (player->audio_buf_index >= player->audio_buf_size) {
            audio_data_size = audio_decode_frame(player,player->audio_buf,sizeof(player->audio_buf));
            /* audio_data_size < 0 标示没能解码出数据，我们默认播放静音 */
            if (audio_data_size ==-2) {
                /* silence */
                player->audio_buf_size = 1024;
                /* 清零，静音 */
                memset(player->audio_buf, 0, player->audio_buf_size);
            } else if(audio_data_size ==-1){
                SDL_PauseAudioDevice(player->audio_devID,1);
                player->ais_running=false;
                qDebug()<<"audio over";
                return;
            }else {
                player->audio_buf_size = audio_data_size;
            }
            player->audio_buf_index = 0;
        }
        /* 查看 stream 可用空间，决定一次 copy 多少数据，剩下的下次继续 copy */
        len1 = player->audio_buf_size - player->audio_buf_index;
        if (len1 > len) {
            len1 = len;
        }
        memset( stream , 0 , len1);
        //混音函数 sdl 2.0 版本使用该函数 替换 SDL_MixAudio
        SDL_MixAudioFormat(stream, (uint8_t *) player->audio_buf + player->audio_buf_index,
                           AUDIO_S16SYS,len1,100);
        len -= len1;
        stream += len1;
        player->audio_buf_index += len1;
    }
}
int audio_decode_frame(Player *player, uint8_t *audio_buf, int buf_size)
{
    static AVPacket pkt;
    static uint8_t *audio_pkt_data = NULL;
    static int audio_pkt_size = 0;
    int len1, data_size;
    int sampleSize = 0;
    AVCodecContext *aCodecCtx = player->audio_codec_ctx;
    AVFrame *audioFrame = NULL;
    static struct SwrContext *swr_ctx = NULL;
    int convert_len;
    int n = 0;
    while(1)
    {
        if(player->is_running==false) return -1;
        if(player->audio_que->empty()&&!player->is_seek) //一定注意
        {
            SDL_Delay(100);
            if(player->audio_que->empty())
            return -1;
        }
        if(player->state==0)return -2;
        pkt=player->audio_que->front();
        player->audio_que->pop();
        while(strcmp((char*)pkt.data,FLUSH_DATA) == 0)
        {
            avcodec_flush_buffers(player->audio_codec_ctx);
            av_free_packet(&pkt);
            pkt=player->audio_que->front();
            player->audio_que->pop();
        }
        audioFrame = av_frame_alloc();
        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
        //计算音频时钟
        if( pkt.pts != AV_NOPTS_VALUE)
        {
            player->audio_clock = pkt.pts *av_q2d( player->audio_stream->time_base ) ;
            //取音频时钟
        }
        else
        {
            player->audio_clock = (*(uint64_t *) audioFrame->opaque)*av_q2d( player->audio_stream->time_base );
        }
        //qDebug()<<"audio clock:"<<player->audio_clock;
        while(audio_pkt_size > 0)
        {
            int got_picture;
            memset(audioFrame, 0, sizeof(AVFrame));
            int ret =avcodec_decode_audio4( aCodecCtx, audioFrame, &got_picture, &pkt);
            if( ret < 0 ) {
                qDebug()<<(char*)pkt.data;
                qDebug()<<"Error in decoding audio frame. ";
                exit(0);
            }
            data_size = audioFrame->nb_samples * player->wanted_frame.channels * 2;
            if( got_picture )
            {
                if (swr_ctx != NULL)
                {
                    swr_free(&swr_ctx);
                    swr_ctx = NULL;
                }
                swr_ctx = swr_alloc_set_opts(NULL, player->wanted_frame.channel_layout,
                                             (AVSampleFormat)player->wanted_frame.format,player->wanted_frame.sample_rate,
                                             audioFrame->channel_layout,(AVSampleFormat)audioFrame->format,
                                             audioFrame->sample_rate, 0, NULL);
                //初始化
                if (swr_ctx == NULL || swr_init(swr_ctx) < 0)
                {
                    qDebug()<<"swr_init error\n";
                    break;
                }
                convert_len = swr_convert(swr_ctx, &audio_buf,
                                          AVCODEC_MAX_AUDIO_FRAME_SIZE,
                                          (const uint8_t **)audioFrame->data,
                                          audioFrame->nb_samples);
            }
            audio_pkt_size -= ret;
            if (audioFrame->nb_samples <= 0)
            {
                continue;
            }
            av_free_packet(&pkt);
            return data_size;
        }
        av_free_packet(&pkt);
    }
}
