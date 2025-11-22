// Image_backend.cpp
#define NOMINMAX
#undef min
#undef max
#include "Image_backend.h"

ImageBackend::ImageBackend() {
    ;
}

ImageBackend::ImageBackend(std::string dir) {
    this->image_dir = dir;
    this->load();
}

std::string ImageBackend::get_image_dir() {
    return this->image_dir;
}

at::Tensor ImageBackend::get_feature() const {
    return this->feature;
}

cv::Mat ImageBackend::get_feature_mat() const {
    return this->feature_mat;
}

void ImageBackend::load() {
    this->load_mat();
    this->ToTensor();
}

void ImageBackend::load(std::string path) {
    this->image_dir = path;
    this->load();
}

void ImageBackend::ToTensor() {
    cv::Mat img = this->feature_mat.clone();
    int h = img.rows;
    int w = img.cols;
    int channels = img.channels();

    at::Tensor img_t = torch::from_blob(img.data, { 1, h, w, channels }, at::kByte);
    img_t = img_t.permute({ 0, 3, 1, 2 });
    img_t = img_t.toType(c10::kFloat).div(255.0);
    img_t.to(c10::DeviceType::CPU);

    this->feature = img_t;
}

void ImageBackend::load_mat() {
    this->feature_mat = cv::imread(this->image_dir);
}

void ImageBackend::save(std::string save_dir) {
    cv::imwrite(save_dir, this->feature_mat);
}

void ImageBackend::resize(int h, int w) {
    cv::resize(this->feature_mat, this->feature_mat, cv::Size(w, h), 0, 0, cv::INTER_AREA);
    this->ToTensor();
}

// 图像处理方法实现
cv::Mat ImageBackend::convert_to_grayscale() const {
    cv::Mat gray;
    cv::cvtColor(this->feature_mat, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

cv::Mat ImageBackend::apply_mean_filter(int kernel_size) const {
    cv::Mat filtered;
    cv::blur(this->feature_mat, filtered, cv::Size(kernel_size, kernel_size));
    return filtered;
}

cv::Mat ImageBackend::apply_edge_detection() const {
    cv::Mat gray, edges;
    cv::cvtColor(this->feature_mat, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 50, 150);
    return edges;
}

cv::Mat ImageBackend::apply_gamma_transform(double gamma) const {
    cv::Mat result;
    cv::Mat lookup_table(1, 256, CV_8U);
    uchar* p = lookup_table.ptr();
    for (int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
    }
    cv::LUT(this->feature_mat, lookup_table, result);
    return result;
}

cv::Mat ImageBackend::apply_binary_threshold(int threshold) const {
    cv::Mat gray, binary;
    cv::cvtColor(this->feature_mat, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, threshold, 255, cv::THRESH_BINARY);
    return binary;
}

cv::Mat ImageBackend::adjust_brightness(int value) const {
    cv::Mat result;
    this->feature_mat.convertTo(result, -1, 1, value);
    return result;
}

cv::Mat ImageBackend::adjust_contrast(double contrast) const {
    cv::Mat result;
    this->feature_mat.convertTo(result, -1, contrast, 0);
    return result;
}

cv::Mat ImageBackend::adjust_saturation(double saturation) const {
    cv::Mat hsv, result;
    cv::cvtColor(this->feature_mat, hsv, cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    channels[1] = channels[1] * saturation;
    cv::merge(channels, hsv);
    cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
    return result;
}

cv::Mat ImageBackend::adjust_rgb_values(double r, double g, double b) const {
    cv::Mat result;
    std::vector<cv::Mat> channels;
    cv::split(this->feature_mat, channels);
    channels[2] = channels[2] * r; // R
    channels[1] = channels[1] * g; // G
    channels[0] = channels[0] * b; // B
    cv::merge(channels, result);
    return result;
}

cv::Mat ImageBackend::apply_mosaic(int block_size) const {
    cv::Mat result = this->feature_mat.clone();
    for (int i = 0; i < result.rows; i += block_size) {
        for (int j = 0; j < result.cols; j += block_size) {
            cv::Rect block(j, i, block_size, block_size);
            cv::Scalar color = cv::mean(result(block));
            cv::rectangle(result, block, color, -1);
        }
    }
    return result;
}
