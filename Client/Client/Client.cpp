#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <string.h>
#include <iostream>
#include "../../Packet/Packet.h"
#include <locale.h>


int main()
{
    char data[252] = { 0 };
    char* locale = setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int nSize;
    char szBuffer[260] = { 0 };
    char menu;
    bool running = true;
    while (running)
    {

        SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = PF_INET;
        sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        sockAddr.sin_port = htons(1234);
        connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
        system("cls");
        std::cout << "1. Макдак" << std::endl <<"2. Never Meant (не закончено и вряд ли будет)"<<std::endl<< "3. Переподключение" << std::endl <<"4. Состояние плат"<<std::endl<< "5. Выход" << std::endl << std::endl;
        std::cin >> menu;
        switch (menu)
        {
        case '1':
        {
            char fc = 65;
            Packet pack(fc, data);
            send(sock, pack.Serialize(), 260, NULL);
            system("cls");
            std::cout << "Отправлен пакет: " << std::endl << std::endl;
            pack.Print();
            system("pause");
            system("cls");
            recv(sock, szBuffer, 260, NULL);
            pack.Deserialize(szBuffer);
            if (strlen(pack.Data))
                std::cout << pack.Data;
            else
                std::cout << "Не удалось связаться с сервером!\n";
            system("pause");
            break;
        }
        case '2':
        {
            char fc = 68;
            Packet pack(fc, data);
            send(sock, pack.Serialize(), 260, NULL);
            system("cls");
            std::cout << "Отправлен пакет: " << std::endl << std::endl;
            pack.Print();
            system("pause");
            system("cls");
            recv(sock, szBuffer, 260, NULL);
            pack.Deserialize(szBuffer);
            if (strlen(pack.Data))
                std::cout << pack.Data;
            else
                std::cout << "Не удалось связаться с сервером!\n";
            system("pause");
            break;
        }
        case '3':
        {
            char fc = 66;
            Packet pack(fc, data);
            send(sock, pack.Serialize(), 260, NULL);
            system("cls");
            std::cout << "Отправлен пакет: " << std::endl << std::endl;
            pack.Print();
            system("pause");
            system("cls");
            recv(sock, szBuffer, 260, NULL);
            pack.Deserialize(szBuffer);
            if (strlen(pack.Data))
                std::cout << pack.Data;
            else
                std::cout << "Не удалось связаться с сервером!\n"; \
            system("pause");
            break;
        }
        case '4':
        {
            char fc = 67;
            Packet pack(fc, data);
            send(sock, pack.Serialize(), 260, NULL);
            system("cls");
            std::cout << "Отправлен пакет: " << std::endl << std::endl;
            pack.Print();
            system("pause");
            system("cls");
            recv(sock, szBuffer, 260, NULL);
            pack.Deserialize(szBuffer);
            if (strlen(pack.Data))
            {
                uint8_t report;
                memcpy(&report, pack.Data, sizeof(report));
                switch (report)
                {
                case 0:
                    std::cout << "Платы не подключены";
                    break;
                case 1:
                    std::cout << "Подключена только первая плата";
                    break;
                case 2:
                    std::cout << "Подключена только вторая плата";
                    break;
                case 3:
                    std::cout << "Подключены первая и вторая платы";
                    break;
                case 4:
                    std::cout << "Подключена только третья плата";
                    break;
                case 5:
                    std::cout << "Подключены первая и третья платы";
                    break;
                case 6:
                    std::cout << "Подключены вторая и третья платы";
                    break;
                case 7:
                    std::cout << "Подключены все три платы";
                    break;
                }
                std::cout << std::endl;
            }
            else
                std::cout << "Не удалось связаться с сервером!\n";
            system("pause");
            break;
        }
        case '5':
        {
            system("cls");
            std::cout << "Завершение работы\n";
            running = false;
        }
        }
    }
}
