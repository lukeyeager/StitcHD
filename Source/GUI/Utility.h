#include <dshow.h>   
#include <Ks.h>               // Required by KsMedia.h   
#include <KsMedia.h>      // For KSPROPERTY_CAMERACONTROL_FLAGS_*   
#include <vector>
#include "PropertyFunctions.h"

using namespace std;
HRESULT test_pan_tilt(IBaseFilter *pBaseFilter);
void SleepTime_Pan_Test(int cam_num, int amount, vector<IAMCameraControl*>& CameraContr, int startIncrement, int endIncrement);
HRESULT issue_command(IBaseFilter *pBaseFilter);
HRESULT PanWithCamera(IBaseFilter *pBaseFilter);  
HRESULT TiltWithCamera(IBaseFilter *pBaseFilter);
HRESULT GetPropertyInfo(IAMCameraControl *pCameraControl);
HRESULT issueCommand(int cam_num, int func, int amount, vector<IAMCameraControl*>& CameraContr);
void process_filter(IBaseFilter *pBaseFilter);     
int enum_devices(vector<IBaseFilter*> &Devices);

int initialize_pan_tilt();
int reset_pan_tilt();
