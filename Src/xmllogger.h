#ifndef XMLLOGGER_H
#define	XMLLOGGER_H
#include "Tinyxml2.h"
#include "Map.h"
#include "Node.h"
#include <unordered_map>
#include <list>


//That's the class that flushes the data to the output XML


class XmlLogger {

public:
    XmlLogger(std::string loglevel) {this->loglevel = loglevel;}

    virtual ~XmlLogger() {};

    bool getLog(const char *FileName, const std::string *LogParams);

    void saveLog();

    void writeToLogMap(const Map &Map, const std::list<Node> &path);

    void writeToLogPath(const std::list<Node> &path);

    void writeToLogHPpath(const std::list<Node> &hppath);

    void writeToLogNotFound();

    void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length, double time, double cellSize);

private:
    std::string LogFileName;
    tinyxml2::XMLDocument doc;
std::string loglevel;
};

#endif

