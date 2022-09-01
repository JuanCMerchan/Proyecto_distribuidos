#include "monitor.h"

void confirmIfAlive(std::string port)
{
    std::string replyAddress = "tcp://*:" + port;
    void *context = zmq_ctx_new();
    void *reply = zmq_socket(context, ZMQ_REP);
    zmq_bind(reply, replyAddress.c_str());
    char buffer[TAM_BUFFER];
    while (true)
    {
        zmq_recv(reply, buffer, TAM_BUFFER, 0);
        zmq_send(reply, buffer, TAM_BUFFER, 0);
    }
}

void subscribeAndPush(std::string subscriberAddress, std::string pushAddress, MonitorArguments programArguments, MonitorConfig configValues)
{
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    zmq_connect(subscriber, subscriberAddress.c_str());
    char buffer[TAM_BUFFER];
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, programArguments.topic.c_str(), programArguments.topic.size() + 1);
    void *push = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(push, pushAddress.c_str());
    std::vector<long> times;
    long bandwidth = 0;

    while(true)
    {
        zmq_recv(subscriber, buffer, TAM_BUFFER, 0);
        std::string message((char *)buffer);
        std::cout << "\r" << "                        " << "\r";
        std::cout << message << std::endl;
        if(message != programArguments.topic)
        {
            std::vector<std::string> tokens = tokenize(message, " ");
            insertToDBTable(programArguments.topic, tokens[0], tokens[1]);
            times.push_back(std::stol(tokens[1]));
            if(times.back() - times.front() >= 5000000000l)
            {
                bandwidth = times.size() * (TAM_BUFFER + programArguments.topic.size());
                bandwidth /= (times.back() - times.front());
                while(times.back() - times.front() > 5000000000l)
                {
                    times.erase(times.begin());
                }
            }
            std::cout << "Bandwidth: " << bandwidth << " bytes/s";
            std::cout.flush();
            int dato = std::stoi(tokens[0]);
            if(dato < configValues.rangeStart || dato > configValues.rangeEnd)
            {
                strcpy(buffer, std::string(programArguments.topic + " " + tokens[0]).c_str());
                zmq_send(push, buffer, TAM_BUFFER, 0);
            }
            
        }
        
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

bool getArguments(MonitorArguments &programArguments, int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Numero de argumentos invalido" << std::endl;
        return false;
    }

    std::string typeString = argv[1];

    if (typeString == TEMPERATURE_ARG)
    {
        programArguments.topic = TEMPERATURE_ARG;
    }
    else if (typeString == OXYGEN_ARG)
    {
        programArguments.topic = OXYGEN_ARG;
    }
    else if (typeString == PH_ARG)
    {
        programArguments.topic = PH_ARG;
    }
    else
    {
        std::cout << "Argumento 1 invalido" << std::endl;
        return false;
    }

    programArguments.configFile = argv[2];

    programArguments.port = argv[3];

    return true;
}

bool getConfig(std::string configFile, MonitorConfig &configValues)
{
    std::ifstream inputStream;
    std::string line;
    std::vector<std::string> tokens;
    try
    {
        inputStream.open(configFile);
        if (inputStream.is_open())
        {
            while (getline(inputStream, line))
            {
                if (line[0] == '#' && line != "")
                {
                    continue;
                }
                tokens = tokenize(line, "=");
                if (tokens[0] == RANGE_START_TAG)
                {
                    configValues.rangeStart = std::stoi(tokens[1]);
                }
                else if (tokens[0] == RANGE_END_TAG)
                {
                    configValues.rangeEnd = std::stoi(tokens[1]);
                }
                else
                {
                    continue;
                }
            }
            inputStream.close();
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error en la lectura del archivo" << std::endl;
        return false;
    }
    return true;
}

void exitProcess()
{
    connection->close();
}

void createDBTable()
{
    try
    {
        statement->executeUpdate("CREATE TABLE IF NOT EXISTS datos_monitores.datos (tipo VARCHAR(25), valor INT, ts BIGINT)");
    }
    catch(sql::SQLException &e)
    {
        std::cout << "Error creando la tabla" << std::endl;
    }
}

void insertToDBTable(std::string type, std::string value, std::string ts)
{
    statement->executeUpdate("INSERT INTO datos_monitores.datos(tipo, valor, ts) VALUES(\'" + type + "\', \'" + value + "\', \'" + ts + "\')");
}

int main(int argc, char *argv[])
{
    atexit(exitProcess);
    MonitorArguments programArguments;
    if(!getArguments(programArguments, argc, argv))
    {
        return 0;
    }

    MonitorConfig configValues;
    if(!getConfig(programArguments.configFile, configValues))
    {
        return 0;
    }
    std::thread checker (confirmIfAlive, programArguments.port);
    
    std::cout << "Connecting to db" << std::endl;
    sql::Driver *driver = sql::mariadb::get_driver_instance();
    sql::SQLString url(DATABASE_JDBC);
    sql::Properties properties({
        {"user", DATABASE_USER},
        {"password", DATABASE_PASSWORD}
    });

    connection.reset(driver->connect(DATABASE_JDBC, properties));
    statement.reset(connection->createStatement());

    
    std::string subscriberAddress = "tcp://192.168.1.122:2049";
    std::string pushAddress = "tcp://192.168.1.130:2050";
    createDBTable();
    subscribeAndPush(subscriberAddress, pushAddress, programArguments, configValues);   
    checker.join();
}