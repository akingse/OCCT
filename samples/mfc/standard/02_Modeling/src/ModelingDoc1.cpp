#include "stdafx.h"
#include "ModelingDoc.h"
#include "ModelingApp.h"
#include "ResultDialog.h"
#include "State.h"

//modeling
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include "..\..\..\OCCT\src\BOPAlgo\DataRecordSingleton.h" //USING_OPENCASCADE_TEST

using namespace test;

#include <direct.h> //_getcwd
inline std::string getExePath() 
{
	char buffer[MAX_PATH];
	return std::string(_getcwd(buffer, sizeof(buffer))); //get current work directory
}


class BoolNode
{
public:
	std::shared_ptr<BoolNode> m_leftNode;
	std::shared_ptr<BoolNode> m_rightNode;
	BOPAlgo_Operation m_operation; //
};

class CsgTree
{
public:
	std::vector<TopoDS_Shape> m_shapeVct;
	std::shared_ptr<BoolNode> m_node;
	CsgTree() = default;
	CsgTree(const TopoDS_Shape& target, const TopoDS_Shape& tool, BOPAlgo_Operation operation)
	{
		m_shapeVct.push_back(target);
		m_shapeVct.push_back(tool);
		m_node = std::make_shared<BoolNode>();
		m_node->m_operation = operation;
		TopoDS_Shape result;
		if (BOPAlgo_Operation::BOPAlgo_COMMON == operation)
			result = BRepAlgoAPI_Common(target, tool);
		else if (BOPAlgo_Operation::BOPAlgo_FUSE == operation)
			result = BRepAlgoAPI_Fuse(target, tool);
		else if (BOPAlgo_Operation::BOPAlgo_CUT == operation)
			result = BRepAlgoAPI_Cut(target, tool);
		m_shapeVct.push_back(result);
	}

	TopoDS_Shape getResult() const
	{
		if (m_shapeVct.size() != 3)
			return {};
		return m_shapeVct[2];
	}

	void checkTopology() const
	{
		//close
		DataRecordSingleton& instance = DataRecordSingleton::getInstance();
		instance.open(false);

		if (m_shapeVct.empty())
			return;
		TopoDS_Shape checkedShape = m_shapeVct.back();
		BRepAlgoAPI_Check checker = BRepAlgoAPI_Check(checkedShape);
		if (checker.IsValid())
		{
			std::cout << "Shape is valid." << std::endl;
			//return;
		}
		Message_ProgressRange theRange;
		checker.Perform(theRange);
		const BOPAlgo_ListOfCheckResult& result = checker.Result();
		return;
	}

};

void writeTimeDataToCsv()
{
	DataRecordSingleton& instance = DataRecordSingleton::getInstance();
	const std::vector<DataRecordSingleton::DataMap>& datas = instance.getData();

	std::string filename = getExePath();
	//windows系统函数，用于获取自系统启动以来所经过的毫秒数
	filename += "\\..\\csv\\DataCount_" + std::to_string(GetTickCount()) + ".csv";
	instance.writeToCsvInOne(filename);
	instance.clear();
}

void writeShapeDataToTxt()
{
	DataRecordSingleton& instance = DataRecordSingleton::getInstance();
	const std::vector<DataRecordSingleton::DataMap>& datas = instance.getData();
	//instance.writeShapeToFile(); //write std data, only run once

	//read
	std::string path = getExePath();
	std::string filenameStd = path + "\\binFile\\shape_std_0.txt";
	TopoDS_Shape shapeRead = instance.readBinaryDataToShape(filenameStd);
	//compare
	DataRecordSingleton::DataMap& current = instance.getData().back();
	TopoDS_Shape shapeTest = current.m_shape;
	std::string filenameTest = path + "\\binFile\\shape_0.txt";
	BRepTools::Write(shapeTest, filenameTest.c_str());

	bool isN = shapeRead.IsNull();
	bool isE = shapeRead.IsEqual(shapeTest);
	UINT64 sz1 = sizeof(shapeRead);
	UINT64 sz2 = sizeof(shapeTest);
	//compare
	std::string str_shape0 = instance.readBinaryData(filenameStd);
	std::string str_shape1 = instance.readBinaryData(filenameTest);
	//std::string str_shape0(shape0.begin(), shape0.end());
	//std::string str_shape1(shape1.begin(), shape1.end());
	//std::cout << str_shape0 << std::endl;

	bool isEq = str_shape0 == str_shape1;
	bool isEq1 = str_shape0.size() == str_shape1.size() &&
		memcmp(str_shape0.data(), str_shape1.data(), str_shape0.size()) == 0;
	std::vector<int> cmpRes = instance.compareBRepFormat();

	CString cs = isEq ? L"true" : L"false";
	AfxMessageBox(cs);
	instance.clear();
}


