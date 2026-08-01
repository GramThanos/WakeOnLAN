/*
 * WakeOnLAN v0.4-beta
 * A simple C program that sends a magic packet
 *
 *
 * MIT License
 * 
 * Copyright (c) 2026 Grammatopoulos Athanasios-Vasileios
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


/* Platform-specific headers */
#if defined(__APPLE__) || defined(__linux__)
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <net/if.h>
#endif
#ifdef _WIN32
	#include <winsock2.h>
	#include <windows.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
	#ifdef _MSC_VER
		/* MSVC does not provide getopt; declared below */
	#else
		/* MinGW/Cygwin provide getopt via unistd.h */
		#include <unistd.h>
	#endif
#endif

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* Sleep function wrapper */
#ifdef _WIN32
	#define delay_ms(ms) Sleep(ms)
#else
	#define delay_ms(ms) usleep((ms) * 1000)
#endif

/* Platform-specific socket binding option */
#ifdef __APPLE__
	#define SO_BIND_OPT_NAME IP_BOUND_IF
#elif defined(__linux__)
	#define SO_BIND_OPT_NAME SO_BINDTODEVICE
#endif

/* Constants */
#define MAC_ADDR_BYTES    6
#define MAC_REPETITIONS   16
#define MAGIC_PACKET_SIZE (MAC_ADDR_BYTES + MAC_REPETITIONS * MAC_ADDR_BYTES) /* 102 bytes */
#define WOL_PORT          9
#define IPV4_ADDR_LEN     15
#define MAC_LINE_BUF_SIZE 64
#define MAC_OCTET_MAX     0xFF

/* Cross-Platform Socket Macros */
#ifdef _WIN32
	#define CLOSE_SOCKET(s) closesocket(s)
	#define PRINT_SOCKET_ERROR(msg) fprintf(stderr, msg ": %d.\n", WSAGetLastError())
#else
	#define CLOSE_SOCKET(s) close(s)
	#define PRINT_SOCKET_ERROR(msg) fprintf(stderr, msg ": '%s'.\n", strerror(errno))
#endif


/*
 * Minimal portable getopt implementation for MSVC on Windows.
 * On Unix/macOS, getopt is provided by <unistd.h>.
 * On MinGW, getopt is provided by <unistd.h> or <getopt.h>.
 */
#if defined(_WIN32) && defined(_MSC_VER)
static char *optarg = NULL;
static int optind = 1;

static int getopt(int argc, char *const argv[], const char *optstring) {
	static int optpos = 0;
	const char *p;

	if (optind >= argc || argv[optind] == NULL)
		return -1;
	if (argv[optind][0] != '-' || argv[optind][1] == '\0')
		return -1;
	if (argv[optind][0] == '-' && argv[optind][1] == '-' && argv[optind][2] == '\0') {
		optind++;
		return -1;
	}

	if (optpos == 0)
		optpos = 1;

	p = strchr(optstring, argv[optind][optpos]);
	if (p == NULL || argv[optind][optpos] == ':') {
		if (argv[optind][++optpos] == '\0') {
			optind++;
			optpos = 0;
		}
		return '?';
	}

	if (p[1] == ':') {
		if (argv[optind][optpos + 1] != '\0') {
			optarg = &argv[optind][optpos + 1];
		} else if (optind + 1 < argc) {
			optarg = argv[++optind];
		} else {
			optind++;
			optpos = 0;
			return '?';
		}
		optind++;
		optpos = 0;
	} else {
		if (argv[optind][++optpos] == '\0') {
			optind++;
			optpos = 0;
		}
		optarg = NULL;
	}

	return *p;
}
#endif


/**
 * @brief Validate that all MAC address octets are within valid range (0x00-0xFF)
 *
 * @param mac Array of MAC_ADDR_BYTES unsigned ints to validate
 * @return 1 if valid, 0 if any octet is out of range
 */
static int isValidMacAddress(const unsigned int mac[]) {
	int i;
	for (i = 0; i < MAC_ADDR_BYTES; i++) {
		if (mac[i] > MAC_OCTET_MAX) {
			return 0;
		}
	}
	return 1;
}


/**
 * @brief Create a Magic Packet object
 *
 * @param packet Where to store the created magic packet (must be at least MAGIC_PACKET_SIZE bytes).
 * @param macAddress The mac address to send the magic packet to (MAC_ADDR_BYTES unsigned ints).
 */
static void createMagicPacket(unsigned char packet[], unsigned int macAddress[]) {
	int i;

	/* MAC address as bytes */
	unsigned char mac[MAC_ADDR_BYTES];

	/* 6 x 0xFF at start of packet */
	for (i = 0; i < MAC_ADDR_BYTES; i++) {
		packet[i] = 0xFF;
		mac[i] = (unsigned char)macAddress[i];
	}

	/* Rest of the packet is MAC address repeated 16 times */
	for (i = 1; i <= MAC_REPETITIONS; i++) {
		memcpy(&packet[i * MAC_ADDR_BYTES], mac, MAC_ADDR_BYTES * sizeof(unsigned char));
	}
}


