#include "stdafx.h"

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
//#pragma comment(lib,"TKPrim.lib")

#ifdef USING_BIMBASE_SLN
#include "commonOCCUtility.h"
//move to occt.sln
using namespace occ;
#endif
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

//输出STEP
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
#include "GC_MakeArcOfCircle.hxx"
#include "BRepFilletAPI_MakeFillet.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepOffsetAPI_MakeThickSolid.hxx"
#include "Geom_CylindricalSurface.hxx"
#include "Geom2d_Ellipse.hxx"
#include "Geom_Plane.hxx"
#include "BRepOffsetAPI_ThruSections.hxx"
#include "GCE2d_MakeSegment.hxx"
#include "BRepLib.hxx"

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
    gp_Trsf trsf;    //trsf.Transforms(gp_XYZ(5, 5, 0));
    trsf.SetTranslation(gp_Vec(5, 5, 0));
    BRepBuilderAPI_Transform theBox3(theBox2, trsf);
    Handle(AIS_Shape) ais2 = new AIS_Shape(theBox3);

    //TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theBox1, theBox2); 
    TopoDS_Shape ShapeFuse = BRepAlgoAPI_Fuse(theBox1, theBox2); //共面布尔后合并
    //ShapeCut.TShape()
    //遍历数据
    TopTools_IndexedMapOfShape M_edge;
    TopTools_IndexedMapOfShape M_face;
    TopExp::MapShapes(ShapeFuse, TopAbs_ShapeEnum::TopAbs_EDGE, M_edge);
    TopExp::MapShapes(ShapeFuse, TopAbs_ShapeEnum::TopAbs_FACE, M_face);

    Handle(AIS_Shape) aisShape = new AIS_Shape(ShapeFuse);

    //Handle(AIS_InteractiveContext) myAISContext;
    //AIS_ListOfInteractive aList;
    //myAISContext->DisplayedObjects(aList);
    //myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
    //myAISContext->SetColor(aisShape, Quantity_NOC_YELLOW, Standard_False);
    //myAISContext->SetMaterial(aisShape, Graphic3d_NOM_PLASTIC, Standard_False);
    //myAISContext->Display(aisShape, Standard_False);
    //const Handle(AIS_InteractiveObject)& anIO2 = aisShape;
    //myAISContext->SetSelected(anIO2, Standard_False);
    //OCC_3dBaseDoc::Fit();
    return;
}

//copy from dev_guides\occt_tutorial.pdf
TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight, const Standard_Real myThickness)
{
    // Profile : Define Support Points
    gp_Pnt aPnt1(-myWidth / 2., 0, 0);
    gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
    gp_Pnt aPnt3(0, -myThickness / 2., 0);
    gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
    gp_Pnt aPnt5(myWidth / 2., 0, 0);
    // Profile : Define the Geometry
    Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);
    // Profile : Define the Topology
    TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
    TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
    TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
    TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);
    // Complete Profile
    gp_Ax1 xAxis = gp::OX();
    gp_Trsf aTrsf;
    aTrsf.SetMirror(xAxis);
    BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
    TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
    TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);
    BRepBuilderAPI_MakeWire mkWire;
    mkWire.Add(aWire);
    mkWire.Add(aMirroredWire);
    TopoDS_Wire myWireProfile = mkWire.Wire();
    // Body : Prism the Profile
    TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
    gp_Vec aPrismVec(0, 0, myHeight);
    TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);
    // Body : Apply Fillets
    BRepFilletAPI_MakeFillet mkFillet(myBody);
    TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
    while (anEdgeExplorer.More()) {
        TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
        //Add edge to fillet algorithm
        mkFillet.Add(myThickness / 12., anEdge);
        anEdgeExplorer.Next();
    }
    myBody = mkFillet.Shape();
    // Body : Add the Neck
    gp_Pnt neckLocation(0, 0, myHeight);
    gp_Dir neckAxis = gp::DZ();
    gp_Ax2 neckAx2(neckLocation, neckAxis);
    Standard_Real myNeckRadius = myThickness / 4.;
    Standard_Real myNeckHeight = myHeight / 10.;
    BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
    TopoDS_Shape myNeck = MKCylinder.Shape();
    myBody = BRepAlgoAPI_Fuse(myBody, myNeck);
    // Body : Create a Hollowed Solid
    TopoDS_Face faceToRemove;
    Standard_Real zMax = -1;
    for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()) {
        TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
        // Check if <aFace> is the top face of the bottle’s neck
        Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
        if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
            Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
            gp_Pnt aPnt = aPlane->Location();
            Standard_Real aZ = aPnt.Z();
            if (aZ > zMax) {
                zMax = aZ;
                faceToRemove = aFace;
            }
        }
    }
    TopTools_ListOfShape facesToRemove;
    facesToRemove.Append(faceToRemove);
    BRepOffsetAPI_MakeThickSolid aSolidMaker;
    aSolidMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
    myBody = aSolidMaker.Shape();
    // Threading : Create Surfaces
    Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
    Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);
    // Threading : Define 2D Curves
    gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
    gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
    gp_Ax2d anAx2d(aPnt, aDir);
    Standard_Real aMajor = 2. * M_PI;
    Standard_Real aMinor = myNeckHeight / 10;
    Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
    Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
    Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
    Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
    gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
    gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);
    Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
    // Threading : Build Edges and Wires
    TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
    TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
    TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
    TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
    TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
    TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
    BRepLib::BuildCurves3d(threadingWire1);
    BRepLib::BuildCurves3d(threadingWire2);
    // Create Threading
    BRepOffsetAPI_ThruSections aTool(Standard_True);
    aTool.AddWire(threadingWire1);
    aTool.AddWire(threadingWire2);
    aTool.CheckCompatibility(Standard_False);
    TopoDS_Shape myThreading = aTool.Shape();
    // Building the Resulting Compound
    TopoDS_Compound aRes;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound(aRes);
    aBuilder.Add(aRes, myBody);
    aBuilder.Add(aRes, myThreading);
    return aRes;
}

static void test3()
{
    TColgp_Array1OfPnt Poles(1, 4);
    Poles.SetValue(1, gp_Pnt(0, -50, 0));
    Poles.SetValue(2, gp_Pnt(50, -50, 0));
    Poles.SetValue(3, gp_Pnt(100, -25, 0));
    Poles.SetValue(4, gp_Pnt(100, 0, 0));
    //Handle(Geom_BSplineCurve) spline = CreateBSplineCurve(Poles, 3);

    TColgp_Array1OfPnt poles(1, 5);
    poles(1) = gp_Pnt(0, 0, 0);
    poles(2) = gp_Pnt(1, 2, 0);
    poles(3) = gp_Pnt(2, 3, 0);
    poles(4) = gp_Pnt(3, 1, 0);
    poles(5) = gp_Pnt(4, 0, 0);

    Standard_Integer degree = 4;
    //Handle(Geom_BSplineCurve) bspline = CreateBSplineCurve(poles, degree);

    return;
}

static void test4()
{
    TopoDS_Shape ConeO = BRepPrimAPI_MakeCylinder(100., 300.).Shape();

}


static int enrol = []()
    {
        //test1();
        //test2();
        //test3();
        //test4();
        return 0;
    }();

