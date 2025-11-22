#include "video_frontend.h"
#include <QVideoFrame>

namespace Frontend {

VideoProcessor::VideoProcessor(QObject *parent) : QObject(parent),
    m_preferredFormat(QVideoFrame::Format_RGB32)
{
}

VideoProcessor::~VideoProcessor()
{
}

QImage VideoProcessor::matToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1) {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for (int i = 0; i < 256; i++) {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++) {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3) {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4) {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else {
        return QImage();
    }
}

cv::Mat VideoProcessor::videoFrameToMat(const QVideoFrame& frame)
{
    QVideoFrame cloneFrame(frame);
    if (!cloneFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        return cv::Mat();
    }

    try {
        QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat());
        QImage image;
        
        if (imageFormat != QImage::Format_Invalid) {
            image = QImage(cloneFrame.bits(), cloneFrame.width(), cloneFrame.height(), 
                          cloneFrame.bytesPerLine(), imageFormat);
        } else {
            // Handle non-supported formats
            cloneFrame.unmap();
            return cv::Mat();
        }

        image = image.convertToFormat(QImage::Format_RGB888);
        cv::Mat mat(image.height(), image.width(), CV_8UC3, 
                   (void*)image.constBits(), image.bytesPerLine());
        
        cloneFrame.unmap();
        return mat.clone();
    } catch (...) {
        cloneFrame.unmap();
        return cv::Mat();
    }
}

void VideoProcessor::processVideoFrame(const QVideoFrame& frame)
{
    cv::Mat cvFrame = videoFrameToMat(frame);
    if (!cvFrame.empty()) {
        emit frameReadyForProcessing(cvFrame);
    }
}

void VideoProcessor::handleProcessedFrame(const cv::Mat& processedFrame)
{
    if (!processedFrame.empty()) {
        QImage image = matToQImage(processedFrame);
        emit frameReadyForDisplay(image);
    }
    emit processingComplete();
}

} // namespace Frontend
