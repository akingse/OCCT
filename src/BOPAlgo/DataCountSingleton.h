#pragma once
#include <string>
#include <vector>
#include <map>
#include <Standard_Macro.hxx>

//#define USING_OPENCASCADE_TEST //set in project config
#ifdef USING_OPENCASCADE_TEST

//macro
#define MACRO_EXPANSION_TIME_START() \
    timestart = std::chrono::high_resolution_clock::now();
#define MACRO_EXPANSION_TIME_END(name)\
    timeend = std::chrono::high_resolution_clock::now();\
    duration_ms = timeend - timestart;\
    current.m_dataTimeVct.push_back(std::make_pair(name, duration_ms.count()));

namespace test
{
    /// <summary>
    /// For time detial count
    /// </summary>
    class DataCountSingleton
    {
    private:
        Standard_EXPORT DataCountSingleton() = default;
        Standard_EXPORT ~DataCountSingleton() = default;
        DataCountSingleton(const DataCountSingleton&) = delete;
        DataCountSingleton(DataCountSingleton&&) = delete;

    public:
        struct DataMap
        {
            std::string m_name;
            std::map<std::string, int> m_dataCount;
            std::map<std::string, double> m_dataFloat;
            std::map<std::string, double> m_dataTime;
            std::map<std::string, unsigned char> m_dataByte;
            std::vector<std::pair<std::string, double>> m_dataTimeVct; //to keep order
        };

        //static bool sm_openSwitch;
        //static int sm_index;
        Standard_EXPORT static std::vector<DataMap> sm_recordData;

    public:
        static DataCountSingleton& getInstance()
        {
            static DataCountSingleton instance;
            return instance;
        }
        Standard_EXPORT static std::vector<DataMap>& getData()// std::vector<DataCountSingleton::DataMap>
        {
            return sm_recordData;
        }
        Standard_EXPORT static void clear()
        {
            sm_recordData.clear();
            //sm_index = 0;
        }
        Standard_EXPORT static void writeToCsvInOne(const std::string& filename);

        //static void open(bool isOpen=true)
        //{
        //    sm_openSwitch = isOpen;
        //}
        //static bool isOpen()
        //{
        //    return sm_openSwitch;
        //}
        //output

    };
}
#endif ///USING_CONTAINER_STL
