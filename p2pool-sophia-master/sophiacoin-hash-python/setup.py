from distutils.core import setup, Extension

sophiacoin_hash_module = Extension('sophiacoin_hash', sources = ['sophiacoinmodule.c', 'sophia.c'])

setup (name = 'sophiacoin_hash',
       version = '1.0',
       ext_modules = [sophiacoin_hash_module])
