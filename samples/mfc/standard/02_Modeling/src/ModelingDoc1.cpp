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
#include "commonOCCUtility.h" 
#include "..\..\..\OCCT\src\BOPAlgo\DataRecordSingleton.h" //USING_OPENCASCADE_TEST
#include "ModelingDoc1.h" //custom class

using namespace std;
using namespace occ;
using namespace test;
using namespace clash;
using opencascade::handle;

static CsgTree g_csgtree;

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

//OCCT��������BUG Torus-Cut-Cone
static CsgTree getBooleanTest_03()
{
	//DataRecordDashboard::getInstance().setOpenCheck(true);

	double R = 10, r = 2;
	double H = 30;
	double offset = 0.;
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

//OCCT����BUGCylinder-Cut-Sphere//���޸�
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

//�򵥲���(sphere-common-sphere)
static CsgTree getBooleanTest_07()
{
	TopoDS_Shape theShapeA = BRepPrimAPI_MakeSphere(2).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(gp_Pnt(1, 0, 0), 2).Shape();
	CsgTree csgtree = CsgTree(theShapeA, theShapeB, BOPAlgo_Operation::BOPAlgo_COMMON);
	//TopoDS_Shape theShapeR = csgtree.m_shapeResult;
	// 
	//record
	TopoInfoRecord topoinfo;
	topoinfo.m_shape = csgtree.m_shapeResult;
	topoinfo.TraverseShape(topoinfo.m_shape);
	topoinfo.getGeomAndShape();
	return csgtree;
}

//�򵥲���(box-cut-sphere)
static CsgTree getBooleanTest_08()
{
    TopoDS_Shape theShapeA = BRepPrimAPI_MakeBox(30, 20, 10).Shape();
	TopoDS_Shape theShapeB = BRepPrimAPI_MakeSphere(gp_Pnt(15, 10, 0), 2).Shape();
	CsgTree csgtree = CsgTree(theShapeA, theShapeB, BOPAlgo_Operation::BOPAlgo_CUT);
	//record
	TopoInfoRecord topoinfo;
	topoinfo.m_shape = csgtree.m_shapeResult;
	topoinfo.TraverseShape(topoinfo.m_shape);
	topoinfo.getGeomAndShape();
	return csgtree;
}

//��tcl�ļ��ж�ȡ
static CsgTree getBooleanTest_09()
{
	std::string filename = get_exe_path();
	filename += "/binFile/BO_1.tcl";
	return readBooleanFromTclFile(filename);
}

//��֤����
void CModelingDoc::OnTestBoolBefore() //using icon -
{
	clearDisplay();
	//DataRecordDashboard::getInstance().setOpenOutput();
	DataRecordSingleton& instance = DataRecordSingleton::getInstance();
	g_csgtree = getBooleanTest_03();
	std::vector<DataRecordSingleton::FaceDetail> faceDetailVct = instance.getFaceDetial();
    DataRecordSingleton::ShapeCheck shapeCheck = instance.getShapeCheck();

	for (int i = 0; i < faceDetailVct.size(); i++)
	{
		std::vector<TopoDS_Shape> shapes = faceDetailVct[i].getShapes();
		for (int j = 0; j < shapes.size(); j++)
		{
			oneShapeDisplay(shapes[j]);
		}
	}

	std::vector<TopoDS_Shape> shapeVct = g_csgtree.m_shapeArgument;
	for (int i = 0; i < shapeVct.size(); i++)
	{
		//oneShapeDisplay(shapeVct[i]);
	}
	Fit();
	instance.clear();
	return;
}

void CModelingDoc::OnTestBoolAfter() //using icon +
{
	clearDisplay();
	//draw bool result
	oneShapeDisplay(g_csgtree.m_shapeResult);
	return;
}
