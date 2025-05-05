// #include "videoplayer.h"
// #include"player.h"

// VideoPlayer::VideoPlayer(Player* player):apkque(),lock(),cond() {
//     this->player=player;
//     work=false;
// }

// void print_error(const char *msg, int err) {
//     char errbuf[AV_ERROR_MAX_STRING_SIZE];
//     av_strerror(err, errbuf, sizeof(errbuf));
//     fprintf(stderr, "%s: %s\n", msg, errbuf);
// }
// // void VideoPlayer::run()
// // {
// //     //2.需要分配一个AVFormatContext，FFMPEG 所有的操作都要通过这个AVFormatContext来进行,以理解为视频文件指针
// //     //中文兼容
// //     std::string path = m_fileName.toStdString();
// //     const char* input_filename = path.c_str();
// //     AVFormatContext *format_ctx = avformat_alloc_context();
// //     /*开始进行初始化*/
// //     // 打开输入文件
// //     int ret = avformat_open_input(&format_ctx, input_filename, NULL, NULL);
// //     if (ret < 0) {
// //         print_error("Could not open input file", ret);
// //         return;
// //     }

// //     // 查找流信息
// //     ret = avformat_find_stream_info(format_ctx, NULL);
// //     if (ret < 0) {
// //         print_error("Could not find stream information", ret);
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }

// //     // 查找视频流
// //     int video_stream_index = -1;
// //     AVCodecParameters *codec_params = NULL;
// //     for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
// //         if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
// //             video_stream_index = i;
// //             codec_params = format_ctx->streams[i]->codecpar;
// //             break;
// //         }
// //     }
// //     if (video_stream_index == -1) {
// //         fprintf(stderr, "Could not find video stream\n");
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }

// //     // 查找解码器
// //     auto codec = avcodec_find_decoder(codec_params->codec_id);
// //     if (!codec) {
// //         fprintf(stderr, "Could not find codec\n");
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }

// //     // 分配解码器上下文
// //     AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
// //     if (!codec_ctx) {
// //         fprintf(stderr, "Could not allocate codec context\n");
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }

// //     // 将编解码器参数复制到解码器上下文
// //     ret = avcodec_parameters_to_context(codec_ctx, codec_params);
// //     if (ret < 0) {
// //         print_error("Could not copy codec parameters to codec context", ret);
// //         avcodec_free_context(&codec_ctx);
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }

// //     // 打开解码器
// //     ret = avcodec_open2(codec_ctx, codec, NULL);
// //     if (ret < 0) {
// //         print_error("Could not open codec", ret);
// //         avcodec_free_context(&codec_ctx);
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }

// //     // 分配数据包和帧
// //     AVPacket *packet = av_packet_alloc();
// //     AVFrame *pFrameRGB =  av_frame_alloc();
// //     AVFrame *frame = av_frame_alloc();
// //     if (!packet || !frame) {
// //         fprintf(stderr, "Could not allocate packet or frame\n");
// //         avcodec_free_context(&codec_ctx);
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }
// //     //7.这里我们将解码后的YUV数据转换成RGB32 YUV420p 格式视频数据-->RGB32--> 图片显示出来
// //     struct SwsContext *img_convert_ctx;
// //     img_convert_ctx = sws_getContext(codec_ctx->width, codec_ctx->height,
// //                                      codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height,
// //                                      AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
// //     if (!img_convert_ctx) {
// //         fprintf(stderr, "Failed to create SwsContext\n");
// //         // 进行错误处理，如释放资源并退出
// //         return;
// //     }
// //     auto numBytes = av_image_get_buffer_size (AV_PIX_FMT_RGB32,codec_ctx->width ,codec_ctx->height,1);
// //     uint8_t *  out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
// //     ret = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
// //     if (ret < 0) {
// //         // 处理填充失败的情况
// //         fprintf(stderr, "av_image_fill_arrays fail\n");
// //         av_free(out_buffer);
// //         av_frame_free(&frame);
// //         avcodec_free_context(&codec_ctx);
// //         avformat_close_input(&format_ctx);
// //         return;
// //     }
// //     /*初始化完毕*/
// //     // 读取数据包并解码帧
// //     while (av_read_frame(format_ctx, packet) >= 0) {
// //         if (packet->stream_index == video_stream_index) {
// //             // 发送数据包到解码器
// //             while(pts > realTime)
// //             {
// //                 msleep(10);
// //                 realTime = av_gettime() - start_time; //主时钟时间
// //             }
// //             ret = avcodec_send_packet(codec_ctx, packet);
// //             if (ret < 0) {
// //                 print_error("Error sending packet to decoder", ret);
// //                 continue;
// //             }
// //             // 从解码器接收帧
// //             while (ret >= 0) {
// //                 ret = avcodec_receive_frame(codec_ctx, frame);
// //                 if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
// //                     break;
// //                 } else if (ret < 0) {
// //                     print_error("Error receiving frame from decoder", ret);
// //                     break;
// //                 }
// //                 frame->pts = frame->best_effort_timestamp;
// //                 pts = frame->pts;
// //                 pts *= 1000000 * av_q2d(format_ctx->streams[video_stream_index]->time_base);
// //                 // 打印 frame 中的数据信息进行检查
// //                 // printf("Decoded frame width: %d, height: %d\n", frame->width, frame->height);
// //                 // for (int i = 0; i < 10; ++i) {
// //                 //     printf("%02x ", frame->data[0][i]);
// //                 // }
// //                 // printf("\n");
// //                 //有解码器解码之后得到的图像数据都是YUV420的格式，而这里需要将其保存成图片文件
// //                 //因此需要将得到的YUV420数据转换成RGB格式
// //                 int scale_ret = sws_scale(img_convert_ctx,
// //                           (uint8_t const * const *) frame->data,
// //                           frame->linesize, 0, codec_ctx->height, pFrameRGB->data,
// //                           pFrameRGB->linesize);
// //                 if (scale_ret < 0) {
// //                     fprintf(stderr, "sws_scale failed with error code: %d\n", scale_ret);
// //                 }
// //                 // 将 out_buffer 里面的数据存在 QImage里面
// //                 // for (int i = 0; i < 10; ++i) {
// //                 //     printf("%02x ", out_buffer[i]);
// //                 // }
// //                 // printf("\n");
// //                 QImage tmpImg((uchar *)out_buffer,codec_ctx->width,codec_ctx->height,QImage::Format_RGB32);
// //                 //把图像复制一份 传递给界面显示
// //                 //显示到控件  多线程 无法控制控件 所以要发射信号

