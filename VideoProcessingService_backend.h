// VideoProcessingService_backend.h
#pragma once
#include <string>
#include <functional>
#include <opencv2/opencv.hpp>

class VideoProcessingBackend {
public:
    VideoProcessingBackend();
    ~VideoProcessingBackend();
    
    // 视频文件操作
    bool loadVideo(const std::string& file_path);
    void closeVideo();
    
    // 播放控制
    bool play();
    bool pause();
    bool stop();
    bool seek(int frame_number);
    
    // 视频处理
    enum ProcessingType {
        NONE = 0,
        GRAYSCALE = 1,
        EDGE_DETECTION = 2,
        SMOOTH = 3,
        BINARY = 4,
        MOSAIC = 5
    };
    
    cv::Mat processFrame(ProcessingType type, const cv::Mat& frame);
    cv::Mat getCurrentFrame();
    
    // 视频信息
    int getTotalFrames() const;
    int getCurrentFrameNumber() const;
    double getFPS() const;
    int getVideoWidth() const;
    int getVideoHeight() const;
    double getDuration() const;
    
    // 状态管理
    bool isPlaying() const;
    bool isVideoLoaded() const;
    
    // 回调函数
    using FrameCallback = std::function<void(const cv::Mat& frame, int frame_number, double current_time)>;
    using ProgressCallback = std::function<void(int progress, const std::string& message)>;
    
    void setFrameCallback(FrameCallback callback);
    void setProgressCallback(ProgressCallback callback);

private:
    cv::VideoCapture video_capture;
    bool is_playing_;
    int current_frame_;
    ProcessingType processing_type_;
    double video_fps_;
    int total_frames_;
    
    FrameCallback frame_callback;
    ProgressCallback progress_callback;
    
    void updateProgress(int progress, const std::string& message = "");
    cv::Mat applyMosaic(const cv::Mat& frame, int block_size = 10);
};
