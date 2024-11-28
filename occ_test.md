

### TKBO布尔

BRepAlgoAPI/BRepAlgoAPI_BooleanOperation 

```c
BRepAlgoAPI_BooleanOperation::Build
	BRepAlgoAPI_BuilderAlgo::IntersectShapes
		BOPAlgo_PaveFiller::Perform
			BOPAlgo_PaveFiller::PerformInternal
			//BOPAlgo\BOPAlgo_PaveFiller.cxx
	BRepAlgoAPI_BuilderAlgo::BuildResult
        BOPAlgo_Builder::PerformWithFiller
            BOPAlgo_Builder::PerformInternal //try-catch
                BOPAlgo_BOP::PerformInternal1
                //BOPAlgo\BOPAlgo_BOP.cxx

添加time-count和check-shape
BRepAlgoAPI_BooleanOperation();//构造 布尔前
BRepAlgoAPI_BooleanOperation::Build() //布尔后
```



### TopoDS_Shape 数据结构

```C++
enum TopAbs_ShapeEnum
{
TopAbs_COMPOUND,
TopAbs_COMPSOLID,
TopAbs_SOLID,
TopAbs_SHELL,
TopAbs_FACE,
TopAbs_WIRE,
TopAbs_EDGE,
TopAbs_VERTEX,
TopAbs_SHAPE
};

class TopoDS_Shape
  Handle(TopoDS_TShape) myTShape;
  TopLoc_Location myLocation;
  TopAbs_Orientation myOrient;

class TopoDS_TShape //topological
  TopoDS_ListOfShape myShapes;
  Standard_Integer   myFlags;

//BRep
class BRep_TFace : public TopoDS_TFace
  Handle(Geom_Surface) mySurface;
  TopLoc_Location myLocation;
  Standard_Real myTolerance;
class BRep_TEdge : public TopoDS_TEdge
  Standard_Real myTolerance;
  Standard_Integer myFlags;
  BRep_ListOfCurveRep myCurves;
class BRep_TVertex : public TopoDS_TVertex
  gp_Pnt myPnt;
  Standard_Real myTolerance;
  BRep_ListOfPointRep myPoints;
    

    
```





### Geom_Geometry 几何数据



![image-20241128175916864](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241128175916864.png)

![image-20241128180405066](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241128180405066.png)

```C++
public Geom_Geometry
class Geom_Point //geometric points in 3D space
class Geom_Vector //vectors in 3D space.
    class Geom_Direction;
    class Geom_VectorWithMagnitude;
class Geom_AxisPlacement //gp_Ax1 单位位矢3D
    class Geom_Axis1Placement;
    class Geom_Axis2Placement;
class Geom_Curve //curves in 3D space, including lines, circles, conics, Bezier or BSpline curves, etc
class Geom_Surface //

public Geom_Curve
class Geom_BoundedCurve  //limited by two finite values of the parameter
    class Geom_BezierCurve;
    class Geom_BSplineCurve;
    class Geom_TrimmedCurve ;
class Geom_Conic //conic curves in 3D space 圆锥曲线
    class Geom_Circle;
    class Geom_Ellipse;
    class Geom_Hyperbola;//双曲线
    class Geom_Parabola; //抛物线
class Geom_Line //an infinite line.
class Geom_OffsetCurve //an offset curve in 3D space
class ShapeExtend_ComplexCurve  //consists of several segments.

public Geom_Surface
class Geom_BoundedSurface //defined by a rectangle in its 2D
    class Geom_BezierSurface;
    class Geom_BSplineSurface;
    class Geom_RectangularTrimmedSurface;
class Geom_ElementarySurface //基本曲面 
    class Geom_ConicalSurface;//圆锥面
    class Geom_CylindricalSurface;//圆柱面
    class Geom_SphericalSurface;//球面
    class Geom_ToroidalSurface;//环面
    class Geom_Plane;//无限平面
class Geom_SweptSurface //constructed by sweeping a curve with another curve
    class Geom_SurfaceOfExtrusion;
    class Geom_SurfaceOfRevolution;
class Geom_OffsetSurface //an offset surface in 3D
class GeomPlate_Surface //平板平面
class ShapeExtend_CompositeSurface //a grid of surfaces

public BRep_CurveRepresentation //包含几何曲线和面上线
    class BRep_CurveOn2Surfaces; //between two surfaces
    class BRep_GCurve ;//Root class for the geometric curves representation 几何曲线
        class BRep_Curve3D; //持有Geom_Curve handle
        class BRep_CurveOnSurface; //面上线
            class BRep_CurveOnClosedSurface;
    class BRep_Polygon3D;//a 3D polygon
    class BRep_PolygonOnSurface;//2D polygon in the parametric space of a surface
        class BRep_PolygonOnClosedSurface;
    class BRep_PolygonOnTriangulation ;//an array of nodes on a triangulation.
        class BRep_PolygonOnClosedTriangulation


```



### 布尔API

```C++
class BRepBuilderAPI_Command //all commands
class BRepBuilderAPI_MakeShape //all  shape constructions
class BRepAlgoAPI_Algo //API algorithms
    //继承 BOPAlgo_Options
class BRepAlgoAPI_BuilderAlgo //General Fuse algorithm
    //myArguments
class BRepAlgoAPI_BooleanOperation //performing Boolean Operations; arguments分为 *Objects* and *Tools*.
    //myTools and myOperation
class BRepAlgoAPI_Cut/BRepAlgoAPI_Common/BRepAlgoAPI_Fuse //布尔接口

class BOPAlgo_PaveFiller //
    

    
```



### MakeShape基本体造型

```C++
public BRepBuilderAPI_MakeShape //all shape constructions
class BRepPrimAPI_MakeOneAxis//construct rotational primitives
    BRepPrimAPI_MakeSphere//球
    BRepPrimAPI_MakeCone//台
    BRepPrimAPI_MakeCylinder//柱
    BRepPrimAPI_MakeTorus//环
    BRepPrimAPI_MakeRevolution //build revolved shapes，持有BRepPrim_Revolution
class BRepPrimAPI_MakeBox
class BRepPrimAPI_MakeSweep
    BRepOffsetAPI_MakePipe 
    BRepOffsetAPI_MakePipeShell 
    BRepPrimAPI_MakePrism //linear swept, called prisms
    BRepPrimAPI_MakeRevol //revolved sweep topologies，持有BRepSweep_Revol
```



