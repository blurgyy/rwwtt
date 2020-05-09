# Final Project: 用两个三角形渲染世界

## 最终效果

![demo](images/demo.png)

## 说明

- 使用 Vulkan 直接传递两个覆盖了整个屏幕的三角形给 Vertex shader. 渲染过程全部使用 GLSL 在 Fragment shader 中实现.
- 在 Fragment shader 中构造了距离场, 然后使用 Ray Marching 方法渲染场景.
- ray marching 中使用了 adaptive eps, 令固定的 epsilon 乘以当前点与起点之间的距离, 代表 ray marching 走得越远, 判断与物体相交的条件越宽松.
- 距离场中某一位置 p 的法线方向可以通过计算距离场的梯度方向来估计, 距离场的梯度方向是该点处到场景中物体的距离变化最快的方向, 也就是法线方向.
- 根据 ray marching 的特性估计了软阴影, 具体: 从着色点向光源做 ray marching, 在每一步估计一个阴影, 某一步距离附近的物体越近则阴影越黑; 同时在某一步离着色点越远, 该步估计的阴影颜色应越浅, 两个因素结合起来再乘以一个系数, 取 ray marching 过程中的估计的最黑的阴影作为着色点的阴影颜色. 如果途中打到物体, 则直接返回 `vec3(0)`.
- 根据 ray marching 的特性估计了环境光遮蔽
- 定义了材质
- 最后添加了 super sampling anti aliasing.

## 参考

- 调用 Vulkan API 的 C++ 代码参考自 [`https://vulkan-tutorial.com/`](https://vulkan-tutorial.com/)
- Fragment shader 中:
  - 平滑融合两个形状的 `smin` (smooth min) 函数参考自 [`https://iquilezles.org/www/articles/smin/smin.htm`](https://iquilezles.org/www/articles/smin/smin.htm), 使用了其中提到的多项式型 blend 函数.
  - 自己实现的圆台距离场函数因为没有考虑点在内部时的距离, 对圆台求法线时会在侧面出现大量噪点. 修正后的圆台的距离场函数参考自 [`https://iquilezles.org/www/articles/distfunctions/distfunctions.htm`](https://iquilezles.org/www/articles/distfunctions/distfunctions.htm).
  - 软阴影的估计方式参考自 [`https://iquilezles.org/www/articles/rmshadows/rmshadows.htm`](https://iquilezles.org/www/articles/rmshadows/rmshadows.htm).
  - 环境光遮蔽的估计方式参考自:
    - [`https://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf`](https://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf) 第 53 页
    - [`https://www.shadertoy.com/view/lsKcDD`](https://www.shadertoy.com/view/lsKcDD)
- ray marching 中的 adaptive eps, 使用距离场通过变换生成绕轴旋转的若干个物体等技巧参考自:
  - NVScene 2008: Rendering Worlds with Two Triangles: Raytracing on the GPU
