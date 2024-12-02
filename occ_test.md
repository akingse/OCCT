

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

![image-20241202143329733](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241202143329733.png)



### TopoDS_Shape 数据结构

![image-20241129143355763](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241129143355763.png)

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
class BRep_TFace : public TopoDS_TFace //持有多种类型数据
  Poly_ListOfTriangulation myTriangulations;
  Handle(Poly_Triangulation) myActiveTriangulation;
  Handle(Geom_Surface) mySurface;
  TopLoc_Location myLocation;
  Standard_Real myTolerance;
  Standard_Boolean myNaturalRestriction;
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

![image-20241129135115037](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241129135115037.png)

![image-20241128180405066](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241128180405066.png)

![image-20241128175916864](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241128175916864.png)

坐标系gp_Ax

```cpp
//gp_Ax1
Geom_AxisPlacement持有gp_Ax1，是位矢
描述：表示一条直线（轴），由一个点和一个方向向量定义。它通常用作旋转轴或线性变换的基准。
构造：需要一个点（gp_Pnt）表示轴上的某个位置，以及一个方向向量（gp_Dir）表示轴的方向。
用途：常用于旋转变换、构建圆弧和圆的参数化。
//gp_Ax2
右手系，持有gp_Ax1和xy两个相互垂直的方向向量，Geom_Axis2Placement不持有gp_Ax2，持有继承的gp_Ax1和自身的vxdir/vydir
构造：需要一个（gp_Ax1）表示坐标系的原点，一个方向向量（gp_Dir vydir）作为Y轴，以及一个法向量（gp_Dir vxdir）作为X轴
用途：常用于定义一个平面，在该平面上进行的几何操作，或者创建圆和椭圆。
//gp_Ax3
与gp_Ax2成员数据相同，不同之处在于可以表示左手系；
描述：表示三维坐标系，由一个点（原点）和三个正交方向向量（X轴、Y轴和Z轴）定义。它提供了一个完整的三维空间坐标系。
用途：常用于复杂的三维几何操作，提供一个完整的坐标系。
    
class gp_Trsf //
  Standard_Real scale;
  gp_TrsfForm shape;
  gp_Mat matrix;
  gp_XYZ loc;
class gp_GTrsf //通用变换，仿射变换
  gp_Mat matrix;
  gp_XYZ loc;
  gp_TrsfForm shape;
  Standard_Real scale;

class gp_Pln //持有gp_Ax3
    
    
"non-persistent"（非持久）指的是一种变换，它并不会永久地改变几何对象的定义或状态，而是仅在特定的上下文中有效或临时存在。
临时性：非持久变换通常是指在进行某些计算或操作时应用的变换，这些变换在操作完成后不会影响原始几何对象的定义。 
```

几何点线面

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
        class BRep_CurveOnSurface; //面上线，持有 Geom2d_Curve handle
            class BRep_CurveOnClosedSurface;
    class BRep_Polygon3D;//a 3D polygon
    class BRep_PolygonOnSurface;//2D polygon in the parametric space of a surface
        class BRep_PolygonOnClosedSurface;
    class BRep_PolygonOnTriangulation ;//an array of nodes on a triangulation.
        class BRep_PolygonOnClosedTriangulation

public BRep_PointRepresentation
    class BRep_PointOnCurve     
    class BRep_PointsOnSurface 
        class BRep_PointOnSurface
        class BRep_PointOnCurveOnSurface
            
public BRep_SurfaceRepresentation  //not exist
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

### BRepBuilderAPI_Command 造型API

```C++
//布尔操作


//拓扑创建
class BRepBuilderAPI_MakeVertex
class BRepBuilderAPI_MakeEdge
class BRepBuilderAPI_MakeWire
class BRepBuilderAPI_MakeFace
class BRepBuilderAPI_MakeSolid

//造型创建
//BRepBuilderAPI_MakeShape

class BRepPrimAPI_MakeWedge//专门用于构建楔形体（wedge），即具有倾斜面（inclined faces）的立方体。
//class BRepOffsetAPI_
class BRepOffsetAPI_ThruSections //LoftBody放样体
class BRepOffsetAPI_MakeEvolved//通过将一个轮廓沿着一个平面脊（可以是线框或面）进行扫掠，从而创建出具有特定形状的实体或壳体
class BRepOffsetAPI_DraftAngle //模具倾斜角度 //变成锥形
    
    
BRepBuilderAPI_ModifyShape // 包提供了以下框架以执行此类修改：
	class BRepBuilderAPI_Copy 用于生成形状的副本，
	class BRepBuilderAPI_Transform 和 BRepBuilderAPI_GTransform 用于对形状应用几何变换，
        //Geometric transformation
	class BRepBuilderAPI_NurbsConvert 用于将形状的整个几何转换为 NURBS 几何，
	class BRepOffsetAPI_DraftAngle 用于构建锥形形状。

class BRepAlgoAPI_Section //截面操作
    //计算不同几何形状（如体、面、边等）之间的交集。它通过分析输入几何体之间的相互关系，生成新的几何体。

    BRepFilletAPI_MakeFillet //yuan'jiao
    BRepFilletAPI_MakeChamfer //直角倒角
```

![image-20241202112102345](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241202112102345.png)



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



```
圆台测试
	topoinfo.m_shape = BRepPrimAPI_MakeCone(3, 2, 10).Shape();

```



### BRep_CurveOnSurface

```
用于表示在某个表面上定义的曲线
  Handle(Geom2d_Curve) myPCurve;
  Handle(Geom_Surface) mySurface;
myUV1 是一个二维点（gp_Pnt2d），表示曲线在其所在表面上的起始参数坐标（U、V）。这通常用于定义曲线在表面上的起点位置。
myUV2 同样是一个二维点（gp_Pnt2d），表示曲线在其所在表面上的结束参数坐标（U、V）。这用于定义曲线在表面上的终点位置。


```

