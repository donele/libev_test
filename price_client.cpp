// C++ program to illustrate the client application in the 
// socket programming 
#include <cmath>
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 

#define PORT_NO 8080

int main() 
{ 
    // creating socket 
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 

    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(PORT_NO); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 

    // sending connection request 
    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    //const char* message = "Hello, server!"; 
    double price = 100.;
    char buf[16];
    double uinterval = 10000;
    double duration = 30;
    for(int i = 0; i < duration / uinterval * 1e6; ++i)
    {
        price += round(100*((double)rand() / RAND_MAX - .5)) / 100;
        sprintf(buf, "%.2f", price);
        send(clientSocket, buf, strlen(buf)+1, 0); 
        usleep(uinterval);
    }

    // closing socket 
    close(clientSocket); 

    return 0; 
}

