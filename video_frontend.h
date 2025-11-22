#ifndef VIDEO_FRONTEND_H
#define VIDEO_FRONTEND_H

#include <QObject>
#include <QImage>
#include <QVideoFrame>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace Frontend {

class VideoProcessor : public QObject
{
    Q_OBJECT

public:
    explicit VideoProcessor(QObject *parent = nullptr);
    ~VideoProcessor();

    // 图像转换工具函数
    static QImage matToQImage(const cv::Mat& mat);
    static cv::Mat videoFrameToMat(const QVideoFrame& frame);

signals:
    void frameReadyForDisplay(const QImage& image);
    void frameReadyForProcessing(const cv::Mat& frame);
    void processingComplete();

public slots:
    void processVideoFrame(const QVideoFrame& frame);
    void handleProcessedFrame(const cv::Mat& processedFrame);

private:
    QVideoFrame::PixelFormat m_preferredFormat;
};

} // namespace Frontend

#endif // VIDEO_FRONTEND_H
