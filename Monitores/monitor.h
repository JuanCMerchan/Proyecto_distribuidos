#pragma once

#include <zmq.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <string.h>
#include <memory>
#include <mariadb/conncpp.hpp>
#include <thread>

#define RANGE_START_TAG "inicio_rango"
#define RANGE_END_TAG "fin_rango"
#define TEMPERATURE_ARG "temperatura"
#define OXYGEN_ARG "oxigeno"
#define PH_ARG "ph"
#define TAM_BUFFER 100
#define DATABASE_JDBC "jdbc:mariadb://localhost:3306/datos_monitores"
#define DATABASE_USER "distribuidos"
#define DATABASE_PASSWORD "distribuidos"

struct MonitorConfig
{
    int rangeStart;
    int rangeEnd;
};

struct MonitorArguments
{
    std::string topic;
    std::string configFile;
    std::string port;
};

std::unique_ptr<sql::Connection> connection;
std::shared_ptr<sql::Statement> statement;

void confirmIfAlive();
void createDBTable();
void exitProcess();
bool getArguments(MonitorArguments &programArguments, int argc, char *argv[]);
bool getConfig(std::string configFile, MonitorConfig &configValues);
void insertToDBTable(std::string type, std::string value, std::string ts);
void subscribeAndPush(std::string subscriberAddress, std::string pushAddress, MonitorArguments programArguments, MonitorConfig configValues);
std::vector<std::string> tokenize(std::string s, std::string del);