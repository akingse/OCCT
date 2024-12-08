

### TKBO布尔

BRepAlgoAPI/BRepAlgoAPI_BooleanOperation 

```c
//添加time-count和check-shape
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

BRepAlgoAPI_Cut
BRepAlgoAPI_BooleanOperation //布尔减的父类
BRepAlgoAPI_BooleanOperation::Build() //执行布尔操作
    
BRepAlgoAPI_BuilderAlgo::IntersectShapes //求交，注意这里求交时Arguments合并了，但是布尔前数据分开存储
BRepAlgoAPI_BuilderAlgo(myArguments) //BRepAlgoAPI_BooleanOperation(myTools);
BOPAlgo_PaveFiller::PerformInternal //执行求交
    
BRepAlgoAPI_BuilderAlgo::BuildResult //构建
BOPAlgo_Builder::PerformWithFiller
BOPAlgo_BOP::PerformInternal //执行构建

//类图
BOPAlgo_Algo - BOPAlgo_BuilderShape - BOPAlgo_Builder - BOPAlgo_ToolsProvider - BOPAlgo_BOP
    		|- BOPAlgo_PaveFiller
BRepAlgo_Algo - BRepAlgoAPI_BuilderAlgo - BRepAlgoAPI_BooleanOperation - BRepAlgoAPI_Cut

//BRepAlgo布尔父类BuilderAlgo有两个核心计算成员（指针）
BRepAlgoAPI_BuilderAlgo持有BOPAlgo_PaveFiller*(BOPDS_DS*)和BOPAlgo_Builder*(BOPDS_DS*)两个计算大类

BOPAlgo_PaveFiller持有关键数据BOPDS_DS*和BOPDS_MapOfPaveBlock(BOPDS_PaveBlock)
    
BOPDS_DS持有BOPDS_VectorOfShapeInfo(myLines)，BOPDS_VectorOfFaceInfo，BOPDS_VectorOfListOfPaveBlock，BOPDS_VectorOfInterf
 




```

![image-20241202143329733](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241202143329733.png)

布尔相关 BRepAlgo_Algo

