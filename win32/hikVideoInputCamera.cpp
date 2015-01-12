#include "hikVideoInputCamera.h"

//一些参考资料
//https://code.google.com/p/iloujiok-ffp-csharp/source/browse/trunk/VideoControl/HikPlayer.cs
// Get R Value
#define GET_R(sy,su,sv) ((298*((sy)-16) + 409*((sv)-128) + 128)>>8)
// Get G Value
#define GET_G(sy,su,sv) ((298*((sy)-16) - 100*((su)-128) - 208*((sv)-128)+128)>> 8)
// Get B Value
#define GET_B(sy,su,sv) ((298*((sy)-16) + 516*((su)-128) + 128)>> 8)
// Check RGB value, if overflow then correct it
#define CLIP(value)  (value)<0 ? 0 : ((value)>255 ? 255 : (value))

//YUV转RGB
void YUV2RGB(unsigned char *pYUV, unsigned char *pRGB,int w, int h)
{

	if (NULL == pYUV || pRGB == NULL)	return ;

	long m_nImageSize = w * h;
	//Get Y/U/V Pointer from YUV frame data
	unsigned char *pYbuf  = pYUV;     //Pointer to Y
	unsigned char *pVbuf  = pYUV + m_nImageSize;   //Pointer to U
	unsigned char* pUbuf  = pVbuf +( m_nImageSize >> 2);  //Pointer to V

	int Y,U,V;
	int offSet = 0;
	int offset1 = 0;
	int k =0;

	for (int i=0; i<h; i++)
	{
		offset1 = (i>>1) * (w >> 1);
		for (int j=0; j<w; j++)
		{
			Y = *(pYbuf + w * i + j);
			offSet = offset1 + (j>>1);
			U = *(pUbuf + offSet);
			V = *(pVbuf + offSet);
			//Get RGB values
			pRGB[k]  = CLIP(GET_R(Y, U, V));
			pRGB[k + 1] =  CLIP(GET_G(Y, U, V));
			pRGB[k + 2] = CLIP(GET_B(Y, U, V));
			k+=3;
		}
	}
}

//解码的回调函数
//因为CALLBACK是全局的,没有办法使用类的变量,所以这里又声明了一份给全局函数用
unsigned char* pOutRGB;	//输出的RGB颜色

bool *haveNewFrame = FALSE;		//结合接口
int *width = NULL;
int *height = NULL;

void CALLBACK DecCBFun(long nPort,char * pBuf,long nSize, FRAME_INFO * pFrameInfo,  long nReserved1,long nReserved2) 
{
	long lFrameType = pFrameInfo->nType;
	if(lFrameType ==T_YV12)
	{ 
		YUV2RGB((unsigned char *)pBuf,pOutRGB,*width,*height);
		* haveNewFrame = true;
	}
}
//实时解码的回调函数
LONG lPort;
void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser)
{
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD: //系统头40字节
		
		if (!PlayM4_GetPort(&lPort)){
			lPort = -1;
			break;//获取播放库未使用的通道号
		}
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME))	break;//设置实时流播放模式,必须在播放之前设置。 

			if (!PlayM4_OpenStream(lPort, pBuffer, dwBufSize, 1024 * 1024))	break;//打开流接口 最后一个参数是设置播放器中存放数据流的缓冲区大小。范围是SOURCE_BUF_MIN~ SOURCE_BUF_MAX。

			PlayM4_SetDecCallBack(lPort,DecCBFun);

			if(!PlayM4_SetDecCBStream(lPort,1))	break;	//设置解码回调流类型 1视频流，2音频流，3复合流
			if (!PlayM4_Play(lPort, NULL))	break;//播放开始
		}
	case NET_DVR_STREAMDATA:   //视频数据
		if (dwBufSize > 0 && lPort != -1)
		{
			if (!PlayM4_InputData(lPort, pBuffer, dwBufSize)){
				break; //TURE,表示已经输入数据。FALSE 表示失败，数据没有输入。
			}
		}
	case NET_DVR_AUDIOSTREAMDATA:	//音频数据
		break;
	}
}

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	switch(dwType) 
	{
	case EXCEPTION_RECONNECT:    //预览时重连
		printf("----------reconnect--------%d\n", time(NULL));
		break;
	default:
		break;
	}
}


