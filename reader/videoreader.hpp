/*
 * VideoReader.hpp
 *
 *  Created on: 02-Sep-2019
 *      Author: Pijush Barik (pijush.barik8@gmail.com)
 */

/**
 * @file videoreader.hpp
 * @brief Reads video from a video stream input file, http mjpeg feed, usb camera,
 * ip camera etc.
 *
 */
#ifndef VIDEOREADER_HPP_
#define VIDEOREADER_HPP_

// FFMPEG headers
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

// OpenCV headers
#include <opencv2/core.hpp>

using namespace std;

class VideoReader {
    AVFormatContext *inctx;     // input format context
    AVCodecContext *vctx;       // input video codec context
    AVPixelFormat destPixFmt;   // output pixel format
    SwsContext *swsctx;         // scaling context
    AVFrame *frame;             // a single video frame
    uint8_t *framebuf;          // frame buffer
    AVFrame *decframe;          // a single frame, decoded
    AVPacket pkt;               // a stream packet
    int vstremIdx;              // index of the video stream in the input
    bool endOfStream;           // indicates end of input stream
    int gotFrame;               // indicates if a frame is found
    bool opened;                // indicates if input is successfully opened or 
                                // not
    int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, 
        AVPacket *pkt);         // decodes a video stream packet
    void init();                // class member initializer
public:
    uint rows,  ///< Number of rows in each frame in the video file
        cols,   ///< Number of columns in each frame in the video file
        frames; ///< Total number of frames in the video file

    /**
     * @brief Construct a new Video Reader object
     *
     */
    VideoReader();
    /**
     * @brief Construct a new Video Reader object for a device like WebCam
     * @param device    Device id
     */
    VideoReader(int device);
    /**
     * @brief Construct a new Video Reader object for a file
     * @param input  Video stream input path
     */
    VideoReader(string input);
    /**
     * @brief Destroy the Video Reader object
     *
     */
    ~VideoReader();
    /**
     * @brief Opens a video file to read. Same as \ref VideoReder(string)
     *
     */
    void open(string input);
    /**
     * @brief Opens a device to read video stream. Same as \ref VideReader(int)
     *
     */
    void open(int device);
    /**
     * @brief Check if the video file or device is successfully opened
     *
     * @return true if the the opened successfully. false otherwise
     */
    bool isOpened();
    /**
     * @brief Reads the next frame and stores the frame into the \p mat. It is 
     * recommended to use the returned value to check if the frame is read 
     * successfully or not, rather than checking mat.isEmpty(), because on
     * failure the mat object is not released or emptied. Only zero is retured.
     * 
     * @param mat   Read frame
     * @return int  Returns a non-zero value if frame is read successfully, zero
     * otherwise
     */
    int read(cv::Mat &mat);
};


#endif /* VIDEOREADER_HPP_ */
