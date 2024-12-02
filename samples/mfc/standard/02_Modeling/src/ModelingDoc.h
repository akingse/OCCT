// ModelingDoc.h : interface of the CModelingDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dDoc.h>

class CModelingDoc : public OCC_3dDoc
{
	
protected: // create from serialization only
	CModelingDoc();
	DECLARE_DYNCREATE(CModelingDoc)

public:
	virtual ~CModelingDoc();

	void InputEvent     (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );
	void Popup          (const Standard_Integer  x       ,
		    			 const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );

// Generated message map functions
protected:
	//{{AFX_MSG(CModelingDoc)
	afx_msg void OnMirror();
	afx_msg void OnMirroraxis();
	afx_msg void OnRotate();
	afx_msg void OnScale();
	afx_msg void OnTranslation();
	afx_msg void OnDisplacement();
	afx_msg void OnDeform();
	afx_msg void OnBox();
	afx_msg void OnCylinder();
	afx_msg void OnCone();
	afx_msg void OnSphere();
	afx_msg void OnTorus();
	afx_msg void OnWedge();
	afx_msg void OnPrism();
	afx_msg void OnRevol();
	afx_msg void OnPipe();
	afx_msg void OnThru();
	afx_msg void OnEvolved();
	afx_msg void OnDraft();
	afx_msg void OnTestBoolBefore();
	afx_msg void OnTestBoolAfter();
	afx_msg void OnTestBoolDetail();
	afx_msg void OnTestBoolExtra();
	afx_msg void OnCut();
	afx_msg void OnFuse();
	afx_msg void OnSection();
	afx_msg void OnCommon();
	afx_msg void OnPsection();
	afx_msg void OnBlend();
	afx_msg void OnChamf();
	afx_msg void OnEvolvedblend();
	afx_msg void OnPrismLocal();
	afx_msg void OnRevolLocal();
	afx_msg void OnGlueLocal();
	afx_msg void OnDprismLocal();
	afx_msg void OnPipeLocal();
	afx_msg void OnLinearLocal();
	afx_msg void OnSplitLocal();
	afx_msg void OnThickLocal();
	afx_msg void OnOffsetLocal();
	afx_msg void OnVertex();
	afx_msg void OnEdge();
	afx_msg void OnWire();
	afx_msg void OnFace();
	afx_msg void OnShell();
	afx_msg void OnCompound();
	afx_msg void OnGeometrie();
	afx_msg void OnSewing();
	afx_msg void OnExplorer();
	afx_msg void OnBuilder();
	afx_msg void OnValid();
	afx_msg void OnLinear();
	afx_msg void OnVolume();
	afx_msg void OnSurface();
	afx_msg void OnFillwithtang();
	afx_msg void OnButtonFill();
	afx_msg void OnStopStop();
	afx_msg inline void clearDisplay()
	{
		//clear
		AIS_ListOfInteractive aList;
		myAISContext->DisplayedObjects(aList);
		AIS_ListIteratorOfListOfInteractive aListIterator;
		for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next())
			myAISContext->Remove(aListIterator.Value(), Standard_False);
	}
    afx_msg inline void oneShapeDisplay(const TopoDS_Shape& shape, int color = -1, int material = -1)
	{
        if (color == -1)
			color = rand() % (508 + 1);
		if (material == -1)
			material = rand() % (25 + 1);
		//交互式可视化系统（AIS，Application Interactive Service）
		Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
		myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
		myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
		myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
		myAISContext->Display(aisShape, Standard_False);
		const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
		myAISContext->SetSelected(anIOShape, Standard_False);
		Fit();
	}
	//纯几何类型的子类，几何继承自Geom_Geometry，AIS继承自AIS_InteractiveObject
	afx_msg inline void oneGeomDisplay(const Handle(Geom_Geometry)& aisGeom, int color = -1, int material = -1)
	{
		if (color == -1)
			color = rand() % (508 + 1);
		if (material == -1)
			material = rand() % (25 + 1);
		if (aisGeom.IsNull())
			return;
		/*
		AIS_Axis (Geom_Line,Geom_Axis2,Geom_Axis1)
		AIS_Circle (Geom_Circle )
		AIS_Line (Geom_Line )
		AIS_Plane (Geom_Plane)
		*/
		Handle(AIS_InteractiveObject) aisShape = nullptr;
        if (typeid(*aisGeom.get()) == typeid(Geom_Plane))
		{
			//aisShape = new AIS_Plane(opencascade::handle<Geom_Plane>(aisGeom.get()));
			Handle(Geom_Plane) ptr = dynamic_cast<Geom_Plane*>(aisGeom.get());
			aisShape = new AIS_Plane(ptr);
		}
		else if (typeid(*aisGeom.get()) == typeid(Geom_Line))
		{
			Handle(Geom_Line) ptr = dynamic_cast<Geom_Line*>(aisGeom.get());
			aisShape = new AIS_Line(ptr);
		}
		else if (typeid(*aisGeom.get()) == typeid(Geom_Axis1Placement))
		{
			Handle(Geom_Axis1Placement) ptr = dynamic_cast<Geom_Axis1Placement*>(aisGeom.get());
			aisShape = new AIS_Axis(ptr);
		}
		else if (typeid(*aisGeom.get()) == typeid(Geom_Axis2Placement))
		{
			Handle(Geom_Axis2Placement) ptr = dynamic_cast<Geom_Axis2Placement*>(aisGeom.get());
			aisShape = new AIS_Axis(ptr, AIS_TypeOfAxis::AIS_TOAX_XAxis);
		}
		if (aisShape.IsNull())
			return;
		myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
		myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
		myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
		myAISContext->Display(aisShape, Standard_False);
		const Handle(AIS_InteractiveObject)& anIOShape = aisShape;
		myAISContext->SetSelected(anIOShape, Standard_False);
		Fit();
	}
	//handle指针可以使AIS_IO隐式转换
    afx_msg inline void oneAisDisplay(const Handle(AIS_InteractiveObject)& aisShape, int color = -1, int material = -1)
	{
        if (color == -1)
			color = rand() % (508 + 1);
		if (material == -1)
			material = rand() % (25 + 1);
		if (aisShape.IsNull())
			return;
		myAISContext->SetDisplayMode(aisShape, 1, Standard_False);
		myAISContext->SetColor(aisShape, Quantity_NameOfColor(color), Standard_False);
		myAISContext->SetMaterial(aisShape, Graphic3d_NameOfMaterial(material), Standard_False);
		myAISContext->Display(aisShape, Standard_False);
		myAISContext->SetSelected(aisShape, Standard_False);
		Fit();
	}

    afx_msg inline void coordinateSystemDisplay(double k = 1)
	{
		Handle(ISession_Direction) aDirectionX = new ISession_Direction(gp_Pnt(0, 0, 0), gp_Vec(k, 0, 0));
		Handle(ISession_Direction) aDirectionY = new ISession_Direction(gp_Pnt(0, 0, 0), gp_Vec(0, k, 0));
        Handle(ISession_Direction) aDirectionZ = new ISession_Direction(gp_Pnt(0, 0, 0), gp_Vec(0, 0, 0.618 * k));
		myAISContext->Display(aDirectionX, Standard_False);
		myAISContext->Display(aDirectionY, Standard_False);
		myAISContext->Display(aDirectionZ, Standard_False);
	}

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int myState;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
