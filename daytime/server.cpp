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
    server_addr.sin_port = htons(3306);        // Порт 3306 (например, для сервиса времени)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-адрес, например, локальный адрес

    // Создание сокета
    int s = socket(AF_INET, SOCK_DGRAM, 0);    // UDP
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
    
    std::cout << "Сервер готов к приему сообщений." << std::endl;

    while (true) {
        // Подготовка для приема данных от клиента
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        char buf[1024];

        // Прием данных
        ssize_t rc = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (rc == -1) {
            std::cerr << "Ошибка приема данных" << std::endl;
            continue;
        }
        buf[rc] = '\0'; // Завершаем строку

        std::cout << "Получено сообщение: " << buf << std::endl;

        // Получаем текущее время
        time_t current_time = time(0);
        std::string datetime = ctime(&current_time);

        // Отправка данных обратно клиенту
        rc = sendto(s, datetime.c_str(), datetime.length(), 0, (struct sockaddr*)&client_addr, addr_len);
        if (rc == -1) {
            std::cerr << "Ошибка отправки данных" << std::endl;
        }
    }

    // Закрытие сокета
    close(s);

    return 0;
}
