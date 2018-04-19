# Ktest2

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
        //接收訊息
        int n;
        n=recv(sd, String_From_Server, MAXLINE, 0);
        String_From_Server[n]='\0';
        //斷線處理
        if(n==-1){
            Failed_Check = 1;
            closesocket(sd);
            system("@cls||clear");
            strcpy(String_From_Server,"Disconnected");
            printf("Disconnected\nPress Enter to exit\n");
            break;
        }
        //輸出來自伺服器的訊號
        char str[1024] = "start Client_displayer.exe ";
        strcat(str,String_From_Server);
        system(str);
    }
}


int main(int argc, char** argv) {
    char  		str[1024]="End";
    WSADATA 		wsadata;
    WSAStartup(0x101,(LPWSADATA) &wsadata); // 呼叫 WSAStartup() 註冊 WinSock DLL 的使用

    sd=socket(AF_INET, SOCK_STREAM, 0); //開啟一個 TCP socket.

    //為連線作準備，包含填寫 sockaddr_in 結構 (serv) 。
    //內容有：server 的 IP 位址，port number 等等。
    serv.sin_family       = AF_INET;
    serv.sin_addr.s_addr  = inet_addr("127.0.0.1");
    serv.sin_port         = htons(5678);

    /*******************************************************************/

    //等待連線
    int dot_counter = 0;
    while(connect(sd, (LPSOCKADDR) &serv, sizeof(serv))==-1){ //連線失敗則重新連線
        system("@cls||clear");
        printf("Connecting");
        if(dot_counter==0) printf(".\n");
        else if(dot_counter==1) printf("..\n");
        else if(dot_counter==2) printf("...\n");
        dot_counter++;
        if(dot_counter>2) dot_counter = 0;
    }

    //連線成功,建立接收訊息的thread
    pthread_t Receiver;
    int Receiver_i;
    Receiver_i = pthread_create(&Receiver,NULL,(void*)Receiving,NULL);
    if(Receiver_i!=0)
	{
		printf ("Create Sending error!\n");
		exit (1);
	}

	system("@cls||clear");

	//控制輸入
    while(1){
        if(Failed_Check) break;
        char Order_Type[10] = "0";
        char Order_Number[10] = "0";
        boolean GetOut = 0;

        //選擇餐點種類
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

        //選擇數量
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

        //發出訊息
        char Order[1024] = "";
        strcat(Order,Order_Type);
        strcat(Order,Order_Number);
        send(sd, Order, strlen(Order)+1, 0);
        system("@cls||clear");
        Sleep(5);
    }
   /*******************************************************************/

   closesocket(sd); //關閉TCP socket

   WSACleanup();  // 結束 WinSock DLL 的使用

   return 0;
}
