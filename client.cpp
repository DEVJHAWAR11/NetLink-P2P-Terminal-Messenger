#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main() {
    // 1. socket() gives us a socket to dial server
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cout << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // inet_pton converts "127.0.0.1"
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address" << std::endl;
        return 1;
    }

    // 2. connect() dials the server
    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return 1;
    }

    std::cout << "[NetLink] Connected to server. Start chatting. Type 'exit' to quit." << std::endl;

    char buffer[1024] = {0};
    std::string message;

    // loop forever for back and forth messaging
    // client sends first, then waits to receive
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break; // leave the loop to close socket
        }

        // 3. send() pushes our typed message
        send(client_fd, message.c_str(), message.length(), 0);

        // now wait for server to reply
        memset(buffer, 0, 1024); // clear buffer so we dont get old garbage
        int valread = recv(client_fd, buffer, 1024, 0); // blocking wait
        
        // 0 means server closed connection
        if (valread <= 0) {
            std::cout << "[NetLink] Server disconnected." << std::endl;
            break;
        }

        std::cout << "Server: " << buffer << std::endl;
    }

    // 4. close() hangs up
    close(client_fd);

    return 0;
}
