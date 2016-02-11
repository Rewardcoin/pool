from distutils.core import setup, Extension

sophiacoin_module = Extension('sophiacoin_subsidy', sources = ['sophiacoin_subsidy.cpp'])

setup (name = 'sophiacoin_subsidy',
       version = '1.0',
       description = 'Subsidy function for SophiaCoin',
       ext_modules = [sophiacoin_module])
