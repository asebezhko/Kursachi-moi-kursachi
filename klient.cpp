#include windows.h
#include stdio.h
#include conio.h
#include iostream
#define MAX_BUF_SIZE 256

using namespace std;

Имя создаваемого канала Pipe
LPSTR  lpszPipeName = (LPSTR).pipe$MyPipe$;
Флаг успешного создания канала
BOOL   fConnected;

DWORD main(int argc, char argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Идентификатор канала Pipe
		HANDLE hNamedPipe;
	Буфер для передачи данных
		char   szBuf[256];
	char   CharNumber[8];
	Буфер для имени канала Pipe
		char   szPipeName[256];
	DWORD PipeOut, PipeIn;
	int NumberFile = 1,
		GlobalZameni = 0, кол - во замен во всех файлах
		Zameni = 0;
	cout  Клиент запущен... n;
	strcpy(szPipeName, .pipe$MyPipe$);

	cout  Создание канала...n;


	Создание канала
		hNamedPipe = CreateNamedPipe(
			lpszPipeName,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE  PIPE_READMODE_MESSAGE  PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			512, 512, 5000, NULL);
	Если возникла ошибка, выводим ее код и зваершаем работу приложения
		if (hNamedPipe == INVALID_HANDLE_VALUE)
		{
			cout  Ошибка создания каналаn;
			GetLastError();
			exit;
		}

	cout  Создание канала прошло успешно.Ожидаем подключение сервера.n;

	Ожидаем соединения со стороны сервиса
		fConnected = ConnectNamedPipe(hNamedPipe, NULL);
	При возникновении ошибки выводим ее код
		if (!fConnected)
		{
			switch (GetLastError())
			{
				case ERROR_NO_DATA
					cout  ConnectNamedPipe ERROR_NO_DATAn;
					CloseHandle(hNamedPipe);
					exit;
					case ERROR_PIPE_CONNECTED
						cout  ConnectNamedPipe ERROR_PIPE_CONNECTEDn;
						CloseHandle(hNamedPipe);
						exit;
						case ERROR_PIPE_LISTENING
							cout  ConnectNamedPipe ERROR_PIPE_LISTENINGn;
							CloseHandle(hNamedPipe);
							exit;
							case ERROR_CALL_NOT_IMPLEMENTED
								cout  ConnectNamedPipe ERROR_CALL_NOT_IMPLEMENTEDn;
								CloseHandle(hNamedPipe);
								exit;
								default
									cout  ConnectNamedPipe Error% ldn;
								CloseHandle(hNamedPipe);
								exit;
			}
			CloseHandle(hNamedPipe);
			exit;
		}

	cout  Клиент присоединился к серверу.n;
	cout  _________________________________________________n;
	cout  Правила пользования клиентомn;
	cout  1)Вводите ссылку на файл для обработкиn;
	cout  2)Вводите Выход для выхода из приложенияn;
	cout  3)Вводите Замены для того, чтобы узнать кол - во замен уже произошедшихn;
	cout  _______________________________________________________n;

	Цикл обмена данными с серверным процессом
		while (true)
		{
			Start
				memset(&szBuf, 0, sizeof(szBuf));
			memset(&CharNumber, 0, sizeof(CharNumber));
			_itoa(NumberFile, CharNumber, 10);
			strcat(CharNumber, ));
			cout  CharNumber;
			NumberFile++;

			VvodFile
				cout  Введите команду;
			cin  szBuf;
			if (!strcmp(szBuf, Выход)) {
				cout  Общее кол - во замен  GlobalZameni;
				strcpy(szBuf, Выход);
				WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);
				CloseHandle(hNamedPipe);
				return 0;
			}
			if (!strcmp(szBuf, Замены)) {
				cout  На данный момент общее кол - во замен  GlobalZameni  n;
				goto VvodFile;
			}
			WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);
			memset(&szBuf, 0, sizeof(szBuf));
			ReadFile(hNamedPipe, szBuf, MAX_BUF_SIZE, &PipeIn, NULL);
			if (!strcmp(szBuf, Невозможно открыть входной файл)) {
				cout  szBuf  n;
				goto VvodFile;
			}
			cout  szBuf;
			cout  _________________________________________________n;

			VvodExitFile
				cout  Введите выходной файл;
			cin  szBuf;
			WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);
			memset(&szBuf, 0, sizeof(szBuf));
			ReadFile(hNamedPipe, szBuf, MAX_BUF_SIZE, &PipeIn, NULL);
			if (!strcmp(szBuf, Такой файл уже есть)) {
				cout  szBuf  n;
				goto VvodExitFile;
			}
			cout  szBuf;
			cout  _________________________________________________n;

			kolZameni
				cout  Введите кол - во замен;
			cin  szBuf;
			for (int g = 0; g strlen(szBuf); g++) {
				if ((szBuf[g]  48)  (szBuf[g]  57)) {
					cout  Введено не число!n;
					goto kolZameni;
				}
			}
			WriteFile(hNamedPipe, szBuf, strlen(szBuf), &PipeOut, NULL);
			memset(&szBuf, 0, sizeof(szBuf));
			cout  _________________________________________________n;

			ReadFile(hNamedPipe, szBuf, MAX_BUF_SIZE, &PipeIn, NULL);
			cout  Замен в выбранном файле  szBuf  n;
			GlobalZameni = GlobalZameni + atoi(szBuf);
			cout  _______________________________________________________n;
			goto Start;
		}
}
