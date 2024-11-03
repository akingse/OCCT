#include "DataCountSingleton.h"
using namespace occ;
//init
bool DataCountSingleton::sm_openSwitch = false;
std::vector<DataCountSingleton::DataMap> DataCountSingleton::sm_recordData;
