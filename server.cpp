#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main() {
    // 1. socket() gives us a socket to work with
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cout << "Failed to create socket" << std::endl;
        return 1;
    }

    // 2. sockaddr_in holds our address info
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    // bind() attaches socket to port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "Bind failed" << std::endl;
        return 1;
    }

    // 3. listen() puts socket in passive mode
    if (listen(server_fd, 3) < 0) {
        std::cout << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "[NetLink] Server started on port 8080" << std::endl;
    std::cout << "[NetLink] Waiting for connection..." << std::endl;

    // 4. accept() waits for client
    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cout << "Accept failed" << std::endl;
        return 1;
    }

    std::cout << "[NetLink] Client connected. Start chatting. Type 'exit' to quit." << std::endl;

    char buffer[1024] = {0};
    std::string message;

    // loop forever for back and forth messaging
    // server receives first, then sends because client sends first
    while (true) {
        memset(buffer, 0, 1024); // clear buffer before reading
        
        // recv() blocking wait for client message
        int valread = recv(new_socket, buffer, 1024, 0);
        
        // if valread is 0, client disconnected cleanly. if -1, error
        if (valread <= 0) {
            std::cout << "[NetLink] Client disconnected." << std::endl;
            break;
        }

        std::cout << "Client: " << buffer << std::endl;

        // now it's our turn to send
        std::cout << "You: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break; // exit loop and close connection
        }

        // send() pushes our typed message to client
        send(new_socket, message.c_str(), message.length(), 0);
    }

    // 6. close() cleanly shuts down
    close(new_socket);
    close(server_fd);

    return 0;
}
