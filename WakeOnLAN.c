/*
 * WakeOnLAN v0.3
 * A simple C program that sends a magic packet
 *
 *
 * MIT License
 * 
 * Copyright (c) 2017 Grammatopoulos Athanasios-Vasileios
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */


#ifdef __linux
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <net/if.h>
#endif
#ifdef _WIN32
	#include <winsock2.h>
	#include <windows.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#endif
#ifdef __APPLE__
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <net/if.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#ifdef __APPLE__
	#define SO_BIND_OPT_NAME IP_BOUND_IF
#elif defined(__linux)
	#define SO_BIND_OPT_NAME SO_BINDTODEVICE
#endif

#define IPV4_ADDR_LEN 15

/**
 * @brief Create a Magic Packet object
 *
 * @param packet Where to store the created magic packet.
 * @param macAddress The mac address to send the magic packet to.
 */
void createMagicPacket(unsigned char packet[], unsigned int macAddress[]){
	int i;

	// Mac Address Variable
	unsigned char mac[6];

	// 6 x 0xFF at start of packet
	for(i = 0; i < 6; i++){
		packet[i] = 0xFF;
		mac[i] = macAddress[i];
	}

	// Rest of the packet is MAC address of the pc
	for(i = 1; i <= 16; i++){
		memcpy(&packet[i * 6], &mac, 6 * sizeof(unsigned char));
	}
}

/**
 * @brief Send the magic packet
 *
 * @param packet The magic packet payload to send.
 * @param broadcastAddress The broadcast address to send the magic packet to. If NULL, the default "255.255.255.255" will be used.
 * @param interface The interface to send the magic packet from. NULL if it should not be used.
 * 
 * @return 0 on success, 1 on failure.
 */
static int sendMagicPacket(const unsigned char *packet,
						   const char *broadcastAddress,
						   const char* interface) {
	// Socket address
	struct sockaddr_in udpClient, udpServer;

	// Set broadcast
	if (!packet) {
		printf("Invalid packet address.\n");
		return 1;
	}

	if (broadcastAddress && strlen(broadcastAddress) > IPV4_ADDR_LEN) {
		printf("Invalid broadcast address.\n");
		return 1;
	} else {
		broadcastAddress = "255.255.255.255";
	}

	printf("Broadcast address %s will be used.\n", broadcastAddress);

	// MacOS and Linux
	#if defined(__APPLE__) || defined(__linux)
		int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if (udpSocket == -1) {
			printf("An error occurred while creating the UDP socket: '%s'.\n", strerror(errno));
			return 1;
		}

		// To enable the datagram socket to send broadcast packet
		int broadcast = 1;
		int setsock_result = setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
		if (setsock_result == -1) {
			printf("Failed to set socket options: '%s'.\n", strerror(errno));
			return 1;
		}

		// Set parameters
		udpClient.sin_family = AF_INET;
		udpClient.sin_addr.s_addr = INADDR_ANY;
		udpClient.sin_port = 0;

		// Bind socket
		int bind_result = bind(udpSocket, (struct sockaddr*) &udpClient, sizeof(udpClient));
		if (bind_result == -1) {
			printf("Failed to bind socket: '%s'.\n", strerror(errno));
			return 1;
		}

		if(interface){
			struct ifreq ifr;
			memset(&ifr, 0, sizeof(ifr));
			snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);
			if (setsockopt(udpSocket, SOL_SOCKET, SO_BIND_OPT_NAME, (void *)&ifr, sizeof(ifr)) < 0) {
				printf("Failed to bind interface: '%s'.\n", strerror(errno));
				return 1;
			}
		}

		// Set server end point (the broadcast addres)
		udpServer.sin_family = AF_INET;
		udpServer.sin_addr.s_addr = inet_addr(broadcastAddress);
		udpServer.sin_port = htons(9);

		// Send the packet
		int result = sendto(udpSocket, packet, sizeof(unsigned char) * 102, 0, (struct sockaddr*) &udpServer, sizeof(udpServer));
		if (result == -1) {
			printf("Failed to send magic packet to socket: '%s'.\n", strerror(errno));
			return 1;
		}
	#endif

	// Windows socket
	#ifdef _WIN32
		// Socket data
		WSADATA data;

		// Socket
		SOCKET udpSocket;

		// Setup broadcast socket
		WSAStartup(MAKEWORD(2, 2), &data);
		udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (char *) &broadcast, sizeof(broadcast)) == -1){
			printf("Failed to setup a broadcast socket.\n");
			return 1;
		}

		// Set parameters
		udpClient.sin_family = AF_INET;
		udpClient.sin_addr.s_addr = INADDR_ANY;
		udpClient.sin_port = htons(0);

		// Bind socket
		bind(udpSocket, (struct sockaddr*) &udpClient, sizeof(udpClient));

		// Set server end point (the broadcast addres)
		udpServer.sin_family = AF_INET;
		udpServer.sin_addr.s_addr = inet_addr(broadcastAddress);
		udpServer.sin_port = htons(9);

		// Send the packet
		sendto(udpSocket, (char*)&packet, sizeof(unsigned char) * 102, 0, (struct sockaddr*) &udpServer, sizeof(udpServer));
	#endif

	return 0;
}

// Main Program
int main(int argc, const char* argv[]){
	// Default broadcast address
	char broadcastAddress[IPV4_ADDR_LEN+1] = {0};

	// Packet buffer
	unsigned char packet[102];

	// Mac address
	unsigned int mac[6];

	// Help variables
	int i, error = 0;

	// If no arguments
	if(argc < 2){
		printf("Usage:\n%s <mac address> [<broadcast address>]", argv[0]);
		#ifndef _WIN32
			printf(" [<interface>]");
		#endif
		printf("\n");
		exit(EXIT_FAILURE);
	}

	// Parse Mac Address
	i = sscanf(argv[1],"%x:%x:%x:%x:%x:%x", &(mac[0]), &(mac[1]), &(mac[2]), &(mac[3]), &(mac[4]), &(mac[5]));
	if(i != 6){
		printf("Invalid mac address. Please specify a valid mac address in the format xx:xx:xx:xx:xx:xx\n");
		exit(EXIT_FAILURE);
	}

	// Check if a broadcast address was given too
	if(argc > 2){
		// Parse Broadcast Address
		i = sscanf(argv[2],"%d.%d.%d.%d", &i, &i, &i, &i);
		if(i == 4){
			strncpy(broadcastAddress, argv[2], IPV4_ADDR_LEN);
		}
	}

	// Create Magic Packet
	createMagicPacket(packet, mac);

	// Send Magic Packet
	error = sendMagicPacket(packet, NULL, argc > 3 ? argv[3] : NULL);
	if(error) {
		printf("Failed to send magic packet.\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Magic packet sent to %02x:%02x:%02x:%02x:%02x:%02x with broadcast %s\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], broadcastAddress);
	}

	exit(EXIT_SUCCESS);
}
