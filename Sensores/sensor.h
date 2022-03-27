#include <unistd.h>
#include <fstream>

#include "shared.h"

#define RANGE_START_TAG "inicio_rango"
#define RANGE_END_TAG "fin_rango"
#define CHANCE_ERROR_TAG "chance_error"
#define CHANCE_IN_TAG "chance_en_rango"
#define CHANCE_OUT_TAG "chance_fuera_rango"
#define TEMPERATURE_ARG "temperatura"
#define OXYGEN_ARG "oxigeno"
#define PH_ARG "ph"

enum DataType {TEMPERATURE, OXYGEN, PH};

struct SensorConfig
{
    int rangeStart;
    int rangeEnd;
    float errorChance;
    float rangeChance;
    float outOfRangeChance;
};

struct SensorArguments
{
    int time;
    std::string configFile;
    DataType type;
};

std::vector<std::string> tokenize(std::string s, std::string del);
bool getArguments(SensorArguments &programArguments, int argc, char *argv[]);
bool getConfig(std::string configFile, SensorConfig &configValues);
int generateData(SensorConfig configValues);
void sendData(SensorConfig configValues, int time, DataType dataType, std::string pushAddress);