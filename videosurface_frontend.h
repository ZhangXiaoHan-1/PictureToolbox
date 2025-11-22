#ifndef VIDEOSURFACE_FRONTEND_H
#define VIDEOSURFACE_FRONTEND_H

#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QObject>

namespace Frontend {

class VideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    explicit VideoSurface(QObject *parent = nullptr);
    ~VideoSurface();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;
    
    bool present(const QVideoFrame &frame) override;

signals:
    void frameAvailable(QVideoFrame &frame);
    void frameProcessed(const QImage &image);

private:
    QVideoFrame::PixelFormat convertToSupportedFormat(QVideoFrame::PixelFormat format) const;
};

} // namespace Frontend

#endif // VIDEOSURFACE_FRONTEND_H
