// VideoProcessingService_backend.cpp
#include "VideoProcessingService_backend.h"
#include <thread>

VideoProcessingBackend::VideoProcessingBackend() 
    : is_playing_(false), current_frame_(0), processing_type_(NONE), 
      video_fps_(0), total_frames_(0) {
}

VideoProcessingBackend::~VideoProcessingBackend() {
    closeVideo();
}

bool VideoProcessingBackend::loadVideo(const std::string& file_path) {
    try {
        video_capture.open(file_path);
        if (!video_capture.isOpened()) {
            updateProgress(0, "Failed to open video file");
            return false;
        }
        
        total_frames_ = static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_COUNT));
        video_fps_ = video_capture.get(cv::CAP_PROP_FPS);
        current_frame_ = 0;
        
        updateProgress(100, "Video loaded successfully");
        return true;
    } catch (const std::exception& e) {
        updateProgress(0, std::string("Failed to load video: ") + e.what());
        return false;
    }
}

void VideoProcessingBackend::closeVideo() {
    if (video_capture.isOpened()) {
        video_capture.release();
    }
    is_playing_ = false;
}

bool VideoProcessingBackend::play() {
    if (!video_capture.isOpened()) {
        return false;
    }
    is_playing_ = true;
    return true;
}

bool VideoProcessingBackend::pause() {
    is_playing_ = false;
    return true;
}

bool VideoProcessingBackend::stop() {
    is_playing_ = false;
    if (video_capture.isOpened()) {
        video_capture.set(cv::CAP_PROP_POS_FRAMES, 0);
        current_frame_ = 0;
    }
    return true;
}

bool VideoProcessingBackend::seek(int frame_number) {
    if (!video_capture.isOpened()) {
        return false;
    }
    
    if (frame_number >= 0 && frame_number < total_frames_) {
        video_capture.set(cv::CAP_PROP_POS_FRAMES, frame_number);
        current_frame_ = frame_number;
        return true;
    }
    return false;
}

cv::Mat VideoProcessingBackend::processFrame(ProcessingType type, const cv::Mat& frame) {
    if (frame.empty()) return cv::Mat();
    
    cv::Mat result = frame.clone();
    
    switch (type) {
        case GRAYSCALE:
            if (result.channels() == 3) {
                cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
                cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            }
            break;
            
        case EDGE_DETECTION: {
            cv::Mat gray;
            if (result.channels() == 3) {
                cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = result.clone();
            }
            cv::Canny(gray, result, 50, 150);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            break;
        }
            
        case SMOOTH:
            cv::GaussianBlur(result, result, cv::Size(3, 3), 0);
            break;
            
        case BINARY: {
            cv::Mat gray;
            if (result.channels() == 3) {
                cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = result.clone();
            }
            cv::threshold(gray, result, 96, 255, cv::THRESH_BINARY);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            break;
        }
            
        case MOSAIC:
            result = applyMosaic(result);
            break;
            
        case NONE:
        default:
            // 不处理，保持原图
            break;
    }
    
    return result;
}

cv::Mat VideoProcessingBackend::getCurrentFrame() {
    if (!video_capture.isOpened()) {
        return cv::Mat();
    }
    
    cv::Mat frame;
    if (video_capture.read(frame)) {
        current_frame_ = static_cast<int>(video_capture.get(cv::CAP_PROP_POS_FRAMES));
        
        // 应用当前处理类型
        frame = processFrame(processing_type_, frame);
        
        // 通知回调
        if (frame_callback) {
            double current_time = current_frame_ / video_fps_;
            frame_callback(frame, current_frame_, current_time);
        }
        
        return frame;
    }
    
    // 视频结束
    if (current_frame_ >= total_frames_ - 1) {
        stop();
    }
    
    return cv::Mat();
}

int VideoProcessingBackend::getTotalFrames() const {
    return total_frames_;
}

int VideoProcessingBackend::getCurrentFrameNumber() const {
    return current_frame_;
}

double VideoProcessingBackend::getFPS() const {
    return video_fps_;
}

int VideoProcessingBackend::getVideoWidth() const {
    if (video_capture.isOpened()) {
        return static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_WIDTH));
    }
    return 0;
}

int VideoProcessingBackend::getVideoHeight() const {
    if (video_capture.isOpened()) {
        return static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    }
    return 0;
}

double VideoProcessingBackend::getDuration() const {
    if (video_fps_ > 0) {
        return total_frames_ / video_fps_;
    }
    return 0;
}

bool VideoProcessingBackend::isPlaying() const {
    return is_playing_;
}

bool VideoProcessingBackend::isVideoLoaded() const {
    return video_capture.isOpened();
}

void VideoProcessingBackend::setFrameCallback(FrameCallback callback) {
    frame_callback = callback;
}

void VideoProcessingBackend::setProgressCallback(ProgressCallback callback) {
    progress_callback = callback;
}

void VideoProcessingBackend::updateProgress(int progress, const std::string& message) {
    if (progress_callback) {
        progress_callback(progress, message);
    }
}

cv::Mat VideoProcessingBackend::applyMosaic(const cv::Mat& frame, int block_size) {
    cv::Mat result = frame.clone();
    int width = result.rows;
    int height = result.cols;

    for (int i = width/3; i < width*2/3; i += block_size) {
        for (int j = height/3; j < height*2/3; j += block_size) {
            cv::Rect block(j, i, 
                          std::min(block_size, height - j), 
                          std::min(block_size, width - i));
            cv::Scalar color = cv::mean(result(block));
            cv::rectangle(result, block, color, -1);
        }
    }
    return result;
}
