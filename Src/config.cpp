#include "Config.h"
#include "Constants.h"
#include "Tinyxml2.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

Config::Config()
{
    LogParams = nullptr;
    SearchParams = nullptr;
}

Config::~Config()
{
    delete[] SearchParams;
    delete[] LogParams;
}

bool Config::getConfig(const char *FileName)
{
    std::string value;
    std::stringstream stream;
    tinyxml2::XMLElement *root = nullptr, *algorithm = nullptr, *element = nullptr, *options = nullptr;

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }

    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' element found in XML file!" << std::endl;
        return false;
    }

    algorithm = root->FirstChildElement(CNS_TAG_ALG);
    if (!algorithm) {
        std::cout << "Error! No '" << CNS_TAG_ALG << "' tag found in XML file!" << std::endl;
        return false;
    }

    element = algorithm->FirstChildElement(CNS_TAG_ST);
    if (!element) {
        std::cout << "Error! No '" << CNS_TAG_ST << "' tag found in XML file!" << std::endl;
        return false;
    }
    if (element->GetText())
        value = element->GetText();
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    if (value == CNS_SP_ST_ASTAR || value == CNS_SP_ST_LAZYTH || value == CNS_SP_ST_TH) {
        N = 7;
        SearchParams = new double[N];
        SearchParams[CN_SP_ST] = CN_SP_ST_ASTAR;
        if (value == CNS_SP_ST_LAZYTH)
            SearchParams[CN_SP_ST] = CN_SP_ST_LAZYTH;
        else if (value == CNS_SP_ST_TH)
            SearchParams[CN_SP_ST] = CN_SP_ST_TH;
        element = algorithm->FirstChildElement(CNS_TAG_HW);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_HW << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
            SearchParams[CN_SP_HW] = 1;
        }
        else {
            stream << element->GetText();
            stream >> SearchParams[CN_SP_HW];
            stream.str("");
            stream.clear();

            if (SearchParams[CN_SP_HW] < 1) {
                std::cout << "Warning! Value of '" << CNS_TAG_HW << "' tag is not correctly specified. Should be >= 1."
                          << std::endl;
                std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
                SearchParams[CN_SP_HW] = 1;
            }
        }

        element = algorithm->FirstChildElement(CNS_TAG_MT);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_MT << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'." << std::endl;
            SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
        }
        else {
            if (element->GetText())
                value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CNS_SP_MT_MANH) SearchParams[CN_SP_MT] = CN_SP_MT_MANH;
            else if (value == CNS_SP_MT_EUCL) SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
            else if (value == CNS_SP_MT_DIAG) SearchParams[CN_SP_MT] = CN_SP_MT_DIAG;
            else if (value == CNS_SP_MT_CHEB) SearchParams[CN_SP_MT] = CN_SP_MT_CHEB;
            else {
                std::cout << "Warning! Value of'" << CNS_TAG_MT << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'" << std::endl;
                SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
            }
            if (SearchParams[CN_SP_ST] == CN_SP_ST_TH && SearchParams[CN_SP_MT] != CN_SP_MT_EUCL) {
                std::cout << "Warning! This type of metric is not admissible for Theta*!" << std::endl;
            }
        }
    }
    else {
        std::cout << "Error! Value of '" << CNS_TAG_ST << "' tag (algorithm name) is not correctly specified."
                  << std::endl;
        std::cout << "Supported algorithm's names are: '" << CNS_SP_ST_ASTAR << "', '" << CNS_SP_ST_TH << "', '"
                  << CNS_SP_ST_LAZYTH << "'." << std::endl;
        return false;
    }


    element = algorithm->FirstChildElement(CNS_TAG_AD);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_AD << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_AD << "' was defined to default - true" << std::endl;
        SearchParams[CN_SP_AD] = 1;
    }
    else {
        std::string check;
        stream << element->GetText();
        stream >> check;
        stream.clear();
        stream.str("");

        if (check != "1" && check != "true" && check != "0" && check != "false") {
            std::cout << "Warning! Value of '" << CNS_TAG_AD << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_AD << "' was defined to default - true " << std::endl;
            SearchParams[CN_SP_AD] = 1;
        }
        else if (check == "1" || check == "true")
            SearchParams[CN_SP_AD] = 1;
        else
            SearchParams[CN_SP_AD] = 0;
    }

    if (SearchParams[CN_SP_AD] == 0) {
        SearchParams[CN_SP_CC] = 0;
        SearchParams[CN_SP_AS] = 0;
    }
    else {
        element = algorithm->FirstChildElement(CNS_TAG_CC);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_CC << "' element found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_CC << "' was defined to default - false" << std::endl;
            SearchParams[CN_SP_CC] = 0;
        }
        else {
            std::string check;
            stream << element->GetText();
            stream >> check;
            stream.clear();
            stream.str("");
            if (check != "1" && check != "true" && check != "0" && check != "false") {
                std::cout << "Warning! Value of '" << CNS_TAG_CC << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_CC << "' was defined to default - false" << std::endl;
                SearchParams[CN_SP_CC] = 0;
            }
            else {
                if (check == "1" || check == "true")
                    SearchParams[CN_SP_CC] = 1;
                else
                    SearchParams[CN_SP_CC] = 0;
            }
        }
        if (SearchParams[CN_SP_CC] == 0) {
            SearchParams[CN_SP_AS] = 0;
        }
        else {
            element = algorithm->FirstChildElement(CNS_TAG_AS);
            if (!element) {
                std::cout << "Warning! No '" << CNS_TAG_AS << "' element found in XML file." << std::endl;
                std::cout << "Value of '" << CNS_TAG_AS << "' was defined to default - false." << std::endl;
                SearchParams[CN_SP_AS] = 0;
            }
            else {
                std::string check;
                stream << element->GetText();
                stream >> check;
                stream.clear();
                stream.str("");
                if (check != "1" && check != "true" && check != "0" && check != "false") {
                    std::cout << "Warning! Value of '" << CNS_TAG_AS << "' is not correctly specified." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_AS << "' was defined to default - false." << std::endl;
                    SearchParams[CN_SP_AS] = 0;
                }
                else {
                    if (check == "1" || check == "true")
                        SearchParams[CN_SP_AS] = 1;
                    else
                        SearchParams[CN_SP_AS] = 0;
                }
            }
        }
    }

    options = root->FirstChildElement(CNS_TAG_OPT);
    LogParams = new std::string[3];
    LogParams[CN_LP_PATH] = "";
    LogParams[CN_LP_NAME] = "";

    if (!options) {
        std::cout << "Warning! No '" << CNS_TAG_OPT << "' tag found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1)." << std::endl;
        LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
    }
    else {
        element = options->FirstChildElement(CNS_TAG_LOGLVL);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGLVL << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1)." << std::endl;
            LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
        }
        else {
            stream << element->GetText();
            stream >> value;
            stream.str("");
            stream.clear();
            //std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CN_LP_LEVEL_NOPE_WORD || value == CN_LP_LEVEL_NOPE_VALUE)
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_NOPE_WORD;
            else if (value == CN_LP_LEVEL_TINY_WORD || value == CN_LP_LEVEL_TINY_VALUE)
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_TINY_WORD;
            else if (value == CN_LP_LEVEL_SHORT_WORD || value == CN_LP_LEVEL_SHORT_VALUE)
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
            else if (value == CN_LP_LEVEL_MEDIUM_WORD || value == CN_LP_LEVEL_MEDIUM_VALUE)
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_MEDIUM_WORD;
            else if (value == CN_LP_LEVEL_FULL_WORD || value == CN_LP_LEVEL_FULL_VALUE)
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_FULL_WORD;
            else {
                std::cout << "'" << CNS_TAG_LOGLVL << "' is not correctly specified" << std::endl;
                std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1)." << std::endl;
                LogParams[CN_LP_LEVEL] = CN_LP_LEVEL_SHORT_WORD;
            }
            std::cout << LogParams[CN_LP_LEVEL] << std::endl;
        }
    }
    return true;
}
