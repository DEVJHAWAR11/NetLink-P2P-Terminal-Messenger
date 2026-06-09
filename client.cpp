#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <pthread.h>
#include <ctime>

// helper to get current time
std::string get_time() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    sprintf(buffer, "[%02d:%02d:%02d] ", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return std::string(buffer);
}

// this is the thread function that waits for incoming messages from server
void* receive_thread(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, 1024);
        int valread = recv(sock, buffer, 1024, 0);

        // 0 means server closed connection
        if (valread <= 0) {
            std::cout << "\n[NetLink] Server disconnected." << std::endl;
            exit(0); // kill program since connection is gone
        }

        if (strcmp(buffer, "exit") == 0) {
            std::cout << "\n[NetLink] Server disconnected." << std::endl;
            exit(0);
        }

        std::cout << "\n" << get_time() << "Server: " << buffer << "\nYou: " << std::flush;
    }
    return NULL;
}

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cout << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address" << std::endl;
        return 1;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return 1;
    }

    std::cout << "[NetLink] Connected to server. Start chatting. Type 'exit' to quit." << std::endl;
    std::cout << "You: ";

    // spawn thread to receive messages independently
    pthread_t recv_thread_id;
    if (pthread_create(&recv_thread_id, NULL, receive_thread, (void*)&client_fd) < 0) {
        std::cout << "Could not create thread" << std::endl;
        return 1;
    }

    std::string message;

    // main thread handles sending only
    while (true) {
        std::getline(std::cin, message);

        send(client_fd, message.c_str(), message.length(), 0);

        if (message == "exit") {
            std::cout << "[NetLink] Connection closed." << std::endl;
            break;
        }
        std::cout << "You: ";
    }

    close(client_fd);

    return 0;
}
