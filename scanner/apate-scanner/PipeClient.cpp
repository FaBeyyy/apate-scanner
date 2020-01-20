#include "PipeClient.h"
#include <fstream>
#include <string>

PipeClient::PipeClient(std::string name) {
	this->bufferSize = 4096;
	while (true)
	{
		auto hPipe = CreateFile(
			name.c_str(),   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

	  // Break if the pipe handle is valid. 
		

		if (hPipe != INVALID_HANDLE_VALUE) {
			this->hPipe = hPipe;
			break;
		}
		

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			break;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(name.c_str(), 20000))
		{
			break;
		}
	}
}

bool PipeClient::writePipe(std::string message) {
	const char * messageToSend = message.c_str();
	DWORD numberOfBytesWritten;

	bool fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		messageToSend,             // message 
		message.length() * sizeof(char),              // message length 
		&numberOfBytesWritten,             // bytes written 
		NULL);   
	// not overlapped 
	return fSuccess;
}



std::string PipeClient::readPipe() {
	TCHAR message[4096];
	DWORD numberOfBytesWritten;

	bool fSuccess = ReadFile(
			hPipe,    // pipe handle 
			&message,    // buffer to receive reply 
			4096 * sizeof(char),  // size of buffer 
			&numberOfBytesWritten,  // number of bytes read 
			NULL);    // not overlapped 

	if (!fSuccess || numberOfBytesWritten == 0)
	{
		return false;
	}

	return std::string(message);
}

PipeClient::~PipeClient() {
	DisconnectNamedPipe(this->hPipe);
}