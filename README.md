Open CASCADE Technology
=======================

fork from [OCCT](https://github.com/search?q=occ&type=repositories)

- all size=413M

- src size=94M


第三方库[打包](https://github.com/Open-Cascade-SAS/OCCT/releases/download/V7_8_0/3rdparty-vc14-64.zip)，(最新版本的官网已经打包好了，直接解压就行了)；

### 源码[编译](https://zhuanlan.zhihu.com/p/536502638)

cmake直接打开源码目录，设置build生成路径，点击configure会报错没有第三方库；

### cmake配置

- 添加3rdparty_dir（下面的库会自动填充，不需要手动选）
- 勾选Build_sample_MFC（不要勾选Qt，cmake编不过）
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

项目添加测试文件
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



类型定义

```cpp
typedef int           Standard_Integer;
typedef unsigned int  Standard_UInteger;
typedef double        Standard_Real;
typedef bool          Standard_Boolean;
typedef float         Standard_ShortReal;
typedef char          Standard_Character;
typedef unsigned char Standard_Byte;
typedef void*         Standard_Address;
typedef size_t        Standard_Size;
typedef std::time_t   Standard_Time;
```



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

