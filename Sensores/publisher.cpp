#include "publisher.h"

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

void pullAndPublish(std::string pullAddress, std::string publishAddress)
{
    void *context = zmq_ctx_new();
    void *pull = zmq_socket(context, ZMQ_PULL);
    zmq_bind(pull, pullAddress.c_str());
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, publishAddress.c_str());
    char buffer[TAM_BUFFER];
    std::vector<std::string> tokens;
    
    while (true)
    {
        zmq_recv(pull, buffer, TAM_BUFFER, 0);
        std::string message((char*)buffer);
        //std::cout << message << std::endl;
        tokens = tokenize(message, " ");
        int size = tokens[0].size();
        zmq_send(publisher, tokens[0].c_str(), tokens[0].size() + 1, ZMQ_SNDMORE);
        strcpy(buffer, std::string(tokens[1] + " " + tokens[2]).c_str());
        zmq_send(publisher, buffer, TAM_BUFFER, 0);
    }
}

int main(int argc, char *argv[])
{
    std::string pullAddress = "tcp://*:2048";
    std::string publishAddress = "tcp://*:2049";
    pullAndPublish(pullAddress, publishAddress);
}