/**
 * @brief Send the magic packet
 *
 * @param packet The magic packet payload to send.
 * @param broadcastAddress The broadcast address to send the magic packet to.
 *                         If NULL, the default "255.255.255.255" will be used.
 * @param interface The interface to send the magic packet from.
 *                  NULL if it should not be used.
 *
 * @return 0 on success, 1 on failure.
 */
static int sendMagicPacket(const unsigned char *packet,
						   const char *broadcastAddress,
						   const char *iface_name) {
	/* Socket address */
	struct sockaddr_in udpClient, udpServer;
	int broadcast_flag = 1;

	memset(&udpClient, 0, sizeof(udpClient));
	memset(&udpServer, 0, sizeof(udpServer));

	/* Validate packet */
	if (!packet) {
		fprintf(stderr, "Invalid packet.\n");
		return 1;
	}

	/* Set default broadcast address if not provided */
	if (!broadcastAddress) {
		broadcastAddress = "255.255.255.255";
	} else if (strlen(broadcastAddress) > IPV4_ADDR_LEN) {
		fprintf(stderr, "Invalid broadcast address.\n");
		return 1;
	}

	{
		/* Socket Creation */
		#ifdef _WIN32
			SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (udpSocket == INVALID_SOCKET) {
				PRINT_SOCKET_ERROR("Failed to create UDP socket");
				return 1;
			}
		#else
			int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
			if (udpSocket < 0) {
				PRINT_SOCKET_ERROR("An error occurred while creating the UDP socket");
				return 1;
			}
		#endif

		/* Enable broadcast on the socket */
		if (setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (const char *)&broadcast_flag, sizeof(broadcast_flag)) < 0) {
			PRINT_SOCKET_ERROR("Failed to setup a broadcast socket");
			CLOSE_SOCKET(udpSocket);
			return 1;
		}

		/* Set parameters */
		udpClient.sin_family = AF_INET;
		udpClient.sin_addr.s_addr = INADDR_ANY;
		udpClient.sin_port = htons(0);

		/* Bind socket */
		if (bind(udpSocket, (struct sockaddr *)&udpClient, sizeof(udpClient)) < 0) {
			PRINT_SOCKET_ERROR("Failed to bind socket");
			CLOSE_SOCKET(udpSocket);
			return 1;
		}

		/* Bind to specific interface if requested */
		if (iface_name) {
			#if defined(__APPLE__) || defined(__linux__)
			struct ifreq ifr;
			memset(&ifr, 0, sizeof(ifr));
			snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", iface_name);
			if (setsockopt(udpSocket, SOL_SOCKET, SO_BIND_OPT_NAME, (void *)&ifr, sizeof(ifr)) < 0) {
				PRINT_SOCKET_ERROR("Failed to bind interface");
				CLOSE_SOCKET(udpSocket);
				return 1;
			}
			#elif defined(_WIN32)
			fprintf(stderr, "Warning: Interface binding is not supported on Windows, ignoring '%s'.\n", iface_name);
			#endif
		}

		/* Set server end point (the broadcast address) */
		udpServer.sin_family = AF_INET;
		if (inet_pton(AF_INET, broadcastAddress, &udpServer.sin_addr) != 1) {
			fprintf(stderr, "Invalid broadcast address format: '%s'.\n", broadcastAddress);
			CLOSE_SOCKET(udpSocket);
			return 1;
		}
		udpServer.sin_port = htons(WOL_PORT);

		/* Send the packet */
		if (sendto(udpSocket, (const char *)packet, MAGIC_PACKET_SIZE, 0,
				   (struct sockaddr *)&udpServer, sizeof(udpServer)) < 0) {
			PRINT_SOCKET_ERROR("Failed to send magic packet");
			CLOSE_SOCKET(udpSocket);
			return 1;
		}

		CLOSE_SOCKET(udpSocket);
	}

	return 0;
}


/**
 * @brief Read MAC addresses from a file and send magic packets directly
 *
 * @param filename Path to the file containing MAC addresses
 * @param broadcastAddress The broadcast address
 * @param iface_name The network interface to bind
 *
 * @return 0 on success, 1 on failure
 */
