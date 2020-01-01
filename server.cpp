//#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#define MY_SERVICE_NAME "MyService"
#define MAX_BUF_SIZE 256

SERVICE_STATUS_HANDLE ssh;
SERVICE_STATUS ss;

HANDLE logFile;              //Материал для файла логирования
DWORD logFileMemorySize;
char adreslog[100] = "C:/Users/Lenovo/source/repos/progrgz/log2.txt";

char msg1[100];                   //Массив сообщения для файла логирования

// Идентификатор канала Pipe
HANDLE hNamedPipe;
// Имя создаваемого канала Pipe
LPSTR  lpszPipeName = (LPSTR)"\\\\.\\pipe\\$MyPipe$";



void log(char msg[100])
{
	WriteFile(logFile, msg, strlen(msg), &logFileMemorySize, NULL);
}

void ServiceCtrl(DWORD cmd)
{
	switch (cmd) {
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		strcpy(msg1, "Произошла аварийная остановка\n");
		log(msg1);
		ss.dwWin32ExitCode = 0;
		ss.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(ssh, &ss);
		break;
	default:
		SetServiceStatus(ssh, &ss);
		break;
	}
}

void WINAPI ServiceMain()
{
	/* Регистраци обработчика */
	ssh = RegisterServiceCtrlHandler(MY_SERVICE_NAME, (LPHANDLER_FUNCTION)ServiceCtrl);
	if (ssh == 0) {
		strcpy(msg1, "Ошибка регистрации сервиса\n");
		log(msg1);
		CloseHandle(logFile);
		exit(-1);
	}

	/* Установка состояния ожидания запуска */
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState = SERVICE_START_PENDING;
	ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	SetServiceStatus(ssh, &ss);
	strcpy(msg1, "Ожидание запуска сервиса...\n");
	log(msg1);
	/* Предварительно готовимся к ошибке */
	ss.dwCurrentState = SERVICE_STOPPED;
	ss.dwWin32ExitCode = -1;

	/* Устанавливаем сервис в состояние RUNNING */
	ss.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(ssh, &ss);
	strcpy(msg1, "Сервис запущен\n");
	log(msg1);

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	HANDLE hIn, hOut; // дискрипторы входных и выходных файлов
	DWORD nIn, // количество полученной памяти для входных и выходных файлов
		nOut,
		b,            // количество символов в файле
		chetchik = 0; // счетчик для фиксированного количества замен
	CHAR Buffer[MAX_BUF_SIZE], // буфер для входного текста
		a;         //последний символ в строке
	char adres[100]; // адрес и имя выходного файла 
	int k; // кол-во замен
	int zameni = 0; //количество заменившихся символов
	char   szBuf[512];      //Буфер для передачи данных по каналу
	CHAR FileName[100];       //Массив для имени выходного файла
	DWORD  PipeIn, PipeOut;          // кол-во полученной памяти для чтения и записи в канале

	strcpy(msg1, "Ожидание подключения\n");
	log(msg1);
	char   szPipeName[256];
	strcpy(szPipeName, "\\\\.\\pipe\\$MyPipe$");
	// создание файла канала
	hNamedPipe = CreateFile(
		szPipeName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	// Если возникла ошибка, выводим ее код и завершаем работу приложения
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		strcpy(msg1, "Невозможно открыть канал\n");
		log(msg1);
		CloseHandle(logFile);
		GetLastError();
		exit(0);
	}

	strcpy(adres, "C:/Users/Lenovo/source/repos/progrgz/");

	// Выводим сообщение об успешном подключении сервиса к клиенту
	strcpy(msg1, "Клиент подключен к сервису!\n");
	log(msg1);

	strcpy(msg1, "Клиент входит в цикл\n");
	log(msg1);

	// Цикл получения команд через канал
	/* Принимаем клиента, получаем от него сообщение, обрабатываем по правилу и отправляем обратно */
	while (ss.dwCurrentState == SERVICE_RUNNING)
	{
	VvodFile:
		//Алгоритм принятия входных команд
		memset(&szBuf, 0, sizeof(szBuf));
		ReadFile(hNamedPipe, szBuf, MAX_BUF_SIZE, &PipeIn, NULL);
		if (!strcmp(szBuf, "Выход")) {
			strcpy(msg1, "Клиент ввел <Выход>\n");
			log(msg1);
			CloseHandle(logFile);
			CloseHandle(hNamedPipe);
			exit(0);
		}
		hIn = CreateFile(szBuf, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hIn == INVALID_HANDLE_VALUE)
		{
			strcpy(msg1, "Невозможно открыть входной файл:");
			strcat(msg1, szBuf);
			strcat(msg1, "\n");
			log(msg1);
			strcpy(szBuf, "Невозможно открыть входной файл");
			WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);
			goto VvodFile;
		}
		strcpy(msg1, "Введен входной файл:");
		strcat(msg1, szBuf);
		strcat(msg1, "\n");
		log(msg1);
		WriteFile(hNamedPipe, msg1, strlen(msg1), &PipeOut, NULL);

	VvodExitFile:
		//Алгоритм выбора имени выходного файла
		memset(&FileName, 0, sizeof(FileName));
		ReadFile(hNamedPipe, FileName, MAX_BUF_SIZE, &PipeIn, NULL);
		strcat(adres, FileName);
		strcat(adres, ".txt");
		hOut = CreateFile(adres, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
		if (hOut == INVALID_HANDLE_VALUE)
		{
			strcpy(msg1, "Такой файл уже есть в папке <C:/Users/Lenovo/source/repos/progrgz/>:");
			strcat(msg1, FileName);
			strcat(msg1, "\n");
			log(msg1);
			strcpy(szBuf, "Такой файл уже есть");
			WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);
			strcpy(adres, "F:/Prog/");
			goto VvodExitFile;
		}
		strcpy(adres, "C:/Users/lenovo/source/repos/progrgz/");
		strcpy(msg1, "Введен выходной файл в папке <C:/Users/Lenovo/source/repos/progrgz/>:");
		strcat(msg1, FileName);
		strcat(msg1, "\n");
		log(msg1);
		WriteFile(hNamedPipe, msg1, strlen(msg1), &PipeOut, NULL);

		memset(&Buffer, 0, sizeof(Buffer));
		ReadFile(hIn, Buffer, MAX_BUF_SIZE, &nIn, NULL);
		strcpy(msg1, "Получен текст:");
		strcat(msg1, Buffer);
		strcat(msg1, "\n");
		log(msg1);
		b = int(nIn);

		//Принятие кол-во замен
		memset(&szBuf, 0, sizeof(szBuf));
		ReadFile(hNamedPipe, szBuf, MAX_BUF_SIZE, &PipeIn, NULL);
		strcpy(msg1, "Получено кол-во замен:");
		strcat(msg1, szBuf);
		strcat(msg1, "\n");
		log(msg1);
		k = atoi(szBuf);

		//Алгоритм переработки текста
		zameni = 0;
		chetchik = 0;
		for (int i = 0; i < b; i++)
		{
			if (chetchik > k) break;
			else
			{
				if (Buffer[i] < 48) {
					Buffer[i] = ' ';
					chetchik++;
					zameni++;
				}
			}
		}
		_itoa(zameni, szBuf, 10);
		//Вывод кол-ва замен
		WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);

		strcpy(msg1, "Файл после обработки:");
		strcat(msg1, Buffer);
		strcat(msg1, "\n");
		log(msg1);

		strcpy(msg1, "Отправлено клиенту замен:");
		strcat(msg1, szBuf);
		strcat(msg1, "\n");
		log(msg1);
		//Запись в выходной файл
		WriteFile(hOut, Buffer, nIn, &nOut, NULL);
	}
}

int main()
{
	//Создание файла логирования
	logFile = CreateFile(adreslog, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (logFile == INVALID_HANDLE_VALUE)
	{
		strcpy(msg1, "Ошибка создания файла логирования\n");
		log(msg1);
		GetLastError();
		exit;
	}

	/* Таблица точек входа в сервис */
	SERVICE_TABLE_ENTRY table[] =
	{
	{
	(LPSTR)MY_SERVICE_NAME,
	(LPSERVICE_MAIN_FUNCTION)ServiceMain
	},
	{
	NULL,
	NULL
	}
	};

	/* Запускаем сервис с использованием таблицы */
	if (!StartServiceCtrlDispatcher(table)) {
		strcpy(msg1, "Ошибка запуска сервиса\n");
		log(msg1);
		CloseHandle(logFile);
		exit(-1);
	}
}
