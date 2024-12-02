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
#include <BRep_TEdge.hxx>
#include <TopoDS_Iterator.hxx>
#include "..\..\..\OCCT\src\BOPAlgo\DataRecordSingleton.h" //USING_OPENCASCADE_TEST
#include "ModelingDoc1.h" //custom class
#include "commonOCCUtility.h" 

using namespace std;
using namespace occ;
using namespace test;
using opencascade::handle;

CsgTree g_csgtree;
TopoInfoRecord g_topoInfo;
std::vector<TopoDS_Shape> g_shapeVct;
std::vector<handle<Geom_Geometry>> g_GeomVct;

//�����ʱ��Ϣ
void writeTimeDataToCsv()
{
	DataRecordSingleton& instance = DataRecordSingleton::getInstance();
	const std::vector<DataRecordSingleton::DataMap>& datas = instance.getData();

	std::string filename = getExePath();
	//windowsϵͳ���������ڻ�ȡ��ϵͳ���������������ĺ�����
	filename += "\\..\\csv\\DataCount_" + std::to_string(GetTickCount()) + ".csv";
	instance.writeToCsvInOne(filename);
	instance.clear();
}

//���shape����������
void writeShapeDataToTxt()
{
	DataRecordSingleton& instance = DataRecordSingleton::getInstance();
	const std::vector<DataRecordSingleton::DataMap>& data = instance.getData();
	std::string path = getExePath();

	//read
	std::string filenameStd = path + "\\binFile\\shape_std_0.txt";
	TopoDS_Shape shapeStd = instance.readBinToShape(filenameStd);
	//compare
	TopoDS_Shape shapeTest = *(data.back().m_shape);
	std::string filenameTest = path + "\\binFile\\shape_0.txt";
	BRepTools::Write(shapeTest, filenameTest.c_str());

	UINT64 sz0 = sizeof(shapeTest); //24 �ڴ����
	UINT64 sz1 = sizeof(Handle(TopoDS_TShape));//8
	UINT64 sz2 = sizeof(TopLoc_Location);//8
	UINT64 sz3 = sizeof(TopAbs_Orientation);//4
	bool isE1 = shapeStd.IsEqual(shapeTest); //ֻ�Ƚ�myTShapeָ���Ƿ����

	//compare
	std::string str_shape0 = instance.readBinToString(filenameStd);
	std::string str_shape1 = instance.readBinToString(filenameTest);
	//std::string str_shape0(shape0.begin(), shape0.end());
	//std::string str_shape1(shape1.begin(), shape1.end());
	//std::cout << str_shape0 << std::endl;

	bool isEq = str_shape0 == str_shape1;
	std::vector<int> cmpRes = instance.compareBRepFormat();

	CString cs = isEq ? L"true" : L"false";
	AfxMessageBox(cs);
	instance.clear();
}

//��������������-����Fuse
static CsgTree getBooleanTest_01()
{
	TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	//TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Vec(5, 5, 0));
	BRepBuilderAPI_Transform theBox2(theBox1, trsf);
	CsgTree csgtree = CsgTree(theBox1, theBox2, BOPAlgo_Operation::BOPAlgo_FUSE);
	return csgtree;
}

//����������
static CsgTree getBooleanTest_02()
{
	CsgTree csgtree;
	return csgtree;
}

//OCCT����BUG-Torus Cut Cone
static CsgTree getBooleanTest_03()
{
	DataRecordSingleton& instance = DataRecordSingleton::getInstance();
	instance.setOpenCheck(true);

	double R = 10, r = 2;
	double H = 30;
	double offset = 0.1;
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeTorus(R, r).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeCone(r, 0, H).Shape();
	gp_Trsf trsfR;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0., 1., 0.));
	trsfR.SetRotation(axe, 90 * M_PI / 180);
	gp_Trsf trsfT;
	trsfT.SetTranslation(gp_Vec(R - H + r + offset, 0, 0));

	BRepBuilderAPI_Transform theShapeC(theShapeB, trsfT * trsfR);
	//TopoDS_Shape shapeBool = BRepAlgoAPI_Cut(theShapeA, theShapeC);
	CsgTree csgtree = CsgTree(theShapeA, theShapeC, BOPAlgo_Operation::BOPAlgo_CUT);

	csgtree.checkTopology();
	//writeTimeDataToCsv();
	//writeShapeDataToTxt();
	//CsgTree::TraverseShape(theShapeA);
	//CsgTree::TraverseShape(theShapeB);
	//CsgTree::TraverseShape(csgtree.getResult());
	return csgtree;
	/*
	���BUG����Ҫԭ����Բ׶�ļ��������Բ���������ˡ�����һ�������ں˺û���һ��������ǿ�
	�����д�����Щ��������ı��֣����漰�����ߡ�������жϣ������ߡ������жϳ����㷨�ϵ���
	�����⻹�����Լ����ں��ݲ���ƿء�
	*/
}

//OCCT����BUG-Cylinder Cut Sphere//���޸�
static CsgTree getBooleanTest_04()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeCylinder(1, 1).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(1).Shape();
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Vec(1, 1, 1));
	BRepBuilderAPI_Transform theShapeC(theShapeB, trsf);
	//TopoDS_Shape shapeBool = BRepAlgoAPI_Cut(theShapeA, theShapeC);
	CsgTree csgtree = CsgTree(theShapeA, theShapeC, BOPAlgo_Operation::BOPAlgo_CUT);
	return csgtree;
}

