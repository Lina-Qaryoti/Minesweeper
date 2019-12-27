#include	"unp.h"
#include	<time.h>

#define Size 9
//check if player first move was mine so restart
//validate user input at client side

void Init_BoardSrv();
void MakeMove();
int End=0; //If Game over, 
int Flags=10; // Must be initialised in Client side, print flag status when decide to use flag
int CorrectFlags=0; //number of flag = mine, must be 10 if won game
int UserRow=0, UserColumn=0, type=0; //index sent by the user
//type=0 for kabseh, type=1 for flag

void PrintBoard(char Board[][Size])
{
	printf(" \t");
	for(int i=0;i<9;i++)
		printf("%d \t",i);
	printf("\n");
	for(int i=0;i<Size;i++)
	{
		printf("%d \t",i);
		for(int j=0;j<Size;j++)
		{
			printf("%c \t",Board[i][j]);
		}
		printf("\n");
	}
}

void MakeMove(int connfd)
{
	int buff[3];
	printf("Reading from client\n");
	int n=read(connfd,buff,sizeof(buff));
	type=buff[0];
	UserRow=buff[1];
	UserColumn=buff[2];
	printf("user move %d %d %d\n",type, UserRow, UserColumn);	
}

void SendBoard(char Board[][Size],int connfd)
{
	char buff[9];
	for(int i=0;i<9;i++)//Go row by row
	{	
		for(int j=0;j<9;j++)//copy character by character to each row
		{
			buff[j]=Board[i][j];
		}
		Write(connfd, buff, Size);
	}	
}

int MineCheck(int R, int C, char BoardSrv[][Size])
{
	if(BoardSrv[R][C]=='*')	
	{
		return 1; //1 if mine
	}
	return 0;	
}

int Validate(int R,int C)
{
	if(R<9 && R>=0)
		if(C<9 && C>=0)
			return 1; //return Valid=1 if valid
	return 0; //return 0 otherwise
}

int Count(int Mines[][2], char BoardSrv[][Size],int R,int C)
{
	int c=0;
	if(MineCheck(R,C,BoardSrv)==1)
		return;
	if(Validate(R-1,C)==1) //Check upper cell
	{
		if(MineCheck(R-1,C,BoardSrv)==1)
			c++;
	}
	if(Validate(R+1,C)==1) //Check lower cell
	{
		if(MineCheck(R+1,C,BoardSrv)==1)
			c++;
	}
	if(Validate(R,C+1)==1) //Check right cell
	{
		if(MineCheck(R,C+1,BoardSrv)==1)
			c++;
	}
	if(Validate(R,C-1)==1) //Check left cell
	{
		if(MineCheck(R,C-1,BoardSrv)==1)
			c++;
	}
	if(Validate(R-1,C+1)==1) //Check upper right cell
	{
		if(MineCheck(R-1,C+1,BoardSrv)==1)
			c++;
	}
	if(Validate(R-1,C-1)==1) //Check upper left cell
	{
		if(MineCheck(R-1,C-1,BoardSrv)==1)
			c++;
	}
	if(Validate(R+1,C+1)==1) //Check lower right cell
	{
		if(MineCheck(R+1,C+1,BoardSrv)==1)
			c++;
	}
	if(Validate(R+1,C-1)==1) //Check lower left cell
	{
		if(MineCheck(R+1,C-1,BoardSrv)==1)
			c++;
	}
	return c;
}

