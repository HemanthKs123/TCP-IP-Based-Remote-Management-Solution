/*
NAME: HEMANTH KS
DATE: 22-04-2024
DESCRITPION: TCP/IP BASED REMOTE MANAGEMENT SOLUTION
*/

#include "RMS.h"

// Main Function
int main(void)
{
	// Variables & Structures Definitions / Declarations
	int Server_Socket, Connect_Ret, Send_Ret, Recv_Ret;
	struct sockaddr_in Server_Address;
	socklen_t Server_Len = sizeof(Server_Address);
	Client_Packet Client;
	char Prompt[INPUT_BUFF] = "CLIENT REQUEST\t: ";

	// Creating a TCP Socket & Validating
	Server_Socket = socket(AF_INET,SOCK_STREAM,0);
	Validate_Sys_Call(Server_Socket,"socket");

	// Storing Server's IP & Port Info
	Server_Address.sin_family      = AF_INET;
	Server_Address.sin_port        = htons(SERVER_PORT);
	Server_Address.sin_addr.s_addr = inet_addr(SERVER_IP);

	// Connecting to Server & Validating
	Connect_Ret = connect(Server_Socket,(struct sockaddr *)&Server_Address,Server_Len);
	Validate_Sys_Call(Server_Socket,"connect");

	// Displaying Help Menu for Client
	printf("INFO: Connected to the Server.\n");
	printf("\nSupported Commands for Remote Management:\n");
	Display_Help_Menu();
	printf("INFO: Send a Command to Server & Server Will Respond Back with Output.\n\n");

	// Super Loop
	while (1)
	{	
		// Clearing Command, Process & Output Strings
		bzero(Client.Command,INPUT_BUFF);
		bzero(Client.Process,INPUT_BUFF);
		bzero(Client.Output,OUTPUT_BUFF);
		
		// Displaying Prompt & Reading Command
		Colored_Prompt(Prompt,GREEN);
		scanf(" %[^\n]",Client.Command);

		// Displaying Help Menu if Client's Command is help
		if ( !strcmp(Client.Command,"help") )
		{
			Display_Help_Menu();
			continue;
		}

		// Exiting if Client's Command is exit
		else if ( !strcmp(Client.Command,"exit") )
		{
			printf("\nINFO: Exited!\n");
			exit(EXIT_SUCCESS);
		}

		// Calling Parse_Command Function to Parse Client's Command
		Parse_Command(&Client);

		// Sending Client's Packet to Server & Validating
		Send_Ret = send(Server_Socket,&Client,sizeof(Client),0);
		Validate_Sys_Call(Send_Ret,"send");

		// Recieving Client's Packet Back From Server & Validating
		Recv_Ret = recv(Server_Socket,&Client,sizeof(Client),0);
		Validate_Sys_Call(Recv_Ret,"recv");

		Colored_Prompt("SERVER RESPONSE\t: ",GREEN);

		// Printing a New Line for get-open-fd & history commands
		if ( !strcmp(Client.Command,"get-open-fd") || !strcmp(Client.Command,"history") )
		printf("\n");

		// Displaying Command Output
		printf("%s",Client.Output);
	}
	
	// Closing Server_Socket
	close(Server_Socket);
	return SUCCESS;
}

