#pragma once
#include <string>
#include <vector>
#include <map>

//#define USING_OPENCASCADE_TEST //set in project config
#ifdef USING_OPENCASCADE_TEST
namespace test
{
    /// <summary>
    /// For time detial count
    /// </summary>
    class DataCountSingleton
    {
    private:
        DataCountSingleton() = default;
        ~DataCountSingleton() = default;
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

        };
        static bool sm_openSwitch;
        static int sm_index;
        static std::vector<DataMap> sm_recordData;

    public:
        static DataCountSingleton& getInstance()
        {
            static DataCountSingleton instance;
            return instance;
        }
        static void clear()
        {
            sm_recordData.clear();
            sm_index = 0;
        }
        static std::vector<DataMap>& getData()// std::vector<DataCountSingleton::DataMap>
        {
            return sm_recordData;
        }
        static void open(bool isOpen=true)
        {
            sm_openSwitch = isOpen;
        }
        static bool isOpen()
        {
            return sm_openSwitch;
        }
        //output
        static void writeToCsv(const std::string& filename);

    };
}
#endif ///USING_CONTAINER_STL

