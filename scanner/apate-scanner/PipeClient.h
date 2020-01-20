#pragma once
#include <Windows.h>
#include <iostream>
#include <string> 

class PipeClient {
private:
	HANDLE hPipe;
	int bufferSize;
public:
	PipeClient(std::string name);
	~PipeClient();
	bool writePipe(std::string message);
	std::string readPipe();
};