hikVideoInputCamera::hikVideoInputCamera(const char * cfg):
	CameraEngine (cfg),
	bGrabberInited(false)
	,bIsFrameNew(false)
	,m_isWork(true)
	,m_haveNewFrame(false)
	,m_dataRGB(NULL)
{
	cam_buffer = NULL;
	running = false;

	timeout= 1000;
	lost_frames=0;
	disconnect = false;
}

hikVideoInputCamera::~hikVideoInputCamera(){
	if (cam_buffer!=NULL) delete cam_buffer;
}

void hikVideoInputCamera::listDevices() {

	//videoInput VI;
	//std::vector <std::string> devList = VI.getDeviceList();

	//int count = devList.size();
	//if (count==0) { std::cout << " no videoInput camera found!" << std::endl; return; }
	//else if (count==1) std::cout << "1 videoInput camera found:" << std::endl;
	//else std::cout << count << " videoInput cameras found:" << std::endl;
	//
	//for(int i = 0; i <count; i++){
	//	std::cout << "\t" << i << ": " << devList[i] << std::endl;
	//}
}

bool hikVideoInputCamera::findCamera() {

	//readSettings();
	//std::vector <std::string> devList = VI->getDeviceList();

	//int count = devList.size();
	//if (count==0) { std::cout << " no videoInput camera found!" << std::endl; return false; }
	//else if (count==1) std::cout << "1 videoInput camera found" << std::endl;
	//else std::cout << count << " videoInput cameras found" << std::endl;
	//
	//if (config.device<0) config.device=0;
	//if (config.device>=count) config.device = count-1;

	//cameraID = config.device;
	//sprintf(cameraName,devList[cameraID].c_str());
	return true;
}

bool hikVideoInputCamera::initCamera() {

	//// TODO: get actual video formats
	//if (config.cam_width<=0) config.cam_width=640;
	//if (config.cam_height<=0) config.cam_height=480;
	//if (config.cam_fps<=0) config.cam_fps=30;

	//VI->setIdealFramerate(this->cameraID, config.cam_fps);
	//if (config.compress == true) VI->setRequestedMediaSubType(VI_MEDIASUBTYPE_MJPG);
	//bool bOk = VI->setupDevice(cameraID, config.cam_width, config.cam_height);

	//if (bOk == true) {	
	//	this->cam_width = VI->getWidth(cameraID);
	//	this->cam_height = VI->getHeight(cameraID);
	//	this->fps = config.cam_fps;
	//} else return false;


	//applyCameraSettings();
	//setupFrame();
	//if (config.frame) cam_buffer = new unsigned char[this->cam_width*this->cam_height];
	//else cam_buffer = new unsigned char[this->cam_width*this->cam_height];

	if(bGrabberInited){
		//ofLogError("ofxHIKCameraGrabber")<<"initGrabber()=> 不能重复初始化";
		return FALSE;
	}

	config.cam_width = 1280;
	config.cam_height = 960;
	config.cam_fps = 30;

	this->cam_width = 1280;
	this->cam_height = 960;
	this->fps = config.cam_fps;

	applyCameraSettings();
	setupFrame();
	cam_buffer = new unsigned char[this->cam_width * this->cam_height];

	m_cameraWidth = 1280;
	m_cameraHeight = 960;
	width = &m_cameraWidth;
	height = &m_cameraHeight;
	m_buffSize = m_cameraWidth * m_cameraHeight * 3;
	

	NET_DVR_Init();//初始化SDK,软件中只调用1次
	//int isupport = NET_DVR_IsSupport(); //判断运行客户端的PC配置是否符合要求
	char tempStrAddr[128];
	char tempStrName[128];
	char tempStrPWD[128];
	strcpy(tempStrAddr,m_def.ipAddress.c_str());
	strcpy(tempStrName,m_def.username.c_str());
	strcpy(tempStrPWD,m_def.password.c_str());
	m_lUserID = NET_DVR_Login_V30(tempStrAddr, m_def.port, tempStrName, tempStrPWD, &struDeviceInfo); //注册登录设备
	if (m_lUserID < 0)
	{
		//ofLogError("ofxHIKCameraGrabber")<<"initGrabber()=> 初始化失败"<<NET_DVR_GetLastError();
		//printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return FALSE;
	}
	//下面开始连接回调函数并分配内存空间
	m_dataRGB = new unsigned char[m_buffSize];//分配内存空间
	if(m_dataRGB==NULL) {
		//ofLogError("ofxHIKCameraGrabber")<<"initGrabber()=> 分配图像内存空间失败";
		return FALSE;
	}
	memset(m_dataRGB,0,m_buffSize);
	//m_dataRGBOut = new unsigned char[m_buffSize];
	//if(m_dataRGBOut==NULL) return FALSE;

	haveNewFrame = &m_haveNewFrame;
	pOutRGB = m_dataRGB;

	NET_DVR_SetExceptionCallBack_V30(0, NULL,g_ExceptionCallBack, NULL); //注册接收异常、重连等消息的窗口句柄或回调函数

	NET_DVR_CLIENTINFO ClientInfo = {0};
	ClientInfo.hPlayWnd = NULL;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
	ClientInfo.lChannel     = 0;       //预览通道号
	ClientInfo.lLinkMode    = 0;       //最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
	ClientInfo.sMultiCastIP = NULL;   //多播地址，需要多播预览时配置

	BOOL bPreviewBlock = false;       //请求码流过程是否阻塞，0：否，1：是
	lRealPlayHandle = NET_DVR_RealPlay_V30(m_lUserID, &ClientInfo, NULL, NULL, bPreviewBlock);//实时预览,分别打开主,次码流视频音频数据回调函数

	if (lRealPlayHandle < 0)
	{
		//printf("NET_DVR_RealPlay_V30 error\n");
		NET_DVR_Logout(m_lUserID);
		NET_DVR_Cleanup();
		//ofLogError("ofxHIKCameraGrabber")<<"initGrabber()=> NET_DVR_RealPlay_V30 error";
		return FALSE;
	}

	if (!NET_DVR_SetRealDataCallBack(lRealPlayHandle, g_RealDataCallBack_V30, 0))
	{
		//printf("NET_DVR_SetRealDataCallBack error\n");
		//ofLogError("ofxHIKCameraGrabber")<<"initGrabber()=> NET_DVR_SetRealDataCallBack error";
	}
	m_haveNewFrame = false;
	bGrabberInited = true;
	m_nPort = lPort;
	//ofLogNotice("ofxHIKCameraGrabber")<<"initGrabber()=> 初始化成功";
	return true;
}

