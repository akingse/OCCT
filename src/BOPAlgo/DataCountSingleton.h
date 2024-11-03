#pragma once
#include<string>
#include<vector>
#include<map>

namespace occ
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
            std::vector<std::string, int> m_dataCount;
            std::vector<std::string, double> m_dataFloat;
            std::vector<std::string, double> m_dataTime;
            std::vector<std::string, unsigned char> m_dataByte;

        };
        static bool sm_openSwitch;
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
        }
        static void open(bool isOpen=true)
        {
            sm_openSwitch = isOpen;
        }
        static bool isOpen()
        {
            return sm_openSwitch;
        }
    };
}

