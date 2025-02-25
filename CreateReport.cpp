#include "CreateReport.h"

CreateReport::CreateReport()
{
}

StatusConnection CreateReport::sendRequest()
{
	const char SERVER_ADDRESS[] = "192.168.56.108";
	const u_short SERVER_PORT = 30000;
	const short BUFF_SIZE = 1024;

	int status = -1;

	WSADATA wsData;
	status = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (status != 0)
	{
		std::cout << WSAGetLastError();
		return StatusConnection::SOCKET_INIT;
	}

	// Socket initialization
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		std::cout << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return StatusConnection::INVAL_SOCKET;
	}

	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_ADDRESS, &ip_to_num);

	// Establishing a connection
	sockaddr_in sockAddr;

	ZeroMemory(&sockAddr, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr = ip_to_num;
	sockAddr.sin_port = htons(SERVER_PORT);

	for (int i = 0; i < 3; i++)
	{
		std::cout << "Try to connect #" << i << std::endl;
		status = connect(sock, (sockaddr*)&sockAddr, sizeof(sockAddr));

		if (status != 0)
		{
			Sleep(10000); // 10 секунд
			continue;
		}
		else
		{
			break;
		}
	}

	if (status != 0)
	{
		wchar_t* lpMsgBuf;

		DWORD dw = GetLastError();

		if (!FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL) == 0)
		{
			MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK);
		}

		closesocket(sock);
		WSACleanup();
		return StatusConnection::CONNECT_ERROR;
	}

	short packetSize = 0;

	std::ofstream out;

	int vecSize = Procedures::listProcedures.size();
	while (true)
	{
		int choice = vecSize + 1;
		while (true)
		{
			for (int i = 0; i < vecSize; i++)
			{
				std::cout << i << " = " << Procedures::listProcedures[i] << std::endl;
			}
			std::cout << "Use \"-1\" to exit" << std::endl;

			std::cin >> choice;

			if (choice == -1)
			{
				std::cout << "Closing connection..." << std::endl;
				shutdown(sock, SD_BOTH);
				closesocket(sock);
				WSACleanup();
				return StatusConnection::SUCCESS;
			}
			else if (choice > vecSize - 1)
			{
				system("cls");
				std::cout << "Try again" << std::endl;
			}
			else
			{
				break;
			}
		}
		std::cout << std::endl;

		std::string procedure = Procedures::listProcedures[choice];
		std::cout << "You have choosen " << procedure << std::endl;

		size_t totalSent = procedure.size();

		packetSize = send(sock, procedure.c_str(), totalSent, 0);

		if (packetSize == SOCKET_ERROR)
		{
			std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			return StatusConnection::TRANSFER_ERROR;
		}

		int32_t length = 0;
		int res = recv(sock, reinterpret_cast<char*>(&length), sizeof(length), 0);

		int32_t i = length;

		std::vector<char> xmlFile;

		while (i != 0)
		{
			std::vector <char> inBuff(BUFF_SIZE);

			packetSize = recv(sock, inBuff.data(), inBuff.size(), 0);

			xmlFile.insert(xmlFile.end(), inBuff.begin(), inBuff.end());

			i -= packetSize;

		}

		if (packetSize == SOCKET_ERROR)
		{
			std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			return StatusConnection::TRANSFER_ERROR;
		}
		else
		{
			std::string fileName = Procedures::listFiles[choice];
			out.open(fileName);

			for (int i = 0; i < xmlFile.size(); i++)
			{
				if (xmlFile[i] == '\0')
				{
					continue;
				}
				out << xmlFile[i];
			}
			break;
		}
	}
	out.close();

	closesocket(sock);
	WSACleanup();

	return StatusConnection::SUCCESS;
}