static int sendMagicPacketsFromFile(const char *filename, const char *broadcastAddress, const char *iface_name) {
	FILE *file;
	char line[MAC_LINE_BUF_SIZE] = {0};
	int packets_sent = 0;

	file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}

	while (fgets(line, sizeof(line), file)) {
		unsigned int mac[MAC_ADDR_BYTES];
		unsigned char packet[MAGIC_PACKET_SIZE];
		int result;

		/* Skip lines that are too short to contain a MAC address */
		if (strlen(line) < 17) {
			continue;
		}

		result = sscanf(line, "%x:%x:%x:%x:%x:%x",
						&mac[0], &mac[1], &mac[2],
						&mac[3], &mac[4], &mac[5]);
		if (result != MAC_ADDR_BYTES) {
			fprintf(stderr, "Invalid MAC address format in file: %s (parsed %d octets)\n", filename, result);
			continue;
		}

		/* Validate octet range (0x00-0xFF) */
		if (!isValidMacAddress(mac)) {
			fprintf(stderr, "MAC address octet out of range (0x00-0xFF) in file: %s\n", filename);
			continue;
		}

		createMagicPacket(packet, mac);
		if (sendMagicPacket(packet, broadcastAddress, iface_name) != 0) {
			fprintf(stderr, "Failed to send magic packet to %02x:%02x:%02x:%02x:%02x:%02x\n",
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			continue;
		}

		printf("Magic packet sent to %02x:%02x:%02x:%02x:%02x:%02x\n",
			   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		packets_sent++;

		/* Delay to prevent UDP flooding (100ms) */
		delay_ms(100);
	}

	fclose(file);

	if (packets_sent == 0) {
		fprintf(stderr, "No valid MAC addresses found or sent from file\n");
		return 1;
	}

	return 0;
}


/* Main Program */
int main(int argc, char *argv[]) {
	/* Help variables */
	int opt, exit_code = EXIT_SUCCESS;

	/* Argument variables */
	char *file_name = NULL;
	char *broadcast_arg = NULL;
	char *interface_arg = NULL;
	char *program_name;
	char *last_slash;
	
	/* Extract program name from argv[0] safely */
	program_name = (argc > 0 && argv[0] != NULL) ? argv[0] : "WakeOnLAN";
	last_slash = strrchr(program_name, '/');
	if (last_slash) program_name = last_slash + 1;
	last_slash = strrchr(program_name, '\\');
	if (last_slash) program_name = last_slash + 1;

	/* If no arguments given, print usage */
	if (argc < 2) {
		fprintf(stderr, "Usage:\n%s [<mac address> | -f macs-list.txt] [<broadcast address>]", program_name);
		#ifndef _WIN32
			fprintf(stderr, " [<interface>]");
		#endif
		fprintf(stderr, "\n");
		return EXIT_FAILURE;
	}

	/* Parse options using getopt */
	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
			case 'f':
				file_name = optarg;
				break;
			default:
				fprintf(stderr, "Unknown option.\n");
				return EXIT_FAILURE;
		}
	}

	#ifdef _WIN32
	{
		WSADATA wsaData;
		int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsaResult != 0) {
			fprintf(stderr, "WSAStartup failed with error: %d.\n", wsaResult);
			return EXIT_FAILURE;
		}
	}
	#endif

	if (file_name) {
		/* Remaining positional args after -f <file>: [broadcast] [interface] */
		if (optind < argc) {
			broadcast_arg = argv[optind];
		}
		if (optind + 1 < argc) {
			interface_arg = argv[optind + 1];
		}

		printf("Parsing file: %s. Invalid MAC addresses will be discarded.\n", file_name);
		if (sendMagicPacketsFromFile(file_name, broadcast_arg, interface_arg) != 0) {
			fprintf(stderr, "Failed to send MAC addresses from file.\n");
			exit_code = EXIT_FAILURE;
			goto cleanup;
		}
	} else {
		unsigned int mac[MAC_ADDR_BYTES];
		unsigned char packet[MAGIC_PACKET_SIZE];
		int result;

		if (optind >= argc) {
			fprintf(stderr, "No MAC address specified.\n");
			exit_code = EXIT_FAILURE;
			goto cleanup;
		}

		/* Parse Mac Address */
		result = sscanf(argv[optind], "%x:%x:%x:%x:%x:%x",
						&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		if (result != MAC_ADDR_BYTES) {
			fprintf(stderr, "Invalid mac address. Please specify a valid mac address in the format xx:xx:xx:xx:xx:xx\n");
			exit_code = EXIT_FAILURE;
			goto cleanup;
		}

		/* Validate octet range (0x00-0xFF) */
		if (!isValidMacAddress(mac)) {
			fprintf(stderr, "Invalid mac address. Each octet must be between 00 and FF.\n");
			exit_code = EXIT_FAILURE;
			goto cleanup;
		}

		/* Remaining positional args: [broadcast] [interface] */
		if (optind + 1 < argc) {
			broadcast_arg = argv[optind + 1];
		}
		if (optind + 2 < argc) {
			interface_arg = argv[optind + 2];
		}

		createMagicPacket(packet, mac);

		if (sendMagicPacket(packet, broadcast_arg, interface_arg) != 0) {
			fprintf(stderr, "Failed to send magic packet.\n");
			exit_code = EXIT_FAILURE;
			goto cleanup;
		}

		printf("Magic packet sent to %02x:%02x:%02x:%02x:%02x:%02x\n",
			   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

cleanup:
	#ifdef _WIN32
	WSACleanup();
	#endif

	return exit_code;
}
