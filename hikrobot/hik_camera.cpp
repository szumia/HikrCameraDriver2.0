#include "hik_cmaera.h"


//初始化单个相机设备
int HikCamera::init_camera(MV_CC_DEVICE_INFO *camera_device){
    pDeviceInfo = camera_device;
    if(create_handle() == 0) return 0;
    if(open_camera() == 0) return 0;
    set_enum_value("TriggerMode", MV_TRIGGER_MODE_OFF);
    set_enum_value("PixelFormat", PixelType_Gvsp_BayerRG8);
    set_enum_value("ADCBitDepth", 0);
    set_resolution(-1,1280,0,-1,1024,0);
    if(grab_image()==0) return 0;
    set_bool_value("AcquisitionFrameRateEnable", true);
    set_float_value("AcquisitionFrameRate", 250);
    //TODO::change exposure & gain
    set_float_value("ExposureTime",5000);
    set_float_value("Gain",10);//0~16
    cout<<m_name<<" init success!!!"<<endl;
    cout<<"actual frame rate:"<< get_float_value("ResultingFrameRate")<<endl;
    return 1;
}

void HikCamera::set_param(int mode, int my_color){
    if(now_mode == mode && now_color == my_color) return;
    // int success = 1;
    now_mode = mode;
    now_color = my_color;
    if(now_mode == 1 || now_mode == 0){
        std::cout << "switch to autoaim!!!!" << std::endl;
        if(my_color == 1)
        {
            //TODO::exposure red
            std::cout << "detect red!!!" << std::endl;
            this->set_float_value("ExposureTime",2000);
            this->set_float_value("Gain",10);   //0~16
        }
        else if(my_color == 0)
        {
            std::cout << "detect blue!!!" << std::endl;
            //TODO::exposure blue
            this->set_float_value("ExposureTime",1300);
            this->set_float_value("Gain",10);   //0~16
        }
        set_resolution(-1,1440,0,-1,1080,0);
    }
    else if(now_mode == 2 || now_mode == 3){
        set_resolution(-1,1440,0,-1,1080,0);
    }

    // if(success == 0){
    //     now_mode = -1;
    //     now_color = -1;
    // }
}

// k
bool HikCamera::set_resolution(int max_width, int roi_width, int roi_width_offset, int max_height, int roi_height, int roi_height_offset){
    if(camera_state >= HIK_Camera_State::GRABBING){
        cout<<"stop grab..."<<endl;
        if(pData!=nullptr) free(pData);
        nRet = MV_CC_StopGrabbing(handle);
        if (MV_OK != nRet)
        {
            printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
            return 0;
        }
        waitKey(200);
    }

    
    // this->set_int_value("WidthMax",max_width);
    this->set_int_value("Width",roi_width);
    this->set_int_value("OffsetX",roi_width_offset);

    // this->set_int_value("HeightMax",max_height);
    this->set_int_value("Height",roi_height);
    this->set_int_value("OffsetY",roi_height_offset);
    if(camera_state >= HIK_Camera_State::GRABBING){
        if(grab_image()){
            return 0;
        }
    }
    return 1;
}

int HikCamera::check_camera_state(){
    if(camera_state == HIK_Camera_State::WRONG_STATE) return 0;
    return 1;
}

// 设备创建句柄
int HikCamera::create_handle(){
    nRet = MV_CC_CreateHandle(&handle, pDeviceInfo);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
        camera_state = HIK_Camera_State::WRONG_STATE;
        return 0;
    }
    return 1;
}

// 打开相机
int HikCamera::open_camera(){
    if(handle == nullptr){
        camera_state = HIK_Camera_State::WRONG_STATE;
        return 0;
    }
    nRet = MV_CC_OpenDevice(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
        camera_state = HIK_Camera_State::WRONG_STATE;
        return 0;
    }
    camera_state = HIK_Camera_State::OPENED;
    return 1;
}

