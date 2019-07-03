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


//This function is used to clean up, to evite unexcepted error
void CleanUpFunc(struct NeedCleanFlag *bNeedCleanFlag,\
                    CameraPtr camera,FramePtrVector frames,\
                    VimbaSystem *sys){
    if(bNeedCleanFlag->bNeedEndCapture){
        camera -> EndCapture ();
    }
    if(bNeedCleanFlag->bNeedFlushQueue){
        camera -> FlushQueue ();
    }
    if(bNeedCleanFlag->bNeedRevokeAllFrames){
        camera -> RevokeAllFrames ();
    }

    if(bNeedCleanFlag->bNeedUnregisterObserver){
        for( FramePtrVector :: iterator iter= frames .begin (); frames .end ()!= iter; ++ iter){
            // Unregister the frame observer / callback
            (* iter)-> UnregisterObserver ();
        }
    }
    if(bNeedCleanFlag->bNeedCloseCamera){
        camera ->Close ();
    }
    if(bNeedCleanFlag->bNeedShutdown){
        sys->Shutdown ();
    }

}

void CleanUpFunc(CleanUpDtaTransfert *pDataTransferToCleanUp){
    CleanUpFunc(pDataTransferToCleanUp->NeedCleanFlag,\
                pDataTransferToCleanUp->camera,\
                pDataTransferToCleanUp->frames,\
                pDataTransferToCleanUp->sys);
}


void CreatAndSaveImag(const FramePtr pFrame ){
    static struct PhotoFormat PhotoFormatInfo={.nImageSize=0,\
                                                .nWidth=0,\
                                                .nHeight=0,\
                                                .bFormatGetted=false};

    std::string pFileNameBase = "SynchronousGrab.bmp";
    static int picnum=0;
    picnum++;
    std::string pFileNametemp=pFileNameBase+std::to_string(picnum);
    const char *pFileName=pFileNametemp.c_str();
    VmbErrorType    err         = VmbErrorSuccess;
    VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
    if(!PhotoFormatInfo.bFormatGetted){
        VmbUint32_t nImageSize = 0;
        err = pFrame->GetImageSize( nImageSize );
        PhotoFormatInfo.nImageSize=nImageSize;
        if ( VmbErrorSuccess == err )
        {
            VmbUint32_t nWidth = 0;
            err = pFrame->GetWidth( nWidth );
            PhotoFormatInfo.nWidth=nWidth;
            if ( VmbErrorSuccess == err )
            {
                VmbUint32_t nHeight = 0;
                err = pFrame->GetHeight( nHeight );
                PhotoFormatInfo.nHeight=nHeight;
                if ( VmbErrorSuccess == err )
                {   PhotoFormatInfo.bFormatGetted=true;
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

                        bitmap.bufferSize = PhotoFormatInfo.nImageSize;
                        bitmap.width = PhotoFormatInfo.nWidth;
                        bitmap.height = PhotoFormatInfo.nHeight;

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
    else{
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

            bitmap.bufferSize = PhotoFormatInfo.nImageSize;
            bitmap.width = PhotoFormatInfo.nWidth;
            bitmap.height = PhotoFormatInfo.nHeight;

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
