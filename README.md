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

建议卸载CMakePredefinedTargets下的两个项目 `ALL_BUILD` `INSTALL`



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

### MarkDown整理

官方文档有4个版本

[html](C:\Users\Aking\source\repos\OCCT\doc\overview\index 7.8.0.html)版本，[chm](C:\Users\Aking\source\repos\OCCT\doc\occt_overview.chm)版本，html版有最新780，chm最新是760的occt_overview，对应官网[OpenCascade User Guide](https://dev.opencascade.org/doc/overview/html/user_guides.html)；

[markdown](C:\Users\Aking\source\repos\OCCT\dox)版本，内容与html文件一致，文件夹目录稍有变化，目前图片无法正确加载；

[pdf](C:\Users\Aking\source\repos\OCCT\doc\pdf\dev_guides)版本，分为dev_guides和user_guides，user_guides对应User Guides用户指南中，按sln项目模块组织的分类，dev_guides对应overview目录下其他内容，包括specification；



![image-20241103134553416](C:/Users/Aking/AppData/Roaming/Typora/typora-user-images/image-20241103134553416.png)



OCC-770配置

```
..\..\..\..\..\OCC7.7.0\OCCPublicAPI\Include
..\..\..\..\..\OCC7.7.0\Release
```

OCC-780配置

```
C:\Users\Aking\source\repos\OCCT\build\inc
C:\Users\Aking\source\repos\OCCT\build\win64\vc14\lib
```


