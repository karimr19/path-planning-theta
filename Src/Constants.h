#ifndef GL_CONST_H
#define	GL_CONST_H

#define CN_SQRT_TWO    1.41421356237

//XML tags
#define CNS_TAG_ROOT "root"

    #define CNS_TAG_MAP             "map"
        #define CNS_TAG_CELLSIZE    "cellsize"
        #define CNS_TAG_WIDTH       "width"
        #define CNS_TAG_HEIGHT      "height"
        #define CNS_TAG_STX         "startx"
        #define CNS_TAG_STY         "starty"
        #define CNS_TAG_FINX        "finishx"
        #define CNS_TAG_FINY        "finishy"
        #define CNS_TAG_GRID        "grid"
            #define CNS_TAG_ROW     "row"

    #define CNS_TAG_ALG             "algorithm"
        #define CNS_TAG_ST          "search_type"
        #define CNS_TAG_HW          "h_weight"
        #define CNS_TAG_MT          "metric_type"
        #define CNS_TAG_BT          "less_g_value_second"
        #define CNS_TAG_AS          "allow_squeeze"
        #define CNS_TAG_AD          "allow_diagonal"
        #define CNS_TAG_CC          "cut_corners"

    #define CNS_TAG_OPT             "options"
        #define CNS_TAG_LOGLVL      "loglevel"

    #define CNS_TAG_LOG             "log"
        #define CNS_TAG_MAPFN       "mapfilename"
        #define CNS_TAG_SUM         "summary"
        #define CNS_TAG_PATH        "path"
        #define CNS_TAG_LPLEVEL     "lplevel"
        #define CNS_TAG_HPLEVEL     "hplevel"
            #define CNS_TAG_SECTION "section"
        #define CNS_TAG_LOWLEVEL    "lowlevel"
            #define CNS_TAG_POINT   "node"

//XML tags' attributes
    #define CNS_TAG_ATTR_NUMOFSTEPS     "number_of_steps"
    #define CNS_TAG_ATTR_NODESCREATED   "nodes_created"
    #define CNS_TAG_ATTR_LENGTH         "length"
    #define CNS_TAG_ATTR_LENGTH_SCALED  "length_scaled"
    #define CNS_TAG_ATTR_TIME           "time"
    #define CNS_TAG_ATTR_X              "x"
    #define CNS_TAG_ATTR_Y              "y"
    #define CNS_TAG_ATTR_NUM            "number"
    #define CNS_TAG_ATTR_STX            "start.x"
    #define CNS_TAG_ATTR_STY            "start.y"
    #define CNS_TAG_ATTR_FINX           "finish.x"
    #define CNS_TAG_ATTR_FINY           "finish.y"


//ThetaSearch Parameters
    #define CN_SP_ST 0

        #define CNS_SP_ST_ASTAR         "astar"
        #define CNS_SP_ST_TH            "thetastar"
        #define CNS_SP_ST_LAZYTH        "lazythetastar"

        #define CN_SP_ST_ASTAR          0
        #define CN_SP_ST_TH             1
        #define CN_SP_ST_LAZYTH         2

    #define CN_SP_AD 1 //AllowDiagonal

    #define CN_SP_CC 2 //CutCorners

    #define CN_SP_AS 3 //AllowSqueeze

    #define CN_SP_MT 4 //MetricType

        #define CNS_SP_MT_DIAG  "diagonal"
        #define CNS_SP_MT_MANH  "manhattan"
        #define CNS_SP_MT_EUCL  "euclidean"
        #define CNS_SP_MT_CHEB  "chebyshev"

        #define CN_SP_MT_DIAG   0
        #define CN_SP_MT_MANH   1
        #define CN_SP_MT_EUCL   2
        #define CN_SP_MT_CHEB   3

    #define CN_SP_HW 5 //HeuristicWeight


    //Log Configuration
    #define CN_LP_LEVEL 0

        #define CN_LP_LEVEL_NOPE_VALUE      "0"
        #define CN_LP_LEVEL_NOPE_WORD       "none"
        #define CN_LP_LEVEL_TINY_VALUE      "0.5"
        #define CN_LP_LEVEL_TINY_WORD       "tiny"
        #define CN_LP_LEVEL_SHORT_VALUE     "1"
        #define CN_LP_LEVEL_SHORT_WORD      "short"
        #define CN_LP_LEVEL_MEDIUM_VALUE    "1.5"
        #define CN_LP_LEVEL_MEDIUM_WORD     "medium"
        #define CN_LP_LEVEL_FULL_VALUE      "2"
        #define CN_LP_LEVEL_FULL_WORD       "full"

    #define CN_LP_PATH 1
    #define CN_LP_NAME 2


//Grid Cell
    #define CN_GC_NOOBS 0
    #define CN_GC_OBS   1

//Other
    #define CNS_OTHER_PATHSELECTION     "*"
    #define CNS_OTHER_MATRIXSEPARATOR   ' '

#endif

