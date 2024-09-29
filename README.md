Open CASCADE Technology
=======================

fork from [OCCT](https://github.com/search?q=occ&type=repositories)

all size=413M

src size=94M



### 源码[编译](https://zhuanlan.zhihu.com/p/536502638)

直接打开源码目录，设置build生成路径，点击configure会报错没有第三方库，设置从occ官网下载的第三方库路径，（cmake config-generate-open）一键三连；(注意，其中下载的第三方包tcltk-86-64一定得是64位；最新版本的官网已经打包好了，直接解压就行了)

### cmake配置

- 添加3rdparty_dir（下面的库会自动填充，不需要手动选）
- 勾选Build_sample_MFC（不要勾选Qt，cmake编不过）
- 设置install_dir

执行Install项目编译的时候，报错（MSB3073 命令 "setlocal）

解决方法，以管理员身份运行 [VS](C:\Program Files (x86)\Common Files\Microsoft Shared\MSEnv\VSLauncher.exe)



### debug配置

occt.natvis 拷贝[至](C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Packages\Debugger\Visualizers\)

---



 

### 七个模块 

现在8个了，增加了DETools

Module FoundationClasses //所有基类

Module ModelingData 2D3D//几何基本体数据结构

Module ModelingAlgorithms //几何拓扑算法库

Module Visualization //图形表达机制

Module ApplicationFramework //应用框架，常用函数机制

Module DataExchange //数据交换

Module Draw //Test测试工具



### 名词翻译

|        |                            |              |
| ------ | -------------------------- | ------------ |
| TK     | toolkits                   |              |
|        | deferred class             | 滞后类       |
|        | Triangulation              | 三角剖分     |
| TopoDS | Topological Data Structure | 拓扑数据结构 |
|        | Standard_Transient         | 短暂的       |
|        | Curve Surface              | 曲线曲面     |
|        | Tessellate/simplification  | Mesh细分     |
| BRep   |                            |              |
|        |                            |              |
|        |                            |              |
|        |                            |              |
|        |                            |              |
|        |                            |              |
|        |                            |              |







