#include "bluehook.h"

#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>

#include "bthutils.h"
#include "clofdev.h"


#define MBNAME(name) name[0] ? name : L"[UNDEFINED]"


static AUTHENTICATION_REQUIREMENTS str2ar(const char *policy)
{
	if (!strcmp("r", policy))			return MITMProtectionRequired;
	else if (!strcmp("rb", policy))		return MITMProtectionRequiredBonding;
	else if (!strcmp("rg", policy))		return MITMProtectionRequiredGeneralBonding;
	else if (!strcmp("nr", policy))		return MITMProtectionNotRequired;
	else if (!strcmp("nrb", policy))	return MITMProtectionNotRequiredBonding;
	else if (!strcmp("nrg", policy))	return MITMProtectionNotRequiredGeneralBonding;
	else								return MITMProtectionNotDefined;
}


static int is_bluetooth_available(void)
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

int bluehook_init(void)
{
	if (!is_bluetooth_available())
	{
		printf("Bluetooth is turned off\nEnable it on "
			"\x1b]8;;ms-settings:bluetooth\x1b\\ms-settings:bluetooth\x1b]8;;\x1b\\\n");
		return 1;
	}
	return 0;
}
int bluehook_cleanup(void)
{
	return 0;
}


static int switch_radio_modes(bth_radio_query_t *query, HANDLE hRadio)
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
			if (BluetoothEnableIncomingConnections(hRadio, query_con) != ERROR_SUCCESS)
				printf("Successfully %s incoming connections\n",
					query_con ? "enabled" : "disabled");
			else
				printf("Failed to %s incoming connections. Error: %lx. (Are you admin?)\n",
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
			if (BluetoothEnableDiscovery(hRadio, query_dis) != ERROR_SUCCESS)
				printf("Successfully %s discovery\n",
					query_dis ? "enabled" : "disabled");
			else
				printf("Failed to %s discovery. Error: %lx\n",
					query_dis ? "enable" : "disable", GetLastError());
		}
	}
	return 0;
}

int bluehook_radio_info(bth_radio_query_t *query)
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
				char addr[BLUETOOTH_ADDRESS_STRLEN] = { 0 };
				addr2str(addr, radio_info.address);
				char cod[RECOMMENDED_COD_REPR_STRLEN] = { 0 };
				cod_format_params_t cfp = {
					.main_name = "%s - %s\n",
					.service_header = "\tServices:\n",
					.service = "\t- %-22s%-3s\n"
				};
				if (query->iqp.hide_services)
				{
					cfp.service_header = NULL;
					cfp.service = NULL;
				}
				class_of_device_format(radio_info.ulClassofDevice, cod, &cfp);

				wprintf(
					L"%s:\n"
					"\tAddress:\t\t%hs\n"
					"\tDevice:\t\t\t%hs"
					"\tManufacturer:\t\t%hs (%hd)\n"
					"\tLMP Subversion:\t\t%hd\n"
					"\tConnectable:\t\t%hs\n"
					"\tDiscoverable:\t\t%hs\n"
					,
					MBNAME(radio_info.szName),
					addr,
					cod,
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


typedef struct
{
	const wchar_t *name;		/* %s */
	const char *address;		/* %s */
	cod_format_params_t cod_format;
	const char *bindings;		/* %s, %s */
	const char *times;			/* %s, %s */
} device_display_params_t;

static const device_display_params_t PREVIEW_INFO_DDP = {
	.name = L"%-32s",
	.address = "%-30s",
	.cod_format = {
		.main_name = "%s (%s)\n",
		.service_header = NULL,
		.service = NULL
	},
	.bindings = NULL,
	.times = NULL
};
static const device_display_params_t FULL_INFO_DDP = {
	.name = L"%s:\n",
	.address = "\tAddress:%33s\n",
	.cod_format = {
		.main_name = "\tDevice:                 %s - %s\n",
		.service_header = "\tServices:\n",
		.service = "\t- %-22s%-3s\n"
	},
	.bindings = "\t%-24s%-3s\n",
	.times = "\t%-24s%-3s\n"
};
static const device_display_params_t FULL_HIDE_SERVICES_DDP = {
	.name = L"%s:\n",
	.address = "\tAddress:%33s\n",
	.cod_format = {
		.main_name = "\tDevice:                 %s - %s\n",
		.service_header = NULL,
		.service = NULL
	},
	.bindings = "\t%-24s%-3s\n",
	.times = "\t%-24s%-3s\n"
};

static const device_display_params_t *const ddp_from_iqp(info_query_params_t iqp)
{
	if (iqp.do_info)
		return iqp.hide_services ? &FULL_HIDE_SERVICES_DDP : &FULL_INFO_DDP;
	else 
		return &PREVIEW_INFO_DDP;
}

static void fprint_device_info(FILE *fp, BLUETOOTH_DEVICE_INFO_STRUCT *device,
	const device_display_params_t *ddp)
{
	fwprintf(fp, ddp->name, MBNAME(device->szName));
	
	if (ddp->address)
	{
		char addr[BLUETOOTH_ADDRESS_STRLEN] = { 0 };
		addr2str(addr, device->Address);
		fprintf(fp, ddp->address, addr);
	}

	char cod[RECOMMENDED_COD_REPR_STRLEN] = { 0 };
	class_of_device_format(device->ulClassofDevice, cod, &ddp->cod_format);
	fprintf(fp, cod);

	if (ddp->bindings)
	{
		fprintf(fp, ddp->bindings, "Connected",		YESNO(device->fConnected));
		fprintf(fp, ddp->bindings, "Authenticated",	YESNO(device->fAuthenticated));
		fprintf(fp, ddp->bindings, "Remembered",	YESNO(device->fRemembered));
	}

	if (ddp->times)
	{
		SYSTEMTIME ls = device->stLastSeen, lu = device->stLastUsed;
		char last_seen[24] = { 0 }, last_used[24] = { 0 };
		format_systemtime(last_seen, ls);
		format_systemtime(last_used, lu);
		fprintf(fp, ddp->times, "Last Seen", last_seen);
		fprintf(fp, ddp->times, "Last Used", last_used);
	}
}

int bluehook_scan(bth_scan_query_t *query, int local)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS params = {
		.dwSize					= sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
		.cTimeoutMultiplier		= (UCHAR)(query->timeout / 1.28),
		.fReturnConnected		= query->connected,
		.fReturnAuthenticated	= query->authetificated,
		.fReturnRemembered		= query->remembered,
		.fReturnUnknown			= query->unknown,
		.fIssueInquiry			= local ? FALSE : TRUE,
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

		fprint_device_info(stdout, &device, ddp_from_iqp(query->iqp));
		if (query->iqp.do_info) putchar('\n');

		device.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_STRUCT);
	}
	while (BluetoothFindNextDevice(hFind, &device));
	
	BluetoothFindDeviceClose(hFind);
	return 0;
}


