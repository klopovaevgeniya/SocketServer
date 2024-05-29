#define WIN32_LEAN_AND_MEAN

#include <Windows.h>   // Библиотека Windows для использования функций Windows API
#include <iostream>    // Библиотека ввода-вывода стандартного потока
#include <WinSock2.h>  // Библиотека для работы с сокетами в Windows
#include <WS2tcpip.h>  // Библиотека для работы с сетевыми функциями Windows

using namespace std;   // Использование пространства имен стандартной библиотеки

int main() {
    SetConsoleCP(1251);  // Установка кодовой страницы консоли для ввода
    setlocale(LC_ALL, "Russian");  // Установка локали для вывода русского текста в консоль
    WSADATA wsaData;  // Структура для хранения информации о использованной версии Winsock
    ADDRINFO hints;   // Структура для указания параметров сокета
    ADDRINFO* addrResult;  // Структура для хранения результатов функции getaddrinfo
    SOCKET ListenSocket = INVALID_SOCKET;  // Дескриптор сокета прослушивания
    SOCKET ConnectSocket = INVALID_SOCKET;  // Дескриптор сокета соединения
    char recvBuffer[512];  // Буфер для приема данных
    const char* sendBuffer = "Привет от сервера";  // Сообщение от сервера

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "Ошибка WSAStartup: " << result << endl;
        return 1;
    }

    // Задание параметров сокета
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // Потоковый сокет
    hints.ai_protocol = IPPROTO_TCP;  // Протокол TCP
    hints.ai_flags = AI_PASSIVE;      // Пассивный сокет (для приема соединений)

    // Получение информации о сокете (локальный адрес)
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "Ошибка getaddrinfo: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета прослушивания
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Ошибка создания сокета" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязка сокета прослушивания к локальному адресу и порту
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Ошибка привязки сокета: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Установка сокета в режим прослушивания
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Ошибка установки режима прослушивания: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принятие соединения от клиента
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Ошибка принятия подключения: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Получение первого сообщения от клиента
    result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
    if (result > 0) {
        recvBuffer[result] = '\0';
        cout << "Сообщение от клиента: " << recvBuffer << endl;
    }

    // Получение второго сообщения от клиента
    result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
    if (result > 0) {
        recvBuffer[result] = '\0';
        cout << "Сообщение от клиента: " << recvBuffer << endl;
    }

    // Отправка сообщения клиенту
    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
    if (result == SOCKET_ERROR) {
        cout << "Ошибка отправки: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие сокетов и освобождение ресурсов
    closesocket(ConnectSocket);
    closesocket(ListenSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
