#include "stdafx.h"
#include "ModelingDoc.h"
#include "ModelingApp.h"
#include "ResultDialog.h"
#include "State.h"
//std

//modeling
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <BRep_TEdge.hxx>
#include <TopoDS_Iterator.hxx>
#include "..\..\..\OCCT\src\BOPAlgo\DataRecordSingleton.h" //USING_OPENCASCADE_TEST

using namespace std;
using namespace test;
using opencascade::handle;

std::vector<const BRep_TFace*> brepFaceVct;
std::vector<const BRep_TEdge*> brepEdgeVct;
std::vector<const BRep_TVertex*> brepVertexVct;
std::vector<handle<Geom_Surface>> surfaceVct;
std::vector<BRep_ListOfCurveRepresentation> curveVct;
std::vector<gp_Pnt> pointVct;

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
		instance.setOpenTime(false);
		if (m_shapeVct.empty())
			return;
        if (m_shapeVct.size() == 3)
		{
			BRepAlgoAPI_Check checkBefore = BRepAlgoAPI_Check(m_shapeVct[0], m_shapeVct[1]);
			Message_ProgressRange rangeBefore;
			checkBefore.Perform(rangeBefore);
			BOPAlgo_ListOfCheckResult resultBefore = checkBefore.Result();
			//if (checker.IsValid())
			//{
			//	std::cout << "Shape is valid." << std::endl;
			//	//return;
			//}
			BRepAlgoAPI_Check checkAfter = BRepAlgoAPI_Check(m_shapeVct[2]);
			Message_ProgressRange rangeAfter;
			checkAfter.Perform(rangeAfter);
			BOPAlgo_ListOfCheckResult resultAfter = checkAfter.Result();

		}
		return;
	}

	static void TraverseShape(const TopoDS_Shape& shape)
	{
		for (TopoDS_Iterator it(shape); it.More(); it.Next())
		{
			TopoDS_Shape subShape = it.Value();
			string typeName;
			if (!subShape.TShape().IsNull())
				typeName = typeid(*subShape.TShape()).name();
			const TopAbs_ShapeEnum shapeType = subShape.ShapeType();
			//switch-case
			if (shapeType == TopAbs_COMPOUND) {
				const TopoDS_Compound& compound = TopoDS::Compound(subShape);
				TraverseShape(compound);
			}
			else if (shapeType == TopAbs_COMPSOLID) {
				const TopoDS_CompSolid& compsolid = TopoDS::CompSolid(subShape);
				TraverseShape(compsolid);
			}
			else if (shapeType == TopAbs_SOLID) {
				const TopoDS_Solid& solid = TopoDS::Solid(subShape);
				TraverseShape(solid);
			}
			else if (shapeType == TopAbs_SHELL) {
				const TopoDS_Shell& shell = TopoDS::Shell(subShape);
				TraverseShape(shell);
			}
			else if (shapeType == TopAbs_FACE) {
				const TopoDS_Face& face = TopoDS::Face(subShape);
				BRep_TFace* brepFace = dynamic_cast<BRep_TFace*>(face.TShape().get());
				if (brepFace != nullptr) //no repeat
				{
					brepFaceVct.push_back(brepFace);
					surfaceVct.push_back(brepFace->Surface());
					//surfaceVct[0].get()->
				}
				TraverseShape(face);
			}
			else if (shapeType == TopAbs_WIRE) {
				const TopoDS_Wire& wire = TopoDS::Wire(subShape);
				TraverseShape(wire);
			}
			else if (shapeType == TopAbs_EDGE) {
				const TopoDS_Edge& edge = TopoDS::Edge(subShape);
				BRep_TEdge* brepEdge = dynamic_cast<BRep_TEdge*>(edge.TShape().get());
				//shared_ptr<BRep_TEdge> ptr = shared_ptr<BRep_TEdge>(brepEdge);//cause crash
				if (brepEdge != nullptr &&
					std::find(brepEdgeVct.begin(), brepEdgeVct.end(), brepEdge) == brepEdgeVct.end())
				{
					brepEdgeVct.push_back(brepEdge);
					curveVct.push_back(brepEdge->Curves());
					//curveVct[0].begin;
				}
				TraverseShape(edge);
			}
			else if (shapeType == TopAbs_VERTEX) {
				const TopoDS_Vertex& vertex = TopoDS::Vertex(subShape);
				BRep_TVertex* brepVertex = dynamic_cast<BRep_TVertex*>(vertex.TShape().get());
				//shared_ptr<const BRep_TVertex> ptr = shared_ptr<const BRep_TVertex>(brepVertex);//cause crash
				if (brepVertex != nullptr &&
					std::find(brepVertexVct.begin(), brepVertexVct.end(), brepVertex) == brepVertexVct.end())
				{
					brepVertexVct.push_back(brepVertex);
					pointVct.push_back(brepVertex->Pnt());
					//pointVct[0].IsEqual()
				}
			}
			//else
		}
	}

};
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


//��֤����
void CModelingDoc::OnTestBoolBefore()
{
	//g_csgtree = getBooleanTest_03();
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
		Handle(AIS_Shape) aisShape = new AIS_Shape(g_csgtree.m_shapeVct[i]);
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
	Handle(AIS_Shape) aisShape = new AIS_Shape(g_csgtree.getResult());
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
