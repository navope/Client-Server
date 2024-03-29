#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#include <clocale>
#include <limits>
// ��������� ����������� (�������� ��� ������) � �� ������� case 4

struct modbus{
unsigned short transaction_id=0,protocol_id =0, length;
unsigned char unit_id=0;
unsigned char function_code;
char data[MAXBYTE-8] = "";
};

void PrintMassage(modbus * package);
void SendingModbusPackets(char * mas ,modbus * package,SOCKET sock);
SOCKET get_socket();

int main(){
    using namespace std;
    setlocale(LC_ALL, "rus");
    modbus package;
    char bytes[sizeof(modbus)];
         // �������������� DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); //0000001000000010 ���������
         // ������� �����
    SOCKET sock;

    unsigned int menu =0;
    do{
    sock=get_socket();
    printf("1.������� ������� ����� ������������ \n2.������� ������� ����� ����������� \n3.��������� ����� \n4.�������� ���������� � ������� ��� � �������\n5.�����\n");
    scanf("%d",&menu);
    switch(menu){
        case 1:
            {
                package.function_code = 65;//��� �������
                SendingModbusPackets(bytes, &package, sock);
                PrintMassage(&package);
                closesocket(sock);// ��������� �����
            }
        break;
        case 2:
            {
                package.function_code = 66;//��� �������
                SendingModbusPackets(bytes, &package, sock);
                PrintMassage(&package);
                closesocket(sock);// ��������� �����
            }
        break;
        case 3:
            {
                package.function_code = 67;//��� �������
                SendingModbusPackets(bytes, &package, sock);
                PrintMassage(&package);
                closesocket(sock);// ��������� �����
            }
        break;
        case 4:
            {

                package.function_code = 68;//��� �������
                SendingModbusPackets(bytes, &package, sock);
                PrintMassage(&package);
                closesocket(sock);// ��������� �����
            }
        break;
        }
        }while (menu!=5);

    // ���������� ������������� DLL
    WSACleanup();
    system("pause");
    return 0;
}

void SendingModbusPackets(char * mas ,modbus * package,SOCKET sock)
{
        package->length = sizeof(package->data);
        memcpy(mas, package, sizeof(modbus));//������������ � ������ ����
        send(sock, mas, sizeof(modbus), NULL);// ��������� �� ������
        // ��������� ������, ������������ ��������
        recv(sock, mas, sizeof(modbus), NULL);// ������� � �������
        memcpy(package, mas, sizeof(modbus));// ������������ � ���������
}

void PrintMassage(modbus * package)
{
    if (strcmp(package->data, ""))
            printf("Message from server: %s\n", package->data);
        else
            printf("Server is not working\n");
            getchar();
            getchar();
            system("cls");
}

SOCKET get_socket(){
// ������� �����
SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
// ���������� ������ � �������
sockaddr_in sockAddr;
memset (& sockAddr, 0, sizeof (sockAddr)); // ������ ���� ����������� 0
sockAddr.sin_family = PF_INET;
sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //127.0.0.1
sockAddr.sin_port = htons(502);
connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
return sock;
};
