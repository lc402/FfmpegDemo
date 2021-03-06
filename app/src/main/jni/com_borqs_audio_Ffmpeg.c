/* DO NOT EDIT THIS FILE - it is machine generated */
#include <com_borqs_audio_Ffmpeg.h>

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    avcodec_find_decoder
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_avcodecFindDecoder
(JNIEnv *env, jobject object, jint codecID) {
    AVCodec* codec = NULL;
    /*register all formats and codecs */
    av_register_all();
    codec = avcodec_find_decoder(codecID);
    LOGE("liuchang codec id is %d", codec->id);
    if(codec != NULL)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    create
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_create
  (JNIEnv *env, jobject object){

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    setFilePath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_borqs_audio_Ffmpeg_setFilePath
  (JNIEnv *env, jobject object, jstring path){

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    getFrameByte
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_borqs_audio_Ffmpeg_getFrameByte
  (JNIEnv *env, jobject object){

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    start
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_start
  (JNIEnv *env, jobject object){

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_stop
  (JNIEnv *env, jobject object){

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    pause
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_pause
  (JNIEnv *env, jobject object) {

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    resume
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_resume
  (JNIEnv *env, jobject object){

  }

/*
 * Class:     com_borqs_audio_Ffmpeg
 * Method:    seekPosition
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_seekPosition
  (JNIEnv *env, jobject object, jint position){
        return -1;
  }
int findDecodec(const char* path, AVCodec *codec) {
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
	AVFormatContext *fmt_ctx;
    if ( avformat_open_input(&fmt_ctx, path, NULL, NULL) < 0) {
        LOGE("Could not open source file");
    }
    int index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (index < 0) {
        LOGE("Could not find  stream in input file");
    } else {
        st = fmt_ctx->streams[index];
        dec_ctx = st->codec;
        codec = avcodec_find_decoder(dec_ctx->codec_id);
        LOGE("the accodec is %s",codec->name);
    }
    return 0;

}


JNIEXPORT jint JNICALL Java_com_borqs_audio_Ffmpeg_audioDecode
  (JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr)
{
    AVFormatContext	*pFormatCtx;
    int				i, videoindex, audioindex, ch;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    AVFrame	*pFrame,*pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double  time_duration = 0.0;

    char input_str[500]={0};
    char output_str[500]={0};
    char info[1000]={0};
    sprintf(input_str,"%s",(*env)->GetStringUTFChars(env,input_jstr, NULL));
    sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,output_jstr, NULL));

    //FFmpeg av_log() callback
    av_log_set_callback(custom_log);

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&pFormatCtx,input_str,NULL,NULL)!=0){
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    videoindex=-1;
    audioindex = -1;
    for(i=0; i<pFormatCtx->nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            continue;
        }
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO){
            audioindex=i;
            continue;
        }
    }
    if(videoindex==-1){
        LOGE("Couldn't find a video stream.\n");
        //return -1;
    }

     LOGE("[Input     ]%s\n", input_str);
        LOGE("[Output    ]%s\n", output_str);
        LOGE("[Format    ]%s\n", pFormatCtx->iformat->name);
        LOGE("[audioindex    ]%d\n", &audioindex);


    pCodecCtx=pFormatCtx->streams[audioindex]->codec;
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    LOGE("pcodec name is %s", pCodec->name);
    if(pCodec==NULL){
        LOGE("Couldn't find Codec.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
        LOGE("Couldn't open codec.\n");
        return -1;
    }

    LOGE("this is debug point");
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
        AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));

    //img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
     //   pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


    LOGE("[Input     ]%s\n", input_str);
    LOGE("[Output    ]%s\n", output_str);
    LOGE("[Format    ]%s\n", pFormatCtx->iformat->name);
    LOGE("[Codec     ]%s\n",pCodecCtx->codec->name);
    LOGE("[Resolution]%dx%d\n", pCodecCtx->width,pCodecCtx->height);


    fp_yuv=fopen(output_str,"wb+");
    if(fp_yuv==NULL){
        printf("Cannot open output file.\n");
        return -1;
    }

    frame_cnt=0;
    time_start = clock();

    while(av_read_frame(pFormatCtx, packet)>=0){
        if(packet->stream_index==videoindex){
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if(ret < 0){
                LOGE("Decode Error.\n");
                return -1;
            }
            if(got_picture){
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                    pFrameYUV->data, pFrameYUV->linesize);

                y_size=pCodecCtx->width*pCodecCtx->height;
                fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
                fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
                fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
                //Output info
                char pictype_str[10]={0};
                switch(pFrame->pict_type){
                    case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
                  case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
                    case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
                    default:sprintf(pictype_str,"Other");break;
                }
                LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
                frame_cnt++;
            }
        } else if(packet->stream_index==audioindex) {
            LOGE("this is a audio data");
            ret = avcodec_decode_audio4(pCodecCtx, pFrame, &got_picture, packet);
            if (got_picture) {
                        /* if a frame has been decoded, output it */
                        int data_size = av_get_bytes_per_sample(pCodecCtx->sample_fmt);
                        if (data_size < 0) {
                            /* This should not occur, checking just for paranoia */
                            fprintf(stderr, "Failed to calculate data size\n");
                            exit(1);
                        }
                        for (i=0; i<pFrame->nb_samples; i++)
                            for (ch=0; ch<pCodecCtx->channels; ch++)
                                fwrite(pFrame->data[ch] + data_size*i, 1, data_size, fp_yuv);
                    }
        }
        av_free_packet(packet);
    }
    //flush decoder
    //FIX: Flush Frames remained in Codec
    /*while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
            pFrameYUV->data, pFrameYUV->linesize);
        int y_size=pCodecCtx->width*pCodecCtx->height;
        fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
        fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
        fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
        //Output info
        char pictype_str[10]={0};
        switch(pFrame->pict_type){
            case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
          case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
            case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
            default:sprintf(pictype_str,"Other");break;
        }
        LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
        frame_cnt++;
    }*/
    time_finish = clock();
    time_duration=(double)(time_finish - time_start);

    LOGE("%s[Time      ]%fms\n",info,time_duration);
    LOGE("%s[Count     ]%d\n",info,frame_cnt);

    //sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
 }
 void custom_log(void *ptr, int level, const char* fmt, va_list vl){
 	FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
 	if(fp){
 		vfprintf(fp,fmt,vl);
 		fflush(fp);
 		fclose(fp);
 	}
 }