//���ཻ��������
static CsgTree getBooleanTest_05()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeSphere(1).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(1).Shape();
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Vec(1, 1, 0));
	BRepBuilderAPI_Transform theShapeC(theShapeB, trsf);
	CsgTree csgtree = CsgTree(theShapeA, theShapeC, BOPAlgo_Operation::BOPAlgo_COMMON);
	return csgtree;
}

//BooleanOperation myArguments ���shape����
static CsgTree getBooleanTest_06()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeSphere(2).Shape();
    TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(gp_Pnt(1, 0, 0), 2).Shape();
	TopTools_ListOfShape objects; //ֻ����objects��resultΪ��
	objects.Append(theShapeA);
	objects.Append(theShapeB);
	TopTools_ListOfShape tools;
	TopoDS_Shape theShapeC = BRepPrimAPI_MakeBox(3,2,2).Shape();
	tools.Append(theShapeC);

	//CsgTree csgtree = CsgTree(objects, tools, BOPAlgo_Operation::BOPAlgo_UNKNOWN);
	//CsgTree csgtree = CsgTree(objects, tools, BOPAlgo_Operation::BOPAlgo_FUSE);
	//CsgTree csgtree = CsgTree(objects, tools, BOPAlgo_Operation::BOPAlgo_COMMON);
	CsgTree csgtree = CsgTree(objects, tools, BOPAlgo_Operation::BOPAlgo_CUT);
	return csgtree;
}

//�򵥲���
static CsgTree getBooleanTest_07()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeSphere(2).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(gp_Pnt(1, 0, 0), 2).Shape();
	CsgTree csgtree = CsgTree(theShapeA, theShapeB, BOPAlgo_Operation::BOPAlgo_COMMON);
	TopoDS_Shape theShapeR = csgtree.m_shapeResult;
	//record
	TopoInfoRecord topoinfo;
	topoinfo.m_shape = csgtree.m_shapeResult;
	topoinfo.TraverseShape(topoinfo.m_shape);
	topoinfo.getGeomAndShape();
	return csgtree;
}

//�򵥲���
static CsgTree getBooleanTest_08()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeSphere(2).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(gp_Pnt(1, 0, 0), 2).Shape();
	CsgTree csgtree = CsgTree(theShapeA, theShapeB, BOPAlgo_Operation::BOPAlgo_COMMON);
	TopoDS_Shape theShapeR = csgtree.m_shapeResult;
	//record
	TopoInfoRecord topoinfo;
	topoinfo.m_shape = csgtree.m_shapeResult;
	topoinfo.TraverseShape(topoinfo.m_shape);
	topoinfo.getGeomAndShape();
	return csgtree;
}

//��������
static TopoInfoRecord getTopoInfoTest_01()
{
	TopoInfoRecord topoinfo;
	//g_extraShape = BRepPrimAPI_MakeBox(3, 2, 1).Shape();
	//MakeOneAxis
    //topoinfo.m_shape = BRepPrimAPI_MakeSphere(gp_Pnt(0, 0, 0), 10).Shape();
	//topoinfo.m_shape = BRepPrimAPI_MakeCone(3, 2, 10).Shape();
	//topoinfo.m_shape = BRepPrimAPI_MakeCylinder(1, 5).Shape();
	topoinfo.m_shape = BRepPrimAPI_MakeTorus(10, 2).Shape();
	topoinfo.TraverseShape(topoinfo.m_shape); //write shape data to topoInfo
	topoinfo.getGeomAndShape();
	return topoinfo;
}

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

static void getShapeCreate_02()
{
	std::vector<TopoDS_Shape> shapeRes;
	//copy from OnRevol
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
	gp_GTrsf GTrsf;
}

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

//��֤����
void CModelingDoc::OnTestBoolBefore() //using icon -
{
	g_csgtree = getBooleanTest_01();
	clearDisplay();
	const std::vector<TopoDS_Shape>& shapeVct = g_csgtree.m_shapeArgument;
	for (int i = 0; i < shapeVct.size(); i++)
	{
		oneShapeDisplay(shapeVct[i]);
	}
	Fit();
	return;
}

void CModelingDoc::OnTestBoolAfter() //using icon +
{
	clearDisplay();
	//draw bool result
	oneShapeDisplay(g_csgtree.m_shapeResult);
	return;
}

void CModelingDoc::OnTestBoolDetail() //using icon common
{
	clearDisplay();
	g_topoInfo = getTopoInfoTest_01();
	g_topoInfo = getTopoInfoTest_02();
	//std::vector<TopoDS_Shape> shapeVct = g_topoInfo.getShapeVct(TopAbs_ShapeEnum::TopAbs_FACE);
	//std::vector<TopoDS_Shape> shapeVct = getShapeCreate_01();

	//getShapeCreate_01();
	//getShapeCreate_02();
	//getShapeCreate_03();

	for (int i = 0; i < g_shapeVct.size(); i++)
	{
		//clearDisplay();
		oneShapeDisplay(g_shapeVct[i]);
	}
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
	//oneShapeDisplay(g_topoInfo.m_shape);
	return;
}
