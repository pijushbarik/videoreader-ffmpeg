#include "videoreader.hpp"

int VideoReader::decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, 
    AVPacket *pkt) {
    int ret;
    *got_frame = 0;
    if(pkt) {
        ret = avcodec_send_packet(avctx, pkt);
        if(ret < 0) {
            return ret == AVERROR_EOF ? 0 : ret;
        }
    }
    ret = avcodec_receive_frame(avctx, frame);
    if(ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        return ret;
    }
    if(ret >= 0) {
        *got_frame = 1;
    }
    return 0;
}

void VideoReader::open(string input) {
    int ret;
    // open input
    ret = avformat_open_input(&inctx, input.c_str(), nullptr, nullptr);
    if(ret < 0) {
        perror("Couldn't open video stream");
        exit(EXIT_FAILURE);
    }

    // retrieve input stream information
    ret = avformat_find_stream_info(inctx, nullptr);
    if(ret < 0) {
        printf("No video stream found in the input");
        exit(EXIT_FAILURE);
    }

    // fine primary video stream and find the codec information
    AVCodec* vcodec = nullptr;
    ret = av_find_best_stream(inctx, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);
    if(ret < 0) {
        printf("No video stream found in the input");
        exit(EXIT_FAILURE);
    }
    vstremIdx = ret;
    AVStream* vstrm = inctx->streams[vstremIdx];

    // create context from video codec information
    vctx = avcodec_alloc_context3(vcodec);
    if(!vctx) {
        printf("Unable to allocate video context");
        exit(EXIT_FAILURE);
    }

    ret = avcodec_parameters_to_context(vctx, vstrm->codecpar);
    if(ret < 0) {
        printf("Unable to create context from video codec information");
        exit(EXIT_FAILURE);
    }

    ret = avcodec_open2(vctx, vcodec, nullptr);
    if(ret < 0) {
        printf("Unable to open video stram");
        exit(EXIT_FAILURE);
    }
    
    // set scaling context
    swsctx = sws_getCachedContext(
        nullptr, vstrm->codecpar->width, vstrm->codecpar->height, vctx->pix_fmt,
        vstrm->codecpar->width, vstrm->codecpar->height, destPixFmt, SWS_BICUBIC, 
        nullptr, nullptr, nullptr);
    if(!swsctx) {
        perror("Failed to allocate scale context");
        exit(EXIT_FAILURE);
    }
    
    // set video information data members
    rows    = vstrm->codecpar->height;  // number of rows of each frame
    cols    = vstrm->codecpar->width;   // number of columns of each frame
    frames  = vstrm->nb_frames;         // number of frames

    // allocate frame buffer for output
    frame = av_frame_alloc();
    framebuf = (uint8_t *) av_malloc(av_image_get_buffer_size(destPixFmt, cols, 
        rows, 1) * sizeof(uint8_t));
    av_image_fill_arrays(frame->data, frame->linesize, framebuf, destPixFmt, 
        cols, rows, 1);
    
    // allocate decoding frame
    decframe = av_frame_alloc();

    opened  = true; // video opened successfully
}

void VideoReader::open(int device) {
    // TODO: test and implement for usb cams
}

void VideoReader::init() {
    rows = cols = frames = 0;
    opened = false;
    inctx = nullptr;
    vctx = nullptr;
    destPixFmt = AV_PIX_FMT_BGR24;
    endOfStream = false;
    gotFrame = 0;
    vstremIdx = 0;
    // av_log_set_level(AV_LOG_DEBUG);
}

VideoReader::VideoReader() {
    init();
}

VideoReader::~VideoReader() {
    av_frame_free(&decframe);
    av_frame_free(&frame);
    avcodec_close(vctx);
    avformat_close_input(&inctx);
    av_free(framebuf);
}

VideoReader::VideoReader(int device) {
    init();
    open(device);
}

VideoReader::VideoReader(string input) {
    init();
    open(input);
}

int VideoReader::read(cv::Mat &mat) {
    int ret;

    do {
        if(!endOfStream) {
            // read packet from input
            ret = av_read_frame(inctx, &pkt);
            if(ret < 0 && ret != AVERROR_EOF) {
                printf("Failed to read frame");
                return 0;
            }
            if(ret == 0 && pkt.stream_index != vstremIdx) {
                endOfStream = 1;
                av_packet_unref(&pkt);
            }
            else endOfStream = (ret == AVERROR_EOF);
        } 
        if(endOfStream) {
            av_init_packet(&pkt);
            pkt.data = nullptr;
            pkt.size = 0;
        }

        // decode video frame
        ret = decode(vctx, decframe, &gotFrame, &pkt);
        if(ret < 0) {
            printf("Failed to decode frame");
            exit(EXIT_FAILURE);
        }
        if(!gotFrame) av_packet_unref(&pkt);
        else {
            // scale the decoded frame
            sws_scale(swsctx, decframe->data, decframe->linesize, 0, 
                decframe->height, frame->data, frame->linesize);

            // copy data to Matrix object
            mat = cv::Mat(rows, cols, CV_8UC3, (void*)framebuf, 
                frame->linesize[0]);

            av_packet_unref(&pkt);

            return gotFrame;
        }
    } while(!endOfStream || gotFrame);

    return gotFrame;
}

bool VideoReader::isOpened() { return opened; }
