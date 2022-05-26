#pragma once

#include <zmq.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>

#define TAM_BUFFER 100

void checkIfAlive(std::string requestAddress, char *argv[]);