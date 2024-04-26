#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = { 0 };

    // Создание TCP сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "сокет не создался" << std::endl;
        return 1;
    }

    // Установка параметров сокета
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "не установились параметры сокеты" << std::endl;
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета к адресу и порту
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "не привязался" << std::endl;
        return 1;
    }

    // Ожидание подключений
    if (listen(server_fd, 3) < 0) {
        std::cerr << "слушаем нового клиета" << std::endl;
        return 1;
    }

    while (true) {
        // Принятие нового подключения
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "перепривязываем сокет и продолжаем слушать" << std::endl;
            return 1;
        }

        // Чтение данных из сокета
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        std::cout << "получаем сообщение от клиента: " << buffer << std::endl;

        // Отправка подтверждения о получении
        const char* ack = "сообщением получено";
        send(new_socket, ack, strlen(ack), 0);

        // Закрытие сокета
        close(new_socket);
    }

    return 0;
}
