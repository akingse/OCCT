Open CASCADE Technology
=======================

fork from [OCCT](https://github.com/search?q=occ&type=repositories)

all size=413M

src size=94M



### 源码[编译](https://zhuanlan.zhihu.com/p/536502638)

直接打开源码目录，设置build生成路径，点击configure会报错没有第三方库，设置从occ官网下载的第三方库路径，（cmake config-generate-open）一键三连；(注意，其中下载的第三方包tcltk-86-64一定得是64位；最新版本的官网已经打包好了，直接解压就行了)

### cmake配置

- 添加3rdparty_dir
- 勾选Build_sample_MFC
- 设置install_dir

执行Install项目编译的时候，报错（MSB3073 命令 "setlocal）

解决方法，以管理员身份运行 [VS](C:\Program Files (x86)\Common Files\Microsoft Shared\MSEnv\VSLauncher.exe)

