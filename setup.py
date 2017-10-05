from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
  name = 'Hello world app',
  ext_modules = cythonize([
    Extension("HistGen_py", ["src/HistGen_py.pyx"],
    library_dirs=["./test"],
    libraries=["histGen.o"])
  ])
)