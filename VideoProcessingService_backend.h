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
    bool load_video(const std::string& file_path);
    void close_video();
    
    // 播放控制
    bool play();
    bool pause();
    bool stop();
    bool seek(int frame_number);
    
    // 视频处理
    cv::Mat get_current_frame();
    cv::Mat process_frame(int processing_type, const cv::Mat& frame);
    
    // 视频信息
    int get_total_frames() const;
    int get_current_frame_number() const;
    double get_fps() const;
    int get_video_width() const;
    int get_video_height() const;
    
    // 状态管理
    bool is_playing() const;
    bool is_video_loaded() const;
    
    // 回调函数
    using FrameCallback = std::function<void(const cv::Mat& frame, int frame_number)>;
    using ProgressCallback = std::function<void(int progress, const std::string& message)>;
    
    void set_frame_callback(FrameCallback callback);
    void set_progress_callback(ProgressCallback callback);

private:
    cv::VideoCapture video_capture;
    bool is_playing_;
    int current_frame_;
    int processing_type_;
    
    FrameCallback frame_callback;
    ProgressCallback progress_callback;
    
    void update_progress(int progress, const std::string& message = "");
};
