#ifndef CAMERAMAKOCLEANUPANDSAVEIMAGE_H_INCLUDED
#define CAMERAMAKOCLEANUPANDSAVEIMAGE_H_INCLUDED

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <signal.h>

//#include "ListCameras.h"

#include "VimbaCPP/Include/VimbaCPP.h"
#include "Common/StreamSystemInfo.h"
#include "Common/ErrorCodeToMessage.h"
#include "VimbaCPP/Include/IFrameObserver.h"
#include "Bitmap.h"
#include "VimbaCPP/Source/Clock.h"
/**printing camera info for a camera.
*\note this function is used with for_each and is called for each camera in range cameras.begin(), cameraas.end()
*/
using namespace  AVT;
using namespace VmbAPI;
using namespace Examples;

struct NeedCleanFlag{
    bool bNeedEndCapture;
    bool bNeedFlushQueue;
    bool bNeedRevokeAllFrames;
    bool bNeedUnregisterObserver;
    bool bNeedCloseCamera;
    bool bNeedShutdown;
};

struct CleanUpDtaTransfert{
    struct NeedCleanFlag *NeedCleanFlag;
    CameraPtr camera;
    FramePtrVector frames;
    VimbaSystem * sys;
};

extern void CreatAndSaveImag(const FramePtr pFrame );
extern void CheckAcquisitionStatus();

void SetAcquisitionStatusSelector(const CameraPtr camera, const char * AcquisitionStatusSelectorVal);
void CleanUpFunc(struct NeedCleanFlag *bNeedCleanFlag,\
                    CameraPtr camera,FramePtrVector frames,\
                    VimbaSystem *sys);
void CleanUpFunc(CleanUpDtaTransfert *pDataTransferToCleanUp);

//Constructor for the FrameObserver class
class FrameObserver :public IFrameObserver{
    public:
    FrameObserver ( CameraPtr pCamera );
    void FrameReceived ( const FramePtr pFrame );
};

//Define un class for parameter change event
class AcquisitionStatusChageEvent:public IFeatureObserver{
    public:
    AcquisitionStatusChageEvent();
    void FeatureChanged(const FeaturePtr &pFeature);
};


struct PhotoFormat{
VmbUint32_t nImageSize;
VmbUint32_t nWidth;
VmbUint32_t nHeight;
bool bFormatGetted;
};

#endif // CAMERAMAKOCLEANUPANDSAVEIMAGE_H_INCLUDED