void HikCamera::set_trigger_mode(){
    // 设置触发模式为off
    // set trigger mode as off
    nRet = MV_CC_SetEnumValue(handle, "TriggerMode", MV_TRIGGER_MODE_OFF);
    if (MV_OK != nRet)
    {
        printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
        return;
    }
}

//获得int类型相机参数
unsigned int HikCamera::get_int_value(const char *Key){
    MVCC_INTVALUE value;
    MV_CC_GetIntValue(handle, Key, &value);
    return value.nCurValue;
}
//获得float类型相机参数
float HikCamera::get_float_value(const char *Key){
    MVCC_FLOATVALUE value;
    MV_CC_GetFloatValue(handle, Key, &value);
    return value.fCurValue;
}
//获得enum类型相机参数
unsigned int HikCamera::get_enum_value(const char *Key){
    MVCC_ENUMVALUE value;
    MV_CC_GetEnumValue(handle, Key, &value);
    return value.nCurValue;
}
//获得bool类型相机参数
bool HikCamera::get_bool_value(const char *Key){
    bool value=false;
    MV_CC_GetBoolValue(handle, Key, &value);
    return value;
}
//获得string类型相机参数
string HikCamera::get_string_value(const char *Key){
    MVCC_STRINGVALUE value = {0};
    MV_CC_GetStringValue(handle, Key, &value);
    return string(value.chCurValue);
}

//修改int类型相机参数
bool HikCamera::set_int_value(const char *Key, unsigned int iValue){
    nRet = MV_CC_SetIntValue(handle, Key, iValue);
    if (MV_OK != nRet)
    {
        cout<<"camera set "<<string(Key)<<" fail!!!"<<endl;
        return 0;
    }
    return 1;
}
//修改float类型相机参数
bool HikCamera::set_float_value(const char *Key, float fValue){
    nRet = MV_CC_SetFloatValue(handle, Key, fValue);
    if (MV_OK != nRet)
    {
        cout<<"camera set "<<string(Key)<<" fail!!!"<<endl;
        return 0;
    }
    return 1;
}
//修改enum类型相机参数
bool HikCamera::set_enum_value(const char *Key, unsigned int Value){
    nRet = MV_CC_SetEnumValue(handle, Key, Value);
    if (MV_OK != nRet)
    {
        cout<<"camera set "<<string(Key)<<" fail!!!"<<endl;
        return 0;
    }
    return 1;
}
//修改bool类型相机参数
bool HikCamera::set_bool_value(const char *Key, bool bValue){
    nRet = MV_CC_SetBoolValue(handle, Key, bValue);
    if (MV_OK != nRet)
    {
        cout<<"camera set "<<string(Key)<<" fail!!!"<<endl;
        return 0;
    }
    return 1;
}
//修改string类型相机参数
bool HikCamera::set_string_value(const char *Key, char* strValue){
    nRet = MV_CC_SetStringValue(handle, Key, strValue);
    if (MV_OK != nRet)
    {
        cout<<"camera set "<<string(Key)<<" fail!!!"<<endl;
        return 0;
    }
    return 1;
}


//开启相机取流
int HikCamera::grab_image(){
    // 开始取流
    // start grab image
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
        camera_state = HIK_Camera_State::WRONG_STATE;
        return 0;
    }
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
    if (MV_OK != nRet)
    {
        printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
        camera_state = HIK_Camera_State::WRONG_STATE;
        return 0;
    }
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    pData = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
    nDataSize = stParam.nCurValue;
    camera_state = HIK_Camera_State::GRABBING;
    cout<<"grabbing image success!"<<endl;
    return 1;
}


