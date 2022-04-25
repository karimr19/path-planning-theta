
set(HEADERS
        Config.h
        EnvironmentOptions.h
        Constants.h
        Map.h
        Mission.h
        Node.h
        ThetaSearch.h
        SearchResult.h
        Tinyxml2.h
        Xmllogger.h
        LazyThetaSearch.h
        AStarSearch.h)

set(SOURCES
        Config.cpp
        EnvironmentOptions.cpp
        Map.cpp
        Mission.cpp
        ThetaSearch.cpp
        Tinyxml2.cpp
        Xmllogger.cpp
        LazyThetaSearch.cpp
        AStarSearch.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)

add_library(PPLIB ${HEADERS} ${SOURCES})

install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Debug
        DESTINATION Bin/Debug)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        DESTINATION Bin/Release)