```CPP
//Brep-Algo设置和数据存储
public BRepAlgoAPI_Algo //继承于造型类BRepBuilderAPI_MakeShape和布尔基类BOPAlgo_Options
	//BOPAlgo_Options包括内存分配，错误和警告，并行处理，模糊容差 (Fuzzy tolerance)，OBB包围盒
	//BRepBuilderAPI_MakeShape是Shape构建的根类，记录了TopoDS_Shape和生成历史记录TopTools_ListOfShape
class BRepAlgoAPI_BuilderAlgo //通用的融合算法
    TopTools_ListOfShape myArguments;//存储参与操作的输入形状列表。只存object
    Standard_Boolean myNonDestructive;//布尔值，用于管理非破坏性模式。
    BOPAlgo_GlueEnum myGlue;//存储粘合模式的管理选项。
    Standard_Boolean myCheckInverted;布尔值，用于管理对倒置固体的检查。
    Standard_Boolean myFillHistory;布尔值，控制历史记录的收集。
    Standard_Boolean myIsIntersectionNeeded;布尔值标识符，控制是否需要执行输入形状的交集计算。
    BOPAlgo_PPaveFiller myDSFiller;交集工具，用于执行输入形状的交集计算。
    BOPAlgo_PBuilder myBuilder;构建工具，负责根据交集结果构建最终的几何体。
    Handle(BRepTools_History) myHistory;一般历史工具，包含在操作过程中所有形状修改的历史记录
    Handle(BRepTools_History) mySimplifierHistory;结果形状简化的历史记录。用于跟踪在简化过程中所做的修改
    
class BRepAlgoAPI_BooleanOperation //执行布尔的基类
//! - FUSE - union of two groups of objects;
//! - COMMON - intersection of two groups of objects;
//! - CUT - subtraction of one group from the other;
//! - SECTION - section edges and vertices of all arguments;//截面剖切
  TopTools_ListOfShape myTools;  //!< Tool arguments of operation //object在父类BRepAlgoAPI_BuilderAlgo中
  BOPAlgo_Operation myOperation; //!< Type of Boolean Operation
    
//BOP-Algo算法计算类==================================================
public BOPAlgo_Algo//布尔运算的基础类。它为布尔运算算法提供了一个通用框架//Boolean Operations
	TColStd_Array1OfReal mySteps;//动态数组类//用于定义与几何计算相关的步长或分割参数。
class BOPAlgo_BuilderShape //处理以几何形状作为结果的算法，记录历史
    TopoDS_Shape myShape;存储操作的结果形状。
    TopTools_ListOfShape myHistShapes;用于存储与历史记录相关的形状列表。
    TopTools_MapOfShape myMapShape;存储所有输入参数形状的缓存映射。
	Standard_Boolean myFillHistory;布尔值，用于控制历史记录的填充。
    Handle(BRepTools_History) myHistory;存储历史记录工具的句柄，负责管理形状修改的详细历史。
        
class BOPAlgo_Builder //通用Fuse算法，构建出split parts 
    //用于执行几何形状的通用融合操作。它的设计目标是处理输入形状的分割，并生成最终的几何结果。
    TopTools_ListOfShape myArguments;存储操作的输入形状列表。
    TopTools_MapOfShape myMapFence;提供输入形状唯一性的围栏映射。
    BOPAlgo_PPaveFiller myPaveFiller;Pave Filler 是一个用于处理子形状交集的算法。
    BOPDS_PDS myDS;数据结构，用于存储交集信息的持有者。
    Handle(IntTools_Context) myContext;上下文工具，用于缓存一些计算密集型的算法，如投影器和分类器。
    Standard_Integer myEntryPoint;控制 Pave Filler 删除的入口点。
    TopTools_DataMapOfShapeListOfShape myImages;存储输入形状子形状的映射关系
    TopTools_DataMapOfShapeShape myShapesSD;存储 SD（Shape Data）形状的映射。
    TopTools_DataMapOfShapeListOfShape myOrigins;反向映射，存储每个影像的原始形状。
    TopTools_DataMapOfShapeListOfShape myInParts;存储输入形状的内部面和获取面。
    Standard_Boolean myNonDestructive;安全处理选项，允许在操作过程中避免对输入形状的修改。
    BOPAlgo_GlueEnum myGlue;粘合选项，允许加速输入形状的交集计算。
    Standard_Boolean myCheckInverted;检查倒置选项，允许用户禁用输入固体的倒置状态检查
class BOPAlgo_ToolsProvider //辅助类，工具提供
  TopTools_ListOfShape myTools;//变量存储了工具形状的列表。工具形状通常是用于布尔运算的辅助形状
  TopTools_MapOfShape  myMapTools;//映射用于提供工具形状的唯一性，确保在工具列表中每个工具形状都是唯一的。

class BOPAlgo_BOP //执行布尔运算的核心算法类//注释类似BRepAlgoAPI_BooleanOperation，支持BOPAlgo_Options的设置
  BOPAlgo_Operation myOperation;当前执行的布尔操作类型
  Standard_Integer  myDims[2];用于存储与输入形状相关的维度信息。
  TopoDS_Shape      myRC;这个变量用于存储操作的结果形状（Result Shape）

class BOPAlgo_PaveFiller //布尔运算算法的交集阶段
//! 1. Vertex/Vertex;<br>
//! 2. Vertex/Edge;<br>
//! 3. Edge/Edge;<br>
//! 4. Vertex/Face;<br>
//! 5. Edge/Face;<br>
//! 6. Face/Face.<br>
  TopTools_ListOfShape myArguments;//存储参与布尔运算的输入形状列表
  BOPDS_PDS myDS;//布尔运算的数据结构（Data Structure）。它用于存储和管理计算过程中的中间结果和交集结果，
  BOPDS_PIterator myIterator;//用于遍历数据结构的迭代器。
  Handle(IntTools_Context) myContext;//存储与计算相关的上下文信息
  BOPAlgo_SectionAttribute mySectionAttribute;//存储与截面属性相关的设置。可以用于自定义面之间的交集处理
  Standard_Boolean myNonDestructive;//是否在计算过程中保持输入形状的非破坏性
  Standard_Boolean myIsPrimary;//指示当前对象是否为主要算法。
  Standard_Boolean myAvoidBuildPCurve;//布尔值，指示是否避免构建参数曲线。
  BOPAlgo_GlueEnum myGlue;//存储与“Glue”选项相关的设置，这些设置用于优化处理在交集过程中相交的子形状。
  BOPAlgo_DataMapOfIntegerMapOfPaveBlock myFPBDone;//存储已交集的面和PaveBlock。记录哪些面已经处理过，避免重复计算
  TColStd_MapOfInteger myIncreasedSS;//存储在操作过程中增加了容差的子形状的集合。
  TColStd_MapOfInteger myVertsToAvoidExtension;//存储在 E/E（边与边）或 E/F（边与面）交集点附近的顶点集合
  NCollection_DataMap <> myDistances;//存储形状之间的最小距离映射，以便在后续计算中提供参考
    
//BOPDS 中间数据类==================================================
class BOPDS_DS//提供布尔组件中算法的数据结构控制，包括通用融合、布尔运算、截面、体积生成、分割器和单元构建器等
  Handle(NCollection_BaseAllocator) myAllocator;
  TopTools_ListOfShape myArguments;
  Standard_Integer myNbShapes;存储输入形状的数量
  Standard_Integer myNbSourceShapes;存储源形状的数量，通常指的是原始输入形状的数量
  BOPDS_VectorOfIndexRange myRanges;存储每个输入形状及其子形状的索引范围。
  BOPDS_VectorOfShapeInfo myLines;//存储关于输入形状和新生成形状的信息 ShapeInfo
  TopTools_DataMapOfShapeInteger myMapShapeIndex;这是一个映射，用于将形状与其对应的索引值关联起来
  BOPDS_VectorOfListOfPaveBlock myPaveBlocksPool;//存储铺块的集合。PaveBlock 
  BOPDS_DataMapOfPaveBlockCommonBlock myMapPBCB;//存储铺块和公共块之间的映射。CommonBlock
  BOPDS_VectorOfFaceInfo myFaceInfoPool;//存储源面状态的信息。FaceInfo
  TColStd_DataMapOfIntegerInteger myShapesSD;存储形状与其对应的同一域形状的映射。
  TColStd_DataMapOfIntegerListOfInteger myMapVE;存储顶点与边之间的映射。
  BOPDS_MapOfPair myInterfTB;存储边缘与边缘之间的interference干涉信息。
  BOPDS_VectorOfInterfVV myInterfVV;
  BOPDS_VectorOfInterfVE myInterfVE;
  BOPDS_VectorOfInterfVF myInterfVF;
  BOPDS_VectorOfInterfEE myInterfEE;
  BOPDS_VectorOfInterfEF myInterfEF;
  BOPDS_VectorOfInterfFF myInterfFF;
  BOPDS_VectorOfInterfVZ myInterfVZ; //vertex/solid
  BOPDS_VectorOfInterfEZ myInterfEZ; //edge/solid.
  BOPDS_VectorOfInterfFZ myInterfFZ; //face/solid
  BOPDS_VectorOfInterfZZ myInterfZZ; //solid/solid
  TColStd_MapOfInteger myInterfered;存储干扰形状的映射。

    
class BOPDS_PaveBlock //存储与边缘上的铺块（pave block）相关信息的类
//它通常是在几何布尔运算中用于处理边缘交集或分割的结构。铺块是通过在边缘上相邻的两个铺块组合而成的，提供了更高层次的几何信息
  Handle(NCollection_BaseAllocator) myAllocator;
  Standard_Integer myEdge;//这个变量表示与铺块关联的边缘的索引
  Standard_Integer myOriginalEdge;原始边缘的索引，是指在布尔运算处理之前的边缘
//BOPDS_Pave vertex on edge信息，记录edge索引和parameter
  BOPDS_Pave myPave1; //与该铺块相关的第一个铺块对象
  BOPDS_Pave myPave2; //与该铺块相关的第二个铺块对象，这两个铺块共同构成了一个完整的铺块块（pave block）
  BOPDS_ListOfPave myExtPaves;//铺块列表，用于存储与该铺块相关的外部铺块
  Standard_Real myTS1;//第一个铺块的某个参数
  Standard_Real myTS2;//第二个铺块的某个参数
  Bnd_Box myShrunkBox;//表示铺块的缩小版本。
  TColStd_MapOfInteger myMFence;//映射，通常用于存储与铺块相关的某些标识或索引
  Standard_Boolean myIsSplittable;//指示该铺块是否可以被拆分
class BOPDS_CommonBlock //管理那些在给定公差范围内与其他铺块或面存在几何重合的铺块
  BOPDS_ListOfPaveBlock myPaveBlocks; //与该公共块相关的所有铺块（pave blocks）
  TColStd_ListOfInteger myFaces;      //存储与公共块相关的面（faces）索引
  Standard_Real myTolerance;
    
class BOPDS_ShapeInfo //存储关于几何形状的有用信息。这些信息可以包括形状的类型、状态、参数等
  TopoDS_Shape myShape;
  TopAbs_ShapeEnum myType;
  Bnd_Box myBox;
  TColStd_ListOfInteger mySubShapes; //储与该形状相关的子形状的索引。
  Standard_Integer myReference;用于存储对形状的引用计数或标识符。
  Standard_Integer myFlag;存储标志信息，是否已被处理
    
class BOPDS_FaceInfo //存储与面（Face）相关的状态和信息。包括面在布尔运算中的状态、是否被修改、是否参与交集等。
  Handle(NCollection_BaseAllocator) myAllocator;
  Standard_Integer myIndex; 面索引，所在myLines中的
  BOPDS_IndexedMapOfPaveBlock myPaveBlocksIn;//存储与该面相关的输入铺块（pave blocks）
  TColStd_MapOfInteger myVerticesIn;存储与该面相关的输入顶点（vertices）
  BOPDS_IndexedMapOfPaveBlock myPaveBlocksOn;//存储与该面相关的在面上（on）存在的铺块
  TColStd_MapOfInteger myVerticesOn;存储与该面相关的在面上存在的顶点
  BOPDS_IndexedMapOfPaveBlock myPaveBlocksSc;//存储与该面相关的剪切铺块（section）
  TColStd_MapOfInteger myVerticesSc;存储与该面相关的剪切顶点。
    
    
class TopoDS_AlertWithShape //错误警告，存储在BOPAlgo_Options(Message_Report myReport)


```