void HikCamera::get_image(Mat &M){
    MV_FRAME_OUT stOutFrame = {0};
    memset(&stOutFrame, 0, sizeof(MV_FRAME_OUT));
    nRet = MV_CC_GetImageBuffer(handle, &stOutFrame, 1000);
    if (nRet == MV_OK)
    {
        // printf("GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n", 
        // stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
    }
    else{
        printf("No data[%x]\n", nRet);
        camera_state = HIK_Camera_State::WRONG_STATE;
        no_data_times++;
    }
    if(stOutFrame.stFrameInfo.enPixelType == PixelType_Gvsp_BayerRG8 && stOutFrame.pBufAddr != nullptr){
        if(pDataForBGR != nullptr){
            free(pDataForBGR);
        }
        pDataForBGR = (unsigned char*)malloc(stOutFrame.stFrameInfo.nWidth * stOutFrame.stFrameInfo.nHeight * 4 + 2048);
        if (pDataForBGR == nullptr)
        {
            camera_state = HIK_Camera_State::WRONG_STATE;
            return;
        }
        // 像素格式转换
        // convert pixel format 
        MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};
        // 从上到下依次是：图像宽，图像高，输入数据缓存，输入数据大小，源像素格式，
        // 目标像素格式，输出数据缓存，提供的输出缓冲区大小
        // Top to bottom are：image width, image height, input data buffer, input data size, source pixel format, 
        // destination pixel format, output data buffer, provided output buffer size
        stConvertParam.nWidth = stOutFrame.stFrameInfo.nWidth;
        stConvertParam.nHeight = stOutFrame.stFrameInfo.nHeight;
        stConvertParam.pSrcData = stOutFrame.pBufAddr;
        stConvertParam.nSrcDataLen = stOutFrame.stFrameInfo.nFrameLen;
        stConvertParam.enSrcPixelType = stOutFrame.stFrameInfo.enPixelType;
        stConvertParam.enDstPixelType = PixelType_Gvsp_BGR8_Packed;
        stConvertParam.pDstBuffer = pDataForBGR;
        stConvertParam.nDstBufferSize = stOutFrame.stFrameInfo.nWidth * stOutFrame.stFrameInfo.nHeight *  4 + 2048;
        nRet = MV_CC_ConvertPixelType(handle, &stConvertParam);
        if (MV_OK != nRet)
        {
            printf("MV_CC_ConvertPixelType fail! nRet [%x]\n", nRet);
            camera_state = HIK_Camera_State::WRONG_STATE;
            return;
        }
        M = cv::Mat(stConvertParam.nHeight, stConvertParam.nWidth, CV_8UC3, pDataForBGR);

        nRet = MV_CC_FreeImageBuffer(handle, &stOutFrame);
    }
    else{
        camera_state = HIK_Camera_State::WRONG_STATE;
        cout<<"error pixel type or frame is nullptr!!!"<<endl;
    }
}

void HikCamera::close_camera(){
    // 停止取流
    // end grab image
    if(pData != nullptr){
        free(pData);
    }
    
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
    }
    // 关闭设备
    // close device
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CloseDevice fail! nRet [%x]\n", nRet);
    }
    // 销毁句柄
    // destroy handle
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
    }
}

HikCamera::~HikCamera(){
    close_camera();
}



HikCameraManager::HikCameraManager(){}

//初始化设备管理器
void HikCameraManager::init_manager_for_one_camera(){
    int enum_success = 0;
    int init_success = 0;
    int error_times = 0;

    while(enum_success==0 || init_success==0){
        enum_success = enum_device();
        if(enum_success==0){
            error_times++;
            if(error_times>2){
                cout<<"error_times >2 close program"<<endl;
                exit(0);
            }
            else{
                cout<<"Retry init..."<<endl;
                waitKey(1000);
                continue;
            }
        }

        this->cameras = new HikCamera;
        init_success = cameras->init_camera(stDeviceList.pDeviceInfo[0]);

        if(init_success == 0){
            if(cameras != nullptr){
                delete cameras;
                cameras=nullptr;
            }
            error_times++;
            if(error_times>2){
                cout<<"error_times >2 close program"<<endl;
                exit(0);
            }
        }
    }
    
    
}

//初始化设备管理器
bool HikCameraManager::init_just_one_camera(HikCamera &hik_camera){
    if(!enum_device()) return 0;

    if(cameras_size > 0){
        string id_str = string((char*)stDeviceList.pDeviceInfo[0]->SpecialInfo.stUsb3VInfo.chSerialNumber);
        cout<<"your hik camera id is:"<<id_str<<endl;
        if(hik_camera.init_camera(stDeviceList.pDeviceInfo[0])){
            return 1;
        }
        else return 0;
    }

    return 0;
}

