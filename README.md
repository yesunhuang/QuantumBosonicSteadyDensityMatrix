# QuantumBosonicSteadyDensityMatrix

Solving steady density matrix problem for bosonic quantum system

## 开发前准备

见同目录下的 `开发准备.md` 

## bostix 包安装说明

首先安装依赖:
```bash
pip install -r requirements.txt
```

Windows:
```bash
# 首先初始化 msvc 环境，在 CMD 中运行 vcvarsall.bat 脚本
# vcvarsall.bat 脚本位于 msvc 文件夹中，例如：
# "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
# 语法为:
#   [SOME_DIR]/vcvarsall.bat [arch]
# 其中 [arch] 可选，例如 x86 或者 amd64
[SOME_DIR]/vcvarsall.bat amd64

# 之后进行安装
python setup.py [build | install] --build Release -j 4
```

linux/mac:
```bash
# 直接运行即可
python setup.py [build | install] --build Release -j 4
```