#ifndef SERVICE_INTERFACE_H
#define SERVICE_INTERFACE_H

#include <opencv2/opencv.hpp>
#include <QObject>
#include <QImage>

// 前后端通信接口
class VideoServiceInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~VideoServiceInterface() = default;

public slots:
    virtual void processVideoFrame(const cv::Mat& frame) = 0;

signals:
    void frameProcessed(const QImage& processedFrame);
    void processingError(const QString& error);
};

#endif // SERVICE_INTERFACE_H
