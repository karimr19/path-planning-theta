#ifndef CONFIG_H
#define	CONFIG_H
#include <string>

class Config
{
    public:
        Config();
        ~Config();
        bool getConfig(const char *FileName);

    public:
        double*         SearchParams;
        std::string*    LogParams;
        unsigned int    N;

};

#endif