bool hikVideoInputCamera::startCamera(){ 
	running = true;
	return true;
}

unsigned char * hikVideoInputCamera::getFrame(){
	if(bGrabberInited){
		bIsFrameNew = false;
		if(isNew()&&m_isWork){
			bIsFrameNew = true;

			unsigned char * src = m_dataRGB;
			unsigned char * dest = cam_buffer;
			unsigned char r,g,b;

			if (config.frame) {
					src += 3*(config.frame_yoff*cam_width);
					//dest += frame_width*frame_height-1;
					int xend = (cam_width-(frame_width+config.frame_xoff));

					for (int i=0;i<frame_height;i++) {

						src +=  3*config.frame_xoff;
						for (int j=frame_width;j>0;j--) {
 							r = *src++;
							g = *src++;
							b = *src++;
							*dest++ = hibyte(r * 77 + g * 151 + b * 28);
						}
						src +=  3*xend;
					}
			} else {
				int size = cam_width*cam_height;
				//dest += size-1;
				for(int i=size;i>0;i--) {
					r = *src++;
					g = *src++;
					b = *src++;
					*dest++ = hibyte(r * 77 + g * 151 + b * 28);
				}
			}

			lost_frames = 0;
			return cam_buffer;
		}
	}
	return NULL;
}

bool hikVideoInputCamera::isNew(){
	bool temp = m_haveNewFrame;
	m_haveNewFrame = false;
	return temp;
}

bool hikVideoInputCamera::stopCamera(){
	if(!running) return false;
	running = false;
	return true;
}
bool hikVideoInputCamera::stillRunning() {
	return running;
}

bool hikVideoInputCamera::resetCamera()
{
  return (stopCamera() && startCamera());

}

