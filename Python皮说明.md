# Python 模块包皮说明

## 构建过程

### 基础知识

> 引用 [Python应用的打包和发布](http://wsfdl.com/python/2015/09/06/Python%E5%BA%94%E7%94%A8%E7%9A%84%E6%89%93%E5%8C%85%E5%92%8C%E5%8F%91%E5%B8%83%E4%B8%8A.html)

python 生成安装包，主要有两种形式:

* Source Distributions(sdist): 即源码包，具有**平台无关性**
* Wheel: 采用预编译格式，相比之下具有安装速度快的特点，但也因此为**平台相关**

### Source Distribution

采用如下命令即可编译成 source distribution:

```
python setup.py sdist
```

### Wheel

根据 application 包含的代码类型以及其所支持的 python 版本， wheel 格式可细分为三种

* Universal wheel: 纯 python 代码，并且支持 python 2 和 3
* Pure python wheel: 纯 python 代码，不同时支持 python2 和 3
* Platform wheel: 非纯 python 代码

采用如下命令可编译成 wheel:

```
python setup.py bdist
```

## 使用方法

### 本机安装

使用命令将 bostix 安装到本机中的 site-packages 中:

```
python setup.py install
```

### import 方式

正常的引用即可

```python
import bostix
```

若需要使用由 C++ 编译出来的 bostix.core 模块，可这样引用:

```python
from bostix import core
```

### core 中方法

目前 core 中的方法，几乎是对 C++ 中原生函数的直接封装:

* **EpSolver**: EpSolver 的构造函数，接受 3 个参数: 前 2 个为 [RawTerm](#raw_term) 的 List，第 3 个为整数的 List。返回值为 EpSolver* 的封装对象。
* **DPSolver**: DPSolver 的构造函数，接受 5 个参数: 
* **DPSolverRun**: DPSolver 的 run 方法，接受 1 个参数，即 DPSolver* 的封装对象。
* **DPSolverGetResult**: DPSolver 的 getResult 方法，接受 1 个参数，即 DPSolver* 的封装对象。返回值为 MatrixMapper* 的封装对象。
* **MatrixMapperRowRho**: MatrixMapper 的 rowRho 方法，接受 1 个参数，即 MatrixMapper* 的封装对象。返回值为 TensorMatrix* 的封装对象。
* **MatrixMapperAvgMoment**: MatrixMapper 的 avgMoment 方法，接受 1 个参数，即 MatrixMapper* 的封装对象。返回值为 python 的原生 complex 类型。
* **TensorMatrixTo2DList**: TensorMatrix 的转为 2-d python list 的方法，接受 1 个参数，即 TensorMatrix* 的封装对象。返回值为 2-d python list。

调用方式实例:

```python
from bostix import core

epsolver = core.EpSolver(...)
dpsolver = core.DPSolver(...)

core.DPSolverRun(dpsolver)

result = core.DPSolverGetResult(dpsolver)
mat = core.MatrixMapperRowRho(result)

# print 2d tensor matrix
print(core.TensorMatrixTo2DList(mat))
```

## 附录

<span id="raw_term"></span>

### RawTerm 结构

TODO
