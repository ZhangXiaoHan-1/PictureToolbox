// MainWindow_frontend.cpp
#include "MainWindow_frontend.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    current_image_index(0),
    language(true),
    is_video_playing(false)
{
    ui->setupUi(this);
    setupBackendCallbacks();
    
    // 初始化UI状态
    ui->pushButton_3->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_6->setEnabled(false);
    
    // 设置窗口属性
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    
    // 连接视频定时器
    connect(&video_timer, &QTimer::timeout, this, &MainWindow::processVideoFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupBackendCallbacks() {
    // 设置图像处理后端回调
    image_backend.setProgressCallback([this](int progress, const std::string& message) {
        QMetaObject::invokeMethod(this, [this, progress, message]() {
            handleImageProgress(progress, message);
        });
    });
    
    // 设置视频处理后端回调
    video_backend.setFrameCallback([this](const cv::Mat& frame, int frame_number, double current_time) {
        QMetaObject::invokeMethod(this, [this, frame, frame_number, current_time]() {
            handleVideoFrame(frame, frame_number, current_time);
        });
    });
    
    video_backend.setProgressCallback([this](int progress, const std::string& message) {
        QMetaObject::invokeMethod(this, [this, progress, message]() {
            handleVideoProgress(progress, message);
        });
    });
}

void MainWindow::on_action_Open_triggered() {
    QStringList file_paths = QFileDialog::getOpenFileNames(this, 
        tr("选择图片"), 
        "E:/Qt/qtworks/MainWindow/images", 
        tr("图像文件(*.jpg *.png *.bmp)"));
    
    if (file_paths.isEmpty()) return;
    
    srcDirPathList = file_paths;
    current_image_index = 0;
    
    if (!srcDirPathList.isEmpty()) {
        loadAndDisplayImage(srcDirPathList.first());
    }
}

void MainWindow::on_pushButton_clicked() {
    on_action_Open_triggered();
}

void MainWindow::loadAndDisplayImage(const QString& file_path) {
    current_image_path = file_path;
    
    if (image_backend.loadImage(file_path.toStdString())) {
        cv::Mat image = image_backend.getCurrentImage();
        updateImageDisplay(image);
        updateStatusMessage("图像加载成功: " + file_path);
    }
    
    // 更新UI状态
    ui->pushButton_3->setDisabled(srcDirPathList.size() <= 1);
    ui->pushButton_4->setDisabled(srcDirPathList.size() <= 1);
    updateImageNavigation();
}

void MainWindow::on_pushButton_3_clicked() {
    if (srcDirPathList.size() <= 1) return;
    
    current_image_index = (current_image_index - 1 + srcDirPathList.size()) % srcDirPathList.size();
    loadAndDisplayImage(srcDirPathList[current_image_index]);
}

void MainWindow::on_pushButton_4_clicked() {
    if (srcDirPathList.size() <= 1) return;
    
    current_image_index = (current_image_index + 1) % srcDirPathList.size();
    loadAndDisplayImage(srcDirPathList[current_image_index]);
}

void MainWindow::updateImageNavigation() {
    // 更新图片导航显示
    // 这里可以添加缩略图显示逻辑
}

void MainWindow::on_pushButton_gray_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat gray_image = image_backend.convertToGrayscale();
    updateImageDisplay(gray_image);
    image_backend.setCurrentImage(gray_image);
}

void MainWindow::on_pushButton_junzhi_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat filtered_image = image_backend.applyMeanFilter(3);
    updateImageDisplay(filtered_image);
    image_backend.setCurrentImage(filtered_image);
}

void MainWindow::on_pushButton_junzhi_2_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat edge_image = image_backend.applyEdgeDetection();
    updateImageDisplay(edge_image);
    image_backend.setCurrentImage(edge_image);
}

void MainWindow::on_pushButton_junzhi_3_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat blended_image = image_backend.blendWithEdgeDetection(0.5);
    updateImageDisplay(blended_image);
    image_backend.setCurrentImage(blended_image);
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value) {
    if (!image_backend.isImageLoaded()) {
        return;
    }
    
    double ratio = value / 100.0;
    cv::Mat blended_image = image_backend.blendWithEdgeDetection(ratio);
    updateImageDisplay(blended_image);
    image_backend.setCurrentImage(blended_image);
    
    ui->label_fuhe->setText(QString::number(ratio, 'f', 2) + ":" + QString::number(1.0 - ratio, 'f', 2));
}

void MainWindow::on_pushButton_gamma_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat gamma_image = image_backend.applyGammaTransform(1.2);
    updateImageDisplay(gamma_image);
    image_backend.setCurrentImage(gamma_image);
}

void MainWindow::on_pushButton_origin_clicked() {
    if (!image_backend.isImageLoaded() && !current_image_path.isEmpty()) {
        loadAndDisplayImage(current_image_path);
    }
}

void MainWindow::on_pushButton_sr_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat sr_image = image_backend.applySuperResolution();
    if (!sr_image.empty()) {
        updateImageDisplay(sr_image);
        image_backend.setCurrentImage(sr_image);
    }
}