### TopoDS_Shape 数据结构

![image-20241203163721481](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241203163721481.png)

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

class TopoDS_Shape //六层次拓扑的基类，持有TopoDS_TShape
  Handle(TopoDS_TShape) myTShape;
  TopLoc_Location myLocation;
  TopAbs_Orientation myOrient;

class TopoDS_TShape //topological，持有TopoDS_Shape列表
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

```cpp
class BRep_TFace继承自TopoDS_TFace，在TopoDS_Face拓扑列表成员myTShape中；
//BRep_TFace拓扑面有多个成员数据，主要是mySurface，基类Geom_Surface纯几何数据
class BRep_TEdge继承自TopoDS_TEdge，在TopoDS_Edge拓扑列表成员myTShape中；
//BRep_TEdge拓扑面有3个成员数据，主要是myCurves列表，数据是BRep_CurveRepresentation/BRep_GCurve，子类持有Geom_Curve指针，对Geom_Curve进行一层封装

class BRep_Curve3D 
  Handle(Geom_Curve) myCurve;

class BRep_CurveOnSurface用于表示在某个表面上定义的曲线
  myUV1 是一个二维点（gp_Pnt2d），表示曲线在其所在表面上的起始参数坐标（U、V）。这通常用于定义曲线在表面上的起点位置。
  myUV2 同样是一个二维点（gp_Pnt2d），表示曲线在其所在表面上的结束参数坐标（U、V）。这用于定义曲线在表面上的终点位置。
  Handle(Geom2d_Curve) myPCurve;
  Handle(Geom_Surface) mySurface;

```



