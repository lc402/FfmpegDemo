package com.borqs.audio;

/**
 * Created by b715 on 16-7-22.
 */
public class Ffmpeg {
    public Ffmpeg() {

        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg_codec");
    }

    public native int avcodecFindDecoder(int codecID);

    public native int audioDecode(String inputPath,String outputPath);

    public native int create();

    public native void setFilePath(String path);

    public native byte[] getFrameByte();

    public native int start();

    public native int stop();

    public native int pause();

    public native int resume();

    public native int seekPosition(int position);



    public native int getAudioSamplerate();
    public native void initAudioDecodec(String path);
    public native byte[] getDecodecData();
}
