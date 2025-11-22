// Image_backend.h
#pragma once
#define NOMINMAX
#undef min
#undef max
#include <string>
#include <opencv2/opencv.hpp>
#include <torch/torch.h>
#include <torch/script.h>

class ImageBackend {
private:
    std::string image_dir;
    cv::Mat feature_mat;
    at::Tensor feature;

    void load_mat();
    void ToTensor();

public:
    ImageBackend();
    ImageBackend(std::string dir);
    
    std::string get_image_dir();
    at::Tensor get_feature() const;
    cv::Mat get_feature_mat() const;
    
    void load();
    void load(std::string path);
    void save(std::string save_dir);
    void resize(int h, int w);
    
    // 图像处理核心方法
    cv::Mat convert_to_grayscale() const;
    cv::Mat apply_mean_filter(int kernel_size = 3) const;
    cv::Mat apply_edge_detection() const;
    cv::Mat apply_gamma_transform(double gamma) const;
    cv::Mat apply_binary_threshold(int threshold) const;
    cv::Mat adjust_brightness(int value) const;
    cv::Mat adjust_contrast(double contrast) const;
    cv::Mat adjust_saturation(double saturation) const;
    cv::Mat adjust_rgb_values(double r, double g, double b) const;
    cv::Mat apply_mosaic(int block_size) const;
};
