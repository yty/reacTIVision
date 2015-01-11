

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

		unsigned char *m_dataRGB;	//�����������ݵ�ָ��,������ݲ����⿪��
		bool m_haveNewFrame;	    //�Ƿ����µ�֡

		bool bGrabberInited;		//�Ƿ���г�ʼ��
		bool bIsFrameNew;
		int m_cameraWidth;		    //����������ؿ�
		int m_cameraHeight;		    //����������ظ�
		int m_buffSize;			    //��������buff�ĳߴ�
		bool m_isWork;			    //�����豸�Ĺ���״̬
		HIKCameraDef m_def;

};