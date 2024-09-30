#include <iostream>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
//add
#include "GC_MakeSegment.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#pragma comment(lib,"TKPrim.lib")
using namespace std;
//using namespace opencascade;

#include <sstream>
#include <iomanip>
#include <string>
#include <Standard.hxx>

std::string doubleToString(double value) 
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    return ss.str();
    //return std::to_string(value);
}

static void test0() 
{
    // 创建截面形状
    gp_Pnt p1(0, 0, 0);
    gp_Pnt p2(10, 0, 0);
    gp_Pnt p3(10, 10, 0);
    gp_Pnt p4(0, 10, 0);

    Handle(Geom_TrimmedCurve) curve = GC_MakeSegment(p1, p2);
    TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(curve);
    curve = GC_MakeSegment(p2, p3);
    TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(curve);
    curve = GC_MakeSegment(p3, p4);
    TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(curve);
    curve = GC_MakeSegment(p4, p1);
    TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(curve);

    BRepBuilderAPI_MakeWire wireMaker;
    wireMaker.Add(edge1);
    wireMaker.Add(edge2);
    wireMaker.Add(edge3);
    wireMaker.Add(edge4);
    TopoDS_Wire sectionWire = wireMaker.Wire();

    // 创建路径
    gp_Pnt pathStart(0, 0, 0);
    gp_Pnt pathEnd(0, 0, 10);
    Handle(Geom_TrimmedCurve) pathCurve = GC_MakeSegment(pathStart, pathEnd);
    TopoDS_Edge pathEdge = BRepBuilderAPI_MakeEdge(pathCurve);
    TopoDS_Wire pathWire = BRepBuilderAPI_MakeWire(pathEdge);

    // 创建扫描体
    BRepPrimAPI_MakePrism prismMaker(sectionWire, gp_Vec(pathStart, pathEnd));//append lib
    TopoDS_Shape sweptSolid = prismMaker.Shape();

    // 将扫描体保存为STEP文件
    BRepTools::Write(sweptSolid, "sweptSolid.stp");

    std::cout << "Swept solid created and saved as 'sweptSolid.stp'." << std::endl;

    return;
}

#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
//builder
#include <BRepBuilderAPI_Transform.hxx>
//boolean
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>

//box-sphere
static void test1()
{
    TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 60, 60).Shape();
    Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);

    TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 20, 20), 80).Shape();
    Handle(AIS_Shape) ais2 = new AIS_Shape(theSphere);

    TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere, theBox);
    Handle(AIS_Shape) aSection = new AIS_Shape(ShapeCut);
    return;
}

//test demo, box-box
static void test2()
{
    TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
    Handle(AIS_Shape) ais1 = new AIS_Shape(theBox1);

    TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
    gp_Trsf trsf;
    //trsf.Transforms(gp_XYZ(5, 5, 0));
    trsf.SetTranslation(gp_Vec(5, 5, 0));
    BRepBuilderAPI_Transform myBRepTransformation(theBox2, trsf);
    Handle(AIS_Shape) ais2 = new AIS_Shape(myBRepTransformation);

    TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theBox1, theBox2);
    Handle(AIS_Shape) aSection = new AIS_Shape(ShapeCut);
    return;
}

static int enrol = []()
    {
        test1();
        return 0;
    }();

