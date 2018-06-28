#include <openvr.h>
#include <iostream>
#include <string>
#include <vector>

struct Device {
	int index;
	vr::ETrackedDeviceClass type;
};
int getActiveDeviceNum(vr::IVRSystem *ivrSystem) {
	int out = 0;
	for(int i=0; i<vr::k_unMaxTrackedDeviceCount; ++i) {
		if(ivrSystem->IsTrackedDeviceConnected(i)) {
			++out;
		}
	}
	return out;
}
//*devices must be a pointer to an array of Device structs of size vr::k_unMaxTrackedDeviceCount
void catalogDevices(vr::IVRSystem *ivrSystem, Device *devices, bool printOutput) {
	std::string TrackedDeviceTypes[] = {"Invalid (disconnected)","HMD","Controller","Generic Tracker","Tracking Reference (base station)","Display Redirect"};

	for(int i=0; i<vr::k_unMaxTrackedDeviceCount; ++i) {
		/*
		if(ivrSystem->IsTrackedDeviceConnected(i)) {
			std::cout << "Tracked Device " << i << " is connected." << std::endl;
			std::cout << "The class of Tracked Device " << i << " is " << ivrSystem->GetTrackedDeviceClass(i) << "." << std::endl;
		} else {
			std::cout << "Tracked Device " << i << " is not connected." << std::endl;
		}*/
		if(printOutput) {
			std::cout << "Tracked Device " << i << " has type " << TrackedDeviceTypes[ivrSystem->GetTrackedDeviceClass(i)] << "." << std::endl;
		}
		(*(devices+i)).index = i;
		(*(devices+i)).type = ivrSystem->GetTrackedDeviceClass(i);
	}
}

int main() {
	// Check whether there is an HMD plugged-in and the SteamVR runtime is installed
	if(vr::VR_IsHmdPresent()) {
		std::cout << "An HMD was successfully found in the system" << std::endl;

		if(vr::VR_IsRuntimeInstalled()) {
			const char* runtime_path = vr::VR_RuntimePath();
			std::cout << "Runtime correctly installed at '" << runtime_path << "'" << std::endl;
		} else {
			std::cout << "Runtime was not found, quitting app" << std::endl;
			return -1;
		}
		
		vr::IVRSystem *ivrSystem;
		vr::HmdError *peError;
		ivrSystem = vr::VR_Init(peError,vr::VRApplication_Other);
		std::cout << "Pointer to the IVRSystem is " << ivrSystem << std::endl;
		
		Device devices[vr::k_unMaxTrackedDeviceCount];
		catalogDevices(ivrSystem,devices,false);
		std::vector<int> controllerIndices;
		for(int i=0; i<vr::k_unMaxTrackedDeviceCount; ++i) {
			if((devices+i)->type == vr::TrackedDeviceClass_Controller) {
				controllerIndices.push_back((devices+i)->index);
			}
		}
		
		for(std::vector<int>::iterator i=controllerIndices.begin(); i != controllerIndices.end(); ++i) {
			std::cout << "There is a controller with index " << *i << "." << std::endl;
		}
		std::cout << "There are " << controllerIndices.size() << " controllers." << std::endl;

		std::string input;
		do {
			//for(std::vector<int>::iterator i=controllerIndices.begin(); i != controllerIndices.end(); ++i) {
			//for(int i=1; i<3; ++i) {
				vr::VRControllerState_t state;
				vr::VRControllerState_t *pstate = &state;
				//int a = *i;
				//std::cout << a << std::endl;
				//ivrSystem->GetControllerState(i,pstate);
				std::cout << state.ulButtonPressed << std::endl;

				vr::TrackedDeviceIndex_t index = 1;
				ivrSystem->GetControllerState(index,pstate);
			//}
			getline(std::cin,input);
		} while(input != "exit");
			
		vr::VR_Shutdown();
	} else {
		std::cout << "No HMD was found in the system, quitting app" << std::endl;
		return -1;
	}
	return 0;
}