void Play(int connfd,char BoardSrv[][Size],char BoardCli[][Size],int Mines[][2],int R,int C)
{


	//calculate numbers
		if(BoardCli[R][C]=='-')
		{
		int c=Count(Mines,BoardSrv,R,C);
		if (c==0){
		BoardCli[R][C]=' ';}
		else
		BoardCli[R][C]=c+'0';
		if(c==0)
		{
	
	if(Validate(R-1,C)==1) //Check upper cell
	{
		if(MineCheck(R-1,C,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R-1,C);
	}
	if(Validate(R+1,C)==1) //Check lower cell
	{
		if(MineCheck(R+1,C,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R+1,C);
	}
	if(Validate(R,C+1)==1) //Check right cell
	{
		if(MineCheck(R,C+1,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R,C+1);
	}
	if(Validate(R,C-1)==1) //Check left cell
	{
		if(MineCheck(R,C-1,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R,C-1);
	}
	if(Validate(R-1,C+1)==1) //Check upper right cell
	{
		if(MineCheck(R-1,C+1,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R-1,C+1);
	}
	if(Validate(R-1,C-1)==1) //Check upper left cell
	{
		if(MineCheck(R-1,C-1,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R-1,C-1);
	}
	if(Validate(R+1,C+1)==1) //Check lower right cell
	{
		if(MineCheck(R+1,C+1,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R+1,C+1);
	}
	if(Validate(R+1,C-1)==1) //Check lower left cell
	{
		if(MineCheck(R+1,C-1,BoardSrv)==0)
			Play(connfd,BoardSrv,BoardCli,Mines,R+1,C-1);
	}
		}}
		else return;
}

void EndGame(int connfd,char BoardCli[][Size])
{
	SendBoard(BoardCli,connfd);
}

void StartGame(int connfd)
{
	char BoardSrv[Size][Size];
	char BoardCli[Size][Size];
	int result[2]={0};
	int Mines[10][2]; //we have 10 mines (rows) and each row has X and Y coordinate	
	Init_Board(BoardSrv,BoardCli,Mines); //initialize the real board (server board)
	PrintBoard(BoardSrv);
	while(1){
	SendBoard(BoardCli,connfd);
	MakeMove(connfd);
	int check=MineCheck(UserRow,UserColumn,BoardSrv); 
	if(type==0 && check==1) //pressed button and mine
	{
		for(int i=0;i<10;i++)
		BoardCli[Mines[i][0]][Mines[i][1]]='*';		
		End=1;
		result[0]=End;
		result[1]=CorrectFlags;
		Write(connfd,result,sizeof(result));
		EndGame(connfd,BoardCli);	
		printf("User Lost \n");
		Close(connfd);
	}
	if(type==1) // pressed flag 
	{	
		
		BoardCli[UserRow][UserColumn]='!';
		Flags--;
		if(check==1) //if flag=mine
		{
			CorrectFlags ++;
			if(CorrectFlags==10)
			{
				End=1;
				result[0]=End;
				result[1]=CorrectFlags;
				Write(connfd,result,sizeof(result));	
				EndGame(connfd,BoardCli);
				printf("User Won!");	
				Close(connfd);		
			}
		}
		End=0;
		result[0]=End;
		result[1]=CorrectFlags;
		Write(connfd,result,sizeof(result));
		
	}	
	if(type==0) //pressed button
	{
		End=0;
		result[0]=End;
		result[1]=CorrectFlags;
		Write(connfd,result,sizeof(result));		
		Play(connfd,BoardSrv,BoardCli,Mines,UserRow,UserColumn);	
	}}
}

void Init_Board(char BoardSrv[][Size],char BoardCli[][Size],int Mines[][2])
{
	int flag[81]={0};
	srand(time (NULL)); //random function, seed is time
	for(int i=0;i<9;i++)
	{
		for(int j=0;j<9;j++)
		{
			BoardCli[i][j]='-';
			BoardSrv[i][j]='-';
		}
	}
	for(int i=0;i<10;) //mine placement
	{
		int temp=rand()%81; //9*9, 81 boxes
		int x=temp/9;
		int y=temp%9;
		if(flag[temp]==0)
		{
			flag[temp]=1;
			Mines[i][0]=x; //save Mine coordinates (x,y) in Mines
			Mines[i][1]=y;
			BoardSrv[x][y]='*';
			i++;
		}
	}
}

int
main(int argc, char **argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buff[]="Welcome to Minesweeper\n";

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	/* daytime server */

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	connfd = Accept(listenfd, (SA *) NULL, NULL);
	for ( ; ; ) 
	{
        Write(connfd, buff, strlen(buff));
	StartGame(connfd);
	Close(connfd);
	}
}