void MainWindow::on_pushButton_bic_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat upscaled_image = image_backend.applyBicubicUpscale(2.0);
    updateImageDisplay(upscaled_image);
    image_backend.setCurrentImage(upscaled_image);
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
    if (!image_backend.isImageLoaded()) {
        return;
    }
    
    cv::Mat bright_image = image_backend.adjustBrightness(value);
    updateImageDisplay(bright_image);
    image_backend.setCurrentImage(bright_image);
    ui->label_light->setText(QString::number(value));
}

void MainWindow::on_horizontalSlider_erzhi_valueChanged(int value) {
    if (!image_backend.isImageLoaded()) {
        return;
    }
    
    cv::Mat binary_image = image_backend.applyBinaryThreshold(value);
    updateImageDisplay(binary_image);
    image_backend.setCurrentImage(binary_image);
    ui->label_yuzhi->setText(QString::number(value));
}

void MainWindow::on_horizontalSlider_R_valueChanged(int value) {
    if (!image_backend.isImageLoaded()) {
        return;
    }
    
    int g_value = ui->horizontalSlider_G->value();
    int b_value = ui->horizontalSlider_B->value();
    
    cv::Mat adjusted_image = image_backend.adjustRGBValues(value, g_value, b_value);
    updateImageDisplay(adjusted_image);
    image_backend.setCurrentImage(adjusted_image);
}

void MainWindow::on_horizontalSlider_G_valueChanged(int value) {
    on_horizontalSlider_R_valueChanged(ui->horizontalSlider_R->value());
}

void MainWindow::on_horizontalSlider_B_valueChanged(int value) {
    on_horizontalSlider_R_valueChanged(ui->horizontalSlider_R->value());
}

void MainWindow::on_pushButton_turnleft_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat rotated_image = image_backend.rotateImage(-90.0);
    updateImageDisplay(rotated_image);
    image_backend.setCurrentImage(rotated_image);
}

void MainWindow::on_pushButton_turnright_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat rotated_image = image_backend.rotateImage(90.0);
    updateImageDisplay(rotated_image);
    image_backend.setCurrentImage(rotated_image);
}

void MainWindow::on_pushButton_turnleft_2_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat mirrored_image = image_backend.mirrorImage(true, false);
    updateImageDisplay(mirrored_image);
    image_backend.setCurrentImage(mirrored_image);
}

void MainWindow::on_pushButton_turnleft_3_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat mirrored_image = image_backend.mirrorImage(false, true);
    updateImageDisplay(mirrored_image);
    image_backend.setCurrentImage(mirrored_image);
}

void MainWindow::on_action_Save_triggered() {
    on_pushButton_save_clicked();
}

void MainWindow::on_pushButton_save_clicked() {
    if (!image_backend.isImageLoaded()) {
        QMessageBox::warning(this, "提示", "请先打开图片！");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this,
        tr("保存图片"),
        "E:/Qtworks/MainWindow/images",
        tr("*.png;; *.jpg;; *.bmp;; *.tif;; *.GIF"));
    
    if (!filename.isEmpty()) {
        if (image_backend.saveImage(filename.toStdString())) {
            updateStatusMessage("图片保存成功！");
        } else {
            QMessageBox::warning(this, "错误", "图片保存失败！");
        }
    }
}

void MainWindow::on_pushButton_save_img_sr_clicked() {
    on_pushButton_save_clicked();
}

// 视频处理相关槽函数
void MainWindow::on_action_V_triggered() {
    QString video_path = QFileDialog::getOpenFileName(this, 
        tr("选择视频"), 
        "E:/Qt/qtworks/MainWindow/images", 
        tr("Video (*.WMV *.mp4 *.rmvb *.flv)"));
    
    if (video_path.isEmpty()) return;
    
    if (video_backend.loadVideo(video_path.toStdString())) {
        ui->tabWidget->setCurrentIndex(1);
        ui->pushButton_6->setEnabled(true);
        is_video_playing = true;
        
        // 设置视频进度条范围
        ui->VideohorizontalSlider_2->setMaximum(video_backend.getTotalFrames());
        
        // 开始视频播放
        video_timer.start(1000 / video_backend.getFPS());
        
        updateStatusMessage("视频加载成功");
    } else {
        QMessageBox::warning(this, "错误", "打开视频失败！");
    }
}

void MainWindow::on_pushButton_6_clicked() {
    if (!video_backend.isVideoLoaded()) return;
    
    if (is_video_playing) {
        video_timer.stop();
        is_video_playing = false;
        ui->pushButton_6->setStyleSheet("border-radius:32px; background-image: url(:/myImage/images/start.png); border:none;");
    } else {
        video_timer.start(1000 / video_backend.getFPS());
        is_video_playing = true;
        ui->pushButton_6->setStyleSheet("border-radius:32px; background-image: url(:/myImage/images/stop.png); border:none;");
    }
}

