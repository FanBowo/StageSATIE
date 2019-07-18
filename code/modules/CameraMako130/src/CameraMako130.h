#ifndef CAMERAMAKO130_H_INCLUDED
#define CAMERAMAKO130_H_INCLUDED
/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListCameras.cpp

  Description: The ListCameras example will list all available cameras that
               are found by VimbaCPP.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <signal.h>

//#include "ListCameras.h"
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
#include "CameraMakoCleanUpAndSaveImage.h"
#include "VimbaCPP/Source/Clock.h"
//#include "CameraTimer.h"


//#include "VimbaCPP/Include/VimbaCPP.h"
//#include "Common/StreamSystemInfo.h"
//#include "Common/ErrorCodeToMessage.h"
//#include "VimbaCPP/Include/IFrameObserver.h"
//#include "Bitmap.h"
//#include "CameraMakoCleanUpAndSaveImage.h"
//#include "VimbaCPP/Source/Clock.h"
//#include "CameraTimer.h"
/**printing camera info for a camera.
*\note this function is used with for_each and is called for each camera in range cameras.begin(), cameraas.end()
*/
using namespace  AVT;
using namespace VmbAPI;
using namespace Examples;

extern void InitCameraTimer();
//void TriggerPWM_pullup(union sigval sv);
//void TriggerPWM_pushdown(union sigval sv);

extern void CloseTimerGPIO();
extern int InitCameraTriggerGPIO();


class CameraMako130 {
    public:
    void InitCameraParas();
    void InitCameraTriggerGPIO();
    void InitCameraTriggrtTimer();
    CameraMako130();

    void CameraFailed();
    void CleanUpCamera();

    CameraPtr camera ;
    private:

    void StarSysAndOpenCamera();
    void SetCameraExposureMode();
    void SetExposureTime();
    void SetTriggerMode();
    void PrepareImagAqc();
    void StartImagAqc();
    void RegisteParaChangeEvent();


    VmbInt64_t nPLS; // Payload size value
    FeaturePtr pFeature ; // Generic feature pointer
    VimbaSystem *sys; // Create and get Vimba singleton
    CameraPtrVector cameras ; // Holds camera handles

    FramePtrVector frames; // Frame array
    struct NeedCleanFlag bNeedCleanFlag;//Some bool value to present whether close peration needed
//    static struct CleanUpDtaTransfert DataTransferToCleanUp;
};


#endif // CAMERAMAKO130_H_INCLUDED
