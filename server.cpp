#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <pthread.h>
#include <ctime>

// helper to get current time like [14:22:31]
std::string get_time() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    sprintf(buffer, "[%02d:%02d:%02d] ", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return std::string(buffer);
}

// this is the thread function that constantly waits for incoming messages
void* receive_thread(void* socket_desc) {
    // cast the void pointer back to an int to get our socket
    int sock = *(int*)socket_desc;
    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, 1024);
        int valread = recv(sock, buffer, 1024, 0);

        // if valread is 0 or less, the client hung up or there was an error
        if (valread <= 0) {
            std::cout << "\n[NetLink] Client disconnected." << std::endl;
            exit(0); // clean exit the whole program
        }

        // check if they told us they are exiting
        if (strcmp(buffer, "exit") == 0) {
            std::cout << "\n[NetLink] Client disconnected." << std::endl;
            exit(0);
        }

        // print what they said with a timestamp
        std::cout << "\n" << get_time() << "Client: " << buffer << "\nYou: " << std::flush;
    }
    return NULL;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cout << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cout << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "[NetLink] Server started on port 8080" << std::endl;
    std::cout << "[NetLink] Waiting for connection..." << std::endl;

    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cout << "Accept failed" << std::endl;
        return 1;
    }

    std::cout << "[NetLink] Client connected. Start chatting. Type 'exit' to quit." << std::endl;
    std::cout << "You: "; // initial prompt

    // spawn our receive thread so it can listen while we type
    pthread_t recv_thread_id;
    if (pthread_create(&recv_thread_id, NULL, receive_thread, (void*)&new_socket) < 0) {
        std::cout << "Could not create thread" << std::endl;
        return 1;
    }

    std::string message;

    // main thread handles sending
    while (true) {
        std::getline(std::cin, message);

        // send() pushes our typed message to client
        send(new_socket, message.c_str(), message.length(), 0);

        if (message == "exit") {
            std::cout << "[NetLink] Connection closed." << std::endl;
            break; // exit loop
        }
        std::cout << "You: ";
    }

    // cleanly shut down sockets
    close(new_socket);
    close(server_fd);

    return 0;
}
