#include <dshow.h>   
#include <Ks.h>               // Required by KsMedia.h   
#include <KsMedia.h>      // For KSPROPERTY_CAMERACONTROL_FLAGS_*   
#include <vector>
#include <iostream>
#include "PropertyFunctions.h"
#include "CameraArrayFunctions.h"
#include <math.h>
#include <fstream>

using namespace std;

struct ControlInfo {   
    long min;   
    long max;   
    long step;   
    long def;   
    long flags;   
}; 

/*  
 * Print information about a control in an easily readable fashion.  
 */   
void print_control_info(ControlInfo *info)   
{   
    char flags[32] = "";   
   
    if(info->flags & KSPROPERTY_CAMERACONTROL_FLAGS_AUTO)   
    {   
        strcat_s(flags, sizeof(flags), "AUTO | ");   
    }   
    else if(info->flags & KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL)   
    {   
        strcat_s(flags, sizeof(flags), "MANUAL | ");   
    }   
   
    if(info->flags & KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE)   
    {   
        strcat_s(flags, sizeof(flags), "RELATIVE");   
    }   
    else   
    {   
        strcat_s(flags, sizeof(flags), "ABSOLUTE");   
    }   
   
    printf(   
        "        min:   %d\n"   
        "        max:   %d\n"   
        "        step:  %d\n"   
        "        def:   %d\n"   
        "        flags: 0x%08X (%s)\n",   
        info->min, info->max, info->step, info->def, info->flags, flags   
    );   
}  

