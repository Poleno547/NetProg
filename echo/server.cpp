#include <iostream> 
#include <string> 
#include <unistd.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <ctime> 

int main() {
    // Подготовка адресной структуры
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;          // Интернет протокол IPv4
    server_addr.sin_port = htons(13648);        // Порт 3306
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Локальный IP-адрес

    // Создание сокета
    int s = socket(AF_INET, SOCK_STREAM, 0);    // TCP
    if (s == -1) {
        std::cerr << "Ошибка открытия сокета" << std::endl;
        return 1;
    }

    // Привязка сокета
    int rc = bind(s, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (rc == -1) {
        std::cerr << "Ошибка привязки сокета" << std::endl;
        close(s);
        return 2;
    }

    // Ожидание подключения клиентов
    if (listen(s, 1) == -1) {
        std::cerr << "Ошибка при прослушивании порта" << std::endl;
        close(s);
        return 3;
    }

    std::cout << "Сервер готов к приему сообщений." << std::endl;

    // Принятие соединения от клиента
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_socket = accept(s, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket == -1) {
        std::cerr << "Ошибка принятия соединения" << std::endl;
        close(s);
        return 4;
    }

    char buf[1024];
    // Получение сообщения от клиента
    ssize_t received = recv(client_socket, buf, sizeof(buf) - 1, 0);
    if (received == -1) {
        std::cerr << "Ошибка приема данных" << std::endl;
    } else {
        buf[received] = '\0'; // Завершаем строку
        std::cout << "Получено сообщение: " << buf << std::endl;

        // Отправляем сообщение обратно клиенту
        send(client_socket, buf, received, 0);

        // Получаем текущее время
        time_t current_time = time(0);
        std::string datetime = ctime(&current_time);

        // Отправка времени обратно клиенту
        send(client_socket, datetime.c_str(), datetime.length(), 0);
    }

    // Закрытие клиентского сокета и основного сокета
    close(client_socket);
    close(s);

    return 0;
}
