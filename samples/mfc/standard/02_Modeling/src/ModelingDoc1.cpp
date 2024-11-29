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

using namespace std;
using namespace test;
using opencascade::handle;

TopoInfoRecord g_topoInfo;
TopoDS_Shape g_extraShape;
CsgTree g_csgtree;

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
static TopoDS_Shape getBooleanTest_01()
{
	TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	//TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Vec(5, 5, 0));
	BRepBuilderAPI_Transform theBox2(theBox1, trsf);
	//Handle(AIS_Shape) ais2 = new AIS_Shape(myBRepTransformation);

	TopoDS_Shape ShapeFuse = BRepAlgoAPI_Fuse(theBox1, theBox2); //���沼����ϲ�
	//�������ݣ�ֻ��ȡ��Ŀ
	TopTools_IndexedMapOfShape M_edge;
	TopTools_IndexedMapOfShape M_face;
	TopExp::MapShapes(ShapeFuse, TopAbs_ShapeEnum::TopAbs_EDGE, M_edge);
	TopExp::MapShapes(ShapeFuse, TopAbs_ShapeEnum::TopAbs_FACE, M_face);

	return ShapeFuse;
}

static TopoDS_Shape getBooleanTest_02()
{
	//����������
	//Handle(Geom_BSplineCurve) bsplineCurve = CreateBSplineCurve();
	//CreateExtrudedShape
	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(50., 0., 0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 0., 0.), gp_Pnt(50., 50., 0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 50., 0.), gp_Pnt(0., 0., 0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1, E2, E3);
	gp_Vec direction(0, 0, 5); // �� Z ������ 5 ����λ
	TopoDS_Shape extrudedShape = BRepPrimAPI_MakePrism(W, direction);

	return extrudedShape;
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

static CsgTree getBooleanTest_07()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeSphere(2).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(gp_Pnt(1, 0, 0), 2).Shape();
	CsgTree csgtree = CsgTree(theShapeA, theShapeB, BOPAlgo_Operation::BOPAlgo_COMMON);
	//CsgTree::TraverseShape(theShapeA);
	//CsgTree::TraverseShape(csgtree.m_shapeResult);
	g_topoInfo.clear();
	//g_extraShape = BRepPrimAPI_MakeBox(3, 2, 1).Shape();
	g_extraShape = BRepPrimAPI_MakeCone(3, 2, 10).Shape();
	g_topoInfo.TraverseShape(g_extraShape); //write shape data to topoInfo
	g_topoInfo.getGeomAndShape();
	return csgtree;
}

//��֤����
void CModelingDoc::OnTestBoolBefore()
{
	g_csgtree = getBooleanTest_07();
	clearDisplay();
	std::vector<TopoDS_Shape> shapeVct = g_csgtree.m_shapeArgument;
	for (int i = 0; i < shapeVct.size(); i++)
	{
		int color = rand() % (508 + 1);
		int material = rand() % (25 + 1);
		Handle(AIS_Shape) aisShape = new AIS_Shape(g_csgtree.m_shapeArgument[i]);
		myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
		myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
		myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
		myAISContext->Display(aisShape, Standard_False);
		const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
		myAISContext->SetSelected(anIOShape, Standard_False);
	}
	Fit();
	return;
}

void CModelingDoc::OnTestBoolAfter()
{
	clearDisplay();
	//draw bool result
	int color = rand() % (508 + 1);
	int material = rand() % (25 + 1);
	Handle(AIS_Shape) aisShape = new AIS_Shape(g_csgtree.m_shapeResult);
	myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
	myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
	myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
	myAISContext->Display(aisShape, Standard_False);
	const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
	myAISContext->SetSelected(anIOShape, Standard_False);
	myAISContext->DisplayAll(true); //ǿ��ˢ��
	Fit();
	return;
}

void CModelingDoc::OnTestBoolDetail()
{
	clearDisplay();
	for (int i = 0; i < g_topoInfo.m_topoFaceVct.size(); i++)
	{
		int color = rand() % (508 + 1);
		int material = rand() % (25 + 1);
		Handle(AIS_Shape) aisShape = new AIS_Shape(g_topoInfo.m_topoFaceVct[i]);
		myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
		myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
		myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
		myAISContext->Display(aisShape, Standard_False);
		const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
		myAISContext->SetSelected(anIOShape, Standard_False);
	}
	Fit();
	return;
}

void CModelingDoc::OnTestBoolExtra()
{
	clearDisplay();
	int color = rand() % (508 + 1);
	int material = rand() % (25 + 1);
	Handle(AIS_Shape) aisShape = new AIS_Shape(g_extraShape);
	myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
	myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
	myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
	myAISContext->Display(aisShape, Standard_False);
	const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
	myAISContext->SetSelected(anIOShape, Standard_False);
	Fit();
	return;
}