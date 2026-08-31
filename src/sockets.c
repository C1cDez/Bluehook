#include <WinSock2.h>
#include <ws2bth.h>

#pragma comment(lib, "ws2_32.lib")

#include "sockets.h"

#include <stdio.h>
#include <winerror.h>

int bthsocks_init(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 2), &wsa);
}
int bthsocks_cleanup(void)
{
	WSACleanup();
}

int bthsocks_ping(BLUETOOTH_ADDRESS_STRUCT bth_addr, int packets, int timeout)
{
	SOCKET sock = INVALID_SOCKET;
	SOCKADDR_BTH sabth = { 0 };
	sabth.addressFamily = AF_BTH;
	sabth.btAddr = bth_addr.ullLong;
	sabth.serviceClassId = SerialPortServiceClass_UUID;
	sabth.port = 0;

	int pongs = 0;
	int min = timeout, max = 0;
	int total = 0;

	char sbth_addr[BLUETOOTH_ADDRESS_STRLEN] = { 0 };
	addr2str(sbth_addr, bth_addr);
	printf("Ping [%s] with %d packets:\n", sbth_addr, packets);

	for (int i = 0; i < packets; i++)
	{
		sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (sock == INVALID_SOCKET)
		{
			printf("Reply from [%s]: Destination unreachable. (Socket error: %d)\n", 
				sbth_addr, WSAGetLastError());
			continue;
		}

		DWORD toms = (DWORD)timeout;
		setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &toms, sizeof(toms));
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &toms, sizeof(toms));

		int opt = 0;
		setsockopt(sock, SOL_SOCKET, SO_BTH_AUTHENTICATE, &opt, sizeof(opt));
		setsockopt(sock, SOL_SOCKET, SO_BTH_ENCRYPT, &opt, sizeof(opt));

		ULONGLONG start = GetTickCount64();
		int res = connect(sock, (SOCKADDR *)&sabth, sizeof(sabth));
		ULONGLONG end = GetTickCount64();

		if (res == 0 || WSAGetLastError() == WSAECONNREFUSED)
		{
			int time = (int)(end - start);
			printf("Reply from [%s]: bytes=0 time=%dms\n", sbth_addr, time);
			pongs++;
			if (time < min) min = time;
			if (time > max) max = time;
			total += time;
		}
		else printf("Request timed out / Unreachable. (Error: %d)\n", WSAGetLastError());

		closesocket(sock);

		if (i < packets - 1) Sleep(1000);
	}

	printf("\nSENT = %d, RECV = %d, LOSS = %.1f%%\n", packets, pongs,
		(double)(packets - pongs) / packets * 100.0);
	if (pongs > 0)
		printf("Time: MIN = %dms, MAX = %dms, AVG = %.1fms\n", min, max,
			(double)total / pongs);

	return 0;
}