bool hikVideoInputCamera::closeCamera(){
	//if (!disconnect) {
	//	updateSettings();
	//	saveSettings();
	//	VI->stopDevice(cameraID);
	//}

	//下面需要删除资源
	NET_DVR_StopRealPlay(lRealPlayHandle);//停止预览,分别停止主,次码流的预览
	//if(m_dataRGBOut!=NULL) delete m_dataRGBOut;
	NET_DVR_Logout_V30(m_lUserID);//会等待或者强制将该用户的所有资源释放或者退出（如线程等），
	//NET_DVR_Logout(m_lUserID);//则不会，仅仅将当前的用户从设备上注销了。
	PlayM4_Stop(m_nPort); //停止播放
	PlayM4_CloseStream(m_nPort); //关闭流
	PlayM4_FreePort(m_nPort);//释放播放通道
	NET_DVR_Cleanup();//释放SDK，软件中只调用1次

	if(m_dataRGB!=NULL){ 
		delete m_dataRGB;
	}

	bGrabberInited = false;
	return true;
}

void hikVideoInputCamera::control(int key) {
	 return;
}

void hikVideoInputCamera::showSettingsDialog() {
	//if (running) VI->showSettingsWindow(cameraID);
}

bool hikVideoInputCamera::setCameraSettingAuto(int mode, bool flag) {
	return true;
}

bool hikVideoInputCamera::getCameraSettingAuto(int mode) {
    
	long min,max,step,value,default,flags;
	min=max=step=value=default=flags=0;

/*	switch (mode) {
		case BRIGHTNESS: VI->getVideoSettingFilter(cameraID, VI->propBrightness, min, max,  step, value, flags, default); break;
		case GAIN:  VI->getVideoSettingFilter(cameraID, VI->propGain, min, max,  step, value, flags, default); break;
		case EXPOSURE: VI->getVideoSettingFilter(cameraID, VI->propExposure, min, max,  step, value, flags, default); break;
		case SHARPNESS: VI->getVideoSettingFilter(cameraID, VI->propSharpness, min, max,  step, value, flags, default); break;
		case FOCUS:  VI->getVideoSettingFilter(cameraID, VI->propFocus, min, max,  step, value, flags, default); break;
		case GAMMA: VI->getVideoSettingFilter(cameraID, VI->propGamma, min, max,  step, value, flags, default); break;
		default: return 0;
	}*/ 

	return (int)flags;
}


bool hikVideoInputCamera::setCameraSetting(int mode, int setting) {

	int current_setting = getCameraSetting(mode);
	if (setting==current_setting) return true;
	setCameraSettingAuto(mode,false);

	long flags = 1;

	//switch (mode) {
	//	case BRIGHTNESS: VI->setVideoSettingFilter(cameraID, VI->propBrightness, setting, NULL, false); break;
	//	case GAIN: VI->setVideoSettingFilter(cameraID, VI->propGain, setting, NULL, false); break;
	//	case EXPOSURE: VI->setVideoSettingFilter(cameraID, VI->propExposure, setting, flags, false); break;
	//	case SHARPNESS: VI->setVideoSettingFilter(cameraID, VI->propSharpness, setting, NULL, false); break;
	//	case FOCUS: VI->setVideoSettingFilter(cameraID, VI->propFocus, setting, NULL, false); break;
	//	case GAMMA: VI->setVideoSettingFilter(cameraID, VI->propGamma, setting, NULL, false); break;
	//	default: return false;
	//}

	return true;
}

int hikVideoInputCamera::getCameraSetting(int mode) {

	long min,max,step,value,default,flags;
	min=max=step=value=default=flags=0;

	//switch (mode) {
	//	case BRIGHTNESS: VI->getVideoSettingFilter(cameraID, VI->propBrightness, min, max,  step, value, flags, default); break;
	//	case GAIN:  VI->getVideoSettingFilter(cameraID, VI->propGain, min, max,  step, value, flags, default); break;
	//	case EXPOSURE: VI->getVideoSettingFilter(cameraID, VI->propExposure, min, max,  step, value, flags, default); break;
	//	case SHARPNESS: VI->getVideoSettingFilter(cameraID, VI->propSharpness, min, max,  step, value, flags, default); break;
	//	case FOCUS:  VI->getVideoSettingFilter(cameraID, VI->propFocus, min, max,  step, value, flags, default); break;
	//	case GAMMA: VI->getVideoSettingFilter(cameraID, VI->propGamma, min, max,  step, value, flags, default); break;
	//	default: return 0;
	//} 

	return (int)value;
}

