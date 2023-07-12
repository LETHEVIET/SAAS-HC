#include "algo_config.h"

int_fast8_t verbose = 1;

bool node_clustering_flag = false;

#if ADAPT_RHO_MACRO
bool adaptive_evaporation_flag = true;
#else
bool adaptive_evaporation_flag = false;
#endif

#if LS_N2_MACRO
bool ls_n_square_flag = true;
#else
bool ls_n_square_flag = false;
#endif

#if O1_EVAP_MACRO
bool o1_evap_flag = true;
#else
bool o1_evap_flag = false;
#endif

#if ES_ANT_MACRO
bool es_ant_flag = true;
#else
bool es_ant_flag = false;
#endif

#if TREE_MAP_MACRO
bool tree_map_flag = true;
#else
bool tree_map_flag = false;
#endif

// not config here
int cmaes_flag;      // 0 or 1
int ipopcmaes_flag;  // 0 or 1
int bipopcmaes_flag; // 0 or 1
int iGreedyLevyFlag; // 0 or 1
