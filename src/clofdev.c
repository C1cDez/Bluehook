#include "clofdev.h"

#include <string.h>
#include <stdio.h>


#define UNRECOGN "[UNRECOGNIZED]"


typedef union
{
	struct
	{
		unsigned pad		: 2;
		unsigned minor		: 6;
		unsigned major		: 5;
		unsigned service	: 11;
	};
	unsigned classofdevice;
} class_of_device_t;


const char* manufacturer(unsigned man)
{
	switch (man)
	{
		// Top 7 in the list
	case 0x00: return "Ericsson AB";
	case 0x01: return "Nokia Mobile Phones";
	case 0x02: return "Intel Corp.";
	case 0x03: return "IBM Corp.";
	case 0x04: return "Toshiba Corp.";
	case 0x05: return "3Com";
	case 0x06: return "Microsoft";
		// Top 18 in the world by production
	case 0x59: 
		return "Nordic Semiconductor";
	case 0x1d: case 0xd8: case 0xb8: case 0x011a: case 0x03e3: case 0x0a: case 0xd7: 
		return "Qualcomm";
	case 0x0d:
		return "Texas Instruments Inc.";
	case 0x02ff:
		return "Silicon Laboratories";
	case 0xe9b: case 0x03a:
		return "Panasonic Corp.";
	case 0x0171:
		return "Amazon.com Services LLC";
	case 0x5d:
		return "Realtek Semiconductor Corp.";
	case 0x46:
		return "MediaTek Inc.";
	case 0x09:
		return "Infineon Techonologies AG";
	case 0x25:
		return "NXP Semiconductors";
	case 0x02e5:
		return "Esspressif Systems Co. Ltd.";
	case 0x0211:
		return "Telink Semiconductor Co. Ltd.";
	case 0x0f:
		return "Broadcom Corp.";
	case 0x013c:
		return "Murata Manufacturing Co. Ltd.";
	case 0x5d6:
		return "Zhuhai Jieli technology Co. Ltd.";
	case 0x03e0:
		return "Actions Technology Co. Ltd.";
	case 0xfef5:
		return "Dialog Semiconductor GmbH";
	case 0x02b0:
		return "Bestechnic Ltd.";
		// and so long, 4234 positions...
	default: return UNRECOGN;
	}
}

static
const char* name_major(class_of_device_t cod)
{
	switch (cod.major)
	{
	case 0b00000: return "Miscellaneous";
	case 0b00001: return "Computer";
	case 0b00010: return "Phone";
	case 0b00011: return "Access Point";
	case 0b00100: return "Audio/Video";
	case 0b00101: return "Peripherial";
	case 0b00110: return "Imaging";
	case 0b00111: return "Wearable";
	case 0b01000: return "Toy";
	case 0b01001: return "Health";
	default: return UNRECOGN;
	}
}

