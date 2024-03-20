/*
 * @Author: ddxy
 * @Date: 2023-10-10 10:43:39
 * @LastEditors: ddxy 3416181310@qq.com
 * @FilePath: /camera/src/camera.cpp
 * WHUROBOCON_SAVED!!!
 */
#include "../inc/camera.h"
using namespace std;
using namespace k4a;
using namespace cv;
cv::Mat cv_depth_8U;
void camera::init_kinect(k4a::capture &capture, k4a::transformation &k4aTransformation, k4a::calibration &k4aCalibration)
{
  device_count = device::get_installed_count();
  if (device_count == 0)
  {
    cout << "Error:no K4A devices found." << endl;
    return;
  }
  else
  {
    cout << "Found" << device_count << "connected devices." << endl;
  }
  device = device::open(0);
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
}
void camera::picture_update(k4a::capture &capture)
{
  device.get_capture(&capture, std::chrono::milliseconds(100));
}
cv::Mat *camera::getpicture(k4a::capture &capture, cv::Mat &cv_color, k4a::transformation &k4aTransformation)
{
  k4a_color = capture.get_color_image();

  cv::Mat *cv_color1;
  cv_color1 = new Mat(k4a_color.get_height_pixels(), k4a_color.get_width_pixels(), CV_8UC4, k4a_color.get_buffer());

  return cv_color1;
}

void camera::stopCamera()
{
  device.close();
}
