#include "stdafx.h"
#include "ModelingDoc.h"
#include "ModelingApp.h"
#include "ResultDialog.h"
#include "State.h"
#include <direct.h> //_getcwd

//modeling
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_TEdge.hxx>
#include <TopoDS_Iterator.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include "commonOCCUtility.h" 
#include "..\..\..\OCCT\src\BOPAlgo\DataRecordSingleton.h" //USING_OPENCASCADE_TEST
#include "ModelingDoc1.h" //custom class

using namespace std;
using namespace occ;
using namespace test;
using namespace clash;
using opencascade::handle;

static TopoInfoRecord g_topoInfo;
static std::vector<TopoDS_Shape> g_shapeVct;
static std::vector<handle<Geom_Geometry>> g_GeomVct;

//��������
static TopoInfoRecord getTopoInfoTest_01()
{
	TopoInfoRecord topoinfo;
	//g_extraShape = BRepPrimAPI_MakeBox(3, 2, 1).Shape();
	//MakeOneAxis
    //topoinfo.m_shape = BRepPrimAPI_MakeSphere(gp_Pnt(0, 0, 0), 10).Shape();
	//topoinfo.m_shape = BRepPrimAPI_MakeCone(3, 2, 10).Shape();
	topoinfo.m_shape = BRepPrimAPI_MakeCylinder(1, 5).Shape();
	//topoinfo.m_shape = BRepPrimAPI_MakeTorus(10, 2).Shape();
	topoinfo.TraverseShape(topoinfo.m_shape); //write shape data to topoInfo
	topoinfo.getGeomAndShape();

	std::vector<const BRep_TFace*> brepFaceVct;
	for (TopExp_Explorer exp(topoinfo.m_shape, TopAbs_FACE); exp.More(); exp.Next())
	{
		TopoDS_Face aCurrentFace = TopoDS::Face(exp.Current());
		BRep_TFace* brepFace = dynamic_cast<BRep_TFace*>(aCurrentFace.TShape().get());
		if (brepFace != nullptr) //no repeat
			brepFaceVct.push_back(brepFace);
		
	}
	return topoinfo;
}

//���� BRepAlgoAPI_Section
static TopoInfoRecord getTopoInfoTest_02()
{
	TopoDS_Shape atorus = BRepPrimAPI_MakeTorus(120, 20).Shape();
	Standard_Real radius = 120;
	Standard_Integer i = -3;
	TopoDS_Shape asphere = BRepPrimAPI_MakeSphere(gp_Pnt(26 * 3 * i, 0, 0), radius).Shape();
	Standard_Boolean PerformNow = Standard_False;
	BRepAlgoAPI_Section section(atorus, asphere, PerformNow);
	section.ComputePCurveOn1(Standard_True);
	section.Approximation(TopOpeBRepTool_APPROX);
	section.Build();
	g_shapeVct = { section.Shape() };

	TopoInfoRecord topoinfo;
	return topoinfo;
}

//������-����
static void getShapeCreate_01()
{
	std::vector<TopoDS_Shape> shapeRes;
	//copy from OnPrism
	//��-��
	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200., -200., 0.));
	TopoDS_Shape S1 = BRepPrimAPI_MakePrism(V1, gp_Vec(0., 0., 100.));
	//��-��
	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150., -150, 0.), gp_Pnt(-50., -50, 0.));
	TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E, gp_Vec(0., 0., 100.));
	//wire-face
	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(50., 0., 0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 0., 0.), gp_Pnt(50., 50., 0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 50., 0.), gp_Pnt(0., 0., 0.));
	TopoDS_Wire Wire = BRepBuilderAPI_MakeWire(E1, E2, E3);
	TopoDS_Face Face = occ::trans(100, 100) * BRepBuilderAPI_MakeFace(Wire).Face();
	TopoDS_Shape Prism1 = BRepPrimAPI_MakePrism(Wire, gp_Vec(0., 0., 100.)); //������
	TopoDS_Shape Prism2 = BRepPrimAPI_MakePrism(Face, gp_Vec(0., 0., 100.)); //����ʵ��
	//shapeRes = { Prism2 };
	g_shapeVct = { V1,E,Wire,Face };
	//g_shapeVct = { S1,S2,Prism1, Prism2 };

}

//copy from OnRevol
static void getShapeCreate_02()
{
	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200., -200., 0.));
	gp_Ax1 axe = gp_Ax1(gp_Pnt(-170., -170., 0.), gp_Dir(0., 0., 1.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	TopoDS_Shape S1 = BRepPrimAPI_MakeRevol(V1, axe);

	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-120., -120, 0.), gp_Pnt(-120., -120, 100.));
	/*gp_Ax1*/ axe = gp_Ax1(gp_Pnt(-100., -100., 0.), gp_Dir(0., 0., 1.));
	Handle(Geom_Axis1Placement) Gax2 = new Geom_Axis1Placement(axe);
	TopoDS_Shape S2 = BRepPrimAPI_MakeRevol(E, axe);

	//MakeRevol
	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(50., 0., 0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 0., 0.), gp_Pnt(50., 50., 0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 50., 0.), gp_Pnt(0., 0., 0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1, E2, E3);
	axe = gp_Ax1(gp_Pnt(0., 0., 30.), gp_Dir(0., 1., 0.));
	Handle(Geom_Axis1Placement) Gax3 = new Geom_Axis1Placement(axe);
	TopoDS_Shape S3 = BRepPrimAPI_MakeRevol(W, axe, 210. * M_PI / 180);

	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200., 200., 0.), gp_Dir(0., 0., 1.)), 80.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), Wc);
	axe = gp_Ax1(gp_Pnt(290, 290., 0.), gp_Dir(0., 1, 0.));
	Handle(Geom_Axis1Placement) Gax4 = new Geom_Axis1Placement(axe);
	TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F, axe, 90. * M_PI / 180);

	//���޼���
	g_GeomVct = { Gax1,Gax2,Gax3,Gax4 };
	return;
}

