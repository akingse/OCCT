#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#ifdef USING_OPENCASCADE_CLASS
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#endif //USING_OPENCASCADE_CLASS

//#define USING_OPENCASCADE_TEST //set in project config
#ifdef USING_OPENCASCADE_TEST
//custom export macro
#ifdef OPENCASCADE_TEST_EXPORTS
#define OPENCASCADE_TEST_API __declspec(dllexport)
#else
#define OPENCASCADE_TEST_API __declspec(dllimport)
#endif //OPENCASCADE_TEST_EXPORTS

#ifndef USING_OPENCASCADE_CLASS
class TopoDS_Shape;
#endif

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
#ifdef USING_TPL_JSONCPP
    inline p3d::Utf8String string2utf8String(const std::string& str) //Utf8StringAlias
    {
        p3d::Utf8String ustr(str.size(), 0);
        memcpy(const_cast<char*>(ustr.data()), str.data(), str.size());
        return ustr;
    }

    inline std::string utf8String2string(const p3d::Utf8String& ustr)
    {
        std::string str(ustr.size(), 0);
        memcpy(const_cast<char*>(str.data()), ustr.data(), ustr.size());
        return str;
    }
#endif

    /// <summary>
    /// For time detial count 记录详细的布尔时间
    /// 记录BPGeometryTestTool调用布尔时的布尔数据，
    /// 包括体积/表面积/拓扑数量等信息，按顺序存到vector列表中，调用getData返回；
    /// 用于每次修改OCC源码后作为对照；
    /// 此单例类有open开关，默认关闭，开启对比是开启，效率统计时关闭；
    /// </summary>
    class DataRecordSingleton //DataRecordSingleton
    {
    private:
        OPENCASCADE_TEST_API DataRecordSingleton() = default;
        OPENCASCADE_TEST_API ~DataRecordSingleton() = default;
        DataRecordSingleton(const DataRecordSingleton&) = delete;
        DataRecordSingleton(DataRecordSingleton&&) = delete;

    public:
        struct Point3d
        {
            double x;
            double y;
            double z;
			inline bool isEqual(const Point3d& rhs, double tolerance = 0.0) const
            {
				return fabs(x - rhs.x) <= tolerance && fabs(y - rhs.y) <= tolerance && fabs(z - rhs.z) <= tolerance;
            }
        };

        struct DataMap
        {
            //int m_index;
			//bool m_valid = false;
            std::string m_name;
            std::map<std::string, int> m_dataCount;
            std::map<std::string, double> m_dataFloat;
            std::map<std::string, double> m_dataTime;
            std::map<std::string, Point3d> m_dataPoint;
            std::map<std::string, std::string> m_dataByte; // equal std::vector<unsigned char>
            //to keep order
            std::vector<std::pair<std::string, int>> m_dataItemVct;
            std::vector<std::pair<std::string, double>> m_dataTimeVct;
//#ifdef USING_OPENCASCADE_CLASS //keep same define
            std::shared_ptr<TopoDS_Shape> m_shape;//std::vector<>
        };

        struct FaceInfo
        {
            std::vector<TopoDS_Shape> m_facesTarget;
            std::vector<TopoDS_Shape> m_facesTool;
            std::vector<TopoDS_Shape> m_edgesIntersect;
        };

        //DataCompareSingleton
    public:
        static double sm_toleDist;
        static double sm_tolerence;

    private:
        //static int sm_index;
        static int sm_hasBuild; //to process once MakeBlocks recursion
        static bool sm_isAverage;
        OPENCASCADE_TEST_API static bool sm_open;// =false
        OPENCASCADE_TEST_API static std::vector<DataMap> sm_recordData;
        OPENCASCADE_TEST_API static std::vector<FaceInfo> sm_recordFace;

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
            sm_recordFace.clear();
            //sm_index = 0;
        }
        static std::vector<DataMap>& getDataP() //private only for BOPAlgo
        {
			//if (sm_hasBuild == 0)
            //     sm_recordData.push_back({});
            sm_hasBuild++;
            return sm_recordData;
        }
        static void hasBuild()//private only for BOPAlgo_BOP Build
        {
            if (sm_hasBuild == 3) //when MakeBlocks call recursion
                sm_recordData.erase(sm_recordData.begin() + sm_recordData.size() - 2);
            sm_hasBuild = 0;
        }
        static void setName(const std::vector<std::string>& names)
        {
            if (sm_recordData.size() == names.size())
            {
                for (int i = 0; i < sm_recordData.size(); i++)
                    sm_recordData[i].m_name = names[i];
            }
            else
            {
                for (int i = 0; i < sm_recordData.size(); i++)
                    sm_recordData[i].m_name = std::to_string(i);
            }
        }

        OPENCASCADE_TEST_API static TopoDS_Shape readBinToShape(const std::string& filename);
        OPENCASCADE_TEST_API static void writeShapeToFile();
        OPENCASCADE_TEST_API static std::string readBinToString(const std::string& filename);
        OPENCASCADE_TEST_API static void writeToCsvInOne(const std::string& filename);
        OPENCASCADE_TEST_API static std::vector<int> compareBRepFormat();
        OPENCASCADE_TEST_API static std::vector<DataMap> compareDataMap(const std::vector<DataMap>& stdDataRead);

    };
	
