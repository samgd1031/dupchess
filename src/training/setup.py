from setuptools import setup
from setuptools.extension import Extension
from Cython.Distutils import build_ext

ext_mods = [
    Extension("speedloader",
            ['speedloader.pyx', "load_batch_funcs.cpp"],
            language="c++",
            extra_compile_args=['-O2', "/openmp", "/std:c++17"],
            extra_link_args=["-std=c++17"])
]

setup(
    name="speedloader",
    cmdclass={"build_ext": build_ext},
    ext_modules=ext_mods,
    zip_safe=False
)