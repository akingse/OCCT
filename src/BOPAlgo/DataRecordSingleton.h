#pragma once
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <memory>

//custom export macro
#ifdef OPENCASCADE_TEST_EXPORTS
#define OPENCASCADE_TEST_API __declspec(dllexport)
#else
#define OPENCASCADE_TEST_API __declspec(dllimport)
#endif //OPENCASCADE_TEST_EXPORTS

//#define USING_OPENCASCADE_TEST //set in project config
#ifdef USING_OPENCASCADE_TEST
namespace test
{
    /// <summary>
    /// DataRecordDashboard 测试控制面板，无具体数据，只包含开关，可以导出
    /// </summary>
    class DataRecordDashboard //DataRecordCtrlPanel
    {
    private:
        OPENCASCADE_TEST_API DataRecordDashboard() = default;
        OPENCASCADE_TEST_API ~DataRecordDashboard() = default;
        DataRecordDashboard(const DataRecordDashboard&) = delete;
        DataRecordDashboard(DataRecordDashboard&&) = delete;

    private:
        OPENCASCADE_TEST_API static bool sm_openTime;// =false
        OPENCASCADE_TEST_API static bool sm_openCheck;// =false
        OPENCASCADE_TEST_API static bool sm_openOutput;// =false
        OPENCASCADE_TEST_API static bool sm_isAverage;// =false

    public:
#pragma region inline_function
        static DataRecordDashboard& getInstance()
        {
            static DataRecordDashboard instance;
            return instance;
        }
        //xor alternative option
        static bool isOpenTime() //using time count
        {
            return sm_openTime;
        }
        //mutual exclusion
        static void setOpenTime(bool isOpen = true)
        {
            sm_openTime = isOpen;
            if (sm_openTime)
                sm_openCheck = false;
        }
        static bool isOpenCheck() //using check shape
        {
            return sm_openCheck;
        }
        static void setOpenCheck(bool isOpen = true)
        {
            sm_openCheck = isOpen;
            if (sm_openCheck)
                sm_openTime = false;
        }
        static bool isOpenOutput() //using output brep
        {
            return sm_openOutput;
        }
        static void setOpenOutput(bool isOpen = true)
        {
            sm_openOutput = isOpen;
        }
        static bool& isAverage() //set and get
        {
            return sm_isAverage;
        }

#pragma endregion

    };
}
#endif //USING_OPENCASCADE_TEST

#ifdef USING_OPENCASCADE_CLASS
//OCC include
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Builder.hxx> //TopoDS:: create
#include <BRepTools.hxx> //Read and Write
#include <BRepAlgoAPI_Check.hxx>

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
    class DataRecordSingleton //DataCountSingleton and DataCompareSingleton
    {
    private:
        OPENCASCADE_TEST_API DataRecordSingleton() = default;
        OPENCASCADE_TEST_API ~DataRecordSingleton() { clear(); };//default
        DataRecordSingleton(const DataRecordSingleton&) = delete;
        DataRecordSingleton(DataRecordSingleton&&) = delete;

    public:
        struct Point3d
        {
            double xyz[3];
			inline bool isEqual(const Point3d& rhs, double tolerance = 0.0) const
            {
				return 
                    fabs(xyz[0] - rhs.xyz[0]) <= tolerance && 
                    fabs(xyz[1] - rhs.xyz[1]) <= tolerance && 
                    fabs(xyz[2] - rhs.xyz[2]) <= tolerance;
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
            std::shared_ptr<TopoDS_Shape> m_shape;//std::vector<>
        };

        //for boolean once
        struct ShapeCheck
        {
            NCollection_List<BOPAlgo_CheckResult> m_checkObsolete; //one-one boolean
            NCollection_List<BOPAlgo_CheckResult> m_checkBefore;
            NCollection_List<BOPAlgo_CheckResult> m_checkAfter;
            Handle(Message_Report) m_msgReport;
        };

        struct FaceDetail
        {
            TopoDS_Face m_faceObject;
            TopoDS_Face m_faceTool;
            std::vector<TopoDS_Edge> m_edgesInterf;
			inline std::vector<TopoDS_Shape> getShapes(bool onlyEdge = false) const
            {
                std::vector<TopoDS_Shape> res;
                if (!onlyEdge)
                {
                    res.push_back(m_faceObject);
                    res.push_back(m_faceTool);
                }
                for (const auto& iter : m_edgesInterf)
                    res.push_back(iter);
                return res;
            }
            inline std::vector<TopoDS_Shape> getFacePair() const
            {
                return std::vector<TopoDS_Shape>{m_faceObject, m_faceTool};
            }
        };

    public:
        OPENCASCADE_TEST_API static double sm_toleDist;
        OPENCASCADE_TEST_API static double sm_tolerence;

    private:
        //static int sm_index;
        static int sm_hasBuild; //to process once MakeBlocks recursion
        OPENCASCADE_TEST_API static ShapeCheck sm_recordCheck;
        OPENCASCADE_TEST_API static std::vector<DataMap> sm_recordData;
        OPENCASCADE_TEST_API static std::vector<FaceDetail> sm_recordFace;

    public:
#pragma region inline_function
        static DataRecordSingleton& getInstance()
        {
            static DataRecordSingleton instance;
            return instance;
        }
        static std::vector<DataMap>& getData()
        {
            return sm_recordData;
        }
        static std::vector<DataMap>& getDataP() //private only for BOPAlgo-PerformInternal
        {
            sm_hasBuild++;
            return sm_recordData;
        }
        static void clear()
        {
            sm_recordData.clear();
            sm_recordFace.clear();
            //sm_index = 0;
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
        static ShapeCheck& getShapeCheck()
        {
            return sm_recordCheck;
        }
        static void appendFaceDetial(const FaceDetail& faceDetail)
        {
            for (auto& iter : sm_recordFace)
            {
                if (iter.m_faceObject == faceDetail.m_faceObject &&
                    iter.m_faceTool == faceDetail.m_faceTool)
                {
                    iter.m_edgesInterf.insert(iter.m_edgesInterf.end(), faceDetail.m_edgesInterf.begin(), faceDetail.m_edgesInterf.end());
                    return;
                }
            }
            sm_recordFace.push_back(faceDetail);
        }
        static const std::vector<FaceDetail>& getFaceDetial()
        {
            return sm_recordFace;
        }

#pragma endregion

        OPENCASCADE_TEST_API static TopoDS_Shape readBinToShape(const std::string& filename);
        OPENCASCADE_TEST_API static void writeShapeToFile(const std::string& filename);
        OPENCASCADE_TEST_API static std::string readBinToString(const std::string& filename);
        OPENCASCADE_TEST_API static void writeToCsvInOne(const std::string& filename = {});
        OPENCASCADE_TEST_API static std::vector<int> compareBRepFormat();
        OPENCASCADE_TEST_API static std::vector<DataMap> compareDataMap(const std::vector<DataMap>& stdDataRead);
        OPENCASCADE_TEST_API static void writeCheckReportToFile(const std::string& filename);

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
                pointValue["x"] = rec.second.xyz[0];
                pointValue["y"] = rec.second.xyz[1];
                pointValue["z"] = rec.second.xyz[2];
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
#endif //USING_OPENCASCADE_CLASS
