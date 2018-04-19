#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <time.h>
#include <pthread.h>
#include <conio.h>
#include <windows.h>

#define MAXLINE 1024

SOCKET        	sd;
struct sockaddr_in serv;

boolean Failed_Check = 0;
char String_From_Server[1024] = "";

void int2str(int i, char *s) {
    sprintf(s,"%d",i);
}
void gotoxy(int xpos, int ypos){
  COORD scrn;
  HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
  scrn.X = xpos; scrn.Y = ypos;
  SetConsoleCursorPosition(hOuput,scrn);
}
void SetColor(int f,int b){
    unsigned short ForeColor=f+16*b;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon,ForeColor);
}

void Receiving(void){
    while(1){
        //�����T��
        int n;
        n=recv(sd, String_From_Server, MAXLINE, 0);
        String_From_Server[n]='\0';
        //�_�u�B�z
        if(n==-1){
            Failed_Check = 1;
            closesocket(sd);
            system("@cls||clear");
            strcpy(String_From_Server,"Disconnected");
            printf("Disconnected\nPress Enter to exit\n");
            break;
        }
        //��X�Ӧۦ��A�����T��
        char str[1024] = "start Client_displayer.exe ";
        strcat(str,String_From_Server);
        system(str);
    }
}


int main(int argc, char** argv) {
    char  		str[1024]="End";
    WSADATA 		wsadata;
    WSAStartup(0x101,(LPWSADATA) &wsadata); // �I�s WSAStartup() ���U WinSock DLL ���ϥ�

    sd=socket(AF_INET, SOCK_STREAM, 0); //�}�Ҥ@�� TCP socket.

    //���s�u�@�ǳơA�]�t��g sockaddr_in ���c (serv) �C
    //���e���Gserver �� IP ��}�Aport number �����C
    serv.sin_family       = AF_INET;
    serv.sin_addr.s_addr  = inet_addr("127.0.0.1");
    serv.sin_port         = htons(5678);

    /*******************************************************************/

    //���ݳs�u
    int dot_counter = 0;
    while(connect(sd, (LPSOCKADDR) &serv, sizeof(serv))==-1){ //�s�u���ѫh���s�s�u
        system("@cls||clear");
        printf("Connecting");
        if(dot_counter==0) printf(".\n");
        else if(dot_counter==1) printf("..\n");
        else if(dot_counter==2) printf("...\n");
        dot_counter++;
        if(dot_counter>2) dot_counter = 0;
    }

    //�s�u���\,�إ߱����T����thread
    pthread_t Receiver;
    int Receiver_i;
    Receiver_i = pthread_create(&Receiver,NULL,(void*)Receiving,NULL);
    if(Receiver_i!=0)
	{
		printf ("Create Sending error!\n");
		exit (1);
	}

	system("@cls||clear");

	//�����J
    while(1){
        if(Failed_Check) break;
        char Order_Type[10] = "0";
        char Order_Number[10] = "0";
        boolean GetOut = 0;

        //����\�I����
        while(1){
            printf("What do you want?\nEnter A for chicken\nEnter B for fries\nEnter C to quit\n");
            printf("\33[2K\r");
            char Type[20];
            gets(Type);
            if(Failed_Check) return 0;

            if(strcmp(Type,"A")==0||strcmp(Type,"a")==0){
                strcpy(Order_Type,"c");
                break;
            }
            else if(strcmp(Type,"B")==0||strcmp(Type,"b")==0){
                strcpy(Order_Type,"f");
                break;
            }
            else if(strcmp(Type,"C")==0||strcmp(Type,"c")==0){
                system("@cls||clear");
                GetOut = 1;
                break;
            }
            else{
                gotoxy(0,8);
                SetColor(4,6);
                printf("   Wrong format   \n");
                gotoxy(0,0);
                SetColor(7,0);
            }
        }
        system("@cls||clear");
        if(GetOut) break;

        //��ܼƶq
        while(1){
            if(strcmp(Order_Type,"c")==0) printf("How many chickens do you want?\n");
            else if(strcmp(Order_Type,"f")==0) printf("How many fries do you want?\n");
            printf("Enter the number :\n");
            printf("\33[2K\r");
            char Number[10];
            gets(Number);
            if(Failed_Check) return 0;

            int Number_temp = atoi ( Number );

            if(Number_temp > 0 && Number_temp < 10000){
                int2str(Number_temp,Order_Number);
                break;
            }
            else{
                gotoxy(0,6);
                SetColor(4,6);
                printf("         Wrong format         \n");
                gotoxy(0,0);
                SetColor(7,0);
            }
        }

        //�o�X�T��
        char Order[1024] = "";
        strcat(Order,Order_Type);
        strcat(Order,Order_Number);
        send(sd, Order, strlen(Order)+1, 0);
        system("@cls||clear");
        Sleep(5);
    }
   /*******************************************************************/

   closesocket(sd); //����TCP socket

   WSACleanup();  // ���� WinSock DLL ���ϥ�

   return 0;
}
