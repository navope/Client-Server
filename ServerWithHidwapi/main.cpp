#define WIN32
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#define MAX_STR 255
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
#define MAX_STR 255

struct modbus{
    unsigned short transaction_id=0,protocol_id =0, length;
    unsigned char unit_id=0;
    unsigned char function_code;
    char data[MAXBYTE-8] = "";
    };



union union_type{
    unsigned char elem16[2];
    uint16_t elem10;
    };

int all_hid_info();
int open_hid_info(hid_device *handle, int* res);
void feature_report(hid_device *handle, int* res,unsigned char *buf);
void MakeMaximumBrightness(hid_device *handle, unsigned char *buf , modbus* package , char * mas);
void MakeMinimumBrightness(hid_device *handle, unsigned char *buf , modbus* package , char * mas);
void PaintOverTheScreen(hid_device *handle, unsigned char *buf , modbus* package , char * mas);
void MeasureVoltageAndChangeRGB(hid_device *handle, unsigned char *buf, union_type * diod_color, int res, modbus* package , char * mas);


int main(int argc, char* argv[])
{
    int nSize;
    int res;
    unsigned char buf[255];
    //объявляем пакет модбас
	modbus package;
    char bytes[sizeof(modbus)];
    // Инициализируем DLL
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);

    // Создаем сокет
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Привязываем сокет
    sockaddr_in sockAddr;
	memset (& sockAddr, 0, sizeof (sockAddr)); // Каждый байт заполняется 0
	sockAddr.sin_family = PF_INET; // Использовать IPv4-адрес
	sockAddr.sin_addr.s_addr = inet_addr ("127.0.0.1"); // Определенный IP-адрес (в вузе 0.0.0.0)
	sockAddr.sin_port = htons (502); // Порт
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    // Входим в состояние мониторинга
    listen(servSock, 20);

    // Получение клиентского запроса
    SOCKADDR clntAddr;

    SOCKET clntSock;

    (void)argc;
    (void)argv;

	hid_device *handle;

    union_type diod_color;

    if (all_hid_info()==-1) return -1;

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;

	// Open the device using the VID, PID, and optionally the Serial number.

	handle = hid_open(0x1234, 0x0001, NULL);

    if (open_hid_info(handle, &res)==1) return 1;

    // LEDs lights
        buf[0] = 0x02; // descriptor number
        buf[1] = 0xff; // light color 1
        buf[2] = 0xff; // 2 byte = uint16_t = power of light color 1
        buf[3] = 0x00; // light color 2
        buf[4] = 0x00; // 2 byte = uint16_t = power of light color 2
        buf[5] = 0xff; // light color 3
        buf[6] = 0xff; // 2 byte = uint16_t = power of light color 3

    feature_report(handle, &res, buf);

    while(1){
        nSize = sizeof(SOCKADDR);
        clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
        recv(clntSock, bytes, sizeof(modbus), NULL);
        memcpy(&package, bytes, sizeof(modbus));
		switch (package.function_code)
        {
            case 65:
                {
                   MakeMaximumBrightness(handle, buf, &package, bytes);
                }
            break;
            case 66:
                {
                    MakeMinimumBrightness(handle, buf, &package, bytes);
                }
            break;
            case 67:
                {
                    PaintOverTheScreen(handle, buf, &package, bytes);
                }
            break;
            case 68:
                {
                    MeasureVoltageAndChangeRGB(handle, buf, &diod_color, res, &package, bytes);
                }
            break;
        };
        // Отправляем данные клиенту
        send(clntSock, bytes, sizeof(modbus), NULL);
    }

 // Закрываем сокет
    closesocket(clntSock);
    closesocket(servSock);

 // Прекращаем использование DLL
    WSACleanup();
    return 0;
}




void MakeMaximumBrightness(hid_device *handle, unsigned char *buf , modbus* package , char * mas)
{

	buf[0] = 0x02;
	for (int i=1; i<7;i++)
	buf[i] = 0xff;
	hid_send_feature_report(handle,buf,7);
	strcpy(package->data, "Установленна максимальная яркость");
	memcpy(mas, package, sizeof(modbus));//кодируем
}




void MakeMinimumBrightness(hid_device *handle, unsigned char *buf , modbus* package , char * mas)
{

	buf[0] = 0x02;
	for (int i=1; i<7;i++)
		buf[i] = 0x00;
	hid_send_feature_report(handle,buf,7);
	strcpy(package->data, "Установленна маинимальная яркость");
	memcpy(mas, package, sizeof(modbus));//кодируем
}



void PaintOverTheScreen(hid_device *handle, unsigned char *buf , modbus* package , char * mas)
	{
		static int color = 1;
		buf[0] = 0x04;
				for (int i =0;i<64;i++)
					for (int j=0;j<128;j++)
					{
                        buf[1] = j;
                        buf[2] = i;
                        buf[3] = color;
                        hid_send_feature_report(handle,buf,4);
					}
                if (color) color = 0;
                    else color = 1;
		strcpy(package->data, "Экран устройства закрашен");
		memcpy(mas, &package, sizeof(modbus));//кодируем
}



void MeasureVoltageAndChangeRGB(hid_device *handle, unsigned char *buf, union_type * diod_color, int res, modbus* package, char * mas)
{
	int rez = 0;

	buf[0] = 0x03;
	hid_get_feature_report(handle,buf,7); //измеряем напряжение
	memcpy(diod_color,&buf[1],2); // копируем данные
	rez = diod_color->elem10; //приводим к инту для вывода в консоль
	package->data[0] = diod_color->elem16[0];
	package->data[1] = diod_color->elem16[1];
	for (int i=0; i++;i<3)
	{
		buf[1+i*2] = diod_color->elem16[0];
		buf[2+i*2] = diod_color->elem16[1];
	}
	buf[0] = 0x02;
	hid_send_feature_report(handle,buf,7); // меняем RGB
	memcpy(mas, package, sizeof(modbus));//кодируем
}

int all_hid_info(){


	struct hid_device_info *devs, *cur_dev;

	printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
	if (hid_version()->major == HID_API_VERSION_MAJOR && hid_version()->minor == HID_API_VERSION_MINOR && hid_version()->patch == HID_API_VERSION_PATCH) {
		printf("Compile-time version matches runtime version of hidapi.\n\n");
	}
	else {
		printf("Compile-time version is different than runtime version of hidapi.\n]n");
	}

	if (hid_init())
		return -1;

    // находим все устройства USB HID, печатаем содержимое дескриптора устройства, доступное через драйвер
    devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

}

int open_hid_info(hid_device *handle, int* res){
    if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}
    wchar_t wstr[MAX_STR];
	// Read the Manufacturer String
	wstr[0] = 0x0000;
	*res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (*res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	*res = hid_get_product_string(handle, wstr, MAX_STR);
	if (*res < 0)
		printf("Unable to read product string\n");
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	*res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	*res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (*res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed String 1: %ls\n", wstr);
	return 0;
}


void feature_report(hid_device *handle, int* res,unsigned char *buf){
    int i;
    *res = hid_send_feature_report(handle,buf,7); // send report, 7 byte

    if(*res == -1) {
        printf("hid_write error.\n");
    }
     // keys
     // Read a Feature Report from the device
	buf[0] = 0x1;
	*res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (*res < 0) {
		printf("Unable to get a feature report.\n");
		printf("%ls", hid_error(handle));
	}
	else {
		// Print out the returned buffer.
		printf("Feature Report\n   ");
		for (i = 0; i < *res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}

}