int bluehook_device_info(bth_info_query_t *query)
{
	BLUETOOTH_ADDRESS_STRUCT bth_addr = str2addr(query->addr);
	if (bth_addr.ullLong == BLUETOOTH_NULL_ADDRESS)
	{
		printf("Incorrect address '%s'\n", query->addr);
		return 1;
	}

	BLUETOOTH_DEVICE_INFO_STRUCT device = { sizeof(BLUETOOTH_DEVICE_INFO_STRUCT) };
	device.Address = bth_addr;

	if (BluetoothGetDeviceInfo(NULL, &device) != ERROR_SUCCESS)
	{
		printf("Failed to locate the device %s\n", query->addr);
		return 1;
	}

	if (BluetoothUpdateDeviceRecord(&device) == ERROR_SUCCESS);

	fprint_device_info(stdout, &device, ddp_from_iqp(query->iqp));

	return 0;
}

int bluehook_remove(const char *addr)
{
	BLUETOOTH_ADDRESS_STRUCT bth_addr = str2addr(addr);
	if (bth_addr.ullLong == BLUETOOTH_NULL_ADDRESS)
	{
		printf("Incorrect address '%s'\n", addr);
		return 1;
	}

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

static BOOL CALLBACK bluehook_auth_callback(LPVOID pvParam, 
	PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams)
{
	(void)pvParam;
	BLUETOOTH_AUTHENTICATE_RESPONSE response = { 0 };
	response.bthAddressRemote = pAuthCallbackParams->deviceInfo.Address;
	response.authMethod = pAuthCallbackParams->authenticationMethod;
	response.negativeResponse = FALSE;

	if (pAuthCallbackParams->authenticationMethod == BLUETOOTH_AUTHENTICATION_METHOD_NUMERIC_COMPARISON)
	{
		response.numericCompInfo.NumericValue = pAuthCallbackParams->Numeric_Value;
		printf("Numeric comparison: %lu\n", pAuthCallbackParams->Numeric_Value);
	}
	else if (pAuthCallbackParams->authenticationMethod == BLUETOOTH_AUTHENTICATION_METHOD_PASSKEY)
	{
		response.passkeyInfo.passkey = pAuthCallbackParams->Passkey;
		printf("Passkey: %lu\n", pAuthCallbackParams->Passkey);
	}

	int result = BluetoothSendAuthenticationResponseEx(NULL, &response);
	return result == ERROR_SUCCESS;
}


int bluehook_auth(bth_auth_query_t *auth_query)
{
	BLUETOOTH_DEVICE_INFO_STRUCT device = { sizeof(BLUETOOTH_DEVICE_INFO_STRUCT) };
	device.Address = str2addr(auth_query->addr);
	if (device.Address.ullLong == BLUETOOTH_NULL_ADDRESS)
	{
		printf("Incorrect address '%s'\n", auth_query->addr);
		return 1;
	}

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
