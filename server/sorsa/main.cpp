#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <stdlib.h>


#include <string.h>
#include <string>

// client structure, koska tänne voi joskus lisätä jotain muuttujia
struct Client {
    int socket; // clientin oma socket
    std::string name; // clientin nimi
};

//client vector
std::vector<Client*> clients;

//prototyyppi connection handlerille
void *connection_handler (void *conn);

int remove_client (Client *c);
void relay_message (Client *from, std::string message);


const char *server_message; // viesti clientille, muutetaan myöhemmin   

int main () {
    // servun socket osoite config
    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY; // auto ip osoite
    server.sin_port = htons(1337); // socketin portti
    server.sin_family = AF_INET; 


    int socket_m; // pääsocketti
    int client_sock; // clientin socket
    
    struct sockaddr_in client; // clientin address

    socket_m = socket(AF_INET , SOCK_STREAM , 0); // tehdään striimaava socket
    if (socket_m == -1)
        std::cout << "Failed to create socket!\n"; // jos socket returnaa -1, sen tekeminen epäonnistui
    else 
        std::cout << "Socket created!\n"; // muuten pitäisi mennä ok

    //bindataan socket
    if(bind(socket_m /*pääsocket*/, (struct sockaddr *)&server /*osoitteet..*/, sizeof(server) /*structin koko*/) < 0)
    {
        // socketin bindaaminen epäonnistui..
        close(socket_m);
        std::cout << "Bind failed!\n";
        return 1;
    }
    else {
        //muuten ok
        std::cout << "Bind done!\n";
    }
    // kuunnellaan sockettia
    listen(socket_m, 3);

    // socket pitäisi olla valmis, sitten vain kuuntelemaan
    int c = sizeof(struct sockaddr_in); // väliaikainen sockaddr_in kokoinen 

    int *new_sock; // uuden socketin väliaikainen var

    // acceptataan socket, samalla kun asetetaan muuttujat
    while( (client_sock = accept(socket_m, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        std::cout << "Connetion accepted\n";         
        pthread_t conn_handler;
        new_sock = (int*)malloc(1); // mallokoidaan C tyylisesti
        *new_sock = client_sock;
        
        //käytetään pthreadia yhteyden ylläpitämiseksi ja hallitsemiseksi

        if(pthread_create(&conn_handler, NULL, connection_handler, (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;
}

//yhteyden ylläpitothreadi
void *connection_handler (void *conn) {
    int sock = *(int*)conn; // asetetaan socket numero
    int read_size; //viestin koko
    char client_message[2000]; // clientin message bufferi

    int message_count = 0; //viesticountti, 0 = nimen asettaminen

    Client *cur_client = new Client;
    cur_client->name = "client_" + sock;
    cur_client->socket = sock;
    clients.push_back(cur_client);

    while((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        if(client_message == NULL) {
            // jos client message on tyhjä, niin poistutaan
            remove_client(cur_client);
            free(conn);
            return 0;
        }
        //muutetaan client message enemmän cpp ystävälliseen muotoon
        std::string s(client_message);
        if(message_count == 0) {
            //aseta nimi
            cur_client->name = s;
            relay_message(cur_client, "Just Connected!");
        }
        else {
            relay_message(cur_client, s);            
        }
        message_count++;
        memset(client_message, 0, strlen(client_message));
    }
    // clientti disconnectannut
    if(read_size == 0)
    {
        relay_message(cur_client, "Just Disconnected!");        
        std::cout << "Client disconnected\n";
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        relay_message(cur_client, "Just Lost Connection!");        
        std::cout << "Receive failure\n";
    }
    remove_client(cur_client);
    // vapautetaan yhteys muistista
    free(conn);
    return 0;
}

int remove_client (Client *c) {
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if((*it) == c) {
            clients.erase(it);
            return 1;
        }
    }
    return 0;
} 

void relay_message (Client *from, std::string message) {
    std::string full = from->name + ": " + message + "\0"; 
    server_message = full.c_str();
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it ) {
        write((*it)->socket, server_message, strlen(server_message));
    }
}