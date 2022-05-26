#include "checker.h"

void checkIfAlive(std::string requestAddress, char *argv[])
{
    void *context = zmq_ctx_new();
    void *request = zmq_socket(context, ZMQ_REQ);
    zmq_connect(request, requestAddress.c_str());
    char buffer[TAM_BUFFER];
    zmq_pollitem_t items[1];
    items[0].socket = request;
    items[0].events = ZMQ_POLLIN;
    char argument[100] = "./monitor ";
    strcat(argument, argv[1]);
    strcat(argument, " ");
    strcat(argument, argv[2]);
    strcat(argument, " ");
    strcat(argument, argv[3]);
    char *const args[6] = {"gnome-terminal", "--window", "-x", "bash", "-c", argument};
    int rc = 0;
    while(true)
    {
        zmq_send(request, buffer, TAM_BUFFER, 0);
        rc = zmq_poll(items, 1, 2000);
        if(rc == 0)
        {
            pid_t pid = fork();
            if(pid == 0)
            {
                execvp("gnome-terminal", args);
            }
        }
        zmq_recv(request, buffer, TAM_BUFFER, 0);
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    std::string port(argv[3]);
    std::string requestAddress = "tcp://localhost:" + port;
    if(argc != 4)
    {
        std::cout << "Numero de argumentos invalido" << std::endl;
        return 0;
    }

    checkIfAlive(requestAddress, argv);
}