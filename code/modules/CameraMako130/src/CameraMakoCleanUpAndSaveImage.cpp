#include "CameraMakoCleanUpAndSaveImage.h"

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

AcquisitionStatusChageEvent::AcquisitionStatusChageEvent(){
    ;
}

void AcquisitionStatusChageEvent::FeatureChanged(const FeaturePtr &pFeature){
    if(pFeature!=NULL){
        CheckAcquisitionStatus();
    }
    else
    {
        std::cout<<"Invalide feature"<<std::endl;
    }
}

//This function is used to clean up, to evite unexcepted error
void CleanUpFunc(struct NeedCleanFlag *bNeedCleanFlag,\
                    CameraPtr camera,FramePtrVector frames,\
                    VimbaSystem *sys){
    if(bNeedCleanFlag->bNeedEndCapture){
        camera -> EndCapture ();
        std::cout<<"EndCapture"<<std::endl;
    }
    if(bNeedCleanFlag->bNeedFlushQueue){
        camera -> FlushQueue ();
        std::cout<<"FlushQueue"<<std::endl;
    }
    if(bNeedCleanFlag->bNeedRevokeAllFrames){
        camera -> RevokeAllFrames ();
        std::cout<<"RevokeAllFrames"<<std::endl;

    }

    if(bNeedCleanFlag->bNeedUnregisterObserver){
        for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
            // Unregister the frame observer / callback
            (* iter)-> UnregisterObserver ();
            std::cout<<"UnregisterObserver"<<std::endl;
        }
    }
    if(bNeedCleanFlag->bNeedCloseCamera){
        camera ->Close ();
        std::cout<<"Close"<<std::endl;
    }
    if(bNeedCleanFlag->bNeedShutdown){
        sys->Shutdown ();
        std::cout<<"Shutdown"<<std::endl;
    }

}

void CleanUpFunc(CleanUpDtaTransfert *pDataTransferToCleanUp){
    CleanUpFunc(pDataTransferToCleanUp->NeedCleanFlag,\
                pDataTransferToCleanUp->camera,\
                pDataTransferToCleanUp->frames,\
                pDataTransferToCleanUp->sys);
}

void SetAcquisitionStatusSelector(const CameraPtr camera, \
        const char * AcquisitionStatusSelectorVal){
    FeaturePtr pFeature;
    if(VmbErrorSuccess==camera->GetFeatureByName ("AcquisitionStatusSelector", pFeature )){
        if(VmbErrorSuccess==pFeature -> SetValue (AcquisitionStatusSelectorVal)){
            std::cout<<"Successfully Set feature exposure mode Timed"<<std::endl;
        }
        else{
            std::cout<<"Can't Set feature AcquisitionStatusSelector" \
                    << AcquisitionStatusSelectorVal<<std::endl;
        }
    }
    else{
        std::cout<<"Can't get feature AcquisitionStatusSelector"<<std::endl;
    }
}
