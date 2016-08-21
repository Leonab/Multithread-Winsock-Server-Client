#include <windows.h>
#include  <stdlib.h>
#include <stdio.h>
#include <winsock.h>

// our thread for receiving commands
DWORD WINAPI receive_cmds(LPVOID lpParam)
{
  printf("thread created\r\n");

  // set our socket to the socket passed in as a parameter
  SOCKET current_client = (SOCKET)lpParam;

  // buffer to hold our recived data
  char buf[100];
  // buffer to hold our sent data
  char sendData[100];
  // for error checking
  int res;

  // our recv loop
  while(1)
  {
     res = recv(current_client,buf,sizeof(buf),0); // recv cmds

     Sleep(10);

     if(res == 0)
     {
      MessageBox(0,"error","error",MB_OK);
      closesocket(current_client);
      ExitThread(0);
     }

     if(strstr(buf,"hello"))
     { // greet this user
       printf("\nrecived hello cmd");

       strcpy(sendData,"hello, greetings\n");
       Sleep(10);
       send(current_client,sendData,sizeof(sendData),0);
     }
     else if(strstr(buf,"bye"))
     { // disconnect this user
       printf("\nrecived bye cmd\n");

       strcpy(sendData,"cya\n");
       Sleep(10);
       send(current_client,sendData,sizeof(sendData),0);

      // close the socket associted with this client and end this thread
       closesocket(current_client);
       ExitThread(0);
     }
     else
     {
       strcpy(sendData,"Invalid cmd\n");
       Sleep(10);
       send(current_client,sendData,sizeof(sendData),0);
     }

     // clear buffers
       strcpy(sendData,"");
       strcpy(buf,"");
   }
}

int main()
{
 printf("Starting up multi-threaded TCP server\r\n");

 // our masterSocket(socket that listens for connections)
 SOCKET sock;

 // for our thread
 DWORD thread;

 WSADATA wsaData;
 SOCKADDR_IN server;

 // start winsock
 int ret = WSAStartup(0x101,&wsaData); // use highest version of winsock avalible

 if(ret != 0)
 {
    return 0;
 }

 // fill in winsock struct ...
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=INADDR_ANY;
 server.sin_port=htons(123); // listen on telnet port 23

 // create our socket
 sock=socket(AF_INET,SOCK_STREAM,0);

 if(sock == INVALID_SOCKET)
 {
    return 0;
 }

 // bind our socket to a port(port 123)
 if( bind(sock,(SOCKADDR*)&server,sizeof(server)) !=0 )
 {
    return 0;
 }

 // listen for a connection
 if(listen(sock,5) != 0)
 {
    return 0;
 }

 // socket that we send & receive data on
 SOCKET client;

 SOCKADDR_IN from;
 int fromlen = sizeof(from);

 // loop forever
 while(1)
 {
  // accept connections
  client = accept(sock,(struct SOCKADDR*)&from,&fromlen);
  printf("Client connected\r\n");

  // create our recv_cmds thread and parse client socket as a parameter
  CreateThread(NULL, 0,receive_cmds,(LPVOID)client, 0, &thread);
 }

 // shutdown winsock
 closesocket(sock);
 WSACleanup();

 // exit
 return 0;
}