HRESULT test_pan_tilt(IBaseFilter *pBaseFilter)   
{   
    HRESULT hr = 0;   
    IAMCameraControl *pCameraControl = NULL;   
    ControlInfo panInfo = { 0 };   
    ControlInfo tiltInfo = { 0 };   
    ControlInfo zoomInfo = { 0 };   
    ControlInfo focusInfo = { 0 };   
    long value = 0, flags = 0;   
   
    printf("    Reading pan/tilt property information ...\n");   
   
    // Get a pointer to the IAMCameraControl interface used to control the camera   
    hr = pBaseFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to access IAMCameraControl interface.\n");   
        return hr;   
    }   
   
    // Retrieve information about the pan and tilt controls   
    hr = pCameraControl->GetRange(CameraControl_Pan, &panInfo.min, &panInfo.max, &panInfo.step, &panInfo.def, &panInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Pan property information.\n");   
        return hr;   
    }   
    printf("      Pan control:\n");   
    print_control_info(&panInfo);   
   
    hr = pCameraControl->GetRange(CameraControl_Tilt, &tiltInfo.min, &tiltInfo.max, &tiltInfo.step, &tiltInfo.def, &tiltInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Tilt property information.\n");   
        return hr;   
    }   
    printf("      Tilt control:\n");   
    print_control_info(&tiltInfo);   
   
    hr = pCameraControl->GetRange(CameraControl_Zoom, &zoomInfo.min, &zoomInfo.max, &zoomInfo.step, &zoomInfo.def, &zoomInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Zoom property information.\n");   
        return hr;   
    }   
    printf("      Zoom control:\n");   
    print_control_info(&zoomInfo);   
   
    hr = pCameraControl->GetRange(CameraControl_Focus, &focusInfo.min, &focusInfo.max, &focusInfo.step, &focusInfo.def, &focusInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Focus property information.\n");   
        return hr;   
    }   
    printf("      Focus control:\n");   
    print_control_info(&focusInfo);   
    int j,k,t=-1;   
    set_mechanical_pan_relative(pCameraControl, -180);   
    Sleep(2000);   
    set_mechanical_tilt_relative(pCameraControl, -180);   
    Sleep(2000);   
   
    set_mechanical_pan_relative(pCameraControl, 180);   
    Sleep(2000);   
    set_mechanical_tilt_relative(pCameraControl, 180);   
    Sleep(2000);   
   
   
   
    for(j=tiltInfo.min;j<0;j+=tiltInfo.step)   
    {   
        for(k=panInfo.min;k<0;k+=panInfo.step)   
        {   
            set_mechanical_pan_relative(pCameraControl, t);   
            Sleep(250);   
            printf("Set Pan: %d\n",k);   
        }   
        set_mechanical_pan_relative(pCameraControl, 0);   
        for(k=0;k<panInfo.max;k+=panInfo.step)   
        {   
            set_mechanical_pan_relative(pCameraControl, t);   
            Sleep(250);   
            printf("Set Pan: %d\n",k);   
        }   
        set_mechanical_tilt_relative(pCameraControl,1);   
        Sleep(500);   
        set_mechanical_pan_relative(pCameraControl,180);   
        t=-t;   
        Sleep(500);   
        printf("Set Tilt: %d\n",j);   
    }   
       
        //printf("    Resetting pan/tilt/zoom ...\n");   
        //reset_mechanical_pan_tilt(pCameraControl);   
        //reset_digital_pan_tilt(pCameraControl);   
        //reset_digital_zoom(pCameraControl);   
        //Sleep(3000);   
        ///*/   
   
        ////*/   
        //printf("    Testing mechanical pan ...\n");   
        //set_mechanical_pan_relative(pCameraControl, 40);   
        //set_mechanical_pan_relative(pCameraControl, 20);   
        //set_mechanical_pan_relative(pCameraControl, -20);   
        //set_mechanical_pan_relative(pCameraControl, -40);   
        //Sleep(3000);   
        ////*/   
   
        ////*/   
        //printf("    Testing mechanical tilt ...\n");   
        //set_mechanical_tilt_relative(pCameraControl, 20);   
        //set_mechanical_tilt_relative(pCameraControl, 10);   
        //set_mechanical_tilt_relative(pCameraControl, -10);   
        //set_mechanical_tilt_relative(pCameraControl, -20);   
        //Sleep(3000);   
        ////*/   
   
        ////*/   
        //printf("    Testing digital pan/tilt/zoom ...\n");   
        //set_digital_zoom_absolute(pCameraControl, 100);       // Zoom to 200%   
        //Sleep(1000);   
   
        //set_digital_pan_absolute(pCameraControl, 40);   
        //Sleep(1000);   
        //set_digital_pan_absolute(pCameraControl, 80);   
        //Sleep(1000);   
   
        //set_digital_zoom_absolute(pCameraControl, 200);       // Zoom to 400%   
        //Sleep(1000);   
   
        //set_digital_tilt_absolute(pCameraControl, 40);   
        //Sleep(1000);   
        //set_digital_tilt_absolute(pCameraControl, 60);   
        //Sleep(1000);   
        //   
        //reset_digital_pan_tilt(pCameraControl);   
        //Sleep(1000);   
        //reset_digital_zoom(pCameraControl);   
        //Sleep(3000);   
        ////*/   
   
        ////*/   
        //printf("    Testing digital zoom ...\n");   
        //for(int i = zoomInfo.min; i = zoomInfo.max; i += zoomInfo.step)   
        //{   
        //  set_digital_zoom_absolute(pCameraControl, i);   
        //  Sleep(10);   
        //}   
        //Sleep(1000);   
        //for(int i = zoomInfo.max; i >= zoomInfo.min; i -= zoomInfo.step)   
        //{   
        //  set_digital_zoom_absolute(pCameraControl, i);   
        //  Sleep(10);   
        //}   
   
        //Sleep(3000);   
        ////*/   
   
   
    /*printf("    Testing focus ...\n");   
    Sleep(1000);   
    for(int i = focusInfo.max; i >= focusInfo.min; i -=focusInfo.step)   
    {   
        set_focus_absolute(pCameraControl, i);   
        printf("setting focus to: %d \n",i);   
        Sleep(100);   
    }   
    for(int i = focusInfo.min; i  focusInfo.max; i +=focusInfo.step)   
    {   
        set_focus_absolute(pCameraControl, i);   
        printf("setting focus to: %d \n",i);   
        Sleep(100);   
    }   
    set_autofocus_absolute(pCameraControl, 0);   
    system("pause");   
    //*/   
   
    return S_OK;   
} 
void SleepTime_Pan_Test(int cam_num, int amount, vector<IAMCameraControl*>& CameraContr, int startIncrement, int endIncrement){
	HRESULT hr;
	ofstream ofs("SleepPanTest.txt");
	for(int increment = startIncrement; increment < endIncrement; increment = increment + 10){
		for(int i = 1; i < 5; i++){
			hr = set_mechanical_pan_relative(CameraContr[cam_num],amount*pow(-1.0,i));
			ofs << "Sleep " << increment << " ms\t"<< "Error: " << hr << endl;
			cout << "Sleep " << increment << " ms\t"<< "Error: " << hr << endl;
			Sleep(increment);
		}
	}
	ofs.close();
}
HRESULT PanWithCamera(int cam_num, int func, int amount, vector<IAMCameraControl*>& CameraContr)   
{   
    HRESULT hr = 0;   
    IAMCameraControl *pCameraControl = NULL;   
    ControlInfo panInfo = { 0 };   
    ControlInfo tiltInfo = { 0 };   
    ControlInfo zoomInfo = { 0 };   
    ControlInfo focusInfo = { 0 };   
    long value = 0, flags = 0;   
   
    printf("    Reading pan/tilt property information ...\n");    
   
    // Retrieve information about the pan and tilt controls   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Pan, &panInfo.min, &panInfo.max, &panInfo.step, &panInfo.def, &panInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Pan property information.\n");   
        return hr;   
    }   
    printf("      Pan control:\n");   
    print_control_info(&panInfo);   
   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Tilt, &tiltInfo.min, &tiltInfo.max, &tiltInfo.step, &tiltInfo.def, &tiltInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Tilt property information.\n");   
        return hr;   
    }   
    printf("      Tilt control:\n");   
    print_control_info(&tiltInfo);   
   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Zoom, &zoomInfo.min, &zoomInfo.max, &zoomInfo.step, &zoomInfo.def, &zoomInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Zoom property information.\n");   
        return hr;   
    }   
    printf("      Zoom control:\n");   
    print_control_info(&zoomInfo);   
   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Focus, &focusInfo.min, &focusInfo.max, &focusInfo.step, &focusInfo.def, &focusInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Focus property information.\n");   
        return hr;   
    }   
    printf("      Focus control:\n");   
    print_control_info(&focusInfo);   
     
    printf("\n\nInterpreting command\n");
	printf("Camera Number: %s\n",cam_num);
	printf("Function: %s\n",func);
	printf("Amount: %s\n",amount);
   
	//select correct function
	//pan

	/*
	If the sleep value is not great enough after any command for pan or tilt, 
	then an error message will be output to the screen
	*/
	reset_mechanical_pan_tilt(pCameraControl);
	printf("Camera Reset\n");

	bool quitFlag = false;
	int panDegree = 0;

	while (!quitFlag){
		cout << "Enter a pan degree:";
		cin >> panDegree;
		printf("Pan\n");
		set_mechanical_pan_relative(CameraContr[cam_num], panDegree);
	}

	/*if(strcmp("0",func))
	{
		printf("PAN!!!");
		set_mechanical_pan_relative(pCameraControl, atoi(amount));
	}

	//tilt
	else if(strcmp("1",func))
	{
		printf("TILT");
		set_mechanical_tilt_relative(pCameraControl, atoi(amount));
	}

	//zoom
	else if(strcmp("2",func))
	{
		printf("ZOOM!!!");
		set_digital_zoom_absolute(pCameraControl, atoi(amount));
	}*/
    
    return S_OK;   
}   
HRESULT TiltWithCamera(int cam_num, int func, int amount, vector<IAMCameraControl*>& CameraContr)   
{   
    HRESULT hr = 0;   
    IAMCameraControl *pCameraControl = NULL;   
    ControlInfo panInfo = { 0 };   
    ControlInfo tiltInfo = { 0 };   
    ControlInfo zoomInfo = { 0 };   
    ControlInfo focusInfo = { 0 };   
    long value = 0, flags = 0;   
   
    printf("    Reading pan/tilt property information ...\n");     
   
    // Retrieve information about the pan and tilt controls   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Pan, &panInfo.min, &panInfo.max, &panInfo.step, &panInfo.def, &panInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Pan property information.\n");   
        return hr;   
    }   
    printf("      Pan control:\n");   
    print_control_info(&panInfo);   
   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Tilt, &tiltInfo.min, &tiltInfo.max, &tiltInfo.step, &tiltInfo.def, &tiltInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Tilt property information.\n");   
        return hr;   
    }   
    printf("      Tilt control:\n");   
    print_control_info(&tiltInfo);   
   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Zoom, &zoomInfo.min, &zoomInfo.max, &zoomInfo.step, &zoomInfo.def, &zoomInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Zoom property information.\n");   
        return hr;   
    }   
    printf("      Zoom control:\n");   
    print_control_info(&zoomInfo);   
   
    hr = CameraContr[cam_num]->GetRange(CameraControl_Focus, &focusInfo.min, &focusInfo.max, &focusInfo.step, &focusInfo.def, &focusInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Focus property information.\n");   
        return hr;   
    }   
    printf("      Focus control:\n");   
    print_control_info(&focusInfo);   

	/*
	If the sleep value is not great enough after any command for pan or tilt, 
	then an error message will be output to the screen
	*/
	reset_mechanical_pan_tilt(pCameraControl);
	printf("Camera Reset\n");

	bool quitFlag = false;
	int panDegree = 0;

	while (!quitFlag){
		cout << "Enter a tilt degree:";
		cin >> panDegree;
		printf("tilt\n");
		set_mechanical_tilt_relative(CameraContr[cam_num], panDegree);
	}

	/*if(strcmp("0",func))
	{
		printf("PAN!!!");
		set_mechanical_pan_relative(pCameraControl, atoi(amount));
	}

	//tilt
	else if(strcmp("1",func))
	{
		printf("TILT");
		set_mechanical_tilt_relative(pCameraControl, atoi(amount));
	}

	//zoom
	else if(strcmp("2",func))
	{
		printf("ZOOM!!!");
		set_digital_zoom_absolute(pCameraControl, atoi(amount));
	}*/
    
    return S_OK;   
}
HRESULT GetPropertyInfo(IAMCameraControl *pCameraControl){

	HRESULT hr = 0;     
    ControlInfo panInfo = { 0 };   
    ControlInfo tiltInfo = { 0 };   
    ControlInfo zoomInfo = { 0 };   
    ControlInfo focusInfo = { 0 }; 
	ControlInfo irisInfo = { 0 };
    long value = 0, flags = 0;   

	hr = pCameraControl->GetRange(CameraControl_Pan, &panInfo.min, &panInfo.max, &panInfo.step, &panInfo.def, &panInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Pan property information.\n");   
        return hr;   
    }   
    printf("      Pan control:\n");   
    print_control_info(&panInfo);   
   
    hr = pCameraControl->GetRange(CameraControl_Tilt, &tiltInfo.min, &tiltInfo.max, &tiltInfo.step, &tiltInfo.def, &tiltInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Tilt property information.\n");   
        return hr;   
    }   
    printf("      Tilt control:\n");   
    print_control_info(&tiltInfo);   
   
    hr = pCameraControl->GetRange(CameraControl_Zoom, &zoomInfo.min, &zoomInfo.max, &zoomInfo.step, &zoomInfo.def, &zoomInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Zoom property information.\n");   
        return hr;   
    }   
    printf("      Zoom control:\n");   
    print_control_info(&zoomInfo);   
   
    hr = pCameraControl->GetRange(CameraControl_Focus, &focusInfo.min, &focusInfo.max, &focusInfo.step, &focusInfo.def, &focusInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Focus property information.\n");   
        return hr;   
    }   
    printf("      Focus control:\n");   
    print_control_info(&focusInfo); 

	hr = pCameraControl->GetRange(CameraControl_Iris, &irisInfo.min, &irisInfo.max, &irisInfo.step, &irisInfo.def, &irisInfo.flags);   
    if(hr != S_OK)   
    {   
        fprintf(stderr, "ERROR: Unable to retrieve CameraControl_Iris property information.\n");   
        return hr;   
    }   
    printf("      iris control:\n");   
    print_control_info(&irisInfo);   
   

	return hr;
}
HRESULT issueCommand(int cam_num, int func, int amount, vector<IAMCameraControl*>& CameraContr){
    HRESULT hr = 0;
	int CameraContr_size = CameraContr.size();
	switch (func){
		case PAN_RELATIVE:
			set_mechanical_pan_relative(CameraContr[cam_num], amount);
			break;
		case PAN_ABSOLUTE:
			set_mechanical_pan_absolute(CameraContr[cam_num], amount);
			break;
		case TILT_RELATIVE:
			set_mechanical_tilt_relative(CameraContr[cam_num], amount);
			break;
		case ZOOM_ABSOLUTE:
			set_digital_zoom_absolute(CameraContr[cam_num], amount);
			break;
		case FOCUS_CHANGE:
			set_focus_absolute(CameraContr[cam_num],amount);
			break;
		case IRIS_CHANGE:
			set_iris_absolute(CameraContr[cam_num],amount);
			break;
		case PAN_ALL_RELATIVE:
			for (int i = 0; i < CameraContr_size; i++)
				set_mechanical_pan_relative(CameraContr[i], amount);
			break;
		case TILT_ALL_RELATIVE:
			for (int i = 0; i < CameraContr_size; i++)
				set_mechanical_tilt_relative(CameraContr[i], amount);
			break;
		case ZOOM_ALL_ABSOLUTE:
				// Range: 50(min) - 200(max)
			for (int i = 0; i < CameraContr_size; i++)
				set_digital_zoom_absolute(CameraContr[i], amount);
			break;
		case CAMERA_PROPERTY_INFO:
			GetPropertyInfo(CameraContr[cam_num]);
			break;
		case RESET_CAMERA_POSITIONS:
			for (int i = 0; i < CameraContr_size; i++)
				reset_mechanical_pan_tilt(CameraContr[i]);
			break;
		}   
		return S_OK;   
}
/*  
 * Enumerate all video devices  
 *  
 * See also:  
 *  
 * Using the System Device Enumerator:  
 *     http://msdn2.microsoft.com/en-us/library/ms787871.aspx  
 */   