int hikVideoInputCamera::getMaxCameraSetting(int mode) {

	long min,max,step,value,default,flags;
	min=max=step=value=default=flags=0;

	//switch (mode) {
	//	case BRIGHTNESS: VI->getVideoSettingFilter(cameraID, VI->propBrightness, min, max,  step, value, flags, default); break;
	//	case GAIN:  VI->getVideoSettingFilter(cameraID, VI->propGain, min, max,  step, value, flags, default); break;
	//	case EXPOSURE: VI->getVideoSettingFilter(cameraID, VI->propExposure, min, max,  step, value, flags, default); break;
	//	case SHARPNESS: VI->getVideoSettingFilter(cameraID, VI->propSharpness, min, max,  step, value, flags, default); break;
	//	case FOCUS:  VI->getVideoSettingFilter(cameraID, VI->propFocus, min, max,  step, value, flags, default); break;
	//	case GAMMA: VI->getVideoSettingFilter(cameraID, VI->propGamma, min, max,  step, value, flags, default); break;
	//	default: return 0;
	//} 

	return (int)max;
}

int hikVideoInputCamera::getMinCameraSetting(int mode) {

	long min,max,step,value,default,flags;
	min=max=step=value=default=flags=0;
	
	//switch (mode) {
	//	case BRIGHTNESS: VI->getVideoSettingFilter(cameraID, VI->propBrightness, min, max,  step, value, flags, default); break;
	//	case GAIN:  VI->getVideoSettingFilter(cameraID, VI->propGain, min, max,  step, value, flags, default); break;
	//	case EXPOSURE: VI->getVideoSettingFilter(cameraID, VI->propExposure, min, max,  step, value, flags, default); break;
	//	case SHARPNESS: VI->getVideoSettingFilter(cameraID, VI->propSharpness, min, max,  step, value, flags, default); break;
	//	case FOCUS:  VI->getVideoSettingFilter(cameraID, VI->propFocus, min, max,  step, value, flags, default); break;
	//	case GAMMA: VI->getVideoSettingFilter(cameraID, VI->propGamma, min, max,  step, value, flags, default); break;
	//	default: return 0;
	//} 

	return (int)min;
}

int hikVideoInputCamera::getCameraSettingStep(int mode) {

	long min,max,step,value,default,flags;
	min=max=step=value=default=flags=0;
	//
	//switch (mode) {
	//	case BRIGHTNESS: VI->getVideoSettingFilter(cameraID, VI->propBrightness, min, max,  step, value, flags, default); break;
	//	case GAIN:  VI->getVideoSettingFilter(cameraID, VI->propGain, min, max,  step, value, flags, default); break;
	//	case EXPOSURE: VI->getVideoSettingFilter(cameraID, VI->propExposure, min, max,  step, value, flags, default); break;
	//	case SHARPNESS: VI->getVideoSettingFilter(cameraID, VI->propSharpness, min, max,  step, value, flags, default); break;
	//	case FOCUS:  VI->getVideoSettingFilter(cameraID, VI->propFocus, min, max,  step, value, flags, default); break;
	//	case GAMMA: VI->getVideoSettingFilter(cameraID, VI->propGamma, min, max,  step, value, flags, default); break;
	//	default: return 0;
	//} 

	return (int)step;
}

bool hikVideoInputCamera::setDefaultCameraSetting(int mode) {
    
	long value=0;

	//switch (mode) {
	//	case BRIGHTNESS: VI->setVideoSettingFilter(cameraID,  VI->propBrightness, value, NULL, true); break;
	//	case GAIN: VI->setVideoSettingFilter(cameraID,  VI->propGain, value, NULL, true); break;
	//	case EXPOSURE: VI->setVideoSettingFilter(cameraID,  VI->propExposure, value, NULL, true); break;
	//	case SHARPNESS:VI->setVideoSettingFilter(cameraID,  VI->propSharpness, value, NULL, true); break;
	//	case FOCUS: VI->setVideoSettingFilter(cameraID,  VI->propFocus, value, NULL, true); break;
	//	case GAMMA: VI->setVideoSettingFilter(cameraID,  VI->propGamma, value, NULL, true); break;
	//	default: return false;
	//}

    return true;
}