//两个共面立方体-布尔Fuse
static TopoDS_Shape getBooleanTest_01()
{
	TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	//TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
	gp_Trsf trsf;
	trsf.SetTranslation(gp_Vec(5, 5, 0));
	BRepBuilderAPI_Transform theBox2(theBox1, trsf);
	//Handle(AIS_Shape) ais2 = new AIS_Shape(myBRepTransformation);

	TopoDS_Shape ShapeFuse = BRepAlgoAPI_Fuse(theBox1, theBox2); //共面布尔后合并
	//遍历数据，只获取数目
	TopTools_IndexedMapOfShape M_edge;
	TopTools_IndexedMapOfShape M_face;
	TopExp::MapShapes(ShapeFuse, TopAbs_ShapeEnum::TopAbs_EDGE, M_edge);
	TopExp::MapShapes(ShapeFuse, TopAbs_ShapeEnum::TopAbs_FACE, M_face);

	return ShapeFuse;
}

static TopoDS_Shape getBooleanTest_02()
{
	//创建拉伸体
	//Handle(Geom_BSplineCurve) bsplineCurve = CreateBSplineCurve();
	//CreateExtrudedShape
	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(50., 0., 0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 0., 0.), gp_Pnt(50., 50., 0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50., 50., 0.), gp_Pnt(0., 0., 0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1, E2, E3);
	gp_Vec direction(0, 0, 5); // 沿 Z 轴拉伸 5 个单位
	TopoDS_Shape extrudedShape = BRepPrimAPI_MakePrism(W, direction);

	return extrudedShape;
}

//OCCT布尔BUG-Torus Cut Cone
static CsgTree getBooleanTest_03()
{
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
	//CsgTree csgtree2 = CsgTree(theShapeA, theShapeC, BOPAlgo_Operation::BOPAlgo_CUT);
	//csgtree.checkTopology();
	//writeTimeDataToCsv();
	writeShapeDataToTxt();
	return csgtree;
	/*
	这个BUG的主要原因是圆锥的尖点正好与圆环面相切了。检验一个几何内核好坏的一个方面就是看
	布尔中处理这些相切情况的表现，会涉及到共线、共面的判断，而共线、共面判断除了算法上的难
	度以外还依赖对几何内核容差的掌控。
	*/
}

//OCCT布尔BUG-Cylinder Cut Sphere
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

//不相交的两个体
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

//验证布尔
CsgTree csgtree;

void CModelingDoc::OnTestBoolBefore()
{
	csgtree = getBooleanTest_03();
	//clear
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next())
		myAISContext->Remove(aListIterator.Value(), Standard_False);

	for (int i = 0; i < 2; i++)
	{
		int color = rand() % (508 + 1);
		int material = rand() % (25 + 1);
		Handle(AIS_Shape) aisShape = new AIS_Shape(csgtree.m_shapeVct[i]);
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
	//clear
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next())
		myAISContext->Remove(aListIterator.Value(), Standard_False);

	//draw bool result
	int color = rand() % (508 + 1);
	int material = rand() % (25 + 1);
	Handle(AIS_Shape) aisShape = new AIS_Shape(csgtree.getResult());
	myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
	myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
	myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
	myAISContext->Display(aisShape, Standard_False);
	const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
	myAISContext->SetSelected(anIOShape, Standard_False);
	myAISContext->DisplayAll(true); //强制刷新
	Fit();

	return;
}
