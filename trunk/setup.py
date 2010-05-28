#!/usr/bin/env python


## use 'gcc_omp' to use openmp
HEALPIX_TARGET='generic_gcc'

from distutils.core import setup, Extension
from os.path import join,isdir
import sys

from numpy import get_include
numpy_inc = get_include()

def compile_healpix_cxx(target):
    import os
    print "Compiling healpix_cxx (this may take a while)"
    compil_result = os.system('cd hpbeta && '
                              'HEALPIX_TARGET=%s make '%(target))
    if compil_result != 0:
        raise Exception('Error while compiling healpix_cxx')

def get_version():
    try:
        for line in file('healpy/version.py'):
            exec line
    except Exception:
        raise ValueError('Error getting revision number from '
                         'healpy/version.py')
    return __version__


healpy_pixel_lib_src = ['_healpy_pixel_lib.cc']
healpy_spht_src = ['_healpy_sph_transform_lib.cc']
healpy_fitsio_src = ['_healpy_fitsio_lib.cc']


################################################
#
#    Healpix data (pixel window and ring files
healpix_cxx_dir='hpbeta/%s'%HEALPIX_TARGET
healpix_cxx_inc = healpix_cxx_dir+'/include'
healpix_cxx_lib = healpix_cxx_dir+'/lib'

if sys.argv[1] != 'sdist':
    compile_healpix_cxx(HEALPIX_TARGET)
    
    if not ( isdir(healpix_cxx_dir+'/include') and
             isdir(healpix_cxx_dir+'/lib') ):
        raise IOError("No inlcude and lib directory : needed for healpy !")

###############################################

healpix_libs=['healpix_cxx','fftpack','cxxsupport','cfitsio']
healpix_args=[]
if HEALPIX_TARGET=='gcc_omp':
    healpix_libs.append('gomp')
    healpix_args.append('-fopenmp')

#start with base extension
pixel_lib = Extension('healpy._healpy_pixel_lib',
                      sources=[join('healpy','src',s)
                               for s in healpy_pixel_lib_src],
                      include_dirs=[numpy_inc,healpix_cxx_inc],
                      library_dirs=[healpix_cxx_lib],
                      libraries=healpix_libs,
                      extra_compile_args=healpix_args
                      )

spht_lib = Extension('healpy._healpy_sph_transform_lib',
                     sources=[join('healpy','src',s) for s in healpy_spht_src],
                     include_dirs=[numpy_inc,healpix_cxx_inc],
                     library_dirs=[healpix_cxx_lib],
                     libraries=healpix_libs,
                     extra_compile_args=healpix_args
                     )

hfits_lib = Extension('healpy._healpy_fitsio_lib',
                      sources=[join('healpy','src',s)
                               for s in healpy_fitsio_src],
                      include_dirs=[numpy_inc,healpix_cxx_inc],
                      library_dirs=[healpix_cxx_lib],
                      libraries=healpix_libs,
                      extra_compile_args=healpix_args
                      )

# 
setup(name='healpy',
      version=get_version(),
      description='Healpix tools package for Python',
      author='C. Rosset',
      author_email='rosset@lal.in2p3.fr',
      url='http://code.google.com/p/healpy',
      packages=['healpy'],
      py_modules=['healpy.pixelfunc','healpy.sphtfunc',
                  'healpy.visufunc','healpy.fitsfunc',
                  'healpy.projector','healpy.rotator',
                  'healpy.projaxes','healpy.version'],
      ext_modules=[pixel_lib,spht_lib,hfits_lib],
      package_data={'healpy': ['data/*.fits']},
      license='GPLv2'
      )

