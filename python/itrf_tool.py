#! /usr/bin/python

import sys, datetime
sys.path.append('.')
from itrfssc import itrf_extrapolate
from compute_psd import compute_psd, enu2xyz

scc_file = sys.argv[1] 
psd_file = sys.argv[2]
station  = sys.argv[3]
t = datetime.datetime.strptime(sys.argv[4], '%Y-%j')

x, y, z = itrf_extrapolate(ssc_file=scc_file, t=t, station=station)
print 'x={}, y={}, z={}'.format(x, y, z)
#print 'Could not find station {} ins ssc file'.format(station)
e, n, u = compute_psd(psd_file, t=t, station=station)
print 'e={}, n={}, u={}'.format(e, n, u)
dx, dy, dz = enu2xyz(e,n,u,x,y,z)
print 'dx={}, dy={}, dz={}'.format(dx, dy, dz)
x += dx
y += dy
z += dz
print 'x={}, y={}, z={}'.format(x, y, z)
