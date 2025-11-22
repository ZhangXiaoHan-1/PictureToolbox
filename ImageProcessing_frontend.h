// ImageProcessing_frontend.h
#pragma once
#include <string>
#include <functional>
#include "Image_backend.h"

class ImageProcessingFrontend {
private:
    ImageBackend* image_processor;
    BaseModelBackend* model_processor;

public:
    ImageProcessingFrontend();
    ~ImageProcessingFrontend();
    
    // 图像文件操作
    bool load_image(const std::string& file_path);
    bool save_image(const std::string& save_path);
    
    // 图像处理操作
    void apply_grayscale();
    void apply_mean_filter(int kernel_size = 3);
    void apply_edge_detection();
    void apply_gamma_transform(double gamma);
    void apply_binary_threshold(int threshold);
    void adjust_brightness(int value);
    void adjust_contrast(double contrast);
    void adjust_saturation(double saturation);
    void adjust_rgb_values(double r, double g, double b);
    void apply_mosaic(int block_size);
    void resize_image(int width, int height);
    
    // 获取处理后的图像数据（用于显示）
    cv::Mat get_current_image() const;
    
    // 回调函数类型定义
    using ProgressCallback = std::function<void(int progress)>;
    using CompletionCallback = std::function<void(bool success, const std::string& message)>;
    
    void set_progress_callback(ProgressCallback callback);
    void set_completion_callback(CompletionCallback callback);

private:
    ProgressCallback progress_callback;
    CompletionCallback completion_callback;
    
    void update_progress(int progress);
    void notify_completion(bool success, const std::string& message);
};
