/*
 * @Author: ddxy
 * @Date: 2023-10-10 10:43:39
 * @LastEditors: ddxy 3416181310@qq.com
 * @FilePath: /camera/src/camera.cpp
 * WHUROBOCON_SAVED!!!
 */
#include "../inc/camera.h"
#include <pthread.h>
using namespace std;
using namespace k4a;
using namespace cv;
cv::Mat cv_depth_8U;
void *init_kinect(void *argc)
{

  camera camera;
  cv::Mat cv_color, cv_color1, cv_depth, cv_infrared, depthFrameSizeAsRGB, rgbFrame, depthFrame;
  cv::Mat color, mask, contour;
  k4a::capture capture;
  k4a::device device;
  k4a::transformation k4aTransformation;
  k4a::calibration k4aCalibration;
  k4a_device_configuration_t init;
  k4a::image k4a_color;
  k4a::image k4a_depth;
  k4a::image k4a_infrared;
  k4a::image k4a_tf_depth;

  int device_count = device::get_installed_count();
  if (device_count == 0)
  {
    cout << "Error:no K4A devices found." << endl;
    return 0;
  }
  else
  {
    cout << "Found" << device_count << "connected devices." << endl;
  }
  device = device::open(1);
  cout << "Done:open device." << endl;
  init = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
  init.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;  // A是alpha，具有alpha纹理格式的颜色
  init.color_resolution = K4A_COLOR_RESOLUTION_1536P; // 1920*1080
  init.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;    // 640*576
  init.camera_fps = K4A_FRAMES_PER_SECOND_30;         // 30帧
  init.synchronized_images_only = true;               // 只支持同步图像

  device.start_cameras(&init);
  cout << "Done:start camera." << endl;
  device.start_imu();
  cout << "Done:start imu." << endl;

  k4aCalibration = device.get_calibration(init.depth_mode, init.color_resolution);
  k4aTransformation = k4a::transformation(k4aCalibration);

  int iAuto = 0;
  while (1)
  {
    if (device.get_capture(&capture))
      cout << iAuto << ". Capture several frames to give auto-exposure" << endl;
    if (iAuto < 30)
    {
      iAuto++;
      continue;
    }
    else
    {
      cout << "Done: auto-exposure" << endl;
      break;
    }
  }
  while (1)
  {
    if (device.get_capture(&capture, std::chrono::milliseconds(50)))
    {
      k4a_color = capture.get_color_image();
      k4a_depth = capture.get_depth_image();
      k4a_tf_depth = k4aTransformation.depth_image_to_color_camera(k4a_depth);

      cv_color1 = Mat(k4a_color.get_height_pixels(), k4a_color.get_width_pixels(), CV_8UC4, k4a_color.get_buffer());
      cv_depth = Mat(k4a_tf_depth.get_height_pixels(), k4a_tf_depth.get_width_pixels(), CV_16U, k4a_tf_depth.get_buffer());
    }
    imshow("rgb", cv_color1);
    // imshow("rgb1", cv_depth);

    capture.reset();
    cv_color1.release();
    cv_depth.release();
    if (cv::waitKey(1) == 27)
      break;
  }
}

void *init_kinect2(void *argc)
{
  VideoCapture capture(4);
  while (true)
  {
    cv::Mat frame;
    capture.read(frame);
    imshow("读取视频", frame);
    waitKey(1); // 延时30
  }
}

void *init_kinect1(void *argc)
{
  VideoCapture capture(2);
  while (true)
  {
    cv::Mat frame;
    capture.read(frame);
    imshow("读取视频2", frame);
    waitKey(1); // 延时30
  }
}

void camera::getpicture(k4a::capture &capture, cv::Mat &cv_color1, cv::Mat &cv_color, cv::Mat &cv_depth, k4a::transformation &k4aTransformation)
{
  if (device.get_capture(&capture, std::chrono::milliseconds(50)))
  {
    k4a_color = capture.get_color_image();
    k4a_depth = capture.get_depth_image();
    // k4a_infrared = capture.get_ir_image();
    k4a_tf_depth = k4aTransformation.depth_image_to_color_camera(k4a_depth);

    cv_color1 = Mat(k4a_color.get_height_pixels(), k4a_color.get_width_pixels(), CV_8UC4, k4a_color.get_buffer());
    // GaussianBlur(cv_color1, cv_color, cv::Size(5, 5), 3, 3);
    // bilateralFilter(cv_color1, cv_color, 9, 50, 5);
    // cvtColor(cv_color, cv_color, cv::COLOR_BGRA2BGR);
    cv_depth = Mat(k4a_tf_depth.get_height_pixels(), k4a_tf_depth.get_width_pixels(), CV_16U, k4a_tf_depth.get_buffer());
    // cv_depth = cv::Mat(k4a_tf_depth.get_height_pixels(), k4a_tf_depth.get_width_pixels(), CV_16U, (void *)k4a_tf_depth.get_buffer(), static_cast<size_t>(k4a_tf_depth.get_stride_bytes()));

    // normalize(cv_depth, cv_depth_8U, 0, 256 * 256, NORM_MINMAX);
    // cv_depth_8U.convertTo(cv_depth, CV_8U, 1);

    // cv_infrared = Mat(k4a_infrared.get_height_pixels(), k4a_infrared.get_width_pixels(), CV_16U, k4a_infrared.get_buffer());
    // cv_depth.convertTo(cv_depth, CV_8U, 1);
    // cv_infrared.convertTo(cv_infrared, CV_8U, 1);
  }
}

void camera::stopCamera()
{
  device.close();
}

// int main()
// {

//   pthread_t threads[3];
//   pthread_create(&threads[1], NULL, init_kinect, NULL);
//   pthread_create(&threads[2], NULL, init_kinect2, NULL);
//   pthread_create(&threads[3], NULL, init_kinect1, NULL);
//   while (1)
//     ;
//   return 0;
// }