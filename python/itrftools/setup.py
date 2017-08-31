from setuptools import setup

setup(name='itrftools',
      version='0.1',
      description='Modules for ITRF manip',
      url='https://github.com/DSOlab/itrf2014',
      author='Dionysos Satellite Observatory',
      author_email='xanthos@mail.ntua.gr',
      license='FY',
      packages=['itrftools'],
      scripts=['bin/itrftool'],
      install_requires=['numpy'])
