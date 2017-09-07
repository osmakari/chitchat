#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <istream>
#include <unistd.h>
#include <stdlib.h>

void *rec_thread (void *sock);

int main()
{
	int sock;
	struct sockaddr_in server;
	const char *message;
	std::cout << "**********************\n";
	std::cout << "Welcome to ShittyChat!\n";
	std::cout << "**********************\n";
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
        std::cout << "Could not create socket\n";
        
	
	std::cout << "Socket created\n\n";
    
    std::string ip_in;
    std::cout << "Anna Serverin IP osoite (ex. \nlocal: 127.0.0.1, \nec2: 35.158.184.105): ";
    std::cin >> ip_in;

	server.sin_addr.s_addr = inet_addr(ip_in.c_str());
    server.sin_family = AF_INET;
    std::cout << "\n\nAnna Serverin portti (ex. 1337): ";
    int port;
    std::cin >> port;
	server.sin_port = htons( port );

	//Yhdistetään serverille
	if (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		std::cout << "Yhdistäminen epäonnistui...\n";
		return 1;
	}
    
    std::string name = "client";
	std::cout << "Yhdistetty!\n";
    std::cout << "Anna nimesi: ";
    std::cin >> name;

    message = name.c_str();
    if( send(sock, message, strlen(message), 0) < 0)
    {
        std::cout << "Send failed!\n";
        return 1;
    }
	std::cout << "Nimi asetettu!\n\n";
	pthread_t recthread;
	int *ss = &sock;
	pthread_create(&recthread, NULL, rec_thread, (void *)ss);

	while(1)
	{	
		char bf[2000];
		std::cin.getline(bf, sizeof(bf));
		std::string mg(bf);
        message = mg.c_str();
		//Send some data
		if(send(sock, message, strlen(message), 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
	}
	
	close(sock);
	return 0;
}

void *rec_thread (void *sock) {
	int s = *(int*)sock;
	char server_reply[2000];	

	while (1) {
		//Receive a reply from the server
		if( recv(s , server_reply , 2000 , 0) < 0)
		{
			std::cout << "receive epäonnistui...\n";
			break;
		}
		
		std::cout << server_reply << "\n";
		memset(server_reply, 0, strlen(server_reply));
	}
}