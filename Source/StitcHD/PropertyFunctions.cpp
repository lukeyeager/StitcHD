#include <dshow.h>   
#include <Ks.h>               // Required by KsMedia.h   
#include <KsMedia.h>      // For KSPROPERTY_CAMERACONTROL_FLAGS_* 
#include <time.h>
#include <stdio.h>
#include <iostream>
using namespace std;

/* Code Timing Resource:
http://www.physicsforums.com/showthread.php?t=224989
*/

double diffclock(clock_t clock1, clock_t clock2){
	double diffticks=clock1-clock2;
	double diffms=(diffticks*10)/CLOCKS_PER_SEC;
	return diffms;
}
/*  
 * Pans the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are clockwise rotation (seen from the top),  
 * negative values are counter-clockwise rotation. If the "Mirror horizontal" option is  
 * enabled, the panning sense is reversed.  
 */   
HRESULT set_mechanical_pan_relative(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
	clock_t begin = clock();
    hr = pCameraControl->Set(CameraControl_Pan, value, flags);   
	clock_t end = clock();
	cout << "Time elapsed to pan " << value << " = " << double(diffclock(end,begin)) << " ms" << endl;
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Pan property value to %d. (Error 0x%08X)\n", value, hr);   
   
    // Note that we need to wait until the movement is complete, otherwise the next request will   
    // fail with hr == 0x800700AA == HRESULT_FROM_WIN32(ERROR_BUSY).   
    Sleep(500);   
   
    return hr;   
}   
   /*  
 * Pans the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are clockwise rotation (seen from the top),  
 * negative values are counter-clockwise rotation. If the "Mirror horizontal" option is  
 * enabled, the panning sense is reversed.  
 */   
HRESULT set_mechanical_pan_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Pan, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Pan property value to %d. (Error 0x%08X)\n", value, hr);   
   
    // Note that we need to wait until the movement is complete, otherwise the next request will   
    // fail with hr == 0x800700AA == HRESULT_FROM_WIN32(ERROR_BUSY).   
    Sleep(500);   
   
    return hr;   
}  
   /*  
 * Sets the digital pan angle.  
 *  
 * Positive values pan to the right, negative values pan to the left. Note that the digital pan  
 * angle only has an influence if the digital zoom is active.  
 */   
HRESULT set_digital_pan_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
   
    // Specifying the KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE flag instructs the driver   
    // to use digital instead of mechanical pan.   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Pan, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Pan property value to %d. (Error 0x%08X)\n", value, hr);   
   
    return hr;   
}   
/*  
 * Tilts the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are downwards, negative values are upwards.  
 * If the "Mirror vertical" option is enabled, the tilting sense is reversed.  
 */   
HRESULT set_mechanical_tilt_relative(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Tilt, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Tilt property value to %d. (Error 0x%08X)\n", value, hr);   
   
    // Note that we need to wait until the movement is complete, otherwise the next request will   
    // fail with hr == 0x800700AA == HRESULT_FROM_WIN32(ERROR_BUSY).   
    Sleep(500);   
   
    return hr;   
}   
   
 /*  
 * Tilts the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are downwards, negative values are upwards.  
 * If the "Mirror vertical" option is enabled, the tilting sense is reversed.  
 */   
HRESULT set_mechanical_tilt_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Tilt, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Tilt property value to %d. (Error 0x%08X)\n", value, hr);   
   
    // Note that we need to wait until the movement is complete, otherwise the next request will   
    // fail with hr == 0x800700AA == HRESULT_FROM_WIN32(ERROR_BUSY).   
    Sleep(500);   
   
    return hr;   
} 
/*  
 * Sets the digital tilt angle.  
 *  
 * Positive values tilt downwards, negative values tilt upwards. Note that the digital pan  
 * angle only has an influence if the digital zoom is active.  
 */   
HRESULT set_digital_tilt_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
   
    // Specifying the KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE flag instructs the driver   
    // to use digital instead of mechanical tilt.   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Tilt, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Tilt property value to %d. (Error 0x%08X)\n", value, hr);   
   
    return hr;   
}   
   /*  
 * Sets the digital zoom value.  
 *  
 * The minimum value is 50 and means no zoom (100%). The maximum value is 200  
 * and means 4x zoom (400%).  
 */   
HRESULT set_digital_zoom_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Zoom, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Zoom property value to %d. (Error 0x%08X)\n", value, hr);   
   
    return hr;   
}   

/* Reset Functions */

/*  
 * Resets the camera's pan/tilt position by moving into a corner and then back to the center.  
 */   
void reset_mechanical_pan_tilt(IAMCameraControl *pCameraControl)   
{   
    set_mechanical_pan_relative(pCameraControl, 180);   
    Sleep(1000);		//Sleep(2000);   
    set_mechanical_tilt_relative(pCameraControl, 180);   
    Sleep(1000);		//Sleep(2000);   
    set_mechanical_pan_relative(pCameraControl, -64);
    Sleep(1000);		//Sleep(2000);   
    set_mechanical_tilt_relative(pCameraControl, -24);
    Sleep(1000);		//Sleep(2000);   
   
}   
/*  
 * Resets the digital pan and tilt angles.  
 */   
void reset_digital_pan_tilt(IAMCameraControl *pCameraControl)   
{   
    set_digital_pan_absolute(pCameraControl, 0);   
    set_digital_tilt_absolute(pCameraControl, 0);   
}   
   
   
/*  
 * Resets the digital zoom.  
 */   
void reset_digital_zoom(IAMCameraControl *pCameraControl)   
{   
    set_digital_zoom_absolute(pCameraControl, 50);   
}  


/* Exposure Functions */
HRESULT set_iris_absolute(IAMCameraControl *pCameraControl, long value)   
{   

    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Iris, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Iris property value to %d. (Error 0x%08X)\n", value, hr);   
   
    return hr;   
}   
   
/*  
 * Sets the focus value.  
 *  
 */   
HRESULT set_focus_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;   
   
    hr = pCameraControl->Set(CameraControl_Focus, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Focus property value to %d. (Error 0x%08X)\n", value, hr);   
   
    return hr;   
}   
   
HRESULT set_autofocus_absolute(IAMCameraControl *pCameraControl, long value)   
{   
    HRESULT hr = 0;   
    long flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;   
   
    hr = pCameraControl->Set(CameraControl_Focus, value, flags);   
    if(hr != S_OK)   
        fprintf(stderr, "ERROR: Unable to set CameraControl_Zoom property value to %d. (Error 0x%08X)\n", value, hr);   
   
    return hr;   
}   
 
