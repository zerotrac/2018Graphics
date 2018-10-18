<center><h1>《多边形的几何操作》实验报告</h1></center>
<div style="text-align:right;">软件52 陈书新 2015013229</div>

### 0. 实验目的

- 实现任意多边形（凹、凸、带内环、不自交）的输入及显示，可自定义多边形（边界、内部）的颜色；
- 实现两个任意多边形的裁剪；
- 实现任意多边形的平移、旋转、缩放、翻转；
- 支持多步以上操作；
- 只能使用画点和画线的 API；

### 1. 使用算法

- 多边形填充：使用扫描线算法。
- 多边形裁剪：使用 Weiler-Atherton 算法。
- 多边形平移、旋转、缩放、翻转：使用 3*3 旋转矩阵。

### 2. 实验结果

- 完成所有基础功能。
- 附加功能包括多边形合法性检测、精度保证和友好的交互方式，具体在第 3/4 节中阐述。

### 3. 问题分析

- Q：如何使 Weiler-Atherton 算法能处理任意多边形的裁剪？
- A：求两个多边形的裁剪本质上是求它们的交，因此当它们的外环不交时，裁剪结果为空。同样可以考虑外环-内环/内环-内环不交时的情况，借此去掉所有不会影响裁剪结果的外环和内环。此时对于任意一个内环，它要么与某个外环/内环有交（W-A 算法可以处理），要么在某个外环内部且不和其它内环有交（保存保存这个内环，在裁剪结束后再加入合适的外环中）。
<br/><br/>
- Q：如何在填色时保持帧率？
- A：使用扫描线算法会得到很多条平行线，用画线的 API 填色即可。
<br/><br/>
- Q：如何保证多边形点的精度？例如在旋转/缩放时实时改变数据结构中存储的点位置会导致误差。
- A：在数据结构中只存储点的原始位置，在进行仿射变换后，存储对应的 3*3 旋转矩阵，在绘制时用矩阵进行变换得到实时位置。
<br/><br/>
- Q：需要做哪些合法性检测？
- A：外环/内环不自交、内环与内环不相交、内环与外环不相交。

### 4. 交互方式

#### 4.1 界面

左侧为图层选择器，右侧为画板，下方为操作按钮。

#### 4.2 添加多边形（外环）

使用 "+" 按钮添加多边形，使用左键在画板上依次选择点，使用右键完成操作（会自动添加最后一条边），使用 Esc 键取消操作。注意多边形的点必须逆时针给定，且不能自交，在操作进行中不合法的点不会添加，在操作完成后不合法的多边形不会添加到图层中。

#### 4.3 选择多边形

在左侧图层选择器中单击某个图层来选择对应的多边形，选中的多边形的图层边框会变蓝，且在画板中会置顶+加粗+虚线边框显示。再次单击选中的图层/单击另一个未选中的图层/使用 Esc 键均可取消当前选中的图层。

#### 4.4 添加内环

在选中某个多边形时才能添加内环。使用 "in+" 按钮添加内环，内环的点必须逆时针给定，且不能自交，在操作进行中不合法的点不会添加，在操作完成后不合法的多边形不会添加到图层中。

#### 4.5 删除多边形

在选中某个多边形时，使用 "-" 按钮删除多边形。

#### 4.6 换颜色

左侧多边形的图层中有两个颜色块，上方表示线颜色，下方表示填充颜色，点击即可更换颜色。

#### 4.7 仿射变换

在选中某个多边形时才能进行仿射变换。

#### 4.7.1 平移

当鼠标在多边形的虚线边框中时按下左键，多边形会随着鼠标的移动而平移。

#### 4.7.2 旋转

当鼠标在多边形虚线边框外时按下左键，多边形会随着鼠标的移动而旋转，旋转中心为多边形的中心。

#### 4.7.3 缩放

使用鼠标滚轮进行缩放。缩放中心为多边形的中心。

#### 4.7.4 翻转

使用 "flipV" 和 "flipH" 按钮进行垂直翻转/水平翻转，翻转轴过多边形的中心。

#### 4.8 裁剪

在选中某个多边形时才能进行裁剪。在 "clip" 按钮的右侧输入另一个多边形的编号，再使用 "clip" 按钮即可进行裁剪。裁剪得到的多边形（可能有多个）将会出现在新的图层中，每个图层中有一个多边形。

### 5. 编译环境

### 6. 吐槽

corner case 太多了...