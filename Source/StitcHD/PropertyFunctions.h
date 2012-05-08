#include <dshow.h>   
#include <Ks.h>               // Required by KsMedia.h   
#include <KsMedia.h>      // For KSPROPERTY_CAMERACONTROL_FLAGS_* 

/*  
 * Pans the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are clockwise rotation (seen from the top),  
 * negative values are counter-clockwise rotation. If the "Mirror horizontal" option is  
 * enabled, the panning sense is reversed.  
 */   
HRESULT set_mechanical_pan_relative(IAMCameraControl *pCameraControl, long value);
   
   
/*  
 * Tilts the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are downwards, negative values are upwards.  
 * If the "Mirror vertical" option is enabled, the tilting sense is reversed.  
 */   
HRESULT set_mechanical_tilt_relative(IAMCameraControl *pCameraControl, long value); 
   
/*  
 * Pans the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are clockwise rotation (seen from the top),  
 * negative values are counter-clockwise rotation. If the "Mirror horizontal" option is  
 * enabled, the panning sense is reversed.  
 */   
HRESULT set_mechanical_pan_absolute(IAMCameraControl *pCameraControl, long value);  
   
   
/*  
 * Tilts the camera by a given angle.  
 *  
 * The angle is given in degrees, positive values are downwards, negative values are upwards.  
 * If the "Mirror vertical" option is enabled, the tilting sense is reversed.  
 */   
HRESULT set_mechanical_tilt_absolute(IAMCameraControl *pCameraControl, long value);  
   
   
   
/*  
 * Resets the camera's pan/tilt position by moving into a corner and then back to the center.  
 */   
void reset_mechanical_pan_tilt(IAMCameraControl *pCameraControl);
   
   
/*  
 * Sets the digital pan angle.  
 *  
 * Positive values pan to the right, negative values pan to the left. Note that the digital pan  
 * angle only has an influence if the digital zoom is active.  
 */   
HRESULT set_digital_pan_absolute(IAMCameraControl *pCameraControl, long value);  
   
   
/*  
 * Sets the digital tilt angle.  
 *  
 * Positive values tilt downwards, negative values tilt upwards. Note that the digital pan  
 * angle only has an influence if the digital zoom is active.  
 */   
HRESULT set_digital_tilt_absolute(IAMCameraControl *pCameraControl, long value);  
   
   
/*  
 * Sets the digital zoom value.  
 *  
 * The minimum value is 50 and means no zoom (100%). The maximum value is 200  
 * and means 4x zoom (400%).  
 */   
HRESULT set_digital_zoom_absolute(IAMCameraControl *pCameraControl, long value);  

HRESULT set_iris_absolute(IAMCameraControl *pCameraControl, long value);  
   
/*  
 * Sets the focus value.  
 *  
 */   
HRESULT set_focus_absolute(IAMCameraControl *pCameraControl, long value); 
   
HRESULT set_autofocus_absolute(IAMCameraControl *pCameraControl, long value);  
/*  
 * Resets the digital pan and tilt angles.  
 */   
void reset_digital_pan_tilt(IAMCameraControl *pCameraControl); 
   
   
/*  
 * Resets the digital zoom.  
 */   
void reset_digital_zoom(IAMCameraControl *pCameraControl) ;  
