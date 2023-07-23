#ifndef HIK_CAMERA
#define HIK_CAMERA

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <sstream>
#include "hik_include/MvCameraControl.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

enum HIK_Camera_State{
    IDLE,
    OPENED,
    GRABBING,
    WRONG_STATE
};

class HikCamera{
public:
    HikCamera(){}
    // HikCamera(MV_CC_DEVICE_INFO &camera_device);
    ~HikCamera();

    int init_camera(MV_CC_DEVICE_INFO *camera_device);
    // int init_camera_in_ID(MV_CC_DEVICE_INFO *camera_device, string device_ID);

    void set_param(int now_mode, int my_color);
    bool set_resolution(int max_width, int roi_width, int roi_width_offset, int max_height, int roi_height, int roi_height_offset);

    int check_camera_state();
    
    int create_handle();

    int open_camera();
    void close_camera();

    void set_trigger_mode();

    unsigned int get_int_value(const char *Key);
    float get_float_value(const char *Key);
    unsigned int get_enum_value(const char *Key);
    bool get_bool_value(const char *Key);
    string get_string_value(const char *Key);

    bool set_int_value(const char *Key, unsigned int iValue);
    bool set_enum_value(const char *Key, unsigned int Value);
    bool set_float_value(const char *Key, float fValue);
    bool set_bool_value(const char *Key, bool bValue);
    bool set_string_value(const char *Key, char* strValue);

    

    int grab_image();

    void get_image(Mat &M);

public:

    int no_data_times = 0;
    string m_name = "camera";
    string m_ID = "none";
    void* handle = nullptr;

    int now_mode = -1;
    int now_color = -1;

    //自动取图，录视频等
    void auto_recordImgs(int maxImgsSum);
    void manual_recordImgs();
    void recordVideo();


private:
    int nRet = MV_OK;
    MV_CC_DEVICE_INFO* pDeviceInfo = nullptr;
    MVCC_INTVALUE stParam;
    unsigned char* pData = nullptr;
    unsigned char *pDataForBGR = nullptr;
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    int camera_state = HIK_Camera_State::IDLE;
    unsigned int nDataSize = 0;
    
};

class HikCameraManager{
public:

    HikCameraManager();

    ~HikCameraManager();

    void init_manager_for_one_camera();
    bool init_just_one_camera(HikCamera &hik_camera);
    bool init_one_camera_in_ID(HikCamera &hik_camera, string device_ID);
    int enum_device();

    HikCamera* get_hik_camera(int index);



private:
    int nRet = MV_OK;
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    HikCamera* cameras = nullptr;
    int cameras_size=0;

};



#endif