// ImageProcessingService_backend.cpp
#include "ImageProcessingService_backend.h"
#include <thread>
#include <chrono>

ImageProcessingBackend::ImageProcessingBackend() : model_loaded(false) {
    // 加载超分辨率模型
    std::string model_path = "E:\\QT5-Image-and-video-processing-software-master\\LSRN.pt";
    model_loaded = load_sr_model(model_path);
}

ImageProcessingBackend::~ImageProcessingBackend() {
}

bool ImageProcessingBackend::load_image(const std::string& file_path) {
    try {
        cv::Mat image = cv::imread(file_path);
        if (image.empty()) {
            update_progress(0, "Failed to load image: File is empty or corrupted");
            return false;
        }
        
        current_image = image.clone();
        original_image = image.clone();
        current_image_path = file_path;
        
        update_progress(100, "Image loaded successfully");
        return true;
    } catch (const std::exception& e) {
        update_progress(0, std::string("Failed to load image: ") + e.what());
        return false;
    }
}

bool ImageProcessingBackend::save_image(const std::string& save_path) {
    if (current_image.empty()) {
        update_progress(0, "No image to save");
        return false;
    }
    
    try {
        bool success = cv::imwrite(save_path, current_image);
        if (success) {
            update_progress(100, "Image saved successfully");
        } else {
            update_progress(0, "Failed to save image");
        }
        return success;
    } catch (const std::exception& e) {
        update_progress(0, std::string("Failed to save image: ") + e.what());
        return false;
    }
}

cv::Mat ImageProcessingBackend::get_current_image() const {
    return current_image.clone();
}

cv::Mat ImageProcessingBackend::convert_to_grayscale() const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat gray;
    if (current_image.channels() == 3) {
        cv::cvtColor(current_image, gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(gray, gray, cv::COLOR_GRAY2BGR); // Convert back to 3 channels for consistency
    } else {
        gray = current_image.clone();
    }
    return gray;
}

cv::Mat ImageProcessingBackend::apply_mean_filter(int kernel_size) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat filtered;
    cv::blur(current_image, filtered, cv::Size(kernel_size, kernel_size));
    return filtered;
}

cv::Mat ImageProcessingBackend::apply_edge_detection() const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat gray, edges;
    if (current_image.channels() == 3) {
        cv::cvtColor(current_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = current_image.clone();
    }
    
    cv::Canny(gray, edges, 50, 150);
    cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR);
    return edges;
}

cv::Mat ImageProcessingBackend::apply_gamma_transform(double gamma) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    cv::Mat lookup_table(1, 256, CV_8U);
    uchar* p = lookup_table.ptr();
    for (int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
    }
    cv::LUT(current_image, lookup_table, result);
    return result;
}

cv::Mat ImageProcessingBackend::apply_binary_threshold(int threshold) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat gray, binary;
    if (current_image.channels() == 3) {
        cv::cvtColor(current_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = current_image.clone();
    }
    
    cv::threshold(gray, binary, threshold, 255, cv::THRESH_BINARY);
    cv::cvtColor(binary, binary, cv::COLOR_GRAY2BGR);
    return binary;
}

cv::Mat ImageProcessingBackend::adjust_brightness(int value) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    current_image.convertTo(result, -1, 1, value);
    return result;
}

cv::Mat ImageProcessingBackend::adjust_contrast(double contrast) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    current_image.convertTo(result, -1, contrast, 0);
    return result;
}

cv::Mat ImageProcessingBackend::adjust_saturation(double saturation) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat hsv, result;
    cv::cvtColor(current_image, hsv, cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    channels[1] = channels[1] * saturation;
    cv::merge(channels, hsv);
    cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
    return result;
}

cv::Mat ImageProcessingBackend::adjust_rgb_values(double r, double g, double b) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    std::vector<cv::Mat> channels;
    cv::split(current_image, channels);
    channels[2] = channels[2] * (1.0 + r/255.0); // R
    channels[1] = channels[1] * (1.0 + g/255.0); // G
    channels[0] = channels[0] * (1.0 + b/255.0); // B
    cv::merge(channels, result);
    return result;
}

cv::Mat ImageProcessingBackend::rotate_image(double angle) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    cv::Point2f center(current_image.cols/2.0, current_image.rows/2.0);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::warpAffine(current_image, result, rotation_matrix, current_image.size());
    return result;
}

cv::Mat ImageProcessingBackend::mirror_image(bool horizontal, bool vertical) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    int flip_code = 0;
    if (horizontal && vertical) flip_code = -1;
    else if (horizontal) flip_code = 1;
    else if (vertical) flip_code = 0;
    
    cv::flip(current_image, result, flip_code);
    return result;
}

cv::Mat ImageProcessingBackend::resize_image(int width, int height) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    cv::resize(current_image, result, cv::Size(width, height));
    return result;
}

cv::Mat ImageProcessingBackend::apply_mosaic(int block_size) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result = current_image.clone();
    for (int i = 0; i < result.rows; i += block_size) {
        for (int j = 0; j < result.cols; j += block_size) {
            cv::Rect block(j, i, std::min(block_size, result.cols - j), 
                          std::min(block_size, result.rows - i));
            cv::Scalar color = cv::mean(result(block));
            cv::rectangle(result, block, color, -1);
        }
    }
    return result;
}

cv::Mat ImageProcessingBackend::apply_bicubic_upscale(double scale) const {
    if (current_image.empty()) return cv::Mat();
    
    cv::Mat result;
    cv::Size new_size(current_image.cols * scale, current_image.rows * scale);
    cv::resize(current_image, result, new_size, 0, 0, cv::INTER_CUBIC);
    return result;
}

cv::Mat ImageProcessingBackend::apply_super_resolution() {
    if (current_image.empty() || !model_loaded) return cv::Mat();
    
    try {
        at::Tensor input_tensor = mat_to_tensor(current_image);
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(input_tensor);
        
        at::Tensor output_tensor = sr_model.forward(inputs).toTensor();
        cv::Mat result = tensor_to_mat(output_tensor);
        
        return result;
    } catch (const std::exception& e) {
        update_progress(0, std::string("Super resolution failed: ") + e.what());
        return cv::Mat();
    }
}

std::string ImageProcessingBackend::get_current_image_path() const {
    return current_image_path;
}

bool ImageProcessingBackend::is_image_loaded() const {
    return !current_image.empty();
}

void ImageProcessingBackend::set_progress_callback(ProgressCallback callback) {
    progress_callback = callback;
}

void ImageProcessingBackend::update_progress(int progress, const std::string& message) {
    if (progress_callback) {
        progress_callback(progress, message);
    }
}

bool ImageProcessingBackend::load_sr_model(const std::string& model_path) {
    try {
        sr_model = torch::jit::load(model_path);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

cv::Mat ImageProcessingBackend::tensor_to_mat(const at::Tensor& tensor) {
    // Implementation for tensor to Mat conversion
    return cv::Mat();
}

at::Tensor ImageProcessingBackend::mat_to_tensor(const cv::Mat& mat) {
    // Implementation for Mat to tensor conversion
    return at::Tensor();
}
