// ImageProcessing_frontend.cpp
#include "ImageProcessing_frontend.h"
#include <thread>
#include <chrono>

ImageProcessingFrontend::ImageProcessingFrontend() 
    : image_processor(nullptr), model_processor(nullptr) {
}

ImageProcessingFrontend::~ImageProcessingFrontend() {
    delete image_processor;
    delete model_processor;
}

bool ImageProcessingFrontend::load_image(const std::string& file_path) {
    try {
        if (image_processor) {
            delete image_processor;
        }
        image_processor = new ImageBackend(file_path);
        update_progress(100);
        notify_completion(true, "Image loaded successfully");
        return true;
    } catch (const std::exception& e) {
        notify_completion(false, std::string("Failed to load image: ") + e.what());
        return false;
    }
}

bool ImageProcessingFrontend::save_image(const std::string& save_path) {
    if (!image_processor) {
        notify_completion(false, "No image loaded");
        return false;
    }
    
    try {
        image_processor->save(save_path);
        notify_completion(true, "Image saved successfully");
        return true;
    } catch (const std::exception& e) {
        notify_completion(false, std::string("Failed to save image: ") + e.what());
        return false;
    }
}

void ImageProcessingFrontend::apply_grayscale() {
    if (!image_processor) return;
    
    // 模拟进度更新
    for (int i = 0; i <= 100; i += 10) {
        update_progress(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    cv::Mat result = image_processor->convert_to_grayscale();
    // 这里应该更新显示，具体实现取决于前端框架
    notify_completion(true, "Grayscale conversion completed");
}

// 其他图像处理方法的实现类似...
// 这里省略具体实现以保持简洁

cv::Mat ImageProcessingFrontend::get_current_image() const {
    if (image_processor) {
        return image_processor->get_feature_mat();
    }
    return cv::Mat();
}

void ImageProcessingFrontend::set_progress_callback(ProgressCallback callback) {
    progress_callback = callback;
}

void ImageProcessingFrontend::set_completion_callback(CompletionCallback callback) {
    completion_callback = callback;
}

void ImageProcessingFrontend::update_progress(int progress) {
    if (progress_callback) {
        progress_callback(progress);
    }
}

void ImageProcessingFrontend::notify_completion(bool success, const std::string& message) {
    if (completion_callback) {
        completion_callback(success, message);
    }
}
