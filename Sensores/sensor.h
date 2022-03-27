#include <zmq.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "constantes.h"

#define RANGE_START_TAG "inicio_rango"
#define RANGE_END_TAG "fin_rango"
#define CHANCE_ERROR_TAG "chance_error"
#define CHANCE_IN_TAG "chance_en_rango"
#define CHANCE_OUT_TAG "chance_fuera_rango"
#define TEMPERATURE_ARG "temperatura"
#define OXYGEN_ARG "oxigeno"
#define PH_ARG "ph"

enum DataType {TEMPERATURE, OXYGEN, PH};

struct ConfigValues
{
    int rangeStart;
    int rangeEnd;
    float errorChance;
    float rangeChance;
    float outOfRangeChance;
};

std::vector<std::string> tokenize(std::string s, std::string del);
std::vector<std::string> tokenize(std::string s, std::string del);
bool getArguments(DataType &dataType, int &time, std::string &configFile, int argc, char *argv[]);
bool getConfig(std::string configFile, ConfigValues &configValues);
int generateData(ConfigValues configValues);