#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")

#define MAX_NAME_LEN 4096
#define MAX_SUB_COUNT 4096
#define MAX_SUB_LEN 1024

void main(void) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;
	bool oO = true;
	FILE *subsf, *out;
	for (; ; ) {
		errno = NULL;
		subsf = fopen("subdomains", "r");
		if (subsf == NULL) {
			DWORD err = errno;
			printf("%s\n\nPress any key to exit.\n", strerror(err));
			_getch();
			return;
		}
		errno = NULL;
		out = fopen("output", "a");
		if (out == NULL) {
			DWORD err = errno;
			printf("%s\n\nPress any key to exit.\n", strerror(err));
			fclose(subsf);
			_getch();
			return;
		}
		struct hostent *remoteHost[1024];
		struct hostent *TempHost, *TempHost2;
		struct in_addr addr = { NULL };
		char name[MAX_NAME_LEN + 1] = { '\0' }, host_name[MAX_SUB_LEN + MAX_NAME_LEN + 1] = { '\0' };
		char **sub = (char **)malloc(MAX_SUB_COUNT * sizeof(char *));
		if (sub == NULL) {
			fclose(subsf);
			fclose(out);
			printf("Memory allocation failure!\n\nPress any key to exit.\n");
			_getch();
			return;
		}
		for (int i = 0; i < MAX_SUB_COUNT; ++i) {
			sub[i] = (char *)malloc((MAX_SUB_LEN + 1) * sizeof(char));
			if (sub[i] == NULL) {
				fclose(subsf);
				fclose(out);
				for (int j = i; i >= 0; --i) free(sub[j]);
				free(sub);
				printf("Memory allocation failure!\n\nPress any key to exit.\n");
				_getch();
				return;
			}
			memset(sub[i], '\0', (MAX_SUB_LEN + 1));
		}
		for (int i = 1; i < MAX_SUB_COUNT && fgets(sub[i], MAX_SUB_LEN, subsf) != NULL; ++i) sub[i][strlen(sub[i]) - 1] = '\0';
		fclose(subsf);
		printf(">> ");
		fflush(stdout);
		fgets(name, MAX_NAME_LEN, stdin);
		name[strlen(name) - 1] = '\0';
		for (; name[0] != '\0' && isspace(name[0]) != 0; ) for (unsigned int f = 0; f < strlen(name); ++f) name[f] = name[f + 1];
		if (name[0] == '\0') return;
		fprintf(out, "%s :\n", name);
		for (int i = 0; i < MAX_SUB_COUNT; ++i) {
			if (i > 0) {
				if (sub[i][0] == '\0') break;
				sprintf(host_name, "%s%s", sub[i], name);
			}
			else sprintf(host_name, "%s", name);
			if (i > 0)
				TempHost2 = remoteHost[i-1];
			TempHost = gethostbyname(host_name);
			remoteHost[i] = TempHost;
			if (i == 0 && remoteHost[i] == NULL) {
				DWORD err = WSAGetLastError();
				char buff[1024 + 1] = { '\0' };
				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, buff, 1024, NULL);
				printf("%s\n", buff);
				break;
			}
			if (remoteHost[i] != NULL) {
				for (int j = 0; remoteHost[i]->h_addr_list[j] != NULL; ++j) {
					memset(&addr, NULL, sizeof(addr));
					addr.S_un.S_addr = *(ULONG *)remoteHost[i]->h_addr_list[j];
					printf("%s : %s", remoteHost[i]->h_name, inet_ntoa(addr));
					fflush(stdout);
					if (i > 0) {
						for (int f = 0; remoteHost[0]->h_addr_list[f] != NULL; ++f) {
							size_t pos1 = 0, pos2 = 0;
							struct in_addr temp_addr1, temp_addr2;
							temp_addr1.S_un.S_addr = *(ULONG *)remoteHost[0]->h_addr_list[f];
							char *buff1 = inet_ntoa(temp_addr1);
							bool fdd = false;
							for (unsigned int c = strlen(buff1); c >= 0; --c) {
								if (buff1[c] == '.') {
									if (!fdd) {
										fdd = true;
										continue;
									}
									else {
										fdd = false;
										pos1 = c;
										break;
									}
								}
							}
							temp_addr2.S_un.S_addr = *(ULONG *)remoteHost[i]->h_addr_list[j];
							char *buff2 = inet_ntoa(temp_addr2);
							fdd = false;
							for (unsigned int c = strlen(buff2); c >= 0; --c) {
								if (buff2[c] == '.') {
									if (!fdd) {
										fdd = true;
										continue;
									}
									else {
										fdd = false;
										pos2 = c;
										break;
									}
								}
							}
							if (pos1 == pos2 && strncmp(buff1, buff2, pos1) == 0) {
								oO = false;
								break;
							}
						}
						if (oO) printf("\toO!\n");
						else {
							printf("\n");
							oO = true;
						}
					}
					else printf("\n");
					fprintf(out, "%s%s\n", remoteHost[i]->h_name, inet_ntoa(addr));
				}
			}
		}
		fprintf(out, "--------------------------------------------------------\n");
		for (int i = 0; i < MAX_SUB_COUNT; ++i) free(sub[i]);
		free(sub);
		fclose(out);
		printf("\nDone.\n\n");
		printf("Press Enter to continue || Scape to exit.\r");
		for (bool restart = false; !restart; ) {
			switch (_getch()) {
			case 0x0D: {
				printf("                                           \r");
				restart = true;
				break;
			}
			case 0x1B: return;
			}
		}
	}
}
