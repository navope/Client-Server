#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#include <clocale>
#include <limits>
// пофиксить зарашивание (возможно нет ошибки) и на клиенте case 4

struct modbus{
unsigned short transaction_id=0,protocol_id =0, length;
unsigned char unit_id=0;
unsigned char function_code;
char data[MAXBYTE-8] = "";
};
union union_type
{
unsigned char elem16[2];
uint16_t elem10;
};

void SendingModbusPackets(char * mas ,modbus * package,SOCKET sock);
SOCKET get_socket(){
// Создаем сокет
SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
// Инициируем запрос к серверу
sockaddr_in sockAddr;
memset (& sockAddr, 0, sizeof (sockAddr)); // Каждый байт заполняется 0
sockAddr.sin_family = PF_INET;
sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //127.0.0.1
sockAddr.sin_port = htons(502);
connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
return sock;
};


int main(){
    union_type voltage;
    using namespace std;
    setlocale(LC_ALL, "rus");
    modbus package;
    char bytes[sizeof(modbus)];
         // Инициализируем DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); //0000001000000010 запускаем
         // Создаем сокет
    SOCKET sock;

    unsigned int menu =0;
    do{
    sock=get_socket();
    printf("1.Сделать яркость диода максимальной \n2.Сделать яркость диода минимальной \n3.Закрасить экран \n4.Измерить напряжение и вывести его в кансоль\n5.Выход\n");
    scanf("%d",&menu);
    switch(menu){
        case 1:
            {
                package.function_code = 65;//код функции
                SendingModbusPackets(bytes, &package, sock);
                if (strcmp(package.data, ""))
                    printf("Message from server: %s\n", package.data);
                    else
                    printf("Server is not working\n");
                    getchar();
                    getchar();
                    system("cls");

                closesocket(sock);// Закрываем сокет
            }
        break;
        case 2:
            {
                package.function_code = 66;//код функции
                SendingModbusPackets(bytes, &package, sock);
                if (strcmp(package.data, ""))
                    printf("Message from server: %s\n", package.data);
                    else
                    printf("Server is not working\n");
                    getchar();
                    getchar();
                    system("cls");
                closesocket(sock);// Закрываем сокет
            }
        break;
        case 3:
            {
                package.function_code = 67;//код функции
                SendingModbusPackets(bytes, &package, sock);
                if (strcmp(package.data, ""))
                    printf("Message from server: %s\n", package.data);
                    else
                    printf("Server is not working\n");
                    getchar();
                    getchar();
                    system("cls");
                closesocket(sock);// Закрываем сокет
            }
        break;
        case 4:
            {
                package.function_code = 68;//код функции
                SendingModbusPackets(bytes, &package, sock);
                if (strcmp(package.data, ""))
                    {
                       memcpy(voltage.elem16,package.data,2);
                       int rez = voltage.elem10;
                       printf("Message from server: %d\n", rez);
                    }
                    else
                    printf("Server is not working\n");
                    getchar();
                    getchar();
                    system("cls");
                closesocket(sock);// Закрываем сокет
            }
        break;
        }
        }while (menu!=5);

    // Прекращаем использование DLL
    WSACleanup();
    system("pause");
    return 0;
}

void SendingModbusPackets(char * mas ,modbus * package,SOCKET sock)
{
        package->length = sizeof(package->data);
        memcpy(mas, package, sizeof(modbus));//закодировали в массив байт
        send(sock, mas, sizeof(modbus), NULL);// отправили на сервер
        // Получение данных, возвращаемых сервером
        recv(sock, mas, sizeof(modbus), NULL);// приняли с сервера
        memcpy(package, mas, sizeof(modbus));// декодировали в структуру
}
