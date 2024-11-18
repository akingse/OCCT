#pragma once
#include <string>
#include <vector>
#include <map>
#include <direct.h> //_getcwd
#ifdef USING_OPENCASCADE_CLASS
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#endif

//#define USING_OPENCASCADE_TEST //set in project config
#ifdef USING_OPENCASCADE_TEST
//custom export macro
#ifdef OPENCASCADE_TEST_EXPORTS
#define OPENCASCADE_TEST_API __declspec(dllexport)
#else
#define OPENCASCADE_TEST_API __declspec(dllimport)
#endif //OPENCASCADE_TEST_EXPORTS

/*
 //communal
  std::chrono::steady_clock::time_point timestart;
  std::chrono::steady_clock::time_point timeend;
  std::chrono::duration<double, std::milli> duration_ms; // milli=ratio<1, 1000> second
  test::DataRecordSingleton& instance = test::DataRecordSingleton::getInstance();
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
    class DataRecordSingleton //DataRecordSingleton
    {
    private:
        OPENCASCADE_TEST_API DataRecordSingleton() = default;
        OPENCASCADE_TEST_API ~DataRecordSingleton() = default;
        DataRecordSingleton(const DataRecordSingleton&) = delete;
        DataRecordSingleton(DataRecordSingleton&&) = delete;

    public:
        struct DataMap
        {
            //int m_index;
            std::string m_name;
            std::map<std::string, int> m_dataCount;
            std::map<std::string, double> m_dataFloat;
            std::map<std::string, double> m_dataTime;
            std::map<std::string, std::string> m_dataByte; // equal std::vector<unsigned char>
            //to keep order
            std::vector<std::pair<std::string, int>> m_dataItemVct;
            std::vector<std::pair<std::string, double>> m_dataTimeVct;
#ifdef USING_OPENCASCADE_CLASS //keep same define
            TopoDS_Shape m_shape;//std::vector<>
#endif
        };

    private:
        //static int sm_index;
        static bool sm_open;
        static int sm_hasBuild; //to process once MakeBlocks recursion
        static bool sm_isAverage;
        OPENCASCADE_TEST_API static std::vector<DataMap> sm_recordData;

    public:
        static DataRecordSingleton& getInstance()
        {
            static DataRecordSingleton instance;
            return instance;
        }
        static bool isOpen()
        {
            return sm_open;
        }
        static void open(bool isOpen = true)
        {
            sm_open = isOpen;
        }
        static std::vector<DataMap>& getData()
        {
            return sm_recordData;
        }
        static void clear()
        {
            sm_recordData.clear();
            //sm_index = 0;
        }
        static std::vector<DataMap>& getDataR()
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
        static void setName(const std::vector<std::string>& names)
        {
            for (int i = 0; i < sm_recordData.size(); i++)
                sm_recordData[i].m_name = std::to_string(i);
        }

#ifdef USING_OPENCASCADE_CLASS
        static void exportShapeToFile()
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
        }
        static TopoDS_Shape readBinaryDataToShape(const std::string& filename)
        {
            //std::ifstream file(filename, std::ios::binary);
            TopoDS_Shape Sh;
            BRep_Builder B;
            BRepTools::Read(Sh, filename.c_str(), B);
            return Sh;
        }
        static std::string readBinaryData(const std::string& filename)
        {
            std::string res;
            std::ifstream file(filename, std::ios::binary);
            if (!file) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return res;
            }
            file.seekg(0, std::ios::end);
            std::streamsize fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            res.resize(fileSize);
            if (!file.read(const_cast<char*>(res.data()), fileSize)) {
                std::cerr << "Error reading file: " << filename << std::endl;
                return {};
            }
            return res;
        }
#endif

        OPENCASCADE_TEST_API static void writeToCsvInOne(const std::string& filename);

    };
}
#endif //USING_OPENCASCADE_TEST
