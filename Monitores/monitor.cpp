#include "monitor.h"

int main(int argc, char *argv[])
{
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, "tcp://localhost:42069");
    char buffer[TAM_BUFFER];
    std::string type;
    type = argv[1];
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, type.c_str(), type.size() + 1);
    while(true)
    {
        zmq_recv(subscriber, buffer, TAM_BUFFER, 0);
        std::string message = buffer;
        std::cout << message << std::endl;
    }
}