### 数学类型gp_Ax

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


```

### 容器

![image-20241205185243138](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241205185243138.png)

```cpp
OCCT中有两个容器集
//TCollection 包在 1998 年标准化之前用于模拟模板。从 OССТ 开始7.0 中，它被相应的 NCollection 实例取代。NCollection 包是一个较新的基于模板的包，提供现代容器。
    
//容器 new collect 新集合
NCollection_DynamicArray(NCollection_Vector) //动态数组容器，允许动态调整大小
NCollection_Array1
NCollection_List：//链表容器，支持在任意位置高效插入和删除元素。
NCollection_Map：//基于哈希表实现的映射容器，用于存储键值对。
NCollection_Sequence：//用于存储序列数据，支持访问、插入和删除操作
NCollection_DataMap
    
DynamicArray
Append(val)：将元素 val 添加到数组的末尾，数组的长度会自动增加。
SetValue(i, val)：在指定索引 i 处设置值 val。
vector myContainer;它是类的核心数据结构，实际存储对象的地方。
allocator_type myAlloc;指定了内存分配器的类型，用于管理容器中对象的内存分配和释放。
size_t myInternalSize;分配总容量
size_t myUsedSize;当前已使用


"non-persistent"（非持久）指的是一种变换，它并不会永久地改变几何对象的定义或状态，而是仅在特定的上下文中有效或临时存在。
临时性：非持久变换通常是指在进行某些计算或操作时应用的变换，这些变换在操作完成后不会影响原始几何对象的定义。 
```



### 几何点线面

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

### Adaptor包装器

<img src="C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241205185959027.png" alt="image-20241205185959027" style="zoom:50%;" />

<img src="C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241205190243238.png" alt="image-20241205190243238" style="zoom:50%;" />

```cpp
//OCCT 为 OSD 包中的低级操作系统设施提供独立于平台的接口

