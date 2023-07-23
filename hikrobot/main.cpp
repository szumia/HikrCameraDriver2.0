#include "hik_cmaera.h"
#include "iostream"

using namespace std;
using namespace cv;

//#define RecordVideo
//#define ManualRecordPitcture
#define AutoRecordPicture


int main(){

    // MV_CC_DEVICE_INFO_LIST stDeviceList;
    // MV_CC_EnumDevices(MV_USB_DEVICE, &stDeviceList);
    // string id_str = string((char*)stDeviceList.pDeviceInfo[0]->SpecialInfo.stUsb3VInfo.chSerialNumber);
    // cout<<"ID "<<(id_str=="DA0213791")<<endl;



    // void *handle1, *handle2;
    // MV_CC_CreateHandle(&handle1, stDeviceList.pDeviceInfo[0]);
    // MV_CC_CreateHandle(&handle2, stDeviceList.pDeviceInfo[0]);

    // MV_CC_OpenDevice(handle1);
    // MV_CC_OpenDevice(handle2);

    // cout<<"isconnected:"<<MV_CC_IsDeviceConnected(handle1)<<endl;

    // MVCC_STRINGVALUE value1 = {0}, value2 = {0};
    // char *str1 = "DeviceSerialNumber";
    // MV_CC_GetStringValue(handle1, str1, &value1);
    // MV_CC_GetStringValue(handle2, "DeviceSerialNumber", &value2);
    // cout<<"value1:"<<value1.chCurValue<<endl;
    // cout<<"value2:"<<value2.chCurValue<<endl;


    // waitKey(100000);


    // return 0;

// // 第一种初始化方式
    // static HikCameraManager camera_manager;
    // HikCamera test_camera;
    // int fail_times = 0;
    // while(!camera_manager.init_one_camera_in_ID(test_camera, "DA0213791")){
    //     cout<<" failed ,retry..."<<endl;
    //     waitKey(1000);
    //     if(++fail_times >= 2){
    //         cout<<"exit...";
    //         close(0);
    //         exit(0);
    //     }
    // }

// 第二种初始化方式
    static HikCameraManager camera_manager;
    HikCamera test_camera;
    int fail_times = 0;
    while(!camera_manager.init_just_one_camera(test_camera)){
        cout<<" failed ,retry..."<<endl;
        waitKey(1000);
        if(++fail_times >= 2){
            cout<<"exit...";
            close(0);
            exit(0);
        }
    }

#if !defined(RecordVideo) && !defined(ManualRecordPitcture) && !defined(AutoRecordPicture)
    Mat M;
    float c_exposure = 1300;
    while(1){

        char c = waitKey(1);
        // char c = 0;
        if(c=='e'){
            exit(0);
        }
        else if(c=='a'){
            string str = test_camera.get_string_value("DeviceSerialNumber");
            cout<<"a:"<<str<<endl;
        }
        else if(c=='r'){
            test_camera.set_param(1,1);
        }
        else if(c=='b'){
            test_camera.set_param(1,0);
        }
        else if(c=='+'){
            c_exposure+=50;
            test_camera.set_float_value("ExposureTime",c_exposure);
            cout<<"set exposure:"<<c_exposure<<endl;
        }
        else if(c=='-'){
            c_exposure-=50;
            test_camera.set_float_value("ExposureTime",c_exposure);
            cout<<"set exposure:"<<c_exposure<<endl;
        }

        // double t1 = getTickCount();
        test_camera.get_image(M);
        // cout<<"time:"<<(getTickCount()-t1)/getTickFrequency()*1000<<endl;
        if(M.empty()) continue;
        imshow("ori_img",M);
    }
#endif


#ifdef AutoRecordPicture
    int savePicturesSum = 250;
    test_camera.auto_recordImgs(savePicturesSum);
#endif

#ifdef ManualRecordPitcture
    test_camera.manual_recordImgs();
#endif

#ifdef RecordVideo
    test_camera.recordVideo();
#endif


    return 0;
}