//初始化设备管理器
bool HikCameraManager::init_one_camera_in_ID(HikCamera &hik_camera ,string device_ID){
    if(!enum_device()) return 0;

    for(int i=0; i<this->cameras_size; i++){
        string id_str = string((char*)stDeviceList.pDeviceInfo[i]->SpecialInfo.stUsb3VInfo.chSerialNumber);
        if(id_str != device_ID) continue;
        else if(hik_camera.init_camera(stDeviceList.pDeviceInfo[i])){
            return 1;
        }
        else return 0;
    }

    return 0;
}


//枚举设备
int HikCameraManager::enum_device(){
    cameras_size=0;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    // nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    nRet = MV_CC_EnumDevices(MV_USB_DEVICE, &stDeviceList);//只枚举USB相机
    if (nRet != MV_OK)
    {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
        return 0;
    }
    if (stDeviceList.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            // printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                break;
            } 
            // PrintDeviceInfo(pDeviceInfo);
            cameras_size+=1;        
        }  
    } 
    else
    {
        printf("Find No Devices!\n");
        return 0;
    }
    cout<<"find "<<cameras_size<<" camera"<<endl;
    return 1;
}

HikCamera* HikCameraManager::get_hik_camera(int index){
    if(index > cameras_size){
        cout<<"index out of range!!!"<<endl;
        index = 0;
    }
    return cameras+index;
}


/*
 * type 文件类型
 *  0 - 照片
 *  1 - 视频（不创建文件）
 */
string setFolderPath(string foldername, int type)
{
    stringstream ss;
    auto systemtime = chrono::system_clock::now();
    time_t  systemtime_now =chrono::system_clock::to_time_t(systemtime);
    tm* systemtime_local = localtime(&systemtime_now);
    ss<<systemtime_local->tm_year + 1900 <<"_"
      <<systemtime_local->tm_mon + 1 <<"_"
      <<systemtime_local->tm_mday <<"_"
      <<systemtime_local->tm_hour <<"_"
      <<systemtime_local->tm_min <<"_"
      <<systemtime_local->tm_sec;

    string savedFolderName  = foldername + ss.str();    //保存的路径

    if(type == 0)
    {
        //c++17才能使用filesystem
        filesystem::path folderpath = savedFolderName;
        if(!filesystem::exists(folderpath))
        {
            filesystem::create_directory(folderpath);
            cout<<savedFolderName<<" created successfully!"<<endl;
        } else
        {
            cout<<savedFolderName<<" already exists!"<<endl;
        }
    } else{;}

    return savedFolderName;
}


/*
 * maxImgSum 取图总数
 */
