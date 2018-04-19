#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <time.h>
#include <pthread.h>
#include <conio.h>

#define MAXLINE 1024    /* 字串緩衝區長度 */
#define MAXCLIENT 10    /*最多幾個客戶*/

//連線變數
SOCKET	serv_sd, cli_sd[10];        /* socket 描述子 */
struct 	sockaddr_in   	serv, cli;
int   	cli_len;
//執行緒
pthread_t Connecter[10];
int Connecter_i[10];
boolean ThreadAlive = 1;
//計算變數
boolean SockAvailable[10];
int Client_Chicken[10];
int Client_Fries[10];
boolean ListenerClosed = 0;

void int2str(int i, char *s) {
    sprintf(s,"%d",i);
}

int FindAvailableSocket(void){ /*尋找可用的socket*/
    int ii = 0;
    for(ii=0;ii<MAXCLIENT;ii++){
        if(SockAvailable[ii]==1){
            return ii;
        }
    }
    return -1;
}

void HandleConnection(void *data){ /*處理對單一客戶的連線*/
    int Client_i = (int)data;
    Display();

    while(ThreadAlive){
        //接收訊息
        char String_From_Client[1024] = "";
        int n=recv(cli_sd[Client_i], String_From_Client, MAXLINE, 0);
        String_From_Client[n]='\0';

        //處理訊息
        if(String_From_Client[0]=='c'){//計算雞排
            int ii = 1,sum = 0;
            while(String_From_Client[ii]>='0'&&String_From_Client[ii]<='9'){
                sum *=10;
                sum += String_From_Client[ii]-48;
                ii++;
            }
            Client_Chicken[Client_i]+=sum;
        }
        else    if(String_From_Client[0]=='f'){//計算薯條
            int ii = 1,sum = 0;
            while(String_From_Client[ii]>='0'&&String_From_Client[ii]<='9'){
                sum *=10;
                sum += String_From_Client[ii]-48;
                ii++;
            }
            Client_Fries[Client_i]+=sum;
        }
        Display();

        //回傳訊息
        char Reply[1024]="You ordered ";
        char Food_Number_str[10];
        int2str(Client_Chicken[Client_i],Food_Number_str);
        strcat(Reply,Food_Number_str);
        strcat(Reply," chickens and ");
        int2str(Client_Fries[Client_i],Food_Number_str);
        strcat(Reply,Food_Number_str);
        strcat(Reply," fries ");
        time_t now;
        time(&now);
        strcat(Reply,ctime(&now));
        //確認連線狀態
        int Connect_Check = send(cli_sd[Client_i], Reply, strlen(Reply), 0);
        if(Connect_Check == -1){//斷線
            SockAvailable[Client_i] = 1;
            Client_Chicken[Client_i] = 0;
            Client_Fries[Client_i] = 0;
            Display();
            break;
        }
    }
}

void Listening(void){ /*重複接聽*/
    while(ThreadAlive){
        //檢查可用Sock
        int ClientToConnect = FindAvailableSocket();
        if(ClientToConnect == -1){//連線已滿,關閉serv_sd
            closesocket(serv_sd);
            ListenerClosed = 1;
            continue;
        }
        if(ListenerClosed==1){//有可用連線,重開serv_sd
            serv_sd = socket(AF_INET, SOCK_STREAM, 0);
            bind(serv_sd, (LPSOCKADDR) &serv, sizeof(serv));
            listen(serv_sd, 1);
            ListenerClosed = 0;
        }

        //有可用sock,接聽
        int cli_sd_temp;
        cli_sd_temp=accept(serv_sd, (LPSOCKADDR) &cli, &cli_len);
        ClientToConnect = FindAvailableSocket();
        cli_sd[ClientToConnect] = cli_sd_temp;
        SockAvailable[ClientToConnect] = 0;

        //開啟新執行續處理單一客戶
        Connecter_i[ClientToConnect] = pthread_create(&Connecter[ClientToConnect],NULL,(void*)HandleConnection,(void *) ClientToConnect);
        if(Connecter_i[ClientToConnect]!=0)
        {
            printf ("Create HandleConnection error!\n");
            exit (1);
        }
    }
}

void Display(void){ /*輸出連線狀況至螢幕*/
    if(!ThreadAlive) return;
    system("@cls||clear");

    int ii = 0;
    int Full_Check = 0;
    for(ii=0;ii<MAXCLIENT;ii++){
        if(SockAvailable[ii] == 1){
            printf("C%d Disconnected\n\n",ii);
        }
        else{
            printf("Client%d   Ordered\n          Chicken : %d - Fries : %d \n",ii,Client_Chicken[ii],Client_Fries[ii]);
            Full_Check++;
        }
    }
    if(Full_Check == 10){
        printf("\nFully connected!\n");
    }
}

int main()
{
  	WSADATA wsadata;
    WSAStartup(0x101, &wsadata); //呼叫 WSAStartup() 註冊 WinSock DLL 的使用
  	serv_sd=socket(AF_INET, SOCK_STREAM, 0);// 開啟 TCP socket
   	//指定 socket 的 IP 位址和 port number
   	serv.sin_family      = AF_INET;
   	serv.sin_addr.s_addr = 0;
   	serv.sin_port        = htons(5678);	// 指定 IPPORT_ECHO 為 echo port
    bind(serv_sd, (LPSOCKADDR) &serv, sizeof(serv));
    //呼叫 listen() 使 socket 進入「監聽」狀態
   	if(listen(serv_sd, 1)==-1){//監聽出錯,結束程式
        ThreadAlive = 0;
        printf("Start listening failed!\n");
        system("pause");
        exit (1);
   	}
   	cli_len = sizeof(cli);

   	/*******************************************************************/
   	//初始化陣列
   	int i;
   	for(i=0;i<MAXCLIENT;i++){
        SockAvailable[i] = 1;
        Client_Chicken[i] = 0;
        Client_Fries[i] = 0;
   	}
   	//使用Thread接聽多個客戶
    pthread_t Listener;
    int Listener_i;
    Listener_i = pthread_create(&Listener,NULL,(void*)Listening,NULL);
    if(Listener_i!=0)
	{
		printf ("Create Listening error!\n");
		exit (1);
	}

	Display();

    //主程式持續進行
    while(1){
        int order;
        order = getch();

        //判斷指令
        if(order=='e'||order=='E'){
          ThreadAlive = 0;
          break;
        }
        else if(order>='0'&&order<='9'){
            int index = order-48;
            //送出餐點
            char Reply[1024]="Meal prepared : ";
            char Food_Number_str[10];
            int2str(Client_Chicken[index],Food_Number_str);
            strcat(Reply,Food_Number_str);
            strcat(Reply," chickens and ");
            int2str(Client_Fries[index],Food_Number_str);
            strcat(Reply,Food_Number_str);
            strcat(Reply," fries ");
            time_t now;
            time(&now);
            strcat(Reply,ctime(&now));
            send(cli_sd[index], Reply, strlen(Reply), 0);
            Client_Chicken[index] = 0;
            Client_Fries[index] = 0;
            Display();
        }
    }

    /*******************************************************************/

	//結束 WinSock DLL 的使用
   	closesocket(serv_sd);
   	closesocket(cli_sd);
   	WSACleanup();
    system("pause");
   	return 0;
}


