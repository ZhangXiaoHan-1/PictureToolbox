#ifndef VIDEO_FRONTEND_H
#define VIDEO_FRONTEND_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>

namespace Frontend {
    class VideoProcessor : public QObject
    {
        Q_OBJECT

    public:
        explicit VideoProcessor(QObject *parent = nullptr);
        ~VideoProcessor();

        // 图像转换工具函数
        static QImage matToQImage(const cv::Mat& mat);

    signals:
        void processingComplete();
        void imageProcessed(const QImage& image);

    public slots:
        void processFrame(const cv::Mat& frame);

    private:
        // 前端相关的私有成员
    };
}

#endif // VIDEO_FRONTEND_H
