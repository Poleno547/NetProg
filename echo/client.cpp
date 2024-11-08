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
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;          // Интернет протокол IPv4
    server_addr.sin_port = htons(13648);        // Порт 3306 (должен совпадать с сервером)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-адрес сервера

    // Создание сокета
    int s = socket(AF_INET, SOCK_STREAM, 0); // TCP
    if (s == -1) {
        print_err("Ошибка открытия сокета", 11);
    }

    // Установка соединения с сервером
    int rc = connect(s, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (rc == -1) {
        close(s);
        print_err("Ошибка соединения с сервером", 13);
    }

    // Отправка сообщения серверу
    string msg = "Привет, сервер!";
    rc = send(s, msg.c_str(), msg.size(), 0);  
    if (rc == -1) { 
        close(s); 
        print_err("Ошибка: не получилось отправить сообщение", 14);
    } 

    // Прием ответа от сервера
    char buf[1024];
    rc = recv(s, buf, sizeof(buf) - 1, 0); 
    if (rc == -1) { 
        close(s); 
        print_err("Ошибка: не получилось принять ответ", 15);
    } 
    buf[rc] = '\0'; // Завершаем строку
     
    cout << "Ответ сервера: " << buf << endl;

    // Прием текущего времени от сервера
    rc = recv(s, buf, sizeof(buf) - 1, 0);
    if (rc > 0) {
        buf[rc] = '\0';
        cout << "Текущее время на сервере: " << buf << endl;
    }

    // Закрытие сокета
    close(s);

    return 0; 
}
