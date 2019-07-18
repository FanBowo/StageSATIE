#include "CameraMako130.h"

using namespace  AVT;
using namespace VmbAPI;
using namespace Examples;

CameraMako130::CameraMako130(){
    sys = &(VimbaSystem :: GetInstance ());// Create and get Vimba singleton
    FramePtrVector _frames(1);
    frames=_frames;
    bNeedCleanFlag.bNeedEndCapture=false;
    bNeedCleanFlag.bNeedFlushQueue=false;
    bNeedCleanFlag.bNeedRevokeAllFrames=false;
    bNeedCleanFlag.bNeedUnregisterObserver=false;
    bNeedCleanFlag.bNeedCloseCamera=false;
    bNeedCleanFlag.bNeedShutdown=false;
}

void CameraMako130::StarSysAndOpenCamera(){
        // Start the API , get and open cameras
    VmbErrorType    err =sys-> Startup ();
    if(VmbErrorSuccess==err){
        bNeedCleanFlag.bNeedShutdown=true;
        err=sys->GetCameras ( cameras );// Fetch all cameras known to Vimba
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
}

void CameraMako130::SetCameraExposureMode(){
    if(VmbErrorSuccess==camera->GetFeatureByName ("ExposureMode", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue ("TriggerWidth")){
            std::cout<<"Successfully Set feature exposure mode"<<std::endl;
        }
        else{
            std::cout<<"Can't Set feature exposure mode"<<std::endl;
            std::cout<<pFeature -> SetValue ("TriggerWidth")<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature exposure mode"<<std::endl;
    }
}

void CameraMako130:: SetExposureTime(){
//    /*Set exposure time*/
    if(VmbErrorSuccess==camera->GetFeatureByName ("ExposureMode", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue ("Timed")){
            std::cout<<"Successfully Set feature exposure mode Timed"<<std::endl;
        }
        else{
            std::cout<<"Can't Set feature exposure mode"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature exposure mode"<<std::endl;
    }

    if(VmbErrorSuccess==camera->GetFeatureByName ("ExposureTime", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue (4999.9)){
            std::cout<<"Successfully Set feature exposure time"<<std::endl;
        }
        else{
            std::cout<<"Can't Set feature exposure time"<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature exposure time"<<std::endl;
    }
}

void CameraMako130:: SetTriggerMode(){
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
}

void CameraMako130::PrepareImagAqc(){
    // Get the image size for the required buffer
    // Allocate memory for frame buffer
    // Register frame observer / callback for each frame
    // Announce frame to the API
    /*Register feature change int*/
        /*Prepare image acquisition*/
    //RegisteParaChangeEvent();
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

}

void CameraMako130::RegisteParaChangeEvent(){
    if(VmbErrorSuccess==camera->GetFeatureByName("AcquisitionStatus",pFeature)){
        pFeature->RegisterObserver(IFeatureObserverPtr(new AcquisitionStatusChageEvent()));
    }
    else{
        std::cout<<"Get AcquisitionStatus Feature failed"<<std::endl;
    }
}

void CameraMako130::StartImagAqc(){
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
}


void CameraMako130::InitCameraParas(){
    std::cout<<"Vimba C++ API Version "<<sys<<"\n";           // Print out version of Vimba

    StarSysAndOpenCamera();
    //SetCameraExposureMode();
    SetExposureTime();
    SetTriggerMode();
    PrepareImagAqc();
    StartImagAqc();

}

void CameraMako130::InitCameraTriggerGPIO(){
    InitCameraTriggerGPIO();
}
void CameraMako130::InitCameraTriggrtTimer(){
    InitCameraTimer();
}

void CameraMako130::CleanUpCamera(){
    // Stop the acquisition engine ( camera )
    /*Stop image acquisition*/
    if(VmbErrorSuccess==camera -> GetFeatureByName ("AcquisitionStop", pFeature )){
        if(VmbErrorSuccess==pFeature -> RunCommand ()){
            std::cout<<"Successfully stop aquisition"<<std::endl;
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
//    DataTransferToCleanUp.camera=camera;
//    DataTransferToCleanUp.frames=frames;
//    DataTransferToCleanUp.sys=sys;
//    DataTransferToCleanUp.NeedCleanFlag=&bNeedCleanFlag;
    CleanUpFunc(&bNeedCleanFlag,camera,frames,sys);
}

void CameraMako130:: CameraFailed(){
    std::cout<<"Camera failed"<<std::endl;
    CleanUpCamera();
    CloseTimerGPIO();
}

//void CameraMako130::ResgistCameraErrInt(){
//    signal(SIGINT,CameraFailed);
//    signal(SIGQUIT,CameraFailed);
//    signal(SIGSEGV,CameraFailed);
//}
