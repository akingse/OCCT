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
int DataRecordSingleton::sm_hasBuild = 0;
bool DataRecordSingleton::sm_openTime = false;
bool DataRecordSingleton::sm_openCheck = false;
bool DataRecordSingleton::sm_isAverage = false;
double DataRecordSingleton::sm_toleDist = 1e-6;
double DataRecordSingleton::sm_tolerence = 1e-6;
std::vector<DataRecordSingleton::DataMap> DataRecordSingleton::sm_recordData;
std::vector<DataRecordSingleton::FaceInfo> DataRecordSingleton::sm_recordFace;
static const int _unvalid_id = -1;

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

std::string DataRecordSingleton::readBinToString(const std::string& filename)
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
        return res;
    }
    return res;
}

#ifdef USING_OPENCASCADE_CLASS 
TopoDS_Shape DataRecordSingleton::readBinToShape(const std::string& filename)
{
    TopoDS_Shape Shape;
    BRep_Builder Builder;
    BRepTools::Read(Shape, filename.c_str(), Builder);
    return Shape;
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
        if (data.m_shape != nullptr)
            BRepTools::Write(*data.m_shape, filename.c_str());
    }
    return;
}

/// <summary>
/// 在修改代码前，先运行writeShapeToFile函数，生成标准数据，可以执行多个布尔，将按序号命名；
/// 修改代码后，执行布尔，调用此函数，如果对比BRepFormat不同，则返回对应索引值；
/// </summary>
/// <returns></returns>
std::vector<int> DataRecordSingleton::compareBRepFormat()
{
    setName({});
    char buffer[MAX_PATH];
    std::string path(_getcwd(buffer, sizeof(buffer)));
    std::vector<std::string> formatRec;
    std::vector<int> strRes;
    std::vector<int> cmpRes;
    for (int i = 0; i < sm_recordData.size(); i++)
    {
        const DataMap& data = sm_recordData[i];
        if (data.m_shape == nullptr)
            continue;
        std::string filenameStd = path + "\\binFile\\shape_std_" + data.m_name + ".txt";
        //compare Shape
        TopoDS_Shape shapeStd = readBinToShape(filenameStd);
        if (!data.m_shape->IsEqual(shapeStd))
            cmpRes.push_back(i);
        //compare string
        std::string strStd = readBinToString(filenameStd);
        std::string filenameTst = path + "\\binFile\\shape_" + data.m_name + ".txt";
        BRepTools::Write(*data.m_shape, filenameTst.c_str());
        std::string strTst = readBinToString(filenameTst);
        if (strStd != strTst)
            strRes.push_back(i);
    }
    return cmpRes;
}
#endif //USING_OPENCASCADE_CLASS

//DataCompareSingleton
template<class T>
std::vector<T> mapvalue2vector(const std::map<std::string, T>& data)
{
	std::vector<T> res;
	for (const auto& iter : data)
		res.push_back(iter.second);
	return res;
}
template<class T>
std::vector<string> mapkey2vector(const std::map<std::string, T>& data)
{
	std::vector<string> res;
	for (const auto& iter : data)
		res.push_back(iter.first);
	return res;
}

vector<DataRecordSingleton::DataMap> DataRecordSingleton::compareDataMap(const vector<DataRecordSingleton::DataMap>& stdDataRead)
{
	if (stdDataRead.size() != sm_recordData.size())
		return {};
	std::set<int> idDifferent;
	std::vector<DataRecordSingleton::DataMap> dataDifferent;
	for (int i = 0; i < stdDataRead.size(); i++)
	{
		const DataRecordSingleton::DataMap& read = stdDataRead[i]; //从Json中读取的标准数据
		const DataRecordSingleton::DataMap& test = sm_recordData[i]; //当前测试计算的数据
		std::vector<int> readDataCount = mapvalue2vector(read.m_dataCount);
		std::vector<int> testDataCount = mapvalue2vector(test.m_dataCount);
		std::vector<double> readDataFloat = mapvalue2vector(read.m_dataFloat);
		std::vector<double> testDataFloat = mapvalue2vector(test.m_dataFloat);
		std::vector<Point3d> readDataPoint = mapvalue2vector(read.m_dataPoint);
		std::vector<Point3d> testDataPoint = mapvalue2vector(test.m_dataPoint);
		std::vector<std::string> keyDataCount = mapkey2vector(read.m_dataCount);
		std::vector<std::string> keyDataFloat = mapkey2vector(read.m_dataFloat);
		std::vector<std::string> keyDataPoint = mapkey2vector(read.m_dataPoint);
		DataRecordSingleton::DataMap dataDiff; //empty name
		for (int j = 0; j < min(readDataCount.size(), testDataCount.size()); j++)
		{
			if (readDataCount[j] != testDataCount[j])
			{
				dataDiff.m_name = test.m_name;
				dataDiff.m_dataCount[keyDataCount[j]] = testDataCount[j];
				idDifferent.insert(i);
			}
			else
				dataDiff.m_dataCount[keyDataCount[j]] = _unvalid_id;// INT_MAX;
		}
		for (int j = 0; j < min(readDataFloat.size(), testDataFloat.size()); j++)
		{
			if (sm_tolerence < fabs(readDataFloat[j] - testDataFloat[j]))
			{
				dataDiff.m_name = test.m_name;
				dataDiff.m_dataFloat[keyDataFloat[j]] = testDataFloat[j];
				idDifferent.insert(i);
			}
			else
				dataDiff.m_dataFloat[keyDataFloat[j]] = std::nan("0");
		}
		for (int j = 0; j < min(readDataPoint.size(), testDataPoint.size()); j++)
		{
			if (!readDataPoint[j].isEqual(testDataPoint[j], sm_tolerence))
			{
				dataDiff.m_name = test.m_name;
				dataDiff.m_dataPoint[keyDataPoint[j]] = testDataPoint[j];
				idDifferent.insert(i);
			}
			else
				dataDiff.m_dataPoint[keyDataCount[j]] = Point3d{ std::nan("0"), std::nan("0"), std::nan("0") };
		}
		if (!dataDiff.m_name.empty())
			dataDifferent.push_back(dataDiff);
	}
	return dataDifferent;
}

#endif //USING_OPENCASCADE_TEST
