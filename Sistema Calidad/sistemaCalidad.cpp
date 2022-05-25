#include "sistemaCalidad.h"

void pullAndPrint(std::string pullAddress)
{
    void *context = zmq_ctx_new();
    void *pull = zmq_socket(context, ZMQ_PULL);
    zmq_bind(pull, pullAddress.c_str());
    void *publisher = zmq_socket(context, ZMQ_PUB);
    char buffer[TAM_BUFFER];
    while(true)
    {
        zmq_recv(pull, buffer, TAM_BUFFER, 0);
        std::string message((char*)buffer);
        std::cout << "Alerta!" << std::endl << message << std::endl;
    }
}

std::vector<std::string> tokenize(std::string s, std::string del)
{
    std::vector<std::string> tokens;
    int start = 0;
    int end = s.find(del);
    while (end != -1)
    {
        tokens.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
    tokens.push_back(s.substr(start, s.size() - start));
    return tokens;
}

bool authenticate(std::string username, std::string password, std::unordered_map<std::string, std::string> users)
{
    if(users.find(username)!= users.end())
    {
        if(users[username] == hashPassword(password))
        return true;
    }
    return false;
}

bool login(std::unordered_map<std::string, std::string> users)
{
    std::string username;
    std::string password;
    std::cout << "Nombre de usuario: ";
    std::cin >> username;
    std::cout << "Contrasena: ";
    std::cin >> password;
    return authenticate(username, password, users);
}

std::string hashPassword(std::string password)
{
    SHA256 sha;
    sha.update(password);
    std::unique_ptr<uint8_t> digest(sha.digest());
    return SHA256::toString(digest.get());
}

std::unordered_map<std::string, std::string> readUsers()
{
    std::unordered_map<std::string, std::string> users;
    std::ifstream inputStream;
    std::string line;
    std::vector<std::string> tokens;
    try
    {
        inputStream.open(USERS_FILE);
        if (inputStream.is_open())
        {
            while (getline(inputStream, line))
            {
                if (line[0] == '#'&& line != "")
                {
                    continue;
                }
                tokens = tokenize(line, " ");
                users[tokens[0]] = tokens[1];
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error en la lectura del archivo" << std::endl;
    }
    return users;
}

int main(int argc, char *arv[])
{
    std::string pullAddress = "tcp://*:2050";
    std::unordered_map<std::string, std::string> users = readUsers();
    if(login(users))
    {
        pullAndPrint(pullAddress);
    }
}