#ifndef RMS_H
#define RMS_H

// Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Macros
#define SERVER_IP		"127.0.0.1"
#define SERVER_PORT		6500
#define MAX_CLIENTS		50
#define DATABASE		"DATABASE.txt"
#define SUCCESS			0
#define FAILURE		   -1
#define RESET			1
#define CHILD			0
#define INPUT_BUFF		100
#define OUTPUT_BUFF 	200
#define MAX_COMMANDS	100
#define STDERR			2
#define GREEN			"\033[1;32m"
#define BLUE			"\x1b[034;1m"
#define RED				"\x1b[031m"
#define NORMAL			"\x1b[0m"

// Structures
typedef struct
{
	char Command[INPUT_BUFF], Process[INPUT_BUFF], Output[OUTPUT_BUFF];
	pid_t Process_ID;
}Client_Packet;

// Function Prototypes
void Validate_Sys_Call(int Return_Val,char *Sys_Call);
void Colored_Prompt(char *Prompt,char *Color);
void Display_Help_Menu(void);
void Remote_Request_Handler(int Client_Socket);
void Parse_Command(Client_Packet *Client);
void Update_Database(char *Client_History,char *Client_IP,int Client_Port);
void Get_Process_ID(Client_Packet *Client);
void Command_Executer(char *Command,Client_Packet *Client);

#endif