int hikVideoInputCamera::getDefaultCameraSetting(int mode) {

	long min,max,step,value,default,flags;
	min=max=step=value=default=flags=0;
	
	//switch (mode) {
	//	case BRIGHTNESS: VI->getVideoSettingFilter(cameraID, VI->propBrightness, min, max,  step, value, flags, default); break;
	//	case GAIN:  VI->getVideoSettingFilter(cameraID, VI->propGain, min, max,  step, value, flags, default); break;
	//	case EXPOSURE: VI->getVideoSettingFilter(cameraID, VI->propExposure, min, max,  step, value, flags, default); break;
	//	case SHARPNESS: VI->getVideoSettingFilter(cameraID, VI->propSharpness, min, max,  step, value, flags, default); break;
	//	case FOCUS:  VI->getVideoSettingFilter(cameraID, VI->propFocus, min, max,  step, value, flags, default); break;
	//	case GAMMA: VI->getVideoSettingFilter(cameraID, VI->propGamma, min, max,  step, value, flags, default); break;
	//	default: return 0;
	//} 

	return (int)default;
}

void hikVideoInputCamera::updateSettings()  {
 
    int brightness = getCameraSetting(BRIGHTNESS);
    if (brightness==getMinCameraSetting(BRIGHTNESS)) config.brightness=SETTING_MIN;
    if (brightness==getMaxCameraSetting(BRIGHTNESS)) config.brightness=SETTING_MAX;
    if (brightness==getDefaultCameraSetting(BRIGHTNESS)) config.brightness=SETTING_DEFAULT;
   //printf("brightness %d\n",brightness);
    
    int contrast = getCameraSetting(CONTRAST);
    if (contrast==getMinCameraSetting(CONTRAST)) config.contrast=SETTING_MIN;
    if (contrast==getMaxCameraSetting(CONTRAST)) config.contrast=SETTING_MAX;
    if (contrast==getDefaultCameraSetting(CONTRAST)) config.contrast=SETTING_DEFAULT;
   //printf("contrast %d\n",contrast);
    
    int gain = getCameraSetting(GAIN);
    if (gain==getMinCameraSetting(GAIN)) config.gain=SETTING_MIN;
    if (gain==getMaxCameraSetting(GAIN)) config.gain=SETTING_MAX;
    if (gain==getDefaultCameraSetting(GAIN)) config.gain=SETTING_DEFAULT;
    //printf("gain %d\n",gain);
    
    int exposure = getCameraSetting(EXPOSURE);
    if (exposure==getMinCameraSetting(EXPOSURE)) config.exposure=SETTING_MIN;
    if (exposure==getMaxCameraSetting(EXPOSURE)) config.exposure=SETTING_MAX;
    if (exposure==getDefaultCameraSetting(EXPOSURE)) config.exposure=SETTING_DEFAULT;
    if (getCameraSettingAuto(EXPOSURE)==true) config.exposure=SETTING_AUTO;
    //printf("exposure %d\n",exposure);
    
    int sharpness = getCameraSetting(SHARPNESS);
    if (sharpness==getMinCameraSetting(SHARPNESS)) config.sharpness=SETTING_MIN;
    if (sharpness==getMaxCameraSetting(SHARPNESS)) config.sharpness=SETTING_MAX;
    if (sharpness==getDefaultCameraSetting(SHARPNESS)) config.sharpness=SETTING_DEFAULT;
    //printf("sharpness %d\n",sharpness);

    int focus = getCameraSetting(FOCUS);
    if (focus==getMinCameraSetting(FOCUS)) config.focus=SETTING_MIN;
    if (focus==getMaxCameraSetting(FOCUS)) config.focus=SETTING_MAX;
    if (focus==getDefaultCameraSetting(FOCUS)) config.focus=SETTING_DEFAULT;
    //printf("focus %d\n",focus);
    
    int gamma = getCameraSetting(GAMMA);
    if (gamma==getMinCameraSetting(GAMMA)) config.gamma=SETTING_MIN;
    if (gamma==getMaxCameraSetting(GAMMA)) config.gamma=SETTING_MAX;
    if (getCameraSettingAuto(GAMMA)==true) config.gamma=SETTING_AUTO;
    if (gamma==getDefaultCameraSetting(GAMMA)) config.gamma=SETTING_DEFAULT;
    //printf("gamma %d\n",gamma);
}