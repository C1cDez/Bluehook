#include "bluehook.h"

#include <stdio.h>

#include <Windows.h>
#include <BluetoothAPIs.h>

#include "clofdev.h"

#pragma comment(lib, "Bthprops.lib")


#define YESNO(cond) (cond) ? "Yes" : "No"

#define MBNAME(name) name[0] ? name : L"[UNDEFINED]"


static
void addr2str(char* buff, BLUETOOTH_ADDRESS_STRUCT addr)
{
	sprintf_s(buff, 18, "%02x:%02x:%02x:%02x:%02x:%02x", 
		addr.rgBytes[5], addr.rgBytes[4], addr.rgBytes[3], addr.rgBytes[2], addr.rgBytes[1], addr.rgBytes[0]);
}
static
BLUETOOTH_ADDRESS_STRUCT str2addr(const char* addr)
{
	unsigned b0, b1, b2, b3, b4, b5;
	sscanf_s(addr, "%2x:%2x:%2x:%2x:%2x:%2x", &b5, &b4, &b3, &b2, &b1, &b0);
	BLUETOOTH_ADDRESS_STRUCT bth_addr = {
		.rgBytes = { b0, b1, b2, b3, b4, b5 }
	};
	return bth_addr;
}

static
void format_systemtime(char* buff, SYSTEMTIME st)
{
	if (st.wYear == 1601)
		sprintf_s(buff, 7, "[NONE]");
	else
		sprintf_s(buff, 24, "%hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

static
AUTHENTICATION_REQUIREMENTS str2ar(const char* policy)
{
	if (!strcmp("r", policy))			return MITMProtectionRequired;
	else if (!strcmp("rb", policy))		return MITMProtectionRequiredBonding;
	else if (!strcmp("rg", policy))		return MITMProtectionRequiredGeneralBonding;
	else if (!strcmp("nr", policy))		return MITMProtectionNotRequired;
	else if (!strcmp("nrb", policy))	return MITMProtectionNotRequiredBonding;
	else if (!strcmp("nrg", policy))	return MITMProtectionNotRequiredGeneralBonding;
	else								return MITMProtectionNotDefined;
}


int is_bluetooth_available()
{
	HANDLE hRadio;
	BLUETOOTH_FIND_RADIO_PARAMS params = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&params, &hRadio);
	if (hFind)
	{
		BluetoothFindRadioClose(hFind);
		return 1;
	}
	else return 0;
}


static
int switch_radio_modes(bth_radio_query_t* query, HANDLE hRadio)
{
	int connectable = BluetoothIsConnectable(hRadio), discoverable = BluetoothIsDiscoverable(hRadio);
	if (connectable | discoverable);

	if (query->connectability)
	{
		int query_con = query->connectability == 1;
		if (!connectable == !query_con)
			printf("Radio is already %s\n", connectable ? "connectable" : "not-connectable");
		else
		{
			if (BluetoothEnableIncomingConnections(hRadio, query_con))
				printf("Successfully %s incoming connections\n",
					query_con ? "enabled" : "disabled");
			else
				printf("Failed to %s incoming connections. Error: %x\n",
					query_con ? "enable" : "disable", GetLastError());
		}
	}
	if (query->discoverability)
	{
		int query_dis = query->discoverability == 1;
		if (!discoverable == !query_dis)
			printf("Radio is already %s\n", discoverable ? "discoverable" : "not-discoverable");
		else
		{
			if (BluetoothEnableDiscovery(hRadio, query_dis))
				printf("Successfully %s discovery\n",
					query_dis ? "enabled" : "disabled");
			else
				printf("Failed to %s discovery. Error: %x\n",
					query_dis ? "enable" : "disable", GetLastError());
		}
	}
	return 0;
}

int bluehook_radio_info(bth_radio_query_t* query)
{
	HANDLE hRadio;
	BLUETOOTH_FIND_RADIO_PARAMS params = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
	BLUETOOTH_RADIO_INFO radio_info = { sizeof(BLUETOOTH_RADIO_INFO) };
	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&params, &hRadio);

	if (!hFind)
	{
		fprintf(stderr, "Unable to get info about radio\n");
		return 1;
	}

	do
	{
		if (BluetoothGetRadioInfo(hRadio, &radio_info) == ERROR_SUCCESS)
		{
			if (query->connectability || query->discoverability)
				switch_radio_modes(query, hRadio);
			else
			{
				char addr[18] = { 0 };
				addr2str(addr, radio_info.address);
				char cod[128] = { 0 };
				class_of_device_easy(radio_info.ulClassofDevice, cod, 128);

				wprintf(
					L"%s:\n"
					"\tAddress:\t\t%hs\n"
					"\tDevice:\t\t\t%hs (%u)\n"
					"\tManufacturer:\t\t%hs (%hd)\n"
					"\tLMP Subversion:\t\t%hd\n"
					"\tConnectable:\t\t%hs\n"
					"\tDiscoverable:\t\t%hs\n"
					"\n"
					,
					MBNAME(radio_info.szName),
					addr,
					cod, radio_info.ulClassofDevice,
					manufacturer(radio_info.manufacturer), radio_info.manufacturer,
					radio_info.lmpSubversion,
					YESNO(BluetoothIsConnectable(hRadio)),
					YESNO(BluetoothIsDiscoverable(hRadio))
				);
			}
		}

	}
	while (BluetoothFindNextRadio(hFind, &hRadio));

	BluetoothFindRadioClose(hFind);
	return 0;
}



static
void print_device_info(BLUETOOTH_DEVICE_INFO_STRUCT* device, const char* addr)
{
	SYSTEMTIME ls = device->stLastSeen, lu = device->stLastUsed;
	char last_used[24] = { 0 };
	format_systemtime(last_used, lu);
	char cod[128] = { 0 };
	class_of_device_easy(device->ulClassofDevice, cod, 128);

	wprintf(
		L"%s:\n"
		"\tAddress: \t\t%hs\n"
		"\tDevice: \t\t%hs\n"
		"\tConnected:\t\t%hs\n"
		"\tAuthentificated:\t%hs\n"
		"\tRemembered:\t\t%hs\n"
		"\tLast Seen: \t\t%hd-%02hd-%02hd %02hd:%02hd:%02hd.%03hd\n"
		"\tLast Used: \t\t%hs\n"
		"\n"
		,
		MBNAME(device->szName),
		addr,
		cod,
		YESNO(device->fConnected),
		YESNO(device->fAuthenticated),
		YESNO(device->fRemembered),
		ls.wYear, ls.wMonth, ls.wDay, ls.wHour, ls.wMinute, ls.wSecond, ls.wMilliseconds,
		last_used
	);
}

int bluehook_scan(bth_scan_query_t* query)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS params = {
		.dwSize					= sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
		.cTimeoutMultiplier		= (int)(query->timeout / 1.28),
		.fReturnConnected		= query->connected,
		.fReturnAuthenticated	= query->authetificated,
		.fReturnRemembered		= query->remembered,
		.fReturnUnknown			= query->unknown,
		.fIssueInquiry			= TRUE,
		.hRadio					= NULL
	};

	BLUETOOTH_DEVICE_INFO_STRUCT device = { sizeof(BLUETOOTH_DEVICE_INFO_STRUCT) };
	HBLUETOOTH_DEVICE_FIND hFind = BluetoothFindFirstDevice(&params, &device);

	if (!hFind)
	{
		fprintf(stderr, "Unable to find bluetooth devices\n");
		return 1;
	}

	printf("Found Bluetooth devices:\n\n");
	do
	{
		if (BluetoothUpdateDeviceRecord(&device) == ERROR_SUCCESS);
		char addr[18] = { 0 };
		addr2str(addr, device.Address);

		if (query->do_info)
			print_device_info(&device, addr);
		else
		{
			wprintf(
				L"%s (%hs)\n",
				MBNAME(device.szName),
				addr
			);
		}
			
		device.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_STRUCT);
	}
	while (BluetoothFindNextDevice(hFind, &device));
	
	BluetoothFindDeviceClose(hFind);
	return 0;
}


