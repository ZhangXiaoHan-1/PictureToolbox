// ImageProcessingService_backend.h
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>
#include <torch/torch.h>
#include <torch/script.h>

class ImageProcessingBackend {
public:
    ImageProcessingBackend();
    ~ImageProcessingBackend();

    // 图像加载和保存
    bool load_image(const std::string& file_path);
    bool save_image(const std::string& save_path);
    cv::Mat get_current_image() const;
    
    // 基本图像处理
    cv::Mat convert_to_grayscale() const;
    cv::Mat apply_mean_filter(int kernel_size = 3) const;
    cv::Mat apply_edge_detection() const;
    cv::Mat apply_gamma_transform(double gamma = 1.2) const;
    cv::Mat apply_binary_threshold(int threshold = 128) const;
    
    // 图像调整
    cv::Mat adjust_brightness(int value) const;
    cv::Mat adjust_contrast(double contrast) const;
    cv::Mat adjust_saturation(double saturation) const;
    cv::Mat adjust_rgb_values(double r, double g, double b) const;
    
    // 几何变换
    cv::Mat rotate_image(double angle) const;
    cv::Mat mirror_image(bool horizontal, bool vertical) const;
    cv::Mat resize_image(int width, int height) const;
    
    // 特殊效果
    cv::Mat apply_mosaic(int block_size) const;
    cv::Mat apply_watermark(const cv::Mat& watermark) const;
    
    // 超分辨率
    cv::Mat apply_bicubic_upscale(double scale = 2.0) const;
    cv::Mat apply_super_resolution();
    
    // 状态管理
    std::string get_current_image_path() const;
    bool is_image_loaded() const;
    
    // 回调函数
    using ProgressCallback = std::function<void(int progress, const std::string& message)>;
    void set_progress_callback(ProgressCallback callback);

private:
    cv::Mat current_image;
    cv::Mat original_image;
    std::string current_image_path;
    
    torch::jit::script::Module sr_model;
    bool model_loaded;
    
    ProgressCallback progress_callback;
    
    void update_progress(int progress, const std::string& message = "");
    cv::Mat tensor_to_mat(const at::Tensor& tensor);
    at::Tensor mat_to_tensor(const cv::Mat& mat);
    bool load_sr_model(const std::string& model_path);
};
