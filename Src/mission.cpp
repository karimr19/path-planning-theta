#include "Mission.h"
#include <iostream>

Mission::Mission()
{
    logger = nullptr;
    file_name = nullptr;
}

Mission::Mission(const char *FileName)
{
    file_name = FileName;
    logger = nullptr;
}

Mission::~Mission()
{
    delete logger;
}

bool Mission::getMap()
{
    return map.getMap(file_name);
}

bool Mission::getConfig()
{
    return config.getConfig(file_name);
}

bool Mission::createLog()
{
    delete logger;
    logger = new XmlLogger(config.LogParams[CN_LP_LEVEL]);
    return logger->getLog(file_name, config.LogParams);
}

void Mission::createEnvironmentOptions()
{
    options = EnvironmentOptions(config.SearchParams[CN_SP_AS], config.SearchParams[CN_SP_AD],
                                 config.SearchParams[CN_SP_CC], config.SearchParams[CN_SP_MT],
                                 config.SearchParams[CN_SP_ST], config.SearchParams[CN_SP_HW]);
}

void Mission::startSearch()
{
    if (options.search_type == 0) {
        search_result = a_star_search.startSearch(map, options);
    } else if (options.search_type == 1) {
        search_result = theta_search.startSearch(map, options);
    } else if (options.search_type == 2) {
        search_result = lazy_theta_search.startSearch(map, options);
    } else {
        search_result = theta_search.startSearch(map, options);
    }
}

void Mission::printSearchResultsToConsole()
{
    std::cout << "Path ";
    if (!search_result.path_found)
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;
    std::cout << "number_of_steps=" << search_result.number_of_steps << std::endl;
    std::cout << "nodes_created=" << search_result.nodes_created << std::endl;
    if (search_result.path_found) {
        std::cout << "path_length=" << search_result.path_length << std::endl;
        std::cout << "pathlength_scaled=" << search_result.path_length * map.getCellSize() << std::endl;
    }
    std::cout << "time=" << search_result.time << std::endl;
}

void Mission::saveSearchResultsToLog() {
    logger->writeToLogSummary(search_result.number_of_steps, search_result.nodes_created, search_result.path_length,
                              search_result.time, map.getCellSize());
    if (search_result.path_found) {
        logger->writeToLogPath(*search_result.primary_path);
        logger->writeToLogHPpath(*search_result.secondary_path);
        logger->writeToLogMap(map, *search_result.primary_path);
    } else
        logger->writeToLogNotFound();
    logger->saveLog();
}

SearchResult Mission::getSearchResult() {
    return search_result;
}