#ifdef USING_TPL_JSONCPP
        using namespace p3d;
        OPENCASCADE_TEST_API static Json::Value getJson(int i = 0)
        {
            if (sm_recordData.empty())
                return {};
            if (i < 0 || i >= sm_recordData.size())
                return {};
            Json::Value oneRecord;
            const DataMap& data = sm_recordData[i];
            oneRecord["Aname"] = string2utf8String(data.m_name);
            for (const auto& rec : sm_recordData[i].m_dataCount)
                oneRecord[rec.first.c_str()] = rec.second;
            for (const auto& rec : sm_recordData[i].m_dataFloat)
                oneRecord[rec.first.c_str()] = rec.second;
            for (const auto& rec : sm_recordData[i].m_dataPoint)
            { //copy from BPGraphicsInputOutput::graphicsOutputToJsonValue
                Json::Value root;
                Json::Value pointValue;
                pointValue["x"] = rec.second.x;
                pointValue["y"] = rec.second.y;
                pointValue["z"] = rec.second.z;
                root["GePoint3d"] = pointValue;
                oneRecord[rec.first.c_str()] = root;
            }
            return oneRecord;
        }

        OPENCASCADE_TEST_API static DataMap readFromJson(const Json::Value& root)
        {
            DataCompareSingleton::DataMap record;
            if (!root["compareData"].isNull())
            {
                Json::Value data = root["compareData"];
                int countVertex = data["countVertex"].asInt();
                int countEdge = data["countEdge"].asInt();
                int countWire = data["countWire"].asInt();
                int countFace = data["countFace"].asInt();
                int countShell = data["countShell"].asInt();
                int countSolid = data["countSolid"].asInt();
                double massArea = data["massArea"].asDouble();
                double massVolume = data["massVolume"].asDouble();
                Json::Value pointV = data["centerVolume"]["GePoint3d"];
                Point3d centerVolume = Point3d{ pointV["x"].asDouble(), pointV["y"].asDouble(), pointV["z"].asDouble() };
                Json::Value pointA = data["centerArea"]["GePoint3d"];
                Point3d centerArea = Point3d{ pointA["x"].asDouble(), pointA["y"].asDouble(), pointA["z"].asDouble() };
                Json::Value pointBmax = data["BndBoxMax"]["GePoint3d"];
                Point3d BndBoxMax = Point3d{ pointBmax["x"].asDouble(), pointBmax["y"].asDouble(), pointBmax["z"].asDouble() };
                Json::Value pointBmin = data["BndBoxMin"]["GePoint3d"];
                Point3d BndBoxMin = Point3d{ pointBmin["x"].asDouble(), pointBmin["y"].asDouble(), pointBmin["z"].asDouble() };
                //cons
                std::map<std::string, int> oneRecordCount;
                std::map<std::string, double> oneRecordFloat;
                std::map<std::string, Point3d> oneRecordPoint;
                oneRecordCount.emplace("countVertex", countVertex);
                oneRecordCount.emplace("countEdge", countEdge);
                oneRecordCount.emplace("countWire", countWire);
                oneRecordCount.emplace("countFace", countFace);
                oneRecordCount.emplace("countShell", countShell);
                oneRecordCount.emplace("countSolid", countSolid);
                oneRecordFloat.emplace("massArea", massArea);
                oneRecordFloat.emplace("massVolume", massVolume);
                oneRecordPoint.emplace("centerArea", centerArea);
                oneRecordPoint.emplace("centerVolume", centerVolume);
                oneRecordPoint.emplace("BndBoxMin", BndBoxMin);
                oneRecordPoint.emplace("BndBoxMax", BndBoxMax);
                string name = utf8String2string(data["Aname"].asString());
                record.m_name = name;
                record.m_dataCount = oneRecordCount;
                record.m_dataFloat = oneRecordFloat;
                record.m_dataPoint = oneRecordPoint;
            }
            else
                record.m_name = "json_null";
            return record;
        }

        OPENCASCADE_TEST_API static void writeToJson(const std::string& filename)
        {
            Json::Value recordArray(Json::arrayValue);
            for (int i = 0; i < sm_recordData.size(); i++)
            {
                Json::Value oneRecord = getJson(i);
                recordArray.append(oneRecord);
            }
            Json::Value root;
            root["agenda"] = recordArray;
            //write file
            Json::StyledWriter fastWriter;
            std::string json_file = fastWriter.write(root).c_str();
            std::ofstream ofs;
            ofs.open(filename.c_str());
            if (ofs.is_open())
                ofs << json_file;
            ofs.close();
        }
#endif //USING_TPL_JSONCPP

}
#endif //USING_OPENCASCADE_TEST
