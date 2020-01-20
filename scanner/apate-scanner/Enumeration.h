#pragma once

#include "Headers.h"
#include <fstream>

void print_handle_information();

NTSTATUS enumerate_handles(ENUM_HANDLE_CALLBACK callback);


