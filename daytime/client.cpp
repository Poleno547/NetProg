#include <iostream> 
#include <string> 
#include <cstring> 
#include <unistd.h> 
#include <cstdlib>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
 
using namespace std; 

//----------------------------------------------------------------------------------------- 
void print_err(const char* why, const int exitCode = 1) { // функция для вывода ошибок
    cerr << why << endl;
    exit(exitCode);
} 
//----------------------------------------------------------------------------------------- 

int main() {
    // Подготовка адресной структуры
    sockaddr_in *addr = new sockaddr_in;
    addr->sin_family = AF_INET;          // Интернет протокол IPv4
    addr->sin_port = htons(3306);        // Порт 3306 (должен совпадать с сервером)
    addr->sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-адрес сервера

    // Создание сокета
    int s = socket(AF_INET, SOCK_DGRAM, 0); // UDP
    if (s == -1) {
        print_err("Ошибка открытия сокета", 11);
    }

    // Привязка сокета
    sockaddr_in self_addr;
    self_addr.sin_family = AF_INET;            // Протокол IPv4
    self_addr.sin_port = htons(0);             // Локальный порт (0 для автоматического выбора)
    self_addr.sin_addr.s_addr = INADDR_ANY;    // Привязка к любому доступному интерфейсу

int rc = bind(s, (const sockaddr*) &self_addr, sizeof(sockaddr_in));
    if (rc == -1) {
        print_err("Ошибка привязки сокета", 12); // Код ошибки 12
    }
    cout << "Сокет успешно привязан к адресу." << endl;

    // Подготовим буфер для передачи и приема данных 
    char buf[1024] = {0};
    string msg = "Привет";

    // Подготовка адреса сервера
    sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;            // Протокол IPv4
    srv_addr.sin_port = htons(3306);          // Порт сервера (должен совпадать с сервером)
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-адрес сервера

    // Отправка данных (sendto для UDP)
    rc = sendto(s, msg.c_str(), msg.size(), 0, (struct sockaddr*)&srv_addr, sizeof(sockaddr_in));  
    if (rc == -1) { 
        close(s); 
        print_err("Ошибка: не получилось отправить сообщение", 14);
    } 
         
    // Принятие ответа (recvfrom для UDP)
    socklen_t adr_len = sizeof(sockaddr_in);
    rc = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&srv_addr, &adr_len); 
    if (rc == -1) { 
        close(s); 
        print_err("Ошибка: не получилось принять ответ", 15);
    } 
    buf[rc] = '\0'; // Завершаем строку
     
    cout << "Ответ сервера: " << buf << endl; 
 
    // Закрытие сокета и освобождение памяти
    close(s); 
    delete addr;
  
    return 0; 
}
