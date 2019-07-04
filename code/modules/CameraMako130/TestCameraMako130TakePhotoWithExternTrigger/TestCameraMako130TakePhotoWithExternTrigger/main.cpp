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

#include "VimbaCPP/Include/VimbaCPP.h"
#include "Common/StreamSystemInfo.h"
#include "Common/ErrorCodeToMessage.h"
#include "VimbaCPP/Include/IFrameObserver.h"
#include "Bitmap.h"
#include "CameraMakoCleanUpAndSaveImage.h"
#include "VimbaCPP/Source/Clock.h"
#include "CameraTimer.h"
/**printing camera info for a camera.
*\note this function is used with for_each and is called for each camera in range cameras.begin(), cameraas.end()
*/

//void FrameObserver :: FrameReceived ( const FramePtr pFrame ){
//    // Send notification to working thread
//    // Do not apply image processing within this callback ( performance )
//    // When the frame has been processed , requeue it
//    VmbFrameStatusType eReceiveStatus;
//    if(VmbErrorSuccess==pFrame->GetReceiveStatus(eReceiveStatus)){
//        if(VmbFrameStatusComplete==eReceiveStatus){
//            //code here to react on a successfully received frame
//        }
//        else{
//            //code here to react on an unsuccessfully received frame
//        }
//    }
//    m_pCamera -> QueueFrame ( pFrame );
//    std::cout<<"One photo taken"<<std::endl;
//    CreatAndSaveImag(pFrame);
//}


#define TeseCppVersion
#ifdef TeseCppVersion
#include "CameraMako130.h"
int main(){
    CameraMako130 TheCamera;
    TheCamera.InitCameraTriggerGPIO();
    TheCamera.InitCameraTriggrtTimer();
    TheCamera.InitCameraParas();
    Clock::SleepMS(60000);
    TheCamera.CleanUpCamera();
    return 0;
}

#endif // TeseCppVersion

//#define TeseCVersion
#ifdef TeseCVersion

using namespace  AVT;
using namespace VmbAPI;
using namespace Examples;



struct CleanUpDtaTransfert DataTransferToCleanUp;

void CameraFailed(int sign_no){
    if(sign_no==SIGINT||sign_no==SIGQUIT||sign_no==SIGSEGV){//Ctrl + C ||
        std::cout<<"I have get SIGINT"<<std::endl;
        CleanUpFunc(& DataTransferToCleanUp);
        CloseTimerGPIO();
    }
}


