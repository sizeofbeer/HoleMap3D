HOLEMAP3D
============================================================================================
### 项目简介
#### 使用AliceVision Pipeline构建纯视觉的三维重建系统
#### GUI部分使用的是ImGui, 稀疏点云以及Mesh模型的展示使用的是abcg和tiny_obj_loader

### 使用说明
#### 目前直接使用AliceVision编译好的可执行文件, 通过CI进行调用
#### 1、下载AliceVision可执行文件, 放入到项目根路径
#### 2、设置LD_LIBRARY_PATH, 使得项目能找到AliceVision Lib文件
#### 3、编译, mkdir build && cd build && cmake .. && make
#### 4、运行, cd bin/holemap && ./holemap

### 项目后续
#### 1、支持colmap、openmvg、openmvs等库
#### 2、优化SFM重建, 包括特征提取、特征匹配和增量SFM迭代优化部分
#### 3、学习openmvs库, 主要集中在patchmatch和tsgm算法

### 相关项目
https://github.com/ocornut/imgui.git
https://github.com/alicevision/AliceVision.git
https://github.com/hbatagelo/abcg.git
https://github.com/colmap/colmap.git
