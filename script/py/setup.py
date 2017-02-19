#!/usr/bin/python
#coding=UTF-8

#from setuptools import setup, find_packages
from distutils.core import setup

setup(
        name='igs_data_getter',
        version='0.0.2',
        description='download igs data',
        author='Gao kang. Wuhan University',
        author_email='whurinex@163.com',
        url='https://github.com/binex/igs_data_getter.git',
        license='LGPL',
        scripts=['get_data.py', 'timeconvert.py', 'wget.py', 'url_list.py'],
        )

