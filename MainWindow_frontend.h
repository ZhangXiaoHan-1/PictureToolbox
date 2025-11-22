// MainWindow_frontend.h
#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include "ImageProcessingService_backend.h"
#include "VideoProcessingService_backend.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 图像显示相关
    QImage ImageCenter(QImage qimage, QLabel *qLabel);
    QImage MatToQImage(const cv::Mat& mat);
    cv::Mat QImageToMat(const QImage& qimage);

private slots:
    // 文件操作
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_V_triggered();
    
    // 图像处理操作
    void on_pushButton_gray_clicked();
    void on_pushButton_junzhi_clicked();
    void on_pushButton_junzhi_2_clicked();
    void on_pushButton_junzhi_3_clicked();
    void on_pushButton_gamma_clicked();
    void on_pushButton_origin_clicked();
    void on_pushButton_sr_clicked();
    void on_pushButton_bic_clicked();
    
    // 调整操作
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_R_valueChanged(int value);
    void on_horizontalSlider_G_valueChanged(int value);
    void on_horizontalSlider_B_valueChanged(int value);
    void on_horizontalSlider_erzhi_valueChanged(int value);
    void on_horizontalSlider_duibi_valueChanged(int value);
    void on_horizontalSlider_baohe_valueChanged(int value);
    
    // 视频操作
    void on_pushButton_6_clicked();
    void on_VideohorizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_suofang_valueChanged(int value);
    
    // 其他操作
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_save_img_sr_clicked();
    void on_action_About_triggered();

private:
    Ui::MainWindow *ui;
    
    // 后端服务
    ImageProcessingBackend image_backend;
    VideoProcessingBackend video_backend;
    
    // 状态变量
    QStringList srcDirPathList;
    int current_image_index;
    QString current_image_path;
    bool language;
    
    // 工具函数
    void update_image_display(const cv::Mat& image);
    void update_status_message(const QString& message);
    void setup_backend_callbacks();
    
    // 后端回调处理
    void handle_image_progress(int progress, const std::string& message);
    void handle_video_frame(const cv::Mat& frame, int frame_number);
    void handle_video_progress(int progress, const std::string& message);
};
