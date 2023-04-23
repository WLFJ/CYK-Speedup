# CYK算法加速报告

SY2206220 王彦伟
yveswong@buaa.edu.cn
北京航空航天大学软件所

## 概述

根据任务书当中算法的描述，我们不难发现该算法属于经典的集合操作密集型算法——该算法涉及到频繁的集合并、集合遍历运算。

因此，本文首先会对计算特性进行分析，之后从以下角度考虑加速的可能性：

* 并行集合遍历
* 减少频繁的内存搬运
* 探索是否可减少重复运算开销

最后，报告会对比优化前后的程序性能以及加速算法本身的阔放性能数据，并给出一些指标分析，思考该算法未来可考虑的优化方向。

*声明：根据学术诚信原则本报告没有直接参考任何与CYK算法直接相关的现有优化方案。*

## 计算特性分析

TODO

## 内存搬运优化

TODO

## 重复计算的发现与优化

TODO

## 优化性能分析

TODO

## 总结

TODO