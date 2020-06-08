# ITRF 2014

[ITRF2014](http://itrf.ign.fr/ITRF_solutions/2014/) is the new realization of the International Terrestrial Reference System.
ITRF2014 (as well as previous releases) are published by the [International Earth Rotation and Reference Systems Service](http://www.iers.org/)
ITRF2014 is the new realization of the International Terrestrial Reference System.

In contrast to older realizations of the frame, ITRF 2014 introduces Post-Seismic Deformation ([PSD](http://itrf.ign.fr/ITRF_solutions/2014/psd.php)) models, thus a user who wants to compute the position of a station affected by 
post-seismic deformations at an epoch during the relaxation (non-linear) period should take these into account. More on the post-seismic deformation (PSD) models can be
found on the ITRF [webpage](http://itrf.ign.fr/ITRF_solutions/2014/psd.php) and relevant [literature](http://itrf.ign.fr/ITRF_solutions/2014/doc/ITRF2014-PSD-model-eqs-IGN.pdf).


# itrftool

itrftool is a program to compute station coordinates in the ITRF2014 Reference Frame for any given epoch. It performs two 
basic tasks:

  * read in a [ssc](http://itrf.ign.fr/ITRF_solutions/2014/more_ITRF2014.php) format file and extrapolate 
  coordinates to any given time (*ssc files are published by ITRF and hold records of station coordinates at
  a reference epoch*)

  * read in a [psd](http://itrf.ign.fr/ITRF_solutions/2014/ITRF2014_files.php) format file and compute (total) PSD 
  for any station at any given epoch (*ssc files are published by ITRF; the program uses the ones in in CATREF internal 
  format*)


# examples

Compute PSD values for a given date and a list of stations; note that the stations 
can be specified either by name (aka their 4-char id) or by DOMES number (or both).
Here we compute the PSD values for stations with id's NRMD, COCO and TONG and stations with 
DOME's 97401M003, 50902M001 and 49971M001 for day of year 150 of year 2020 (that is
2020-05-29). Note that the DOMES 50902M001 and the id TONG correspond to the same station, 
hence only one record is written for it. Information on computed the PSD values are
extracted from the file `../data/ITRF2014-psd-gnss.dat`. Stations with PSD values of 0e0 
do not have corresponding records in the input PSD information file.

```
$> itrftool -s NRMD COCO TONG -m 97401M003 50902M001 49971M001 -p ../data/ITRF2014-psd-gnss.dat -y 2020 -d 150 --psd-only
NAME   DOMES   East(mm) North(mm) Up(mm)        EPOCH
---- --------- -------- -------- -------- ------------------
     97401M003     0.00     0.00     0.00 2020-05-29 00:00:00
COCO 50127M001    14.51    24.50     0.00 2020-05-29 00:00:00
NRMD               0.00     0.00     0.00 2020-05-29 00:00:00
TONG 50902M001    43.78   -13.65     0.00 2020-05-29 00:00:00
```

Extrapolate coordinates in ITRF2014 at epoch 2020-05-29 for the given stations (some 
specified by id others by domes). Note that the DOMES 50902M001 and the id TONG correspond to the same station,
hence only one record is written for it. Also, there is no record for a station with 
domes number 49971M001, hence no result is printed.

```
$> src/itrftool -s NRMD COCO TONG -m 97401M003 50902M001 49971M001  -y 2020 -d 150 -c ../data/ITRF2014_GNSS.SSC.txt -p ../data/ITRF2014-psd-gnss.dat

Reference Frame: ITRF2014, Reference Epoch: 2010-01-01 00:00:00
NAME   DOMES         X(m)           Y(m)            Z(m)        EPOCH
---- --------- --------------- --------------- --------------- ------------------
COCO 50127M001    -741951.09602   6190961.71574  -1337767.36193 2020-05-29 00:00:00
NRMD 92701M005   -5743538.11585   1380503.86427  -2397895.98837 2020-05-29 00:00:00
REUN 97401M003    3364098.92612   4907944.67286  -2293466.68314 2020-05-29 00:00:00
TONG 50902M001   -5930303.53647   -500148.80597  -2286366.30075 2020-05-29 00:00:00
```



> Minor format changes may be exhibeted between the C++ and the Python implementation; 
> e.g. Python results are not sorted (alphabeticaly)

# the program

The repository actually includes two independent implementations of the program:
  
  * a Python implementetation, found in `python` directory, and
  * a C++ implementation, found in the `cpp` folder

both do the same thing! If you are only interested in running the program install any of the two. 
If you plan on using the functions/headers/modules install the one that fits your needs.

## python implementation

The package requires [numpy](http://www.numpy.org/) and should work both for
Python 2.x and Python 3.x. No other requirement exists.

The package is acompanied by a (Python) script under `python/itrftools/bin/itrftool`, which gets
automatically installed (during the ppackage installation process)

To install the package and the executable, run (in the folder `python/itrftools`)
`$> python setup.py install`
After that, you should have an executable named `itrftools` installed. After installation users 
can also make use of the modules in `itrftools` (see `python/itrftools/itrftools`).

## c++ implementation

The c++ implementation can be found under the `cpp` folder. The installation requires the 
[ggdatetime](https://github.com/xanthospap/ggdatetime.git) library to work (for reading and manipulating dates). 
It also needs the [ggeodesy](https://github.com/xanthospap/ggeodesy.git) library to perform geodetic 
calculations. To install, either use autotools:

```
$> cd cpp
$> autoreconf -if
$> ./configure
$> make
```

or (**the easier way**), uncompress the tarball `cpp/itrf_tools-1.00.tar.gz` and 
inside the uncompressed folder, do:

```
$> configure
$> make
```

## data files

To make use of the package and/or script, you will need the relevant files
(e.g. PSD and SSC files). All of these are publicly available at the ITRF
website (http://itrf.ensg.ign.fr/) and are **NOT** included within the package.

# todo:

Compute standard deviation values for the extrapolated station coordinates.

# bug and comments

Please send any bugs, feedback, suggestions, comments, etc ..... to
xanthos@mail.ntua.gr or dganastasiou@gmail.com
