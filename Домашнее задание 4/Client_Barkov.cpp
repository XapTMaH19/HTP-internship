#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 12345
// адрес заглушки
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024];

    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Ошибка создания сокета\n";
        return 1;
    }

    // Настройка сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_address.sin_port = htons(PORT);

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Ошибка соединения с сервером\n";
        return 1;
    }

    // Отправка запроса
    const char* request = "RRC Connection Setup Request";
    send(client_socket, request, strlen(request), 0);

    // Приём ответа
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        std::cerr << "Ошибка получения данных\n";
        close(client_socket);
        return 1;
    }

    buffer[bytes_received] = '\0';
    std::cout << "Received RRC Connection Setup Успешно!: " << buffer << std::endl;

    // Закрытие сокета клиента
    close(client_socket);

    return 0;
}
