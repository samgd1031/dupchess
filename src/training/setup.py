from setuptools import setup
from Cython.Build import cythonize
from setuptools.extension import Extension
from Cython.Distutils import build_ext

e_mods = [
                Extension("prepare_batch",
                ["test.pyx", "prepare_batch.cpp"],
                language="c++")
]

setup(
	name="data loader",
    cmdclass = {"build_ext": build_ext},
	ext_modules=e_mods,
	zip_safe=False
	)