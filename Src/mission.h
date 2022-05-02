#ifndef MISSION_H
#define	MISSION_H

#include "Map.h"
#include "Config.h"
#include "ThetaSearch.h"
#include "SearchResult.h"
#include "EnvironmentOptions.h"
#include "ThetaSearch.h"
#include "Xmllogger.h"
#include "LazyThetaSearch.h"

//That's the wrap up class that first creates all the needed objects (Map, ThetaSearch etc.)
//and then runs the search and then cleans everything up.

//Hint: Create Mission object in the main() function and then use it 1) to retreive all the data from input XML
//2) run the search 3) flush the results to output XML

class Mission
{
    public:
        Mission();
        Mission (const char* fileName);
        ~Mission();

        bool getMap();
        bool getConfig();
        bool createLog();
        void createEnvironmentOptions();
        void startSearch();
        void printSearchResultsToConsole();
        void saveSearchResultsToLog();
        SearchResult getSearchResult();
        
    private:
        Map                     map;
        Config                  config;
        EnvironmentOptions      options;
        ThetaSearch                  theta_search;
        LazyThetaSearch         lazy_theta_search;
        XmlLogger*                logger;
        const char*             file_name;
        SearchResult            search_result;

    void outputResultsToFiles();
};

#endif

