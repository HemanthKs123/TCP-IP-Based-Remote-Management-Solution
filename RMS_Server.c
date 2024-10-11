/*
NAME: HEMANTH KS
DATE: 22-04-2024
DESCRITPION: TCP/IP BASED REMOTE MANAGEMENT SOLUTION
*/

#include "RMS.h"

// Global Variables & Strings
char Client_IP[INPUT_BUFF];
int Client_Port;
Client_Packet Client;
char Commands_Database[MAX_COMMANDS][INPUT_BUFF]; 

// Main Function
int main(void)
{
	// Variables & Structure Defintions / Declarations
	int Server_Socket, Client_Socket, Bind_Ret, Listen_Ret;
	struct sockaddr_in Server_Address, Client_Address;
	socklen_t Server_Length = sizeof(Server_Address), Client_Length = sizeof(Client_Address);
	char Prompt[INPUT_BUFF] = "RMS SERVER: ";

	// Creating a TCP Socket & Validating
	Server_Socket = socket(AF_INET,SOCK_STREAM,0);
	Validate_Sys_Call(Server_Socket,"socket");

	// Binding Server IP & Port & Validating
	Server_Address.sin_family 	   = AF_INET;
	Server_Address.sin_port 	   = htons(SERVER_PORT);
	Server_Address.sin_addr.s_addr = inet_addr(SERVER_IP);
	Bind_Ret = bind(Server_Socket,(struct sockaddr *)&Server_Address,Server_Length);
	Validate_Sys_Call(Bind_Ret,"bind");


	//Displaying Server's IP & Port
	Colored_Prompt(Prompt,BLUE);
	printf("Server IP & Port -> %s:%d.\n",SERVER_IP,SERVER_PORT);

	// Fixing Client Queue & Waiting / Listening & Validating
	Listen_Ret = listen(Server_Socket,MAX_CLIENTS);
	Validate_Sys_Call(Listen_Ret,"listen");

	// Super Loop
	while (1)
	{
		// Accepting Client Request & Validating
		Client_Socket = accept(Server_Socket,(struct sockaddr *)&Client_Address,&Client_Length);
		Validate_Sys_Call(Client_Length,"accept");

		// Getting Client IP & Port
		strcpy(Client_IP,inet_ntoa(Client_Address.sin_addr));
		Client_Port = ntohs(Client_Address.sin_port);

		// Displaying Client's IP & Port
		Colored_Prompt(Prompt,BLUE);
		printf("Incoming Remote Request from Client with IP & Port -> %s:%d.\n",Client_IP,Client_Port);

		// Creating a Process to Enable Concurrency
		pid_t PID = fork();

		// Switch Case for PID
		switch ( PID )
		{
			// Parent Process
			default:

			// Closing Client Socket & Waiting for Child to Complete Execution
			close(Client_Socket);
			waitpid(PID,NULL,WNOHANG);

			break;

			// Child Process
			case CHILD:

			// Closing Server Socket & Calling Remote_Request_Handler Function
			close(Server_Socket);
			Remote_Request_Handler(Client_Socket);
			exit(EXIT_SUCCESS);

			// Fork Failure
			case FAILURE:

			perror("fork");
			exit(EXIT_FAILURE);
		}

	}

	// Closing Server_Socket
	close(Server_Socket);
	return SUCCESS;
}

// Remote_Request_Handler Function Definition
void Remote_Request_Handler(int Client_Socket)
{
	// Variables & String Definitions
	int Num_Commands = 0, Send_Ret, Recv_Ret;
	char Commands_History[INPUT_BUFF];
	char Executable[INPUT_BUFF];

	// Clearing Commands_History & Executable Strings
	bzero(Commands_History,INPUT_BUFF);
	bzero(Executable,INPUT_BUFF);

	// Super Loop
	while (1)
	{
		// Receiving Client's Packet & Validating
		Recv_Ret = recv(Client_Socket,&Client,sizeof(Client),0);
		Validate_Sys_Call(Recv_Ret,"recv");

		// Displaying a Message when Client has Disconnected
		if ( !Recv_Ret )
		{
			Colored_Prompt("RMS SERVER: ",BLUE);
			printf("Client with IP & Port -> %s:%d has been Disconnected.\n",Client_IP,Client_Port);
			return;
		}

		// Storing Command & Process in a String & Updating the Database
		snprintf(Commands_History,sizeof(Client),"%d. %s %s\n",Num_Commands + 1,Client.Command,Client.Process);
		Update_Database(Commands_History,Client_IP,Client_Port);
		
		// Updating Command's History to a Database & Incrementing Num_Commands
		strcpy(Commands_Database[Num_Commands],Commands_History);
		Num_Commands++;

		// Calling Get_Process_ID Function to Get the Process ID
		Get_Process_ID(&Client);

		/* Comparing Client Command's & Assigning Executable Command depending on Client's Command & Executing, Storing Command Output */

		if ( !strcmp(Client.Command,"get-mem") )
		{
			// Using pmap Command to Fetch Memory Usage
			sprintf(Executable,"pmap -x %d | tail -1 | awk '{print $3}'",Client.Process_ID);
			Command_Executer(Executable,&Client);
		}

		else if ( !strcmp(Client.Command,"get-cpu-usage") )
		{
			// Using ps Command to Fetch CPU Usage
			sprintf(Executable,"ps -p %d -o %%cpu | tail -1",Client.Process_ID);
			Command_Executer(Executable,&Client);
		}

		else if ( !strcmp(Client.Command,"get-ports-used") )
		{
			// Using lsof Command to Fetch Socket Ports Used
			sprintf(Executable,"lsof -i -P | grep %d | awk '{print $8,$9}'",Client.Process_ID);
			Command_Executer(Executable,&Client);
		}

		else if ( !strcmp(Client.Command,"get-open-fd") )
		{
			// Using lsof Command to Fetch Open FDs
			sprintf(Executable,"lsof -p %d | awk '{print $4}'",Client.Process_ID);
			Command_Executer(Executable,&Client);
		}

		else if ( !strcmp(Client.Command,"kill") )
		{
			// Using kill System Call to Kill
			int Kill_Ret = kill(Client.Process_ID,SIGKILL);
			Validate_Sys_Call(Kill_Ret,"kill");
			snprintf(Client.Output,sizeof(Client),"Process %s has been Killed\n",Client.Process);
		}

		else if ( !strcmp(Client.Command,"history") )
		{
			// Storing Command's Database in Output
			for ( int Ind = 0; Ind < MAX_COMMANDS; Ind++ )
			strcpy(&Client.Output[ strlen(Client.Output) ],Commands_Database[Ind]);
		}

		// Invalid Command
		else 
		strcpy(Client.Output,"Invalid Command\n");

		// Invalid Process
		if ( !strlen(Client.Output) )
		strcpy(Client.Output,"Invalid Process\n");

		// Sending Client's Packet Back & Validating
		Send_Ret = send(Client_Socket,&Client,sizeof(Client),0);
		Validate_Sys_Call(Send_Ret,"send");
	}

	// Closing Client_Socket
	close(Client_Socket);
	return;
}