// //                 emit SIG_getainmage(tmpImg);
// //             }
// //             av_packet_unref(packet);
// //         }/*else if(packet->stream_index == audio_stream_index){

// //         }*/
// //         av_packet_unref(packet);
// //     }
// //     // 释放资源
// //     av_frame_free(&frame);
// //     av_packet_free(&packet);
// //     avcodec_free_context(&codec_ctx);
// //     avformat_close_input(&format_ctx);
// //     return;
// // }
// void VideoPlayer::run(){

//     auto video_codec = avcodec_find_decoder(player->video_codec_params->codec_id);
//     if (!video_codec) {
//         fprintf(stderr, "Could not find codec\n");
//         return;
//     }
//     // 分配解码器上下文
//     AVCodecContext* video_codec_ctx = avcodec_alloc_context3(video_codec);
//     if (!video_codec_ctx) {
//         fprintf(stderr, "Could not allocate codec context\n");
//         return;
//     }

//     // 将编解码器参数复制到解码器上下文
//     int ret = avcodec_parameters_to_context(video_codec_ctx, player->video_codec_params);
//     if (ret < 0) {
//         print_error("Could not copy codec parameters to codec context", ret);
//         avcodec_free_context(&video_codec_ctx);
//         return;
//     }

//     // 打开解码器
//     ret = avcodec_open2(video_codec_ctx, video_codec, NULL);
//     if (ret < 0) {
//         print_error("Could not open codec", ret);
//         avcodec_free_context(&video_codec_ctx);
//         return;
//     }

//     // 分配数据包和帧
//     AVFrame *pFrameRGB =  av_frame_alloc();
//     AVFrame *frame = av_frame_alloc();
//     if (!frame) {
//         fprintf(stderr, "Could not allocate frame\n");
//         return;
//     }
//     //7.这里我们将解码后的YUV数据转换成RGB32 YUV420p 格式视频数据-->RGB32--> 图片显示出来
//     struct SwsContext *img_convert_ctx;
//     img_convert_ctx = sws_getContext(video_codec_ctx->width, video_codec_ctx->height,
//                                      video_codec_ctx->pix_fmt, video_codec_ctx->width, video_codec_ctx->height,
//                                      AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
//     if (!img_convert_ctx) {
//         fprintf(stderr, "Failed to create SwsContext\n");
//         // 进行错误处理，如释放资源并退出
//         return;
//     }
//     auto numBytes = av_image_get_buffer_size (AV_PIX_FMT_RGB32,video_codec_ctx->width ,video_codec_ctx->height,1);
//     uint8_t *  out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
//     ret = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGB32, video_codec_ctx->width, video_codec_ctx->height, 1);
//     if (ret < 0) {
//         // 处理填充失败的情况
//         fprintf(stderr, "av_image_fill_arrays fail\n");
//         av_free(out_buffer);
//         av_frame_free(&frame);
//         return;
//     }
//     while(1){
//         lock.lock();
//         while(apkque.que.empty()||!work){
//             qDebug()<<"consumer is sleep";
//             cond.wait(&lock);
//         }
//         qDebug()<<"consumer is wake";
//         AVPacket packet=apkque.front();
//         apkque.pop();

//         ret = avcodec_send_packet(video_codec_ctx, &packet);
//         if (ret < 0) {
//             print_error("Error sending packet to decoder", ret);
//             continue;
//         }
//         // 从解码器接收帧
//         while (ret >= 0) {
//             ret = avcodec_receive_frame(video_codec_ctx, frame);
//             if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                 break;
//             } else if (ret < 0) {
//                 print_error("Error receiving frame from decoder", ret);
//                 break;
//             }
//             frame->pts = frame->best_effort_timestamp;
//             int scale_ret = sws_scale(img_convert_ctx,
//                       (uint8_t const * const *) frame->data,
//                       frame->linesize, 0, video_codec_ctx->height, pFrameRGB->data,
//                       pFrameRGB->linesize);
//             if (scale_ret < 0) {
//                 fprintf(stderr, "sws_scale failed with error code: %d\n", scale_ret);
//             }
//             QImage tmpImg((uchar *)out_buffer,video_codec_ctx->width,video_codec_ctx->height,QImage::Format_RGB32);
//             //把图像复制一份 传递给界面显示
//             //显示到控件  多线程 无法控制控件 所以要发射信号
//             emit SIG_getainmage(tmpImg);
//             qDebug()<<"consumer work over";
//         }
//         av_packet_unref(&packet);
//         work=false;
//         lock.unlock();
//     }
//     return;
// }