static
const char* name_minor(class_of_device_t cod)
{
	switch (cod.major)
	{
	case 0b00001:		// Computer
	{
		switch (cod.minor)
		{
		case 0b000001: return "Desktop Workstation";
		case 0b000010: return "Server-class Computer";
		case 0b000011: return "Laptop";
		case 0b000100: return "Handheld PC/PDA";
		case 0b000101: return "Palm-size PC/PDA";
		case 0b000110: return "Wearable Computer";
		case 0b000111: return "Tablet";
		default: return UNRECOGN;
		}
	}
	case 0b00010:		// Phone
	{
		switch (cod.minor)
		{
		case 0b000001: return "Cellular";
		case 0b000010: return "Cordless";
		case 0b000011: return "Smartphone";
		case 0b000100: return "Wired modern / Voice gateway";
		case 0b000101: return "Common ISDA Access";
		default: return UNRECOGN;
		}
	}
	case 0b00011:		// AP
	{
		switch (cod.minor)
		{
		case 0b000000: return "Fully available";
		case 0b001000: return "1-17% utilized";
		case 0b010000: return "17-33% utilized";
		case 0b011000: return "33-50% utilized";
		case 0b100000: return "50-67% utilized";
		case 0b101000: return "67-83% utilized";
		case 0b110000: return "83-99% utilized";
		case 0b111000: return "No serive available";
		default: return UNRECOGN;
		}
	}
	case 0b00100:		// Audio/Video
	{
		switch (cod.minor)
		{
		case 0b000001: return "Wearable Headset Device";
		case 0b000010: return "Hands-free Device";
		// case 0b000011: reserved
		case 0b000100: return "Microphone";
		case 0b000101: return "Loudspeaker";
		case 0b000110: return "Headphones";
		case 0b000111: return "Portable Audio";
		case 0b001000: return "Car Audio";
		case 0b001001: return "Set-top box";
		case 0b001010: return "HiFi Audio Device";
		case 0b001011: return "VCR";
		case 0b001100: return "Video Camera";
		case 0b001101: return "Camcorder";
		case 0b001110: return "Video Monitor";
		case 0b001111: return "Video Display & Loudspeaker";
		case 0b010000: return "Video Conferencing";
		// case 0b010001: reserved
		case 0b010010: return "Gaming Toy";
		case 0b010011: return "Hearing Aid";
		case 0b010100: return "Glasses";
		default: return UNRECOGN;
		}
	}
	case 0b00101:		// Peropherial
	{
		switch (cod.minor)
		{
			// Keyboard
		case 0b010001: return "Keyboard - Joystick";
		case 0b010010: return "Keyboard - Gamepad";
		case 0b010011: return "Keyboard - Remote Control";
		case 0b010100: return "Keyboard - Sensing Device";
		case 0b010101: return "Keyboard - Digitizer Tablet";
		case 0b010110: return "Keyboard - Card Reader";
		case 0b010111: return "Keyboard - Digital Pen";
		case 0b011000: return "Keyboard - Handheld Scanner";
		case 0b011001: return "Keyboard - Handheld Gestural Input Device";
			// Pointing
		case 0b100001: return "Pointing Device - Joystick";
		case 0b100010: return "Pointing Device - Gamepad";
		case 0b100011: return "Pointing Device - Remote Control";
		case 0b100100: return "Pointing Device - Sensing Device";
		case 0b100101: return "Pointing Device - Digitizer Tablet";
		case 0b100110: return "Pointing Device - Card Reader";
		case 0b100111: return "Pointing Device - Digital Pen";
		case 0b101000: return "Pointing Device - Handheld Scanner";
		case 0b101001: return "Pointing Device - Handheld Gestural Input Device";
			// Combo
		case 0b110001: return "Combo - Joystick";
		case 0b110010: return "Combo - Gamepad";
		case 0b110011: return "Combo - Remote Control";
		case 0b110100: return "Combo - Sensing Device";
		case 0b110101: return "Combo - Digitizer Tablet";
		case 0b110110: return "Combo - Card Reader";
		case 0b110111: return "Combo - Digital Pen";
		case 0b111000: return "Combo - Handheld Scanner";
		case 0b111001: return "Combo - Handheld Gestural Input Device";
		default: return UNRECOGN;
		}
	}
	case 0b00110:		// Imaging
	{
		switch (cod.minor)
		{
		case 0b000100: return "Display";
		case 0b001000: return "Camera";
		case 0b010000: return "Scanner";
		case 0b100000: return "Printer";
		default: return UNRECOGN;
		}
	}
	case 0b00111:		// Wearable
	{
		switch (cod.minor)
		{
		case 0b000001: return "Wristwatch";
		case 0b000010: return "Pager";
		case 0b000011: return "Jacket";
		case 0b000100: return "Helmet";
		case 0b000101: return "Glasses";
		case 0b000110: return "Pin";
		default: return UNRECOGN;
		}
	}
	case 0b001000:		// Toy
	{
		switch (cod.minor)
		{
		case 0b000001: return "Robot";
		case 0b000010: return "Vehicle";
		case 0b000011: return "Doll / Action Figure";
		case 0b000100: return "Controller";
		case 0b000101: return "Game";
		default: return UNRECOGN;
		}
	}
	case 0b001001:		// Health
	{
		switch (cod.minor)
		{
		case 0b000001: return "Blood Pressure Monitor";
		case 0b000010: return "Thermometer";
		case 0b000011: return "Weighing Scale";
		case 0b000100: return "Glucose Meter";
		case 0b000101: return "Pulse Oximeter";
		case 0b000110: return "Heart/Pulse Rate Monitor";
		case 0b000111: return "Health Data Display";
		case 0b001000: return "Step Counter";
		case 0b001001: return "Body Composition Analyzer";
		case 0b001010: return "Peak Flow Monitor";
		case 0b001011: return "Medication Monitor";
		case 0b001100: return "Knee Prosthesis";
		case 0b001101: return "Ankle Prosthesis";
		case 0b001110: return "Genric Health Manager";
		case 0b001111: return "Personal Mobility Device";
		default: return UNRECOGN;
		}
	}
	default: return UNRECOGN;
	}
}


#define YESNO(cond) (cond) ? "Yes" : "No"

void class_of_device_format(unsigned ucod, char* buff, int size, const char* param_prefix)
{
	class_of_device_t cod = { .classofdevice = ucod };
	unsigned service = cod.service;

	sprintf_s(buff, size,
		"%s - %s\n"
		"\tServices:\n"
		"%sLimited Discovery:\t%s\n"
		"%sLow Energy Audio:\t%s\n"
		"%sPositioning:\t\t%s\n"
		"%sNetworking:\t\t%s\n"
		"%sRendering:\t\t%s\n"
		"%sCapturing:\t\t%s\n"
		"%sObject-Transfer:\t%s\n"
		"%sAudio:\t\t%s\n"
		"%sTelephony:\t\t%s\n"
		"%sInformation (WEB):\t%s"
		,
		name_major(cod), name_minor(cod),
		param_prefix, YESNO(service & 0x000001),
		param_prefix, YESNO(service & 0x000002),
		param_prefix, YESNO(service & 0x000008),
		param_prefix, YESNO(service & 0x000010),
		param_prefix, YESNO(service & 0x000020),
		param_prefix, YESNO(service & 0x000040),
		param_prefix, YESNO(service & 0x000080),
		param_prefix, YESNO(service & 0x000100),
		param_prefix, YESNO(service & 0x000200),
		param_prefix, YESNO(service & 0x000400)
	);
}
