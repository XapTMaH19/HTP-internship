#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() 
{
    setlocale(LC_ALL, "Russian");
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[BUFFER_SIZE];

    // Создание сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Ошибка создания сокета\n";
        return 1;
    }

    // Настройка сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Привязка адреса и порта к сокету
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Ошибка привязки сокета к порту и адресу\n";
        return 1;
    }

    // Прослушивание подключений
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Ошибка слушания подключений\n";
        return 1;
    }

    std::cout << "Серверс успешно запущен. Ждём новых запросов на соединение...\n";

    while (true) {
        // Принятие подключения
        socklen_t client_address_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket == -1) {
            std::cerr << "Ошибка подключения клиента к сокету\n";
            return 1;
        }

        // Приём запроса
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            std::cerr << "Ошибка получения данных\n";
            close(client_socket);
            continue;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Получено RRC Connection Setup Request: " << buffer << std::endl;

        // Формирование и отправка ответа
        const char* response = "RRC Connection Setup ответ";
        send(client_socket, response, strlen(response), 0);

        // Закрытие сокета клиента
        close(client_socket);
    }

    // Закрытие сокета сервера
    close(server_socket);

    return 0;
}
