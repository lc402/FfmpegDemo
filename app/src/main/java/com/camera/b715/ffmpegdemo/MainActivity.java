package com.camera.b715.ffmpegdemo;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.borqs.audio.Ffmpeg;

public class MainActivity extends AppCompatActivity {

       /*static{
            System.loadLibrary("avutil-55");
            System.loadLibrary("avcodec-57");
            System.loadLibrary("swresample-2");
            System.loadLibrary("avformat-57");
            System.loadLibrary("swscale-4");
            System.loadLibrary("avfilter-6");
            System.loadLibrary("ffmpeg_codec");
        }*/
    /* public native int avcodec_find_decoder(int codecID);*/

    AudioTrack mAudioTrack;
    private int samplerate;
    private int mAudioMinBufSize;
    private Ffmpeg mFfmpeg;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView tv = (TextView) this.findViewById(R.id.text);
        mFfmpeg = new Ffmpeg();
        mFfmpeg.initAudioDecodec("/sdcard/Music/moment.wma");
        Log.d("ffmpeg", "result is " + mFfmpeg.avcodecFindDecoder(0x15007));
        Log.d("ffmpeg", "result is " + mFfmpeg.avcodecFindDecoder(0x15008));
        Log.d("ffmpeg", "result is " + mFfmpeg.avcodecFindDecoder(0x15023));
        new Thread() {
            @Override
            public void run() {
                Log.d("liuchang", "audio decodec");
                mFfmpeg.audioDecode("/sdcard/Music/moment.wma", "/sdcard/Music/moment.mp3");
            }

        }.start();
    }

    private void initAudioPlay() {
        samplerate = mFfmpeg.getAudioSamplerate();
        System.out.println("samplerate = " + samplerate);
        samplerate = samplerate / 2;
        // 声音文件一秒钟buffer的大小
        mAudioMinBufSize = AudioTrack.getMinBufferSize(samplerate,
                AudioFormat.CHANNEL_CONFIGURATION_STEREO,
                AudioFormat.ENCODING_PCM_16BIT);

        mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, // 指定在流的类型
                // STREAM_ALARM：警告声
                // STREAM_MUSCI：音乐声，例如music等
                // STREAM_RING：铃声
                // STREAM_SYSTEM：系统声音
                // STREAM_VOCIE_CALL：电话声音

                samplerate,// 设置音频数据的采样率
                AudioFormat.CHANNEL_CONFIGURATION_STEREO,// 设置输出声道为双声道立体声
                AudioFormat.ENCODING_PCM_16BIT,// 设置音频数据块是8位还是16位
                mAudioMinBufSize, AudioTrack.MODE_STREAM);// 设置模式类型，在这里设置为流类型
    }
}