int enum_devices(vector<IBaseFilter*> &Devices)   
{   
    HRESULT hr;   
   
    printf("Enumerating video input devices ...\n");   
   
    // Create the System Device Enumerator.   
    ICreateDevEnum *pSysDevEnum = NULL;   
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, // CLSID_systemDeviceEnum is the class identifier of for a Syst. Dev. Enum.   
        IID_ICreateDevEnum, (void **)&pSysDevEnum);   
    if(FAILED(hr))   
    {   
        fprintf(stderr, "ERROR: Unable to create system device enumerator.\n");   
        return hr;   
    }   
   
    // Obtain a class enumerator for the video input device category.   
    IEnumMoniker *pEnumCat = NULL;   
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);   
   
    if(hr == S_OK)    
    {   
        // Enumerate the monikers.   
        IMoniker *pMoniker = NULL;   
        ULONG cFetched;   
        while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)   // Next() method retrieves the specified number of items in the enumeration sequence
        {   
			cout << "Moniker: " << pMoniker;
            IPropertyBag *pPropBag;   
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,    
                (void **)&pPropBag);   
            if(SUCCEEDED(hr))   
            {   
                // To retrieve the filter's friendly name, do the following:   
                VARIANT varName;   
                VariantInit(&varName);   
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);   
                if (SUCCEEDED(hr))   
                {   
                    // Display the name in your UI somehow.   
                    wprintf(L"  Found device: %s\n", varName.bstrVal);   
                }   
                VariantClear(&varName);   
   
                // To create an instance of the filter, do the following:   
                IBaseFilter *pFilter;   
                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,   
                    (void**)&pFilter);   
                   
				Devices.push_back(pFilter);
   
                //Remember to release pFilter later.   
                pPropBag->Release();   
            }   
            pMoniker->Release();   
        }   
        pEnumCat->Release();   
    }   
    pSysDevEnum->Release();   
   
    return 0;   
}   
   

