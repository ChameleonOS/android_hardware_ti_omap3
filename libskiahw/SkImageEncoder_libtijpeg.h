
/*
 * Copyright (C) Texas Instruments - http://www.ti.com/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
/* ==============================================================================
*             Texas Instruments OMAP (TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================ */
/*
TODO:

Memcpy into bm upon receiving fillbufferdone

Replace Allocate Buffer with UseBuffer(using bm) for output buffer.

Better Error handling
*/
#include "SkBitmap.h"
#include "SkStream.h"
#include "SkImageEncoder.h"
#include <stdio.h>
#include <semaphore.h>

extern "C" {
    #include "OMX_Component.h"
    #include "OMX_IVCommon.h"
}

class SkTIJPEGImageEncoder :public SkImageEncoder
{
protected:
    virtual bool onEncode(SkWStream* stream, const SkBitmap& bm, int quality);

public:

    enum JPEGENC_State
    {
        STATE_LOADED,
        STATE_IDLE,
        STATE_EXECUTING,
        STATE_FILL_BUFFER_DONE_CALLED,
        STATE_EMPTY_BUFFER_DONE_CALLED,
        STATE_ERROR,
        STATE_EXIT
    };

    typedef struct JpegEncoderParams
    {
        //nWidth;
        //nHeight;
        //nQuality;
        //nComment;
        //nThumbnailWidth;
        //nThumbnailHeight;
        //APP0
        //APP1
        //APP13
        //CustomQuantizationTable
        //CustomHuffmanTable
        //nCropWidth
        //nCropHeight
    }JpegEncoderParams;

    sem_t *semaphore;
    JPEGENC_State iState;
    JPEGENC_State iLastState;

    SkTIJPEGImageEncoder();
    ~SkTIJPEGImageEncoder();
    bool encodeImage(void* outputBuffer, int outBuffSize, void *inputBuffer, int inBuffSize, int width, int height, int quality);    
    bool SetJpegEncodeParameters(JpegEncoderParams * jep) {memcpy(&jpegEncParams, jep, sizeof(JpegEncoderParams)); return true;}
    void Run();
    void PrintState();
    void FillBufferDone(OMX_U8* pBuffer, OMX_U32 size);
    void EventHandler(OMX_HANDLETYPE hComponent,
                                            OMX_EVENTTYPE eEvent,
                                            OMX_U32 nData1,
                                            OMX_U32 nData2,
                                            OMX_PTR pEventData);

private:

    OMX_HANDLETYPE pOMXHandle;
    OMX_BUFFERHEADERTYPE *pInBuffHead;
    OMX_BUFFERHEADERTYPE *pOutBuffHead;
    OMX_PARAM_PORTDEFINITIONTYPE InPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE OutPortDef;
    JpegEncoderParams jpegEncParams;
    OMX_U8* pEncodedOutputBuffer;
    OMX_U32 nEncodedOutputFilledLen;

};


SkImageEncoder* SkImageEncoder_HWJPEG_Factory() {
    return SkNEW(SkTIJPEGImageEncoder);
}

extern "C" SkTIJPEGImageEncoder* SkImageEncoder_TIJPEG_Factory() {
    return SkNEW(SkTIJPEGImageEncoder);
}

OMX_ERRORTYPE OMX_JPEGE_FillBufferDone (OMX_HANDLETYPE hComponent, OMX_PTR ptr, OMX_BUFFERHEADERTYPE* pBuffHead);
OMX_ERRORTYPE OMX_JPEGE_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptr, OMX_BUFFERHEADERTYPE* pBuffer);
OMX_ERRORTYPE OMX_JPEGE_EventHandler(OMX_HANDLETYPE hComponent,
                                            OMX_PTR pAppData,
                                            OMX_EVENTTYPE eEvent,
                                            OMX_U32 nData1,
                                            OMX_U32 nData2,
                                            OMX_PTR pEventData);

