/*****************************************************************************  
 * DirectShow Pan/Tilt/Zoom sample for Logitech QuickCam devices  
 *   
 * Copyright 2007 (c) Logitech. All Rights Reserved.  
 *  
 * This code and information is provided "as is" without warranty of  
 * any kind, either expressed or implied, including but not limited to  
 * the implied warranties of merchantability and/or fitness for a  
 * particular purpose.  
 *  
 * Version: 1.1  
 ****************************************************************************/   
   
#include <dshow.h>   
#include <Ks.h>               // Required by KsMedia.h   
#include <KsMedia.h>      // For KSPROPERTY_CAMERACONTROL_FLAGS_*   
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include "DShowUtility.h"
#include "PropertyFunctions.h"
#include "CameraArrayFunctions.h"
   
using namespace std;     
   
//int wmain(int argc, wchar_t* argv[])   
//{   
//	int CameraID = 0;
//	int CameraFunction = 0; 
//	int CameraAdjustment = 0;
//
//    int result;  
//    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);   
//	vector<IBaseFilter*> Devices;
//    result = enum_devices(Devices);
//	HRESULT hr;
//
//	int Devices_size = Devices.size();
//	vector<IAMCameraControl*> CameraControllers(Devices_size,0);
//	for (int i = 0; i < Devices_size; i++){
//		hr = Devices[i]->QueryInterface(IID_IAMCameraControl, (void **)&CameraControllers[i]);   
//		if(hr != S_OK)   
//		{   
//			fprintf(stderr, "ERROR: Unable to access IAMCameraControl interface.\n");   
//			return hr;   
//		}   
//	}
//	
//	bool ACCEPT_FLAG = true;
//	while(ACCEPT_FLAG){
//		//cam_num = camera issuing command
//		//function = 0: pan		1: tilt		2:zoom
//		//amount = amount of command
//		string state = "";
//		cout << "Waiting for command...\n";
//		
//		// Command : $ cam_num function amount
//		cin >> state >> CameraID >> CameraFunction >> CameraAdjustment;
//		cout << "State: " << state << endl
//			<< "CaneraID: " << CameraID << endl
//			<< "CameraFunction: " << CameraFunction << endl
//			<< "CameraAdjustment: " << CameraAdjustment << endl;
//		//for (int i = 0; i < 20; i++){
//			/*state = '$';
//			CameraID = 0;
//			CameraFunction = 10;
//			CameraAdjustment = pow(-1.0,i)*10;*/
//			if (state == "$" && CameraID < CameraControllers.size()){
//				issueCommand(CameraID, CameraFunction, CameraAdjustment, CameraControllers);
//			}
//			else if(state == "STest"){
//				SleepTime_Pan_Test(0,85,CameraControllers,250,800);
//			}
//			else if(state == "Q") 
//				ACCEPT_FLAG = false;
//			else {
//				cout << "Invalid Command in PTZ\n";
//				cin.clear();
//			}
//				Sleep(1000);
//		}
//	//}
//	
//	
//	for(int i = 0; i < Devices_size; i++){
//		Devices[i]->Release();
//	}
//
//    CoUninitialize();      
//	getchar();
//    return result;   
//}   
