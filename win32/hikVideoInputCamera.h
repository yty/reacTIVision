

#pragma once
#include "windows.h"
#include <string.h>
#include "stdio.h"
#include "time.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include "../common/CameraEngine.h"

using namespace std;

#define hibyte(x) (unsigned char)((x)>>8)

struct HIKCameraDef{

	public:
		HIKCameraDef(const string & _ipAddress = "192.168.1.68", const string & _username = "admin", const string & _password = "12345", int _port = 8000)
			:ipAddress(_ipAddress)
			,username(_username)
			,password(_password)
			,port(_port)
		{
		}

		int port; 
		string ipAddress;
		string username;
		string password;

};

class hikVideoInputCamera : public CameraEngine{

	public:
		hikVideoInputCamera(const char * config_file);
		~hikVideoInputCamera();

		static void listDevices();

		bool findCamera();
		bool initCamera();
		bool startCamera();
		unsigned char* getFrame();
		bool stopCamera();
		bool stillRunning();
		bool resetCamera();
		bool closeCamera();

		bool isNew();
		int getCameraSettingStep(int mode);
		int getMinCameraSetting(int mode);
		int getMaxCameraSetting(int mode);
		int getCameraSetting(int mode);
		bool setCameraSetting(int mode, int value);
		bool setCameraSettingAuto(int mode, bool flag);
		bool getCameraSettingAuto(int mode);
		bool setDefaultCameraSetting(int mode);
		int getDefaultCameraSetting(int mode);
		void updateSettings();

		void control(int key);
		void showSettingsDialog();

	private:
		bool disconnect;
		LONG m_lUserID;
		LONG m_nPort;
		NET_DVR_DEVICEINFO_V30 struDeviceInfo;
		LONG lRealPlayHandle;

		unsigned char *m_dataRGB;	//保存像素数据的指针,这个数据不对外开放
		bool m_haveNewFrame;	    //是否有新的帧

		bool bGrabberInited;		//是否进行初始化
		bool bIsFrameNew;
		int m_cameraWidth;		    //摄像机的像素宽
		int m_cameraHeight;		    //摄像机的像素高
		int m_buffSize;			    //像素数据buff的尺寸
		bool m_isWork;			    //设置设备的工作状态
		HIKCameraDef m_def;

};