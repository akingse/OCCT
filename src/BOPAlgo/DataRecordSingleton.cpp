#include "DataRecordSingleton.h"
#include <iostream>
#include <fstream>
#include <iomanip> //setprecision()
#include <direct.h> //_getcwd

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

std::string DataRecordSingleton::readBinaryData(const std::string& filename)
{
    std::string res;
    std::ifstream file(filename, std::ios::binary);
    if (!file) 
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return res;
    }
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    res.resize(fileSize);
    if (!file.read(const_cast<char*>(res.data()), fileSize)) 
    {
        std::cerr << "Error reading file: " << filename << std::endl;
        return {};
    }
    return res;
}

void DataRecordSingleton::writeShapeToFile()
{
    setName({});
    char buffer[MAX_PATH];
    std::string path(_getcwd(buffer, sizeof(buffer)));
    for (int i = 0; i < sm_recordData.size(); i++)
    {
        const DataMap& data = sm_recordData[i];
        std::string filename = path + "\\binFile\\shape_std_" + data.m_name + ".txt";
        BRepTools::Write(data.m_shape, filename.c_str());
    }
    return;
}

/// <summary>
/// 在修改代码前，先运行writeShapeToFile函数，生成标准数据，
/// 
/// </summary>
/// <returns></returns>
std::vector<int> DataRecordSingleton::compareBRepFormat()
{
    setName({});
    char buffer[MAX_PATH];
    std::string path(_getcwd(buffer, sizeof(buffer)));
    std::vector<std::string> formatRec;
    std::vector<int> cmpRes;
    for (int i = 0; i < sm_recordData.size(); i++)
    {
        const DataMap& data = sm_recordData[i];
        std::string filenameStd = path + "\\binFile\\shape_std_" + data.m_name + ".txt";
        std::string strStd = readBinaryData(filenameStd);
        if (strStd.empty())
            cmpRes.push_back(i);
        std::string filenameTst = path + "\\binFile\\shape_" + data.m_name + ".txt";
        if (data.m_shape.IsNull())
            cmpRes.push_back(i);
        BRepTools::Write(data.m_shape, filenameTst.c_str());
        std::string strTst = readBinaryData(filenameTst);
        if (strStd != strTst)
            cmpRes.push_back(i);
    }
    return cmpRes;
}

#endif //USING_OPENCASCADE_TEST