int main(){
    signal(SIGINT,CameraFailed);
    signal(SIGQUIT,CameraFailed);
    signal(SIGSEGV,CameraFailed);
    InitGPIO();
    InitTimer();
    //Vimba :: RunExample (void)
    VmbInt64_t nPLS; // Payload size value
    FeaturePtr pFeature ; // Generic feature pointer
    VimbaSystem &sys = VimbaSystem :: GetInstance (); // Create and get Vimba singleton
    std::cout<<"Vimba C++ API Version "<<sys<<"\n";           // Print out version of Vimba
    CameraPtrVector cameras ; // Holds camera handles
    CameraPtr camera ;
    FramePtrVector frames (1); // Frame array

    //Some bool value to present whether close peration needed
    struct NeedCleanFlag bNeedCleanFlag;
    bNeedCleanFlag.bNeedEndCapture=false;
    bNeedCleanFlag.bNeedFlushQueue=false;
    bNeedCleanFlag.bNeedRevokeAllFrames=false;
    bNeedCleanFlag.bNeedUnregisterObserver=false;
    bNeedCleanFlag.bNeedCloseCamera=false;
    bNeedCleanFlag.bNeedShutdown=false;
    // Start the API , get and open cameras
    VmbErrorType    err =sys. Startup ();
    if(VmbErrorSuccess==err){
        bNeedCleanFlag.bNeedShutdown=true;
        err=sys.GetCameras ( cameras );// Fetch all cameras known to Vimba
        if( VmbErrorSuccess == err ){
            std::cout << "Cameras found: " << cameras.size() <<"\n\n";
        }
        else{
            std::cout << "Could not list cameras. Error code: " << err << "("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "\n";
        }
        camera = cameras [0];// To get the pointer of the first camera
        std::cout<<"Try to open camera ..."<<std::endl;
        if(VmbErrorSuccess== camera->Open(VmbAccessModeFull)){
            std::cout<<"Camera opened"<<std::endl;
            bNeedCleanFlag.bNeedCloseCamera=true;
            camera -> FlushQueue ();
            camera -> RevokeAllFrames ();
        }
        else{
            std::cout<<"Camera open failed"<<std::endl;
        }
    }
    else{
        std::cout << "Could not start system. Error code: " << err <<"("<<AVT::VmbAPI::Examples::ErrorCodeToMessage(err)<<")"<< "\n";
    }

    // Get the image size for the required buffer
    // Allocate memory for frame buffer
    // Register frame observer / callback for each frame
    // Announce frame to the API

        /*Set exposure mode*/

//    /*Set exposure time*/
//        if(VmbErrorSuccess==camera->GetFeatureByName ("ExposureMode", pFeature )){
//        if(VmbErrorSuccess==pFeature -> SetValue ("TriggerWidth")){
//            std::cout<<"Successfully Set feature exposure mode"<<std::endl;
//        }
//        else{
//            std::cout<<"Can't Set feature exposure mode"<<std::endl;
//            std::cout<<pFeature -> SetValue ("TriggerWidth")<<std::endl;
//        }
//    }
//    else{
//        std::cout<<"Can't get feature exposure mode"<<std::endl;
//    }

    if(VmbErrorSuccess==camera->GetFeatureByName ("ExposureMode", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue ("Timed")){
            std::cout<<"Successfully Set feature exposure mode"<<std::endl;
        }
        else{
            std::cout<<"Can't Set feature exposure mode"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature exposure mode"<<std::endl;
    }

    if(VmbErrorSuccess==camera->GetFeatureByName ("ExposureTime", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue (5000.0)){
            std::cout<<"Successfully Set feature exposure time"<<std::endl;
        }
        else{
            std::cout<<"Can't Set feature exposure time"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature exposure time"<<std::endl;
    }
    /* Set trigger mode*/
    if(VmbErrorSuccess==camera->GetFeatureByName ("TriggerActivation", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue ("RisingEdge")){
            std::cout<<"Successfully set feature TriggerActivation"<<std::endl;
        }
        else{
            std::cout<<"Can't set feature TriggerActivation"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature TriggerActivation"<<std::endl;
    }
    if(VmbErrorSuccess==camera->GetFeatureByName ("TriggerSource", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue ("Line2")){
            if(VmbErrorSuccess==camera->GetFeatureByName ("TriggerMode", pFeature )){
                if(VmbErrorSuccess==pFeature -> SetValue ( "On" )){
                    std::cout<<"Set trigger mode successfully"<<std::endl;
                }
                else{
                    std::cout<<"Can't set trigger mode"<<std::endl;
                }
            }
            else{
                std::cout<<"Can't get feature trigger mode"<<std::endl;
            }
        }
        else{
            std::cout<<"Can't set trigger source"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature trigger source"<<std::endl;
    }

    /*Prepare image acquisition*/
    if(VmbErrorSuccess==camera -> GetFeatureByName ("PayloadSize", pFeature )){
        if(VmbErrorSuccess==pFeature -> GetValue (nPLS )){
            for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
                (*iter).reset(new Frame (nPLS));
                (*iter)-> RegisterObserver ( IFrameObserverPtr (new FrameObserver ( camera )));
                camera -> AnnounceFrame (* iter );
            }
            bNeedCleanFlag.bNeedUnregisterObserver=true;
            bNeedCleanFlag.bNeedRevokeAllFrames=true;
            // Start the capture engine (API)
            if(VmbErrorSuccess==camera -> StartCapture ()){
                bNeedCleanFlag.bNeedEndCapture=true;
                for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
                    // Put frame into the frame queue
                    camera -> QueueFrame (* iter );
                }
                bNeedCleanFlag.bNeedFlushQueue=true;
            }
            else{
                std::cout<<"Failed start capture"<<std::endl;
            }

        }
        else{
            std::cout<<"Failed to get value of PayloadSize"<<std::endl;
        }
    }
    else{
        std::cout<<"Failed to get PayloadSize feature"<<std::endl;
        std::cout<<camera -> GetFeatureByName ("PayloadSize", pFeature )<<std::endl;
    }

    /*Start image acquisition*/
    // Start the acquisition engine ( camera )
    if(VmbErrorSuccess==camera->GetFeatureByName("AcquisitionStart",pFeature)){
        if(VmbErrorSuccess==pFeature->RunCommand()){
            std::cout<<"Acquisition started"<<std::endl;
        }
        else{
            std::cout<<"Acquisition failed"<<std::endl;
        }
    }
    else{
        std::cout<<"Failed get AcquisitionStart feature"<<std::endl;
    }
    std::cout<<"Waiting to take photo"<<std::endl;
    // Program runtime , e.g., Sleep (2000);
    Clock::SleepMS(60000);
    std::cout<<"Stop Taking photo"<<std::endl;

    // Stop the acquisition engine ( camera )
    /*Stop image acquisition*/
    if(VmbErrorSuccess==camera -> GetFeatureByName ("AcquisitionStop", pFeature )){
        if(VmbErrorSuccess==pFeature -> RunCommand ()){
        }
        else{
            std::cout<<"Failed to stop aquisition"<<std::endl;
        }
    }
    else{
            std::cout<<"Failed to get acquisition stop feature"<<std::endl;
    }

    // Stop the capture engine (API)
    // Flush the frame queue
    // Revoke all frames from the API
    /*Clean up*/
    DataTransferToCleanUp.camera=camera;
    DataTransferToCleanUp.frames=frames;
    DataTransferToCleanUp.sys=&sys;
    DataTransferToCleanUp.NeedCleanFlag=&bNeedCleanFlag;

    CleanUpFunc(&bNeedCleanFlag,camera,frames,&sys);
     // Always pair sys. Startup and sys. Shutdown
    CloseTimerGPIO();
    return 0;
}
#endif // TeseCVersion