int initialize_pan_tilt()
{
	int result;  
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);   
	vector<IBaseFilter*> Devices;
    result = enum_devices(Devices);
	HRESULT hr;

	int Devices_size = Devices.size();
	vector<IAMCameraControl*> CameraControllers(Devices_size,0);
	for (int i = 0; i < Devices_size; i++)
	{
		hr = Devices[i]->QueryInterface(IID_IAMCameraControl, (void **)&CameraControllers[i]);   
		if(hr != S_OK)   
		{   
			fprintf(stderr, "ERROR: Unable to access IAMCameraControl interface.\n");   
			return hr;   
		}
		//reset_mechanical_pan_tilt(CameraControllers[i]);
	}
	

	//camera 0 (lower left w/respect to back of mount)
	//pan 30 degrees counterclockwise (+ = CW, - = CCW)
	set_mechanical_pan_relative(CameraControllers[0], -10);
	//pan 0 degrees downward (+ = down, - = up)
	set_mechanical_tilt_relative(CameraControllers[0], 20);
	//camera 1 (lower right)
	set_mechanical_pan_relative(CameraControllers[1], 10);
	set_mechanical_tilt_relative(CameraControllers[1], 20);
	//camera 2 (upper left, directions flipped upside down)
	set_mechanical_pan_relative(CameraControllers[2], 10);
	//set_mechanical_tilt_relative(CameraControllers[2], 20);
	//camera 3 (upper right, directions flipped upside down)
	set_mechanical_pan_relative(CameraControllers[3], -10);
	//set_mechanical_tilt_relative(CameraControllers[3], 20);

	for(int i = 0; i < Devices_size; i++)
	{
		Devices[i]->Release();
	}

    CoUninitialize();      

    return result;
}

int reset_pan_tilt()
{
	int result;  
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);   
	vector<IBaseFilter*> Devices;
    result = enum_devices(Devices);
	HRESULT hr;

	int Devices_size = Devices.size();
	vector<IAMCameraControl*> CameraControllers(Devices_size,0);
	for (int i = 0; i < Devices_size; i++)
	{
		hr = Devices[i]->QueryInterface(IID_IAMCameraControl, (void **)&CameraControllers[i]);   
		if(hr != S_OK)   
		{   
			fprintf(stderr, "ERROR: Unable to access IAMCameraControl interface.\n");   
			return hr;   
		}
		reset_mechanical_pan_tilt(CameraControllers[i]);
	}

	for(int i = 0; i < Devices_size; i++)
	{
		Devices[i]->Release();
	}

    CoUninitialize();      

    return result;
}

