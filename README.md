# itrftools
-------------------------------------------------------------------------------
itrftools is a collection of Python modules/programs to assist the processing and
manipulation of ITRF14-related files; ease computation and extrapolation of
station coordinates.

The package requires numpy (http://www.numpy.org/) and should work both for
Python 2.x and Python 3.x. No other requirement exists.

The package is acompanied by a (Python) script under /bin/itrftool, which gets
automatically installed (during the ppackage installation process) and can
perform:

* Coordinate extrapolation to a reference epoch using a SSC file, optionaly
      including a PSD file (for ITRF2014)

* PSD computation (per component)

Stations to be considered can be identified either by their 4-char ID or via
their DOMES. See the script's help message for more info.

To install the package and the executable, run (in the folder `python/itrftools`)
`$> python setup.py install`
After that, you should have an executable named `itrftools` installed

To make use of the package and/or script, you will need the relevant files
(e.g. PSD and SSC files). All of these are publicly available at the ITRF
website (http://itrf.ensg.ign.fr/) and are **NOT** included within the package.

# update:
-------------------------------------------------------------------------------
There is now a C++ version of the project availabe under the folder `cpp`. To install 
it (inside the `cpp` folder):
```
./configure
make
```
and you should have the `src/itrftool` executable.

# usage
-------------------------------------------------------------------------------

## compute PSD values
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

# todo:
-------------------------------------------------------------------------------
Compute standard deviation values for the extrapolated station coordinates.

# bug and comments
-------------------------------------------------------------------------------
Please send any bugs, feedback, suggestions, comments, etc ..... to
xanthos@mail.ntua.gr or dganastasiou@gmail.com
