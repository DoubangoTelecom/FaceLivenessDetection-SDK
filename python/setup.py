#!/usr/bin/env python

"""
pyton_setup.py file for SWIG FaceLivenessDetectionSDK
You must run this file from 'binaries/os/arch' (e.g. 'binaries/windows/x86_64') folder.
"""

from distutils.core import setup, Extension
from distutils import sysconfig
from Cython.Distutils import build_ext
from sys import platform
import os

# Shared library name
print("Your platform: %s" % platform)
LIBNAME = 'FaceLivenessDetectionSDK'

# Do not add suffix (e.g. 'cp36-win_amd64')
class NoSuffixBuilder(build_ext):
    def get_ext_filename(self, ext_name):
        filename = super().get_ext_filename(ext_name)
        suffix = sysconfig.get_config_var('EXT_SUFFIX')
        ext = os.path.splitext(filename)[1]
        return filename.replace(suffix, "") + ext

FaceLivenessDetectionSDK_module = Extension('_FaceLivenessDetectionSDK',
                           sources=[os.path.abspath('../../../python/FLD-SDK-API-PUBLIC-SWIG_python.cxx')],
                           include_dirs=['../../../cpp'],
                           language='c++11',
                           library_dirs=['.'],
                           libraries=[LIBNAME]
                           )

setup (name = 'FaceLivenessDetectionSDK',
       version = '0.4.0',
       author      = "Doubango AI",
       description = """FaceLivenessDetectionSDK for python""",
       ext_modules = [FaceLivenessDetectionSDK_module],
       py_modules = ["FaceLivenessDetectionSDK"],
       cmdclass={"build_ext": NoSuffixBuilder},
       )