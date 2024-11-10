#pragma once
#include <string>
#include <vector>
#include <map>
//#include <Standard_Macro.hxx>

//#define USING_OPENCASCADE_TEST //set in project config
#ifdef USING_OPENCASCADE_TEST
//custom export macro
#ifdef OPENCASCADE_TEST_EXPORTS
#define OPENCASCADE_TEST_API __declspec(dllexport)
#else
#define OPENCASCADE_TEST_API __declspec(dllimport)
#endif

/*
  //communal
  std::chrono::steady_clock::time_point timestart;
  std::chrono::steady_clock::time_point timeend;
  std::chrono::duration<double, std::milli> duration_ms; // milli=ratio<1, 1000> second
  test::DataCountSingleton& instance = test::DataCountSingleton::getInstance();
  //respective

*/

//macro expand
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
        OPENCASCADE_TEST_API DataCountSingleton() = default;
        OPENCASCADE_TEST_API ~DataCountSingleton() = default;
        DataCountSingleton(const DataCountSingleton&) = delete;
        DataCountSingleton(DataCountSingleton&&) = delete;

    public:
        struct DataMap
        {
            //int m_index;
            std::string m_name;
            std::map<std::string, int> m_dataCount;
            std::map<std::string, double> m_dataFloat;
            std::map<std::string, double> m_dataTime;
            std::map<std::string, unsigned char> m_dataByte;
            //to keep order
            std::vector<std::pair<std::string, int>> m_dataItemVct;
            std::vector<std::pair<std::string, double>> m_dataTimeVct;
        };

    private:
        //static bool sm_openSwitch;
        //static int sm_index;
        static int sm_hasBuild; //to process once MakeBlocks recursion
        OPENCASCADE_TEST_API static std::vector<DataMap> sm_recordData;

    public:
        static DataCountSingleton& getInstance()
        {
            static DataCountSingleton instance;
            return instance;
        }
        OPENCASCADE_TEST_API static std::vector<DataMap>& getData()
        {
            return sm_recordData;
        }
        OPENCASCADE_TEST_API static void clear()
        {
            sm_recordData.clear();
            //sm_index = 0;
        }
        static std::vector<DataMap>& getDataCount()
        {
			//if (sm_hasBuild == 0)
            //     sm_recordData.push_back({});
            sm_hasBuild++;
            return sm_recordData;
        }
        static void hasBuild()
        {
            if (sm_hasBuild == 3) //when MakeBlocks call recursion
            {
                sm_recordData.erase(sm_recordData.begin() + sm_recordData.size() - 2);
            }
            sm_hasBuild = 0;
        }
        OPENCASCADE_TEST_API static void writeToCsvInOne(const std::string& filename);

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
