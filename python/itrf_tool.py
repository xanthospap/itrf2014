#! /usr/bin/python

import sys, datetime, argparse
sys.path.append('.')
from itrfssc import itrf_extrapolate
from compute_psd import compute_psd, enu2xyz

##  set the cmd parser
parser = argparse.ArgumentParser(
    description='Extrapolate coordinates from a SSC file, optionaly including'
    'a PSD file',
    epilog ='Ntua - 2017'
)
parser.add_argument('-s', '--stations',
    nargs    = '*',
    action   = 'store',
    required = False,
    help     = 'A whitespace seperated list of stations to compute coordinates'
    ' for. The given names are checked against the \"4-char ID\" in the input '
    'files.',
    metavar  = 'STATION_LIST',
    dest     = 'stations',
    default  = []
)
parser.add_argument('-m', '--domes',
    nargs    = '*',
    action   = 'store',
    required = False,
    help     = 'A whitespace seperated list of station domes to compute coordinates'
    ' for. The given deomes are checked against the \"DOMES\" field in the input '
    'files.',
    metavar  = 'DOMES_LIST',
    dest     = 'domes',
    default  = []
)
parser.add_argument('-c', '--ssc',
    action   = 'store',
    required = True,
    help     = 'A SSC ascci file to extract coordinates and velocities from. These'
    ' files are normaly accessible at: http://itrf.ign.fr/ITRF_solutions/',
    metavar  = 'SSC_FILE',
    dest     = 'ssc_file'
)
parser.add_argument('-p', '--psd',
    action   = 'store',
    required = True,
    help     = 'A PSD ascci file to extract Post-Seismic-Deformation models and'
    'parameters from. These files are normaly accessible at: '
    'ftp://itrf.ign.fr/pub/itrf/itrf2014/ITRF2014-psd-gnss.dat',
    metavar  = 'PSD_FILE',
    dest     = 'psd_file'
)
parser.add_argument('-y', '--year',
    action   = 'store',
    type     = int,
    required = True,
    help     = 'The year to extrapolate coordinates at.',
    metavar  = 'YEAR',
    dest     = 'year'
)
parser.add_argument('-d', '--doy',
    action   = 'store',
    type     = int,
    required = True,
    help     = 'The day of year to extrapolate coordinates at.',
    metavar  = 'DOY',
    dest     = 'doy'
)

# parse cmd
args = parser.parse_args()

# convert the give year-doy to a datetime.datetime instance
t = datetime.datetime.strptime('{}-{}'.format(args.year, args.doy), '%Y-%j')

# for all stations in station list, extrapolate coordinates
station=args.stations
results = itrf_extrapolate(ssc_file=args.ssc_file, t=t, station=args.stations)
results += itrf_extrapolate(ssc_file=args.ssc_file, t=t, domes=args.domes)

# find PSD corrections
for idx, item in enumerate(results):
    #try:
    e, n, u = compute_psd(args.psd_file, t=t, station=item['station'])
    e, n, u = [ i/1000e0 for i in [e, n, u] ] ## mm to m
    print('#Found PSD for station {}, [e, n, u] = [{}, {}, {}]'.format(item['station'], e, n, u))
    dx, dy, dz = enu2xyz(e, n, u, item['x'], item['y'], item['z']) ## local to cartesian
    print('#In cartesian that is [x, y, z] = [{}, {}, {}]'.format(dx, dy, dz))
    print('#Adding psd to station {} {} {} {}'.format(item['station'], item['x'], item['y'], item['z']))
    results[idx]['x'] += dx
    results[idx]['y'] += dy
    results[idx]['z'] += dz
    print('#New coordinates of station {} {} {} {}'.format(item['station'], item['x']+dx, item['y']+dy, item['z']+dz))
    #except:
    #    pass

for idx, item in enumerate(results):
    print('{0} {1} {2:15.5f} {3:15.5f} {4:15.5f}'.format(item['station'], item['domes'], item['x'], item['y'], item['z']))
