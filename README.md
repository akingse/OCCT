Open CASCADE Technology
=======================

### 文件准备

fork from [OCCT](https://github.com/search?q=occ&type=repositories)


第三方库[打包](https://github.com/Open-Cascade-SAS/OCCT/releases/download/V7_8_0/3rdparty-vc14-64.zip)，(最新版本的官网已经打包好了，直接解压就行了)；

### 源码[编译](https://zhuanlan.zhihu.com/p/536502638)

cmake直接打开源码目录，设置build生成路径，点击configure会报错没有第三方库；

### cmake配置

- 添加3rdparty_dir（下面的库会自动填充，不需要手动选）
- 勾选Build_sample_MFC（不要勾选Qt，cmake可能编不过）
- 设置install_dir（防止INSTALL项目自动写入）
- （cmake config-generate-open）一键三连；

执行Install项目编译的时候，报错（MSB3073 命令 setlocal）

解决方法，以管理员身份运行 [VS](C:\Program Files (x86)\Common Files\Microsoft Shared\MSEnv\VSLauncher.exe)

### debug配置

occt.natvis 拷贝至[路径](C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Packages\Debugger\Visualizers\)

将项目Samples/Modeling设为启动项，首次启动需要拷贝freetype.dll，或添加环境变量

```
coppy to: repos\OCCT\build\win64\vc14\bind
PATH= repos\TPL\3rdparty-vc14-64\freetype-2.5.5-vc14-64\bin;

```

### DRAWEXE测试项目

```
pload ALL
axo
box b 10 10 10
fit
```

---





### 名词翻译

|            |                                        |                                                              |
| ---------- | -------------------------------------- | ------------------------------------------------------------ |
| TK         | toolkits                               | 工具集                                                       |
|            | deferred class                         | 滞后类                                                       |
|            | Triangulation                          | 三角剖分                                                     |
| TopoDS     | Topological Data Structure             | 拓扑数据结构                                                 |
|            | Standard_Transient                     | 短暂的                                                       |
|            | Curve Surface                          | 曲线曲面                                                     |
|            | Tessellate/simplification              | Mesh细分                                                     |
| BRep       | Boundary representation                |                                                              |
|            | Transient                              | 瞬态，暂时                                                   |
| gp         | geometric processor                    |                                                              |
|            | Confusion                              | 模糊，不明确                                                 |
|            | Orientation                            | 方向/朝向                                                    |
| AIS        |                                        |                                                              |
| OSD        |                                        | OCCT 为 OSD 包中的低级操作系统设施提供独立于平台的接口。该包类似“python”编程语言中的“os”模块。 |
| Collection | TCollection/NCollection   容器         | NCollection 包是一个较新的基于模板的包，提供现代容器。应该使用它尽可能代替 TCollection 类。<br/>OCCT 中索引从 1 开始，NCollection_Vector 除外（其中索引从 0 开始）。这样做是为了与基于 STL的算法兼容。 |
| MVC        | Model-View-Controller                  |                                                              |
| GC         | TKGeomBase/GC_Root（geometry common?） |                                                              |

Trihedron 三面体

tetraHedron 四面体

pentahedron 五面体



Transient，意味着数据是临时的或瞬态的，它们不会被持久化保存，意味着对象只存在于内存中，一旦程序终止或对象生命周期结束时，它们就会消失。
Persistent，意味着数据是持久化的，它们被存储在某种持久化存储介质，如数据库或者磁盘文件，并且在程序终止后仍然存在。程序重新启动时，该对象的数据仍然可以加载到内存中。
Standard_Transient类主要提供了三个机制，内存分配，RTTI和引用计数；



### Handle机制

handle是整个 OCCT 中使用的智能指针。一般来说，handle机制基于两种要素：

- 存储对内存中对象的引用次数的计数器
- 指向内存中对象的指针，每次创建对象的新handle时，该对象的引用计数器都会递增  



博客[教程](https://www.cnblogs.com/opencascade/p/OpenCASCADE_StartGuide.html)



OCC中曲面有两种形式：

- 1 代数方程f(x,y,z)=0表示
- 2 参数方程S(u,v)表示

Plane，Cylinder，Cone，Sphere：为代数方程表示

BezierSurface，BSplineSurface：为参数方程的表示

Param代表的参数方程曲面，Geom代表的代数方程曲面



---

### MarkDown整理

copy from [path](C:\Users\Aking\source\repos\OCCT\dox) ，markdown版本，内容与html文件一致 [html](C:\Users\Aking\source\repos\OCCT\doc\overview\index 7.8.0.html)；

```
build
contribution
debug
introduction
resources
samples
specification
tutorial
upgrade
user_guides
```



[OpenCascade User Guide](https://dev.opencascade.org/doc/overview/html/user_guides.html)



**Modeling Data（建模数据）**

#### **1. 引言**

引言部分简要介绍了Modeling Data模块的目的和功能，它提供了一系列复杂的数据结构，用于在计算机辅助设计（CAD）和计算机辅助制造（CAM）中表示和处理二维和三维几何模型。这些数据结构是后续进行几何分析、模型编辑和可视化处理的基础。

#### **2. Geometry Utilities（几何工具）**

几何工具部分详细介绍了用于创建和修改几何形状的服务和算法。这些工具包括形状的插值和逼近，以及将几何实体转换为BSpline形式。这些服务对于实现精确和自动化的几何设计至关重要。

#### **3. Interpolations and Approximations（插值和逼近）**

插值和逼近是几何建模中的重要概念，用于从给定的点集中创建平滑的曲线或曲面。本节讨论了插值和逼近的区别，以及如何使用不同的函数和算法在二维和三维空间中进行这些操作。

#### **4. Direct Construction（直接构造）**

直接构造方法允许用户通过简化的算法直接构建基本的几何形状，如直线、圆和曲线。这些方法不依赖于复杂的几何约束，使得用户可以快速地创建和修改几何实体。

#### **5. Points on Curves（曲线上的点）**

在参数化曲线上计算特定点的位置是许多几何分析和工程应用的基础。本节介绍了如何计算等间距点、等弦长点以及给定距离的点，这些计算对于理解和使用曲线至关重要。

#### **6. Extrema（极值）**

极值计算用于确定几何实体间的最短或最长距离，这在碰撞检测、间隙分析和优化设计中非常重要。本节介绍了如何计算点与曲线、曲线与曲面、以及两个曲面之间的极值距离。

#### **7. 2D Geometry（二维几何）**

二维几何部分详细介绍了Geom2d包，它包含了描述二维空间中点、向量、曲线的类，以及它们的几何变换和参数化表示。这些工具对于二维设计和分析非常有用。

#### **8. 3D Geometry（三维几何）**

三维几何包Geom提供了三维空间中点、向量、曲线和曲面的描述。这些几何实体可以通过参数化方程进行变换和分析，是三维建模和分析的基础。

#### **9. Properties of Shapes（形状属性）**

形状属性部分讨论了如何计算和分析形状的局部和全局属性，包括局部几何特性和全局拓扑特性。这些属性对于理解和分析复杂几何模型非常重要。

#### **10. Local Properties of Shapes（形状的局部属性）**

摘要：局部属性包BRepLProp提供了算法来计算BRep模型中边和面的局部属性，如点、导数、切向量、法向量、曲率和曲率中心。这些属性对于局部几何分析和设计至关重要。

#### **11. Continuity of Curves and Surfaces（曲线和曲面的连续性）**

连续性是评估曲线和曲面平滑度的重要标准。本节介绍了不同级别的连续性，包括C0到CN的不同级别，以及几何连续性G1和G2的概念。

#### **12. Regularity of Shared Edges（共享边的规律性）**

共享边的规律性描述了两个面共享一条边时在每一点上的最小连续性。这对于评估和构建高质量的几何模型非常重要。

#### **13. Global Properties of Shapes（形状的全局属性）**

全局属性部分提供了计算复合几何系统全局属性的算法，如质量、质心、惯性矩阵等。这些属性对于物理模拟和工程设计分析非常重要。

#### **14. Adaptors for Curves and Surfaces（曲线和曲面的适配器）**

适配器是一组接口，允许算法通过统一的API与不同类型的曲线或曲面一起工作。这使得算法具有更好的通用性和灵活性。

#### **15. Topology（拓扑）**

拓扑部分允许用户访问和操作对象的数据结构，而无需处理它们的二维或三维表示。拓扑描述了对象在参数空间中的位置和限制。

#### **16. Shape Location（形状位置）**

形状位置部分介绍了局部坐标系统的概念，以及如何使用TopLoc包来处理这些坐标系统，这对于理解和操作几何变换非常重要。

#### **17. Naming shapes, sub-shapes, their orientation and state（命名形状、子形状、它们的定向和状态）**

在复杂的几何模型中，为形状和子形状分配合适的名称、定向和状态是组织和管理模型的关键。本节讨论了这些概念及其在拓扑数据结构中的作用。

#### **18. Manipulating shapes and sub-shapes（操作形状和子形状）**

操作形状和子形状是几何建模中的基本任务。本节介绍了如何对拓扑数据结构中的形状和子形状进行组合、分解和转换。

#### **19. Exploration of Topological Data Structures（拓扑数据结构的探索）**

TopExp包提供了工具来探索TopoDS包描述的拓扑数据结构，包括查找给定类型的对象，这对于理解和分析复杂的几何模型结构非常有用。

#### **20. Lists and Maps of Shapes（形状的列表和映射）**

TopTools包提供了工具来操作TopoDS数据结构，包括形状的列表、映射和其他数据结构。这些工具对于管理和优化几何模型的存储和访问非常重要。

#### **21. Wire Explorer（线框浏览器）**

BRepTools_WireExplorer类提供了按连接顺序访问线框边的功能，这对于线框模型的分析和处理非常有用。

#### **22. Storage of shapes（形状的存储）**

BRepTools和BinTools包提供了方法来读取和写入形状到流或文件，支持ASCII和二进制格式。这些方法对于几何模型的持久化存储和交换非常重要。





------

**BRep Format（BRep格式）**

#### **1. Introduction（引言）**

引言部分简要介绍了BREP（边界表示）格式，这是一种用于存储3D模型的格式，它允许存储由顶点、边、线框、面、壳体、实体等组成的模型。BREP格式是Open CASCADE Technology（OCCT）的一部分。

#### **2. Format Common Structure（格式通用结构）**

BREP格式的通用结构部分介绍了格式的基础，包括ASCII编码的使用、BNF术语的定义以及格式数据的存储方式。这些构成了BREP文件的基本框架。

#### **3. Locations（位置）**

位置部分存储了用于描述几何变换的3x4矩阵，这些变换可以包括平移、旋转、缩放和对称等。这些位置信息对于理解模型在空间中的定位至关重要。

#### **4. Geometry（几何）**

几何部分包含了3D曲线、3D多边形、曲面和三角化等几何实体的定义。这些实体是构成3D模型的基础，并且可以通过参数化方程进行精确的表示和分析。

#### **5. Shapes（形状）**

形状部分描述了如何使用BREP格式来表示3D模型中的各种形状，包括顶点、边、线框、面、壳体、实体等。这些形状的定义是BREP格式的核心，它们共同构成了复杂的三维模型。

#### **6. Appendix（附录）**

附录提供了一个BREP文件的示例，展示了如何将上述各部分组合起来，形成一个完整的3D模型表示。这个示例有助于理解BREP格式的具体应用和实现。



------

**基础类库（Foundation Classes）**

**
**

Open CASCADE Technology（简称OCCT）是一个提供3D建模功能的软件技术，其用户指南详细介绍了如何使用其丰富的基础类库。以下是根据提供的文档内容顺序对OCCT的介绍：

1. **用户指南组织**：OCCT的用户指南根据其模块组织，涵盖了基础类、建模数据、布尔运算、形状修复、可视化、VTK集成服务、数据交换以及STEP和IGES转换器等。
2. **引言**：介绍了OCCT基础类的目的，它们为开发人员提供了自动化动态内存管理、集合处理、异常处理、泛型下转型和插件创建等服务。
3. **根类（Root Classes）**：这些是所有其他类构建的基础，提供了基本数据类型和类，确保了动态创建对象的安全处理。
4. **字符串（Strings）**：处理基于ASCII和Unicode的动态大小字符序列。
5. **集合（Collections）**：提供了动态大小的数据聚合类，这些类是泛型的，可以容纳各种对象。
6. **标准对象的集合（Collections of Standard Objects）**：提供了常用泛型集合类的对象实例化。
7. **向量和矩阵（Vectors and Matrices）**：提供了常用的数学算法和基本计算。
8. **基础几何类型（Primitive Geometric Types）**：提供了STEP兼容的基本几何和代数实体的实现。
9. **公共数学算法（Common Math Algorithms）**：提供了最常用的数学算法的C++实现。
10. **异常（Exceptions）**：提供了一个异常类层次结构，用于描述在函数执行期间可能出现的异常情况。
11. **量（Quantities）**：支持日期和时间信息以及表示大多数物理量的基本类型。
12. **应用服务（Application services）**：包括单位转换工具、表达式解释器、资源文件处理工具等。
13. **基础知识（Basics）**：涉及库组织、持久性、数据类型、内存管理、使用句柄编程、异常处理、通过下转型实现泛型和插件创建等基本服务。
14. **库组织（Library organization）**：介绍了OCCT库的基本组织概念，包括模块和工具包、包、类、继承和数据类型。
15. **编程与句柄（Programming with Handles）**：句柄是OCCT实现智能指针的方式，用于操作动态内存中的对象。
16. **类型管理（Type Management）**：OCCT提供了一种以通用方式描述数据类型层次结构的方法，并能在运行时检查给定对象的确切类型。
17. **使用句柄创建对象（Using Handles to Create Objects）**：通过声明句柄并使用C++的new操作符初始化来创建被句柄操作的对象。
18. **调用方法（Invoking Methods）**：一旦拥有对象的句柄，就可以像使用C++中的指针一样使用它。
19. **调用类方法（Invoking Class Methods）**：类方法像静态C++函数一样被调用。
20. **句柄分配（Handle deallocation）**：OCCT通过引用计数来管理对象的生命周期，当引用计数为0时，对象会被自动删除。
21. **循环（Cycles）**：当两个或更多对象通过句柄相互引用时，会出现循环，这会阻止自动销毁。
22. **内存管理（Memory Management）**：OCCT使用专门的内存管理器来优化动态内存中大量C++对象的创建和删除。
23. **异常（Exceptions）**：OCCT提供了一个异常类层次结构，以及将系统信号转换为异常的支持。
24. **插件管理（Plug-In Management）**：介绍了插件的分布和管理，插件可以动态加载到客户端应用程序中。
25. **集合、字符串、数量和单位转换（Collections, Strings, Quantities and Unit Conversion）**：介绍了OCCT中的集合类、字符串处理、量的定义和单位转换工具。
26. **数学基础和算法（Math Primitives and Algorithms）**：介绍了OCCT中提供的数学基础类型和算法。
27. **精度（Precision）**：讨论了在OCCT平台上处理对象时精度的重要性，并提供了精度包来解决几何算法开发者在比较两个数字时的精度设置问题。

**Modeling Algorithms**



**几何工具 (Geometric Tools)**

几何工具部分详细介绍了OpenCASCADE Technology提供的算法，这些算法用于计算2D和3D几何对象的交点。具体来说，包括两个2D曲线、一个3D曲线与一个曲面、以及两个曲面之间的交点计算。此外，还包括点到曲线和曲面的投影算法，以及根据约束条件构造线（Lines）和圆（Circles）的能力。这些工具对于CAD软件中的3D建模至关重要，因为它们允许精确地分析和构建复杂的几何形状。



**交点计算 (Intersections)**

交点计算部分深入探讨了如何使用`Geom2dAPI_InterCurveCurve`类来计算两个几何曲线之间的交点，以及如何评估一个曲线的自交点。这部分内容强调了公差（Tolerance）的概念，即在计算交点时用来区分两个点是否为同一个点的容差值。公差值对于确定算法返回的是交点还是切线交点非常关键。文中还提供了关于如何选择和使用公差值的指导，以及如何通过索引来访问特定的交点或交线段。



**插值方法 (Interpolations)**

插值方法部分描述了如何使用插值函数来定义形状的演化规律，例如圆角（Fillet）的发展。文中提到了`Geom2dAPI_Interpolate`类，它用于通过一组2D点插值构造BSpline曲线。这部分内容讨论了不同类型的插值方法，包括线性插值、S函数插值和一般插值函数。插值方法的选择取决于所需的曲线特性，如是否需要切线连续性或周期性。



**约束条件下的线和圆 (Lines and Circles from Constraints)**

这部分内容详细介绍了如何使用几何约束来构造2D空间中的线和圆。算法可以根据其他曲线的数值或几何约束来构造具有特定属性的线和圆，例如半径、角度、切线关系、通过特定点或以特定点为中心。文中还讨论了如何使用定性约束来限制解的数量，例如通过指定解相对于其他几何对象的位置，从而减少可能的解决方案数量。



**约束条件下的曲线和曲面 (Curves and Surfaces from Constraints)**

这部分内容介绍了如何从约束条件构建曲线和曲面，包括公平曲线（Faired Curves）和最小变化2D曲线（Minimal Variation 2D Curves）。公平曲线是通过在给定的约束条件下生成平滑曲线，而最小变化曲线则是在满足约束的同时，力求曲率变化最小。这些曲线的构建对于创建具有特定美学或工程要求的复杂形状非常有用。





------



**Boolean Operations**



**输入与结果参数 (Input and Result Arguments)**

这部分内容讨论了布尔运算的输入参数和结果，包括形状的修改和布尔运算的实现细节。布尔运算是计算机图形学中常用的操作，可以创建新的形状或修改现有形状，如合并（Fuse）、相交（Common）、相切（Cut）等。文中还介绍了如何使用这些运算来构建复杂的几何形状。



**实现细节 (Implementation)**

实现细节部分深入讨论了布尔运算的具体实现方法，包括形状的填充、投影、拓扑工具的使用等。这些工具和算法是布尔运算能够正确执行的基础。文中还提到了如何使用这些工具来处理2D和3D几何对象的交点计算。



**倒角和圆角 (Fillets and Chamfers)**

倒角和圆角部分介绍了如何在形状上应用倒角和圆角。这包括在形状边缘创建平滑的过渡，以减少尖锐角落或边缘。文中还提到了`BRepFilletAPI_MakeFillet`类，它允许对形状进行倒角和圆角处理。这部分内容对于理解如何在CAD软件中实现这些常见的工程特征非常重要。



**偏移、草图、管道和演化形状 (Offsets, Drafts, Pipes and Evolved shapes)**

这部分描述了如何计算偏移，构建草图，以及如何通过扫描、旋转和管道方法构造基本形状。这些操作可以用于创建复杂几何形状，如通过旋转一个形状来创建旋转体。文中还讨论了如何使用这些方法来构建和修改3D模型。



**缝合算法 (Sewing Algorithm)**

缝合算法部分讨论了如何将一系列独立的拓扑元素（如面和边）缝合成连接的拓扑（如壳和线框）。文中提到了公差管理、流形和非流形缝合、局部缝合以及特征。缝合算法在3D建模中非常重要，尤其是在处理由多个独立面组成的复杂形状时。



**网格化 (Meshing)**

网格化部分介绍了网格表示法，包括网格呈现、网格算法和OpenCASCADE Technology提供的网格功能。这些工具可以用于3D可视化、有限元分析和快速原型制造。文中还提到了如何使用这些工具来处理3D模型的网格化，以及如何将这些网格用于进一步的分析和制造。
