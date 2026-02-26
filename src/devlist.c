#include "devlist.h"

#include <stdio.h>
#include <stdlib.h>


#define DEVLIST_FILE "devlist"


static
struct _node
{
	BLUETOOTH_DEVICE_INFO_STRUCT device;
	struct _node* next;
} * head = NULL;

int devlist_init()
{
	FILE* fp = fopen(DEVLIST_FILE, "rb");
	if (!fp)
	{
		FILE* wfp = fopen(DEVLIST_FILE, "wb");
		if (!wfp) return 1;
		fclose(wfp);
	}
	if (fp) fclose(fp);
	return 0;
}
int devlist_load()
{
	FILE* fp = fopen(DEVLIST_FILE, "rb");
	if (!fp) return 1;

	BLUETOOTH_DEVICE_INFO_STRUCT device = { 0 };
	while (fread((void*)&device, sizeof(BLUETOOTH_DEVICE_INFO_STRUCT), 1, fp))
	{
		device.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_STRUCT);
		struct _node* node = malloc(sizeof(struct _node));
		if (!node) continue;
		node->device = device;
		node->next = head;
		head = node;
	}

	fclose(fp);
	return 0;
}

static
struct _node* g_current = NULL;

int devlist_next(BLUETOOTH_DEVICE_INFO_STRUCT* device)
{
	if (g_current)
	{
		*device = g_current->device;
		g_current = g_current->next;
		return 1;
	}
	else return 0;
}
int devlist_rewind()
{
	g_current = head;
}

static
struct _node* lookfor_same_addr(BLUETOOTH_ADDRESS_STRUCT bth_addr)
{
	struct _node* current = head;
	while (current)
	{
		if (current->device.Address.ullLong == bth_addr.ullLong) return current;
		current = current->next;
	}
	return NULL;
}
int devlist_add(BLUETOOTH_DEVICE_INFO_STRUCT* device)
{
	struct _node* same_device = lookfor_same_addr(device->Address);
	if (same_device)
	{
		same_device->device = *device;		// Update information
		return 1;
	}

	struct _node* node = malloc(sizeof(struct _node));
	if (!node) return 1;
	node->device = *device;
	node->next = head;
	head = node;

	return 0;
}

int devlist_store()
{
	struct _node* current = head;

	FILE* fp = fopen(DEVLIST_FILE, "wb");
	if (!fp) return 1;

	while (current)
	{
		fwrite((void*)&current->device, sizeof(BLUETOOTH_DEVICE_INFO_STRUCT), 1, fp);
		current = current->next;
	}

	fclose(fp);
	return 0;
}
int devlist_cleanup()
{
	while (head)
	{
		struct _node* tmp = head;
		head = head->next;
		free(tmp);
	}
	return 0;
}
