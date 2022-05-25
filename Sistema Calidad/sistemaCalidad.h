#pragma

#include <string>
#include <string.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <zmq.h>

#include "SHA256-master/SHA256.h"

#define USERS_FILE "usuarios.txt"
#define TAM_BUFFER 100

struct Usuario
{
    std::string username;
    std::string password;
};

bool authenticate(std::string username, std::string password, std::unordered_map<std::string, std::string> users);
std::string hashPassword(std::string password);
bool login(std::unordered_map<std::string, std::string> users);
void pullAndPrint(std::string pullAddress);
std::vector<std::string> tokenize(std::string s, std::string del);