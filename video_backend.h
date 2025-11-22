#ifndef VIDEO_BACKEND_H
#define VIDEO_BACKEND_H

#include <QObject>
#include <opencv2/core.hpp>

namespace Backend {

class VideoProcessingEngine : public QObject
{
    Q_OBJECT

public:
    explicit VideoProcessingEngine(QObject *parent = nullptr);
    ~VideoProcessingEngine();

    // 视频处理配置
    void setProcessingEnabled(bool enabled);
    bool isProcessingEnabled() const;

signals:
    void frameProcessed(const cv::Mat& processedFrame);
    void processingError(const QString& errorMessage);

public slots:
    void processFrame(const cv::Mat& frame);

private:
    bool m_processingEnabled;
    
    // 内部处理函数
    cv::Mat applyImageProcessing(const cv::Mat& frame);
};

} // namespace Backend

#endif // VIDEO_BACKEND_H
