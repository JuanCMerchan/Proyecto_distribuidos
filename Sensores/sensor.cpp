#include "sensor.h"

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

bool getArguments(DataType &dataType, int &time, std::string &configFile, int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Numero de argumentos invalido" << std::endl;
        return false;
    }

    std::string typeString = argv[1];

    if (typeString == TEMPERATURE_ARG)
    {
        dataType = TEMPERATURE;
    }
    else if (typeString == OXYGEN_ARG)
    {
        dataType = OXYGEN;
    }
    else if (typeString == PH_ARG)
    {
        dataType = PH;
    }
    else
    {
        std::cout << "Argumento 1 invalido" << std::endl;
        return false;
    }

    try
    {
        time = std::stoi(argv[2]);
    }
    catch (const std::exception &e)
    {
        std::cout << "Argumento 2 invalido";
        return false;
    }

    configFile = argv[3];
    return true;
}

bool getConfig(std::string configFile, ConfigValues &configValues)
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
                if (line[0] == '#')
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
                else if (tokens[0] == CHANCE_ERROR_TAG)
                {
                    configValues.errorChance = std::stof(tokens[1]);
                }
                else if (tokens[0] == CHANCE_IN_TAG)
                {
                    configValues.rangeChance = std::stof(tokens[1]);
                }
                else if (tokens[0] == CHANCE_OUT_TAG)
                {
                    configValues.outOfRangeChance = std::stof(tokens[1]);
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

int generateData(ConfigValues configValues)
{
    float probability;
    int data;

    data = (rand() % (configValues.rangeEnd - configValues.rangeStart)) + configValues.rangeStart;

    probability = (rand() % 100) / 100.0;

    if (probability >= 0 && probability < configValues.rangeChance)
    {
        return data;
    }
    else if (probability >= configValues.rangeChance && probability < configValues.rangeChance + configValues.outOfRangeChance)
    {
        data += configValues.rangeEnd;
        return data;
    }
    else
    {
        data = data * -1;
        return data;
    }
}

void sendData(ConfigValues configValues, int time, DataType dataType)
{
    void *context = zmq_ctx_new();
    void *push = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(push, "tcp://localhost:6969");
    char buffer[TAM_BUFFER];
    std::string dataString;
    int data;
    while (true)
    {
        data = generateData(configValues);
        switch (dataType)
        {
        case DataType::TEMPERATURE:
            dataString = TEMPERATURE_ARG;
            break;
        
        case DataType::OXYGEN:
            dataString = OXYGEN_ARG;
            break;

        case DataType::PH:
            dataString = PH_ARG;
            break;
        
        default:
            break;
        }
        dataString = dataString + " " + std::to_string(data);
        strcpy(buffer, dataString.c_str());
        zmq_send(push, buffer, TAM_BUFFER, 0);
        sleep(time);
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    DataType dataType;
    int time;
    std::string configFile;
    if (!getArguments(dataType, time, configFile, argc, argv))
    {
        return 0;
    }

    ConfigValues configValues;
    if (!getConfig(configFile, configValues))
    {
        return 0;
    }

    sendData(configValues, time, dataType);
}