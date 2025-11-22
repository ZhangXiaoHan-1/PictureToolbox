// ImageConverter_frontend.h
#pragma once
#include <QImage>
#include <opencv2/opencv.hpp>

class ImageConverter {
public:
    static QImage cvMatToQImage(const cv::Mat& mat);
    static cv::Mat QImageToCvMat(const QImage& qimage);
    static QImage resizeImageForDisplay(const QImage& image, const QSize& target_size);
};
