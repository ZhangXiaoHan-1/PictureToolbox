// ImageProcessing_service.h
#pragma once
#include <string>
#include <memory>
#include "Image_backend.h"
#include "BaseModel_backend.h"

class ImageProcessingService {
private:
    std::unique_ptr<ImageBackend> image_processor;
    std::unique_ptr<BaseModelBackend> model_processor;

public:
    ImageProcessingService();
    
    // RESTful API 风格的方法
    struct ProcessingRequest {
        std::string operation;
        std::string image_data; // Base64 encoded
        std::string parameters; // JSON string
    };
    
    struct ProcessingResponse {
        bool success;
        std::string message;
        std::string processed_image; // Base64 encoded
    };
    
    ProcessingResponse process_image(const ProcessingRequest& request);
    bool load_model(const std::string& model_path);
    
private:
    cv::Mat base64_to_mat(const std::string& base64_data);
    std::string mat_to_base64(const cv::Mat& image);
};
