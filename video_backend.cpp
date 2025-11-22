#include "video_backend.h"

namespace Backend {

VideoProcessingEngine::VideoProcessingEngine(QObject *parent) : QObject(parent),
    m_processingEnabled(true)
{
}

VideoProcessingEngine::~VideoProcessingEngine()
{
}

void VideoProcessingEngine::setProcessingEnabled(bool enabled)
{
    m_processingEnabled = enabled;
}

bool VideoProcessingEngine::isProcessingEnabled() const
{
    return m_processingEnabled;
}

void VideoProcessingEngine::processFrame(const cv::Mat& frame)
{
    if (!m_processingEnabled || frame.empty()) {
        emit frameProcessed(frame);
        return;
    }

    try {
        cv::Mat processedFrame = applyImageProcessing(frame);
        emit frameProcessed(processedFrame);
    } catch (const std::exception& e) {
        emit processingError(QString("Video processing error: %1").arg(e.what()));
        // 出错时返回原帧
        emit frameProcessed(frame);
    }
}

cv::Mat VideoProcessingEngine::applyImageProcessing(const cv::Mat& frame)
{
    // 这里可以添加各种图像处理算法
    // 目前只是简单返回原图，实际应用中会调用深度学习模型等
    cv::Mat processed = frame.clone();
    
    // 示例：简单的图像处理（可以替换为实际的模型推理）
    // cv::cvtColor(frame, processed, cv::COLOR_RGB2GRAY);
    // cv::cvtColor(processed, processed, cv::COLOR_GRAY2RGB);
    
    return processed;
}

} // namespace Backend
