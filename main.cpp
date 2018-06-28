#include <stdio.h>
#include <openvr.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <Windows.h>

struct Device {
	int index;
	vr::ETrackedDeviceClass type;
};

const std::string stateNames[3] = { "Released", "Touched ", "Pressed " }; // padded so each occupies the same number of characters
enum ButtonStates {
	Button_Released = 0,
	Button_Touched = 1,
	Button_Pressed = 2
};

const std::string buttonNames[4] = { "menu", "grip", "dpad", "trigger" };
const uint64_t buttonBitmasks[4] = {
	(uint64_t)pow(2, (int) vr::k_EButton_ApplicationMenu),
	(uint64_t)pow(2, (int) vr::k_EButton_Grip),
	(uint64_t)pow(2, (int) vr::k_EButton_SteamVR_Touchpad),
	(uint64_t)pow(2, (int) vr::k_EButton_SteamVR_Trigger)
};

Device devices[vr::k_unMaxTrackedDeviceCount];
std::vector<int> controllerIndices;

//*devices must be a pointer to an array of Device structs of size vr::k_unMaxTrackedDeviceCount
void catalogDevices(vr::IVRSystem* ivrSystem, Device* devices, bool printOutput) {
	std::string TrackedDeviceTypes[] = { "Invalid (disconnected)","HMD","Controller","Generic Tracker","Tracking Reference (base station)","Display Redirect" };

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
		if (printOutput) {
			std::cout << "Tracked Device " << i << " has type " << TrackedDeviceTypes[ivrSystem->GetTrackedDeviceClass(i)] << "." << std::endl;
		}
		(*(devices + i)).index = i;
		(*(devices + i)).type = ivrSystem->GetTrackedDeviceClass(i);
	}
}
void catalogControllers(Device* devices, bool printOutput) {
	controllerIndices.erase(controllerIndices.begin(), controllerIndices.begin() + controllerIndices.size());
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
		if ((devices + i)->type == vr::TrackedDeviceClass_Controller) {
			controllerIndices.push_back((devices + i)->index);
		}
	}

	if (printOutput) {
		for (std::vector<int>::iterator i = controllerIndices.begin(); i != controllerIndices.end(); ++i) {
			std::cout << "There is a controller with index " << *i << "." << std::endl;
		}
		std::cout << "There are " << controllerIndices.size() << " controllers." << std::endl;
	}
}

int main() {
	// Check whether there is an HMD plugged-in and the SteamVR runtime is installed
	if (vr::VR_IsHmdPresent()) {
		std::cout << "An HMD was successfully found in the system" << std::endl;

		if (vr::VR_IsRuntimeInstalled()) {
			const char* runtime_path = vr::VR_RuntimePath();
			std::cout << "Runtime correctly installed at '" << runtime_path << "'" << std::endl;
		}
		else {
			std::cout << "Runtime was not found, quitting app" << std::endl;
			return -1;
		}

		// Initialize system
		vr::HmdError error;
		//char stopSegFault[2048];
		vr::IVRSystem* ivrSystem = vr::VR_Init(&error, vr::VRApplication_Other/*, stopSegFault*/);
		std::cout << "Pointer to the IVRSystem is " << ivrSystem << std::endl;

		catalogDevices(ivrSystem, devices, false);
		catalogControllers(devices, true);

		std::string input;
		while (input != "exit") {
			if (input == "update") {
				catalogDevices(ivrSystem, devices, true);
				catalogControllers(devices, true);
			}
			else if (input == "track") {
				while (true) {
					ButtonStates states[4];
					for (std::vector<int>::iterator itr = controllerIndices.begin(); itr != controllerIndices.end(); ++itr) {
						vr::VRControllerState_t state;
						vr::TrackedDevicePose_t pose;
						std::cout << "Controller " << *itr << "    ";

						ivrSystem->GetControllerStateWithPose(vr::TrackingUniverseSeated, *itr, &state, sizeof(state), &pose);

						
						for (int button = 0; button < 4; ++button) {
							if ((state.ulButtonPressed & buttonBitmasks[button]) != 0) {
								states[button] = Button_Pressed;
							}
							else if ((state.ulButtonTouched & buttonBitmasks[button]) != 0) {
								states[button] = Button_Touched;
							}
							else {
								states[button] = Button_Released;
							}

							std::cout << buttonNames[button] << ": " << stateNames[states[button]] << "      ";
						}
					}

					std::cout << "\r";
					if ((GetKeyState(VK_SHIFT) & 0x8000)) {
						std::cout << std::endl;
						input = "";
						break;
					}
				}
			}
			else if (input == "pulse") {
				for (std::vector<int>::iterator itr = controllerIndices.begin(); itr != controllerIndices.end(); ++itr) {
					ivrSystem->TriggerHapticPulse(*itr, 1, pow(2, 16) - 1);
					input = "";
				}
			}
			getline(std::cin, input);
		}

		//std::cout << controllerIndices.size() << std::endl;
		//std::cout << "Button Mask: " << buttonMask << std::endl;

		std::cout << "exiting..." << std::endl;

		vr::VR_Shutdown();
	}
	else {
		std::cout << "No HMD was found in the system, quitting app" << std::endl;
		return -1;
	}
	return 0;
}