//������
static void getShapeCreate_03()
{
	gp_Elips Elips(gp_Ax2(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 10, 5);
	gp_Circ circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1);
	circle.Transform(trans(0, 0.5) * rotz(M_PI_2) * scale(0.5));
	TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(Elips, 0, M_PI);
	TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(circle, 0, M_PI);
	//g_shapeVct = { Edge1,Edge2 };

	//loft������
	//isSolid������ֵ������ָ����������״��ʵ�壨solid�����ǿ��壨shell����
	// �������Ϊ true,���㷨������һ���պϵ���άʵ�壻�������Ϊ false��Ĭ��ֵ����������һ�����壬��һ�����ŵı��档
	//ruled������ֵ������ָ�������������߿�֮�����ɵ����Ƿ�Ϊ�������档
	// �������Ϊ true�������ɵ��潫�ǹ����棻�������Ϊ false��Ĭ��ֵ���������ɵ��潫ͨ������ƽ������ͨ���������ɸ����ӵ���״��

	BRepOffsetAPI_ThruSections loftMaker(true);
	TopoDS_Wire wire1 = BRepBuilderAPI_MakeWire(
		BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0)).Edge(),
		BRepBuilderAPI_MakeEdge(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0)).Edge(),
		BRepBuilderAPI_MakeEdge(gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0)).Edge(),
		Edge2 //BRepBuilderAPI_MakeEdge(gp_Pnt(0, 1, 0), gp_Pnt(0, 0, 0)).Edge()
	);
    loftMaker.AddWire(scale(2) * wire1);
	TopoDS_Wire wire2 = BRepBuilderAPI_MakeWire(
		BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 5), gp_Pnt(1, 0, 5)).Edge(),
		BRepBuilderAPI_MakeEdge(gp_Pnt(1, 0, 5), gp_Pnt(1, 1, 5)).Edge(),
		BRepBuilderAPI_MakeEdge(gp_Pnt(1, 1, 5), gp_Pnt(0, 1, 5)).Edge(),
		BRepBuilderAPI_MakeEdge(gp_Pnt(0, 1, 5), gp_Pnt(0, 0, 5)).Edge()
	);
	loftMaker.AddWire(wire2);
	// ����������
	loftMaker.Build();
	g_shapeVct = { loftMaker.Shape() };

	//dx��Ш����Ŀ�ȡ�
	//dy��Ш�������ȡ�
	//dz��Ш����ĸ߶ȡ�
	//angle��Ш�������б�Ƕȡ�
	TopoDS_Shape Wedge = BRepPrimAPI_MakeWedge(60., 50., 100., 30.).Shape();
	g_shapeVct = { Wedge };
	gp_Ax2 ax2 = gp_Ax2(gp_Pnt(), gp_Dir(0., 0., 1.));//default
	//
	TopoDS_Shape Torus1 = BRepPrimAPI_MakeTorus(ax2, 60., 20., M_PI / 2).Shape();
	TopoDS_Shape Torus2 = BRepPrimAPI_MakeTorus(ax2, 60., 20., -M_PI / 4, M_PI / 4, M_PI / 2).Shape();
	g_shapeVct = { Torus1,Torus2 };

}

//��ֵ�����
//Interpolation and approximation
static void getShapeCreate_04()
{
	// Prepare data to be converted.
	int aPos = 1;
	Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, 100);

	//OCCT ���߲�ֵ�㷨�������㣩
	GeomAPI_Interpolate anInterpolate (aPnts, false, Precision::Confusion());
	anInterpolate.Perform();
	if (!anInterpolate.IsDone())
		return;
	Handle(Geom_BSplineCurve) aCurve = anInterpolate.Curve();
		
}

static void getShapeCreate_05()
{
	//OCCT ���߽����㷨�����Ƶ㣩
	TColgp_HArray1OfPnt aPnts = TColgp_HArray1OfPnt(1, 100);

	GeomAPI_PointsToBSpline anApprox;
	anApprox.Init(aPnts, 1., 0., 0., 8, GeomAbs_C2, 0.001);
	if (!anApprox.IsDone())
		return;
	Handle(Geom_Curve) aCurve = anApprox.Curve();
	
	TColgp_Array2OfPnt aPnt2(1, 3, 1, 3);
	GeomAPI_PointsToBSplineSurface aSurfMaker(aPnt2);
	if (!aSurfMaker.IsDone())
		return;
	Handle(Geom_BSplineSurface) aSuface = aSurfMaker.Surface();


}

//��������
void CModelingDoc::OnTestBoolDetail() //using icon common
{
	clearDisplay();
	//getShapeCreate_01();
	//getShapeCreate_02();
	getShapeCreate_03();

	//����TopoDS_Shape
	for (int i = 0; i < g_shapeVct.size(); i++)
	{
		//clearDisplay();
		oneShapeDisplay(g_shapeVct[i]);
	}
	//����Geom_Geometry
	for (int i = 0; i < g_GeomVct.size(); i++)
	{
		//clearDisplay();
		oneGeomDisplay(g_GeomVct[i]);
	}
	coordinateSystemDisplay(150);
	Fit();
	return;
}

void CModelingDoc::OnTestBoolExtra() //using icon section
{
	clearDisplay();
	g_topoInfo = getTopoInfoTest_01();
	g_topoInfo = getTopoInfoTest_02();
	//oneShapeDisplay(g_topoInfo.m_shape);
	return;
}