int bluehook_device_info(const char* addr)
{
	BLUETOOTH_ADDRESS_STRUCT bth_addr = str2addr(addr);
	BLUETOOTH_DEVICE_INFO_STRUCT device = { sizeof(BLUETOOTH_DEVICE_INFO_STRUCT) };
	device.Address = bth_addr;

	if (BluetoothGetDeviceInfo(NULL, &device) != ERROR_SUCCESS)
	{
		printf("Failed to locate the device %s\n", addr);
		return 1;
	}

	if (BluetoothUpdateDeviceRecord(&device) == ERROR_SUCCESS);

	print_device_info(&device, addr);

	return 0;
}

int bluehook_class_of_device_info(const char* addr)
{
	BLUETOOTH_ADDRESS_STRUCT bth_addr = str2addr(addr);
	BLUETOOTH_DEVICE_INFO_STRUCT device = { sizeof(BLUETOOTH_DEVICE_INFO_STRUCT) };
	device.Address = bth_addr;

	if (BluetoothGetDeviceInfo(NULL, &device) != ERROR_SUCCESS)
	{
		printf("Failed to locate the device %s\n", addr);
		return 1;
	}

	if (BluetoothUpdateDeviceRecord(&device) == ERROR_SUCCESS);

	wprintf(L"%s:   [", device.szName);
	class_of_device_full(device.ulClassofDevice);

	return 0;
}

int bluehook_remove(const char* addr)
{
	BLUETOOTH_ADDRESS_STRUCT bth_addr = str2addr(addr);
	if (BluetoothRemoveDevice(&bth_addr) == ERROR_SUCCESS)
	{
		printf("Device %s was removed successfully\n", addr);
		return 0;
	}
	else
	{
		printf("Error occured during the removal\n");
		return 1;
	}
}

static
BOOL CALLBACK bluehook_auth_callback(LPVOID pvParam, 
	PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams)
{
	BLUETOOTH_AUTHENTICATE_RESPONSE response = { 0 };
	response.bthAddressRemote = pAuthCallbackParams->deviceInfo.Address;
	response.authMethod = pAuthCallbackParams->authenticationMethod;
	response.negativeResponse = FALSE;

	if (pAuthCallbackParams->authenticationMethod == BLUETOOTH_AUTHENTICATION_METHOD_NUMERIC_COMPARISON)
	{
		response.numericCompInfo.NumericValue = pAuthCallbackParams->Numeric_Value;
		printf("Numeric comparison: %u\n", pAuthCallbackParams->Numeric_Value);
	}
	else if (pAuthCallbackParams->authenticationMethod == BLUETOOTH_AUTHENTICATION_METHOD_PASSKEY)
	{
		response.passkeyInfo.passkey = pAuthCallbackParams->Passkey;
		printf("Passkey: %u\n", pAuthCallbackParams->Passkey);
	}

	int result = BluetoothSendAuthenticationResponseEx(NULL, &response);
	return result == ERROR_SUCCESS;
}


int bluehook_auth(bth_auth_query_t* auth_query)
{
	BLUETOOTH_DEVICE_INFO_STRUCT device = { sizeof(BLUETOOTH_DEVICE_INFO_STRUCT) };
	device.Address = str2addr(auth_query->addr);

	HBLUETOOTH_AUTHENTICATION_REGISTRATION hReg = NULL;
	if (BluetoothRegisterForAuthenticationEx(&device, &hReg, &bluehook_auth_callback, NULL) != ERROR_SUCCESS)
	{
		printf("Failed to configure authentification callback\n");
		return 1;
	}

	AUTHENTICATION_REQUIREMENTS ar = str2ar(auth_query->mitm_protection_policy);
	int res = BluetoothAuthenticateDeviceEx(NULL, NULL, &device, NULL, ar);

	if (res == ERROR_SUCCESS)
	{
		if (BluetoothGetDeviceInfo(NULL, &device) == ERROR_SUCCESS)
		{
			if (device.fAuthenticated) wprintf(L"Successfully paired with '%s' (%hs)\n", 
				MBNAME(device.szName), auth_query->addr);
			else printf("Something went wrong.\nTry change MITM protection policy. Use -m=...\n");
		}
	}
	else
	{
		printf("Initiate request failed: %d\n", res);
		if (res == ERROR_NOT_AUTHENTICATED) printf("Device refused to pair\n");
		else if (res == ERROR_INVALID_PARAMETER) printf("MITM protection policy not defined. Use -m=...\n");
	}

	BluetoothUnregisterAuthentication(hReg);
	return 0;
}