void HikCamera::auto_recordImgs(int maxImgsSum)
{
    string savedFolderName  = setFolderPath("autoSavedPictures", 0);
    int click_down          = 5;                    //开始取图倒计时  (s)
    int interval            = 2;                    //取图间隔       (s)
    int getImgRate          = 300;                  //取图速率      （ms）
    int imgs_sum_now        = 0;
    int img_order           = 0;
    string  img_name;

    //cv::namedWindow("AutoRecordPictures",cv::WINDOW_NORMAL);
    cv::namedWindow("AutoRecordPictures",0);
    cv::Mat src;

    //准备时间
    while (click_down > 0)
    {
        double  time = 0;
        while (1)
        {
            auto  start = chrono::high_resolution_clock ::now();
            this->get_image(src);
            //下面可以加上图像处理
            //this->dealImg();

            cv::putText(src, to_string(click_down), cv::Point(src.cols/2,src.rows/2),
                        FONT_HERSHEY_SIMPLEX,5,cv::Scalar(0,0,255),4);
            cv::imshow("AutoRecordPictures",src);
            cv::waitKey(1);

            auto  end = chrono::high_resolution_clock ::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);
            time += duration.count();
            if(time>=1000)
            {
                break;
            }
        }
        click_down --;
    }

    int tmp_interval = interval;
    while (imgs_sum_now < maxImgsSum)
    {
        while (interval > 0)
        {
            double  time = 0;
            while (1)
            {
                auto  start = chrono::high_resolution_clock ::now();
                this->get_image(src);
                //可以加上图像处理
                //this->dealImg();

//                cv::putText(src, to_string(interval), cv::Point(50,50),
//                            FONT_HERSHEY_SIMPLEX,2,cv::Scalar(0,255,0),3);
                cv::imshow("AutoRecordPictures",src);
                cv::waitKey(1);

                auto  end = chrono::high_resolution_clock ::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);
                time += duration.count();
                if(time >= getImgRate)
                {
                    break;
                }
            }
            interval --;
        }

        cv::putText(src, to_string(img_order) + ".jpg", cv::Point(50,50),
                    FONT_HERSHEY_SIMPLEX,2,cv::Scalar(0,255,0),3);
        cv::imshow("AutoRecordPictures",src);
        cv::waitKey(1);

        //save pictures
        img_name ="./" + savedFolderName+ "/" + to_string(img_order++) + ".jpg";
        cv::imwrite(img_name, src);
        cout<<img_name<<" saved successfully!!!"<<endl;
        imgs_sum_now ++;
        interval = tmp_interval;
    }
}


//按'S' 或 ‘s’保存图片
void HikCamera::manual_recordImgs()
{
    string savedFolderName  = setFolderPath("manualSavedPictures", 0);
    int img_order = 0;
    string img_name;
    cv::namedWindow("ManualRecordPictures",cv::WINDOW_NORMAL);
    cv::Mat src;
    char key;
    while (1)
    {
        this->get_image(src);
        //可以加上图像处理
        //this->dealImg();
        cv::putText(src, to_string(img_order) + ".jpg", cv::Point(50,50),
                    FONT_HERSHEY_SIMPLEX,2,cv::Scalar(0,255,0),3);
        cv::imshow("ManualRecordPictures",src);
        key = cv::waitKey(10);

        if(key == 'S' || key == 's')
        {
            //save pictures
            img_name ="./" + savedFolderName+ "/" + to_string(img_order++) + ".jpg";
            cv::imwrite(img_name, src);
            cout<<img_name<<" saved successfully!!!"<<endl;
        }

        if(key == 27)
        {
            exit(0);
        }
    }

}


void HikCamera::recordVideo()
{
    //TODO::saved video fps set
    int fps = 100;
    int frame_cnt = 0;

    Restart:
    string savedFolderName  = setFolderPath("", 1);
    string savedVideoPath = "./" + savedFolderName + ".avi";


    cv::Mat src;
    this->get_image(src);
    cv::Size size_ = src.size();
    // or you can choose any size
    //cv::Size size_=cv::Size(1280,1024);

    VideoWriter videowriter_(savedVideoPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, size_, true);
    if (!videowriter_.isOpened()) {
        std::cout << "videowriter opened failure!" << std::endl;
        exit(0);
    }else
    {
        std::cout << "videowriter opened successfully!!! " << std::endl;
    }

    cv::namedWindow("RecordVideos",cv::WINDOW_NORMAL);
    char key;
    while (1)
    {
        this->get_image(src);
        cv::imshow("RecordVideos", src);
        key = cv::waitKey(1);
        if(key == 27)
        {
            exit(0);
        }

        cv::resize(src, src, size_);
        videowriter_ << src;
        frame_cnt ++;
        if((frame_cnt % 15000 )== 0)
        {
            cout<<savedVideoPath<<" record finished!!!"<<endl;
            videowriter_.release();
            goto Restart;
        }
    }
}


HikCameraManager::~HikCameraManager(){
    if(cameras != nullptr){
        if(cameras_size==1){
            delete cameras;
        }
        else{
            delete[] cameras;
        }
        cameras = nullptr;
    }
}