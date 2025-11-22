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

    // 图像文件操作
    bool loadImage(const std::string& file_path);
    bool saveImage(const std::string& save_path);
    cv::Mat getCurrentImage() const;
    std::string getCurrentImagePath() const;
    bool isImageLoaded() const;

    // 基本图像处理
    cv::Mat convertToGrayscale() const;
    cv::Mat applyMeanFilter(int kernel_size = 3) const;
    cv::Mat applyEdgeDetection() const;
    cv::Mat applyGammaTransform(double gamma = 1.2) const;
    cv::Mat applyBinaryThreshold(int threshold = 128) const;
    
    // 图像调整
    cv::Mat adjustBrightness(int value) const;
    cv::Mat adjustContrast(double contrast) const;
    cv::Mat adjustSaturation(double saturation) const;
    cv::Mat adjustRGBValues(int r, int g, int b) const;
    
    // 几何变换
    cv::Mat rotateImage(double angle) const;
    cv::Mat mirrorImage(bool horizontal, bool vertical) const;
    cv::Mat resizeImage(double scale) const;
    
    // 复合操作
    cv::Mat blendWithEdgeDetection(double ratio) const;
    
    // 超分辨率
    cv::Mat applyBicubicUpscale(double scale = 2.0) const;
    cv::Mat applySuperResolution();
    
    // 状态管理
    void setCurrentImage(const cv::Mat& image);
    
    // 回调函数
    using ProgressCallback = std::function<void(int progress, const std::string& message)>;
    void setProgressCallback(ProgressCallback callback);

private:
    cv::Mat current_image;
    cv::Mat original_image;
    std::string current_image_path;
    
    torch::jit::script::Module sr_model;
    bool model_loaded;
    
    ProgressCallback progress_callback;
    
    void updateProgress(int progress, const std::string& message = "");
    bool loadSRModel(const std::string& model_path);
    cv::Mat tensorToMat(const at::Tensor& tensor);
    at::Tensor matToTensor(const cv::Mat& mat);
};
