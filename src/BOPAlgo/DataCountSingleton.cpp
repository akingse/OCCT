#include "DataCountSingleton.h"

#ifdef USING_OPENCASCADE_TEST
using namespace test;
//init
bool DataCountSingleton::sm_openSwitch = false;
int DataCountSingleton::sm_index = 0;
std::vector<DataCountSingleton::DataMap> DataCountSingleton::sm_recordData;

void DataCountSingleton::writeToCsv(const std::string& filename)
{

}

#endif //USING_OPENCASCADE_TEST
