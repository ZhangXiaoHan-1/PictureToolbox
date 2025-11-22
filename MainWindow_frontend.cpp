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
    language(true)
{
    ui->setupUi(this);
    setup_backend_callbacks();
    
    // 初始化UI状态
    ui->pushButton_3->setDisabled(true);
    ui->pushButton_4->setDisabled(true);
    
    // 设置窗口属性
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup_backend_callbacks() {
    // 设置图像处理后端回调
    image_backend.set_progress_callback([this](int progress, const std::string& message) {
        QMetaObject::invokeMethod(this, [this, progress, message]() {
            handle_image_progress(progress, message);
        });
    });
    
    // 设置视频处理后端回调
    video_backend.set_frame_callback([this](const cv::Mat& frame, int frame_number) {
        QMetaObject::invokeMethod(this, [this, frame, frame_number]() {
            handle_video_frame(frame, frame_number);
        });
    });
    
    video_backend.set_progress_callback([this](int progress, const std::string& message) {
        QMetaObject::invokeMethod(this, [this, progress, message]() {
            handle_video_progress(progress, message);
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
    
    if (srcDirPathList.size() > 0) {
        QString first_file = srcDirPathList.first();
        current_image_path = first_file;
        
        // 使用后端加载图像
        if (image_backend.load_image(first_file.toStdString())) {
            cv::Mat image = image_backend.get_current_image();
            update_image_display(image);
            update_status_message("图像加载成功: " + first_file);
        }
        
        // 更新UI状态
        ui->pushButton_3->setDisabled(srcDirPathList.size() <= 1);
        ui->pushButton_4->setDisabled(srcDirPathList.size() <= 1);
    }
}

void MainWindow::on_pushButton_gray_clicked() {
    if (!image_backend.is_image_loaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat gray_image = image_backend.convert_to_grayscale();
    update_image_display(gray_image);
}

void MainWindow::on_pushButton_junzhi_clicked() {
    if (!image_backend.is_image_loaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat filtered_image = image_backend.apply_mean_filter(3);
    update_image_display(filtered_image);
}

void MainWindow::on_pushButton_junzhi_2_clicked() {
    if (!image_backend.is_image_loaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat edge_image = image_backend.apply_edge_detection();
    update_image_display(edge_image);
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
    if (!image_backend.is_image_loaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat bright_image = image_backend.adjust_brightness(value);
    update_image_display(bright_image);
    ui->label_light->setText(QString::number(value));
}

void MainWindow::on_pushButton_sr_clicked() {
    if (!image_backend.is_image_loaded()) {
        QMessageBox::warning(this, "提示", "请先选择一张图片！");
        return;
    }
    
    cv::Mat sr_image = image_backend.apply_super_resolution();
    if (!sr_image.empty()) {
        update_image_display(sr_image);
        update_status_message("超分辨率处理完成");
    }
}

void MainWindow::update_image_display(const cv::Mat& image) {
    if (image.empty()) return;
    
    QImage qimage = MatToQImage(image);
    QImage display_image = ImageCenter(qimage, ui->label_show);
    ui->label_show->setPixmap(QPixmap::fromImage(display_image));
    ui->label_show->setAlignment(Qt::AlignCenter);
}

void MainWindow::update_status_message(const QString& message) {
    ui->statusBar->showMessage(message, 3000);
}

void MainWindow::handle_image_progress(int progress, const std::string& message) {
    update_status_message(QString::fromStdString(message));
    // 可以在这里更新进度条等UI元素
}

void MainWindow::handle_video_frame(const cv::Mat& frame, int frame_number) {
    QImage qimage = MatToQImage(frame);
    
    // 缩放处理
    double scale = ui->horizontalSlider_suofang->value() / 100.0;
    QSize target_size = ui->label_11->rect().size() * scale;
    
    ui->label_11->setPixmap(QPixmap::fromImage(qimage).scaled(target_size));
    ui->label_11->setAlignment(Qt::AlignCenter);
    
    // 更新进度信息
    // ... 时间显示逻辑
}

void MainWindow::handle_video_progress(int progress, const std::string& message) {
    update_status_message(QString::fromStdString(message));
}

// 其他槽函数实现...
// [由于篇幅限制，这里只展示关键部分，其他槽函数实现类似]

QImage MainWindow::MatToQImage(const cv::Mat& mat) {
    // 实现Mat到QImage的转换
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
