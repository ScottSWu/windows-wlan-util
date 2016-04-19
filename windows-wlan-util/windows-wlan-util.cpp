// windows-wlan-util.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Struct containing useful network info
struct NetworkInfo {
	UCHAR ssid[32];
	UCHAR bssid[32];
	ULONG frequency;
	LONG rssi;
	ULONG quality;
};

// Get a WLAN handle, necessary for all other operations
HANDLE getWlanHandle() {
	// Result variable
	DWORD dwResult;

	// Maximum number of clients (?)
	DWORD dwMaxClient = 2;
	// Current version
	DWORD dwCurVersion;

	// Handle
	HANDLE hClient = NULL;
	// Open WLAN handle
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);

	if (dwResult != ERROR_SUCCESS) {
		return NULL;
	}
	else {
		return hClient;
	}
}

// Get a list of WLAN interfaces
PWLAN_INTERFACE_INFO_LIST getWlanInterfaces(HANDLE whandle) {
	PWLAN_INTERFACE_INFO_LIST ifList = NULL;
	DWORD dwResult;
	dwResult = WlanEnumInterfaces(whandle, NULL, &ifList);

	if (dwResult != ERROR_SUCCESS) {
		return NULL;
	}
	else {
		return ifList;
	}
}

// Initiate a scan of Wifi Networks on the given interface
boolean scanWifiNetworks(HANDLE hClient, GUID ifGuid) {
	DWORD dwResult;
	dwResult = WlanScan(hClient, &ifGuid, NULL, NULL, NULL);
	if (dwResult != ERROR_SUCCESS) {
		return false;
	}
	else {
		return true;
	}
}

// Get a list of Wifi Networks from the last scan
std::vector<NetworkInfo> getWifiNetworks(HANDLE hClient, GUID ifGuid) {
	// Return variable
	std::vector<NetworkInfo> ns;

	// Result variable
	DWORD dwResult;

	// List of interfaces
	PWLAN_INTERFACE_INFO_LIST ifList = NULL;
	// Interface info
	PWLAN_INTERFACE_INFO ifInfo = NULL;
	// Service list
	PWLAN_BSS_LIST bssList = NULL;
	// Network service
	PWLAN_BSS_ENTRY bssEntry = NULL;

	// Retrieve a list of available networks
	dwResult = WlanGetNetworkBssList(hClient, &ifGuid, NULL, dot11_BSS_type_any, NULL, NULL, &bssList);
	if (dwResult != ERROR_SUCCESS) {
	}
	else {
		// For each network
		for (unsigned int j = 0; j < bssList->dwNumberOfItems; j++) {
			// Grab the network
			bssEntry = (WLAN_BSS_ENTRY *)&bssList->wlanBssEntries[j];

			// Network info struct
			NetworkInfo ni;
			// SSID
			for (int k = 0, l = bssEntry->dot11Ssid.uSSIDLength; k < 32; k++) {
				if (k < l) ni.ssid[k] = (int)bssEntry->dot11Ssid.ucSSID[k];
				else ni.ssid[k] = 0;
			}
			// Hardware MAC Address
			for (int k = 0, l = 6; k < 32; k++) {
				if (k < l) ni.bssid[k] = (int)bssEntry->dot11Bssid[k];
				else ni.bssid[k] = 0;
			}
			// Channel Frequency
			ni.frequency = bssEntry->ulChCenterFrequency;
			// Rssi
			ni.rssi = bssEntry->lRssi;
			// Signal quality
			ni.quality = bssEntry->uLinkQuality;

			// Add to the list
			ns.push_back(ni);
		}
	}

	// Clean up
	if (bssList != NULL) {
		WlanFreeMemory(bssList);
		bssList = NULL;
	}

	// Finish
	return ns;
}

// Print usage
void usage(const char* name) {
	printf("Usage: %s <task> [interface number]\n", name);
}

// Tasks
const int TASK_USAGE = 0;
const int TASK_LIST_INTERFACES = 1;
const int TASK_SCAN_NETWORKS = 2;
const int TASK_LIST_NETWORKS = 3;

// Main program
int main(int argc, char** argv)
{
	// Parse arguments
	int task = 0;
	if (argc < 2) {
		usage(argv[0]);
		return 2;
	}
	// Interface number
	DWORD ifSelect = 0;

	// Task
	if (argc >= 2) {
		if (strncmp(argv[1], "if", 2) == 0) {
			task = TASK_LIST_INTERFACES;
		}
		else if (strncmp(argv[1], "scan", 4) == 0) {
			task = TASK_SCAN_NETWORKS;
		}
		else if (strncmp(argv[1], "list", 4) == 0) {
			task = TASK_LIST_NETWORKS;
		}
	}

	// Interface
	if (argc >= 3) {
		sscanf_s(argv[2], "%d", &ifSelect);
	}

	// Setup
	HANDLE whandle = getWlanHandle();
	std::string input;

	if (whandle == NULL) {
		printf("Error: Failed to open wlan handle.\n");
		return 1;
	}

	// Print usage and exit
	if (task == TASK_USAGE) {
		usage(argv[0]);
		return 2;
	}

	// Get interfaces
	PWLAN_INTERFACE_INFO_LIST ifList = NULL;
	ifList = getWlanInterfaces(whandle);

	if (ifList == NULL) {
		printf("Error: Failed to find wlan interfaces.\n");
		return 1;
	}

	// Print interfaces and exit
	if (task == TASK_LIST_INTERFACES) {
		for (DWORD i = 0; i < ifList->dwNumberOfItems; i++) {
			PWLAN_INTERFACE_INFO info = (WLAN_INTERFACE_INFO *)&ifList->InterfaceInfo[i];
			printf("% 2d  %ws\n", i, info->strInterfaceDescription);
		}
		WlanFreeMemory(ifList);
		return 0;
	}

	// Select the wlan interface
	GUID ifGuid;
	if (ifList->dwNumberOfItems == 0) {
		printf("Error: No wlan interfaces exist.\n");
		return 1;
	}
	else {
		if (ifSelect >= ifList->dwNumberOfItems) {
			//printf("Warning: Interface %d does not exist. Defaulting to interface 0.\n", ifSelect);
			ifSelect = 0;
		}
		ifGuid = ((WLAN_INTERFACE_INFO *)&ifList->InterfaceInfo[ifSelect])->InterfaceGuid;
	}
	WlanFreeMemory(ifList);

	if (task == TASK_SCAN_NETWORKS) {
		// Scan for network
		scanWifiNetworks(whandle, ifGuid);
	}
	else if (task == TASK_LIST_NETWORKS) {
		// List available networks
		std::vector<NetworkInfo> networks = getWifiNetworks(whandle, ifGuid);
		
		for (auto itr = networks.begin(); itr != networks.end(); itr++) {
			// Rssi
			printf("% 4d  ", (*itr).rssi);
			// Quality
			printf("% 4d  ", (*itr).quality);
			// Bssid
			for (int i = 0; i < 6; i++) printf("%02x", (*itr).bssid[i]) && (i<5) && printf(":");
			printf("  ");
			// Ssid
			for (int i = 0; i < 32; i++) printf("%c", (*itr).ssid[i]);
			printf("\n");
		}
	}

	return 0;
}
