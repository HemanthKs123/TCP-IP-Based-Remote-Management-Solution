#include "RMS.h"

char Write_Once;

// Validate_Sys_Call Function Definition
void Validate_Sys_Call(int Return_Val,char *Sys_Call)
{
	// Printing an Error & Exiting if Return_Val is FAILURE
	if ( Return_Val == FAILURE )
	{
		perror(Sys_Call);
		exit(EXIT_FAILURE);
	}

	return;
}

// Colored_Prompt Function Defintion
void Colored_Prompt(char *Prompt,char *Color)
{
	// Displaying Colored Prompt
	printf("%s",Color);
    printf("%s",Prompt);
    printf("%s",NORMAL);

	return;
}

// Display_Help_Menu Function Definition
void Display_Help_Menu(void)
{
	// Displaying a Help Menu to Client
	printf("\n----------------------------------------------------------------------------------------\n");
    printf("COMMAND                          DESCRIPTION\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("get-mem        <process-name>  : Gets the Memory Usage of a Given Process\n");
    printf("get-cpu-usage  <process-name>  : Gets the CPU Usage of a Given Process\n");
    printf("get-ports-used <process-name>  : Gets the Socket Ports Used by a Given Process\n");
    printf("get-open-fd    <process-name>  : Gets the Open File Descriptors of a Given Process\n");
    printf("kill           <process-name>  : Kills the Given Process\n");
    printf("history                        : Shows Commands History\n");
    printf("exit                           : Exits from Current Application\n");
    printf("help                           : Displays this Menu\n");
    printf("----------------------------------------------------------------------------------------\n\n");

	return;
}

// Parse_Command Function Defintion
void Parse_Command(Client_Packet *Client)
{
	// Getting Command Length
	int Command_Len = strlen(Client -> Command);

	// Running a Loop till Command_Len
	for ( int Ind = 0; Ind < Command_Len; Ind++ )
	{
		// Extracting Process Name & Storing When Space is Found
		if ( Client -> Command[Ind] == ' ' )
		{
			Client -> Command[Ind] = '\0';
			strcpy(Client -> Process,&Client -> Command[Ind + 1]);
			return;
		}
	}

	return;
}

// Update_Database Function Defintion
void Update_Database(char *Client_History,char *Client_IP,int Client_Port)
{
	int Written_Bytes;

	// Opening DATABASE in Write Mode & Validating
	int FD = open(DATABASE,O_WRONLY | O_CREAT | O_APPEND,S_IRUSR | S_IWUSR);
	Validate_Sys_Call(FD,"open");
	
	// Validating Write_Once
	if ( !Write_Once )
	{
		Write_Once = RESET;
		char Client_Info[INPUT_BUFF];

		// Updating Client's IP & Port & Writing to File
		sprintf(Client_Info,"Commands History of Client with IP & Port -> %s & %d: \n",Client_IP,Client_Port);
		Written_Bytes = write(FD,Client_Info,strlen(Client_Info));
	}
	
	// Writing Client History to DATABASE & Validating
	Written_Bytes = write(FD,Client_History,strlen(Client_History));
	Validate_Sys_Call(Written_Bytes,"write");	

	// Closing DATABASE
	close(FD);
	return;
}

// Get_Process_ID Function Definition
void Get_Process_ID(Client_Packet *Client)
{

	// Closing STDERR to Avoid Printing Errors on Server Side
    // Will Redirect Errors to Client
	close(STDERR);
	
	// Using pgrep Command to Fetch Process ID from Given Process Name
	char Buffer[INPUT_BUFF] = "pgrep ";
	strcpy(&Buffer[6],Client -> Process);

	// Executing Command by Using popen
	FILE *Pipe_File = popen(Buffer,"r");

	// Reading the Process ID From Pipe_File & Storing
	fgets(Buffer,INPUT_BUFF,Pipe_File);
	Client -> Process_ID = atoi(Buffer);

	// Closing Pipe_File
	pclose(Pipe_File);
	return;
}

// Command_Executer Function Definition
void Command_Executer(char *Command,Client_Packet *Client)
{
	// Closing STDERR to Avoid Printing Errors on Server Side
    // Will Redirect Errors to Client
	close(STDERR);

	// Clearing Output String
	bzero(Client -> Output,OUTPUT_BUFF);

	// Executing the Command by using popen
	FILE *Pipe_File = popen(Command,"r");

	// Reading, Storing & Validating Command Output from Pipe_File
	int Read_Bytes = fread(Client -> Output,OUTPUT_BUFF,sizeof(Client -> Output),Pipe_File);
	Validate_Sys_Call(Read_Bytes,"fread");

	// Closing Pipe_File
	pclose(Pipe_File);
	return;
}