void MainWindow::processVideoFrame() {
    if (!video_backend.isVideoLoaded() || !is_video_playing) {
        video_timer.stop();
        return;
    }
    
    cv::Mat frame = video_backend.getCurrentFrame();
    if (!frame.empty()) {
        // 更新进度条
        ui->VideohorizontalSlider_2->setValue(video_backend.getCurrentFrameNumber());
        
        // 更新时间显示
        double current_time = video_backend.getCurrentFrameNumber() / video_backend.getFPS();
        double total_time = video_backend.getDuration();
        ui->label_12->setText(formatTime(static_cast<int>(current_time)) + "/" + formatTime(static_cast<int>(total_time)));
    } else {
        // 视频结束
        on_pushButton_6_clicked(); // 停止播放
        video_backend.seek(0); // 回到开始
    }
}

void MainWindow::on_VideohorizontalSlider_2_valueChanged(int value) {
    if (!video_backend.isVideoLoaded()) return;
    
    // 防止递归调用
    static bool internal_change = false;
    if (internal_change) return;
    
    internal_change = true;
    video_backend.seek(value);
    internal_change = false;
}

void MainWindow::on_horizontalSlider_suofang_valueChanged(int value) {
    ui->label_suofangvalue->setText(QString::number(value / 100.0, 'f', 2));
}

void MainWindow::on_pushButton_7_clicked() {
    video_backend.setProcessingType(VideoProcessingBackend::GRAYSCALE);
}

void MainWindow::on_pushButton_8_clicked() {
    video_backend.setProcessingType(VideoProcessingBackend::NONE);
}

void MainWindow::on_pushButton_9_clicked() {
    video_backend.setProcessingType(VideoProcessingBackend::EDGE_DETECTION);
}

void MainWindow::on_pushButton_10_clicked() {
    video_backend.setProcessingType(VideoProcessingBackend::SMOOTH);
}

void MainWindow::on_pushButton_11_clicked() {
    video_backend.setProcessingType(VideoProcessingBackend::BINARY);
}

void MainWindow::on_pushButton_2_clicked() {
    video_backend.setProcessingType(VideoProcessingBackend::MOSAIC);
}

void MainWindow::updateImageDisplay(const cv::Mat& image) {
    if (image.empty()) return;
    
    QImage qimage = MatToQImage(image);
    QImage display_image = ImageCenter(qimage, ui->label_show);
    ui->label_show->setPixmap(QPixmap::fromImage(display_image));
    ui->label_show->setAlignment(Qt::AlignCenter);
}

void MainWindow::updateStatusMessage(const QString& message) {
    ui->statusBar->showMessage(message, 3000);
}

void MainWindow::handleImageProgress(int progress, const std::string& message) {
    updateStatusMessage(QString::fromStdString(message));
}

void MainWindow::handleVideoFrame(const cv::Mat& frame, int frame_number, double current_time) {
    QImage qimage = MatToQImage(frame);
    
    // 缩放处理
    double scale = ui->horizontalSlider_suofang->value() / 100.0;
    QSize target_size = ui->label_11->rect().size() * scale;
    
    ui->label_11->setPixmap(QPixmap::fromImage(qimage).scaled(target_size));
    ui->label_11->setAlignment(Qt::AlignCenter);
}

void MainWindow::handleVideoProgress(int progress, const std::string& message) {
    updateStatusMessage(QString::fromStdString(message));
}

QImage MainWindow::MatToQImage(const cv::Mat& mat) {
    if (mat.type() == CV_8UC1) {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for (int i = 0; i < 256; i++) {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++) {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    } else if (mat.type() == CV_8UC3) {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    } else if (mat.type() == CV_8UC4) {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    return QImage();
}

QImage MainWindow::ImageCenter(QImage qimage, QLabel *qLabel) {
    QSize imageSize = qimage.size();
    QSize labelSize = qLabel->size();
    
    double dWidthRatio = 1.0 * imageSize.width() / labelSize.width();
    double dHeightRatio = 1.0 * imageSize.height() / labelSize.height();
    
    if (dWidthRatio > dHeightRatio) {
        return qimage.scaledToWidth(labelSize.width());
    } else {
        return qimage.scaledToHeight(labelSize.height());
    }
}

QString MainWindow::formatTime(int seconds) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}

// 其他槽函数的实现...
void MainWindow::on_action_About_triggered() {
    QMessageBox::about(this, "关于本软件", 
        "欢迎使用《多功能图像视频处理》软件！本软件具有简单的图像和视频处理功能。\n"
        "图像功能包括多选打开、旋转、镜像、灰度化、均值滤波、边缘检测、原图复合、伽马检测、二值化、色彩调整、亮度调整、对比度调整、饱和度调整等功能。\n"
        "视频功能包括暂停、播放、进度条、灰度化、边缘检测、平滑、二值化、局部马赛克、缩放等功能。\n"
        "——By fyf");
}

void MainWindow::on_action_L_triggered() {
    // 语言切换逻辑
}

void MainWindow::on_action_Dock_triggered() {
    ui->dockWidget->show();
}
