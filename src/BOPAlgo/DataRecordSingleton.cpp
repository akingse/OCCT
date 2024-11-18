#include "DataRecordSingleton.h"
#include <iostream>
#include <fstream>
#include <iomanip> //setprecision()

#ifdef USING_OPENCASCADE_TEST
using namespace test;
using namespace std;
//init
//int DataRecordSingleton::sm_index = 0;
bool DataRecordSingleton::sm_open = true;
int DataRecordSingleton::sm_hasBuild = 0;
bool DataRecordSingleton::sm_isAverage = false;
std::vector<DataRecordSingleton::DataMap> DataRecordSingleton::sm_recordData;

void DataRecordSingleton::writeToCsvInOne(const std::string& filename)
{
    //merge into one DataMap
    if (sm_recordData.empty())
        return;
    DataMap mergeData = sm_recordData[0];//copy
    const int time = (int)sm_recordData.size();
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
    if (sm_isAverage && time != 1)
    {
        for (int j = 0; j < size; j++)
            mergeData.m_dataTimeVct[j].second /= time;
    }
    //output
    std::ofstream ofsFile(filename);
    //ofsFile.open(filename, std::ios::out | std::ios::out);
    if (!ofsFile.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }
    ofsFile << "function" << "," << "time/ms" << endl;
    for (int i = 0; i < mergeData.m_dataTimeVct.size(); i++)
    {
        ofsFile << mergeData.m_dataTimeVct[i].first << ",";
        ofsFile << mergeData.m_dataTimeVct[i].second << std::setprecision(numeric_limits<double>::digits10) << std::endl;
    }
    ofsFile.close();
}

#endif //USING_OPENCASCADE_TEST
