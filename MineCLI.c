#include	"unp.h"
#include <stdio.h>

#define BoldBlue "\x1b[1;34m"
#define Blue "\x1b[34m"
#define Red "\x1b[31m"
#define BoldRed "\x1b[1;31m"
#define Green "\x1b[32m"
#define BoldGreen "\x1b[1;32m"
#define Yellow "\x1b[33m"
#define BoldYellow "\x1b[1;33m"
#define White "\x1b[0m"

int Flags=10; // Number of Flags
#define Size 9
void PrintBoard(char Board[][Size])
{
	printf(" \t");
	for(int i=0;i<9;i++)
		printf(BoldGreen "%d \t",i);
	printf("\n");
	for(int i=0;i<Size;i++)
	{
		printf(BoldGreen "%d \t",i);
		for(int j=0;j<Size;j++)
		{
			printf(BoldBlue);
			if(Board[i][j]=='-')
			printf(BoldBlue "%c \t",Board[i][j]);
			else if(Board[i][j]=='!')
			printf(BoldYellow "%c \t",Board[i][j]);
			else if(Board[i][j]=='*')
			printf(BoldRed "%c \t",Board[i][j]);			
			else
			printf(BoldBlue "%c \t",Board[i][j]);
		}
		printf(White"\n");
	}
}

void RecvBoard(int sockfd, char Board[][Size])
{	
	char recvline[9];
	int n;
	for(int i=0;i<9;i++)//take row by row
	{
		n=read(sockfd,recvline,9);
		for(int j=0;j<9;j++)//copy character by character for one row
		{
			Board[i][j]=recvline[j];
		}
	}
}

void MakeMove(int sockfd,char Board[][9])
{
	int buff[3]={0};
Again:	printf("Enter 0 for button press, 1 for flag \n");
	scanf("%d",&buff[0]);
	if(buff[0]<0 || buff[0]>1)
	{	
		printf("Wrong Input\n");
		goto Again;
	}
Again1:	printf("Enter Row Index\n");
	scanf("%d",&buff[1]);
	if(buff[1]<0 || buff[1]>8)
	{	
		printf("Wrong Input\n");
		goto Again1;
	}
Again2:	printf("Enter Column Index\n");
	scanf("%d",&buff[2]);
	if(buff[2]<0 || buff[2]>8)
	{	
		printf("Wrong Input\n");
		goto Again2;
	}
	if(buff[0]==1)
	{
		if(Board[buff[1]][buff[2]]=='-')
			Flags--;
	}
	Write(sockfd,buff,sizeof(buff));
}

void StartGame(int sockfd)
{
	int n;
	int res[2]={0};	
	char welcome[MAXLINE],Board[9][9];
	n = read(sockfd, welcome, MAXLINE);
	printf("%s \n",welcome);
	while(res[0]==0){
	printf("Flags Left= %d \n",Flags);
	RecvBoard(sockfd,Board);
	PrintBoard(Board);
	MakeMove(sockfd,Board);
	n=read(sockfd,res,sizeof(res));
	if(Flags==0 && res[1]!=10)
	{	
		printf("Flags Exceeded, you lose!\n");
		return;
	}
	}
	//print if you win or lose
	RecvBoard(sockfd,Board);
	PrintBoard(Board);
	if(res[1]==10) //Did all flags so win
	printf("YOU WON!\n");
	else
	printf("YOU LOSE!\n");
}

int
main(int argc, char **argv)
{
	int sockfd, n;
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");
	StartGame(sockfd);
	
	if (n < 0)
		err_sys("read error");

	exit(0);
}
