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

    // 图像显示工具函数
    QImage ImageCenter(QImage qimage, QLabel *qLabel);
    QImage MatToQImage(const cv::Mat& mat);
    QString formatTime(int seconds);

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
    
    // 图像调整操作
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_R_valueChanged(int value);
    void on_horizontalSlider_G_valueChanged(int value);
    void on_horizontalSlider_B_valueChanged(int value);
    void on_horizontalSlider_erzhi_valueChanged(int value);
    void on_horizontalSlider_duibi_valueChanged(int value);
    void on_horizontalSlider_baohe_valueChanged(int value);
    
    // 几何变换
    void on_pushButton_turnleft_clicked();
    void on_pushButton_turnright_clicked();
    void on_pushButton_turnleft_2_clicked();
    void on_pushButton_turnleft_3_clicked();
    
    // 图片导航
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    
    // 保存操作
    void on_pushButton_save_clicked();
    void on_pushButton_save_img_sr_clicked();
    
    // 视频操作
    void on_pushButton_6_clicked();
    void on_VideohorizontalSlider_2_valueChanged(int value);
    void on_horizontalSlider_suofang_valueChanged(int value);
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_2_clicked();
    
    // 其他操作
    void on_action_About_triggered();
    void on_action_L_triggered();
    void on_action_Dock_triggered();

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
    bool is_video_playing;
    
    QTimer video_timer;
    
    // 工具函数
    void updateImageDisplay(const cv::Mat& image);
    void updateStatusMessage(const QString& message);
    void setupBackendCallbacks();
    void loadAndDisplayImage(const QString& file_path);
    void updateImageNavigation();
    
    // 后端回调处理
    void handleImageProgress(int progress, const std::string& message);
    void handleVideoFrame(const cv::Mat& frame, int frame_number, double current_time);
    void handleVideoProgress(int progress, const std::string& message);
    
    // 视频定时器处理
    void processVideoFrame();
};
