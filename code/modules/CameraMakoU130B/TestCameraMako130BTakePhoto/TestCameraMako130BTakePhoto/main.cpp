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
void CreatAndSaveImag(const FramePtr pFrame );
//Constructor for the FrameObserver class
class FrameObserver :public IFrameObserver{
    public:
    FrameObserver ( CameraPtr pCamera );
    void FrameReceived ( const FramePtr pFrame );
};

FrameObserver:: FrameObserver ( CameraPtr pCamera ) : IFrameObserver ( pCamera ){}
// Frame callback notifies about incoming frames
void FrameObserver :: FrameReceived ( const FramePtr pFrame ){
    // Send notification to working thread
    // Do not apply image processing within this callback ( performance )
    // When the frame has been processed , requeue it
    VmbFrameStatusType eReceiveStatus;
    if(VmbErrorSuccess==pFrame->GetReceiveStatus(eReceiveStatus)){
        if(VmbFrameStatusComplete==eReceiveStatus){
            //code here to react on a successfully received frame
        }
        else{
            //code here to react on an unsuccessfully received frame
        }
    }
    m_pCamera -> QueueFrame ( pFrame );
    std::cout<<"One photo taken"<<std::endl;
    CreatAndSaveImag(pFrame);
}


int main(){
    //Vimba :: RunExample (void)
    VmbInt64_t nPLS; // Payload size value
    FeaturePtr pFeature ; // Generic feature pointer
    VimbaSystem &sys = VimbaSystem :: GetInstance (); // Create and get Vimba singleton
    std::cout<<"Vimba C++ API Version "<<sys<<"\n";           // Print out version of Vimba
    CameraPtrVector cameras ; // Holds camera handles
    CameraPtr camera ;
    FramePtrVector frames (15); // Frame array

    //Some bool value to present whether close peration needed
    bool bNeedEndCapture=false;
    bool bNeedFlushQueue=false;
    bool bNeedRevokeAllFrames=false;
    bool bNeedUnregisterObserver=false;
    bool bNeedCloseCamera=false;
    bool bNeedShutdown=false;
    // Start the API , get and open cameras
    VmbErrorType    err =sys. Startup ();
    if(VmbErrorSuccess==err){
        bNeedShutdown=true;
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
            bNeedCloseCamera=true;
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
    /*Prepare image acquisition*/
    if(VmbErrorSuccess==camera -> GetFeatureByName ("PayloadSize", pFeature )){
        if(VmbErrorSuccess==pFeature -> GetValue (nPLS )){
            for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
                (*iter).reset(new Frame (nPLS));
                (*iter)-> RegisterObserver ( IFrameObserverPtr (new FrameObserver ( camera )));
                camera -> AnnounceFrame (* iter );
            }
            bNeedUnregisterObserver=true;
            bNeedRevokeAllFrames=true;
            // Start the capture engine (API)
            if(VmbErrorSuccess==camera -> StartCapture ()){
                bNeedEndCapture=true;
                for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
                    // Put frame into the frame queue
                    camera -> QueueFrame (* iter );
                }
                bNeedFlushQueue=true;
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
    Clock::SleepMS(20);
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
    if(bNeedEndCapture){
        camera -> EndCapture ();
    }
    if(bNeedFlushQueue){
        camera -> FlushQueue ();
    }
    if(bNeedRevokeAllFrames){
        camera -> RevokeAllFrames ();
    }

    if(bNeedUnregisterObserver){
        for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
            // Unregister the frame observer / callback
            (* iter)-> UnregisterObserver ();
        }
    }
    if(bNeedCloseCamera){
        camera ->Close ();
    }
    if(bNeedShutdown){
        sys. Shutdown ();
    }
     // Always pair sys. Startup and sys. Shutdown

    return 0;
}

void CreatAndSaveImag(const FramePtr pFrame ){
    std::string pFileNameBase = "SynchronousGrab.bmp";
    static int picnum=0;
    picnum++;
    std::string pFileNametemp=pFileNameBase+std::to_string(picnum);
    const char *pFileName=pFileNametemp.c_str();
    VmbErrorType    err         = VmbErrorSuccess;
    VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
    VmbUint32_t nImageSize = 0;
    err = pFrame->GetImageSize( nImageSize );
    if ( VmbErrorSuccess == err )
    {
        VmbUint32_t nWidth = 0;
        err = pFrame->GetWidth( nWidth );
        if ( VmbErrorSuccess == err )
        {
            VmbUint32_t nHeight = 0;
            err = pFrame->GetHeight( nHeight );
            if ( VmbErrorSuccess == err )
            {
                VmbUchar_t *pImage = NULL;
                err = pFrame->GetImage( pImage );
                if ( VmbErrorSuccess == err )
                {
                    AVTBitmap bitmap;

                    if ( VmbPixelFormatRgb8 == ePixelFormat )
                    {
                        bitmap.colorCode = ColorCodeRGB24;
                    }
                    else
                    {
                        bitmap.colorCode = ColorCodeMono8;
                    }

                    bitmap.bufferSize = nImageSize;
                    bitmap.width = nWidth;
                    bitmap.height = nHeight;

                    // Create the bitmap
                    if ( 0 == AVTCreateBitmap( &bitmap, pImage ))
                    {
                        std::cout << "Could not create bitmap.\n";
                        err = VmbErrorResources;
                    }
                    else
                    {
                        // Save the bitmap
                        if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))
                        {
                            std::cout << "Could not write bitmap to file.\n";
                            err = VmbErrorOther;
                        }
                        else
                        {
                            std::cout << "Bitmap successfully written to file \"" << pFileName << "\"\n" ;
                            // Release the bitmap's buffer
                            if ( 0 == AVTReleaseBitmap( &bitmap ))
                            {
                                std::cout << "Could not release the bitmap.\n";
                                err = VmbErrorInternalFault;
                            }
                        }
                    }
                }
            }
        }
    }
}
