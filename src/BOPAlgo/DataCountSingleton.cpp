#include "DataCountSingleton.h"
#include <iostream>
#include <fstream>
#include <iomanip> //setprecision()

#ifdef USING_OPENCASCADE_TEST
using namespace test;
using namespace std;
//init
//bool DataCountSingleton::sm_openSwitch = false;
//int DataCountSingleton::sm_index = 0;
int DataCountSingleton::sm_hasBuild = 0;
std::vector<DataCountSingleton::DataMap> DataCountSingleton::sm_recordData;

void DataCountSingleton::writeToCsvInOne(const std::string& filename)
{
    //merge into one DataMap
    if (sm_recordData.empty())
        return;
    DataMap mergeData = sm_recordData[0];
    const int size = (int)mergeData.m_dataTimeVct.size();
    for (int i = 1; i < sm_recordData.size(); i++)
    {
        for (int j = 0; j < size; j++) //default same size
        {
            if (size == sm_recordData[i].m_dataTimeVct.size() &&
                mergeData.m_dataTimeVct[j].first == sm_recordData[i].m_dataTimeVct[j].first)
                mergeData.m_dataTimeVct[j].second += sm_recordData[i].m_dataTimeVct[j].second;
        }
    }
    //output
    std::ofstream ofsFile(filename);
    //ofsFile.open(filename, std::ios::out | std::ios::out);
    if (!ofsFile.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }
    for (int i = 0; i < mergeData.m_dataTimeVct.size(); i++)
    {
        ofsFile << mergeData.m_dataTimeVct[i].first << ",";
        ofsFile << mergeData.m_dataTimeVct[i].second << std::setprecision(numeric_limits<double>::digits10) << std::endl;
    }
    ofsFile.close();
}

#endif //USING_OPENCASCADE_TEST