public Adaptor3d_Surface//虚基类，用于几何算法对曲面的操作
    //允许将曲面在 U 和 V 方向上按任意连续性分解为多个间隔//BSpline 曲面的多项式系数会被缓存以提高性能
class GeomAdaptor_Surface //用于连接 Geom 包中的曲面服务和使用这些曲面的算法所需的功能
  Handle(Geom_Surface) mySurface;对 Geom 包中的曲面的句柄。它表示当前适配的曲面实例
  Standard_Real myUFirst;表示曲面在 U 方向上的起始参数值。
  Standard_Real myULast;表示曲面在 U 方向上的结束参数值
  Standard_Real myVFirst;表示曲面在 V 方向上的起始参数值
  Standard_Real myVLast;表示曲面在 V 方向上的结束参数值
  Standard_Real myTolU;表示在 U 方向上的公差值
  Standard_Real myTolV;表示在 V 方向上的公差值
  Handle(Geom_BSplineSurface) myBSplineSurface; ///< B-spline representation to prevent downcasts
  mutable Handle(BSplSLib_Cache) mySurfaceCache; //缓存B-spline or Bezier surface
  GeomAbs_SurfaceType mySurfaceType;曲面的类型
  Handle(GeomEvaluator_Surface) myNestedEvaluator; 对嵌套评估器的句柄，负责计算嵌套复杂曲面的值
class BRepAdaptor_Surface //将 BRep 拓扑中的面（Face）视为一个三维曲面
  GeomAdaptor_Surface mySurf;它用于适配和管理与 BRep 面相关的几何曲面。利用 GeomAdaptor_Surface 提供的功能来访问和操作 BRep 拓扑中的面
  gp_Trsf myTrsf;几何变换
  TopoDS_Face myFace;表示 BRep 拓扑中的一个面
    
    
public Adaptor3d_Curve
class GeomAdaptor_Curve
class BRepAdaptor_Curve
    
    
```



### BRepBuilderAPI_Command 造型API

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

    BRepFilletAPI_MakeFillet //圆角倒角
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





### OCC源码修改-TKBO-BOPAlgo

| source                           | process                                                      |
| -------------------------------- | ------------------------------------------------------------ |
| BOPAlgo_PaveFiller.cxx           | IntersectShapes求交阶段的计时                                |
| BOPAlgo_BOP.cxx                  | BuildResult构建阶段的计时                                    |
| BRepAlgoAPI_BooleanOperation.cxx | 布尔前后的BRepAlgoAPI_Check<br />BRepAlgoAPI_DumpOper输出布尔数据 |
| BOPAlgo_PaveFiller_6.cxx         | PerformFF记录中间求交信息                                    |
|                                  |                                                              |

