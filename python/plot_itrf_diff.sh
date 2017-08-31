#!/bin/bash
#
#    GMT Example 21  $Id: job21.csh,v 1.14 2005/03/04 03:45:01 remko Exp $
#
# Purpose:    Plot a time-series
# GMT progs:    gmtset, gmtconvert, minmax, psbasemap, psxy 
# Unix progs:    echo
#

# File has time stored as dd-Mon-yy so set input format to match it
# Make sure language is us since monthnames are in US english.

gmt gmtset INPUT_DATE_FORMAT yy-mm-dd PLOT_DATE_FORMAT o ANNOT_FONT_SIZE_PRIMARY +10p
gmt gmtset TIME_FORMAT_PRIMARY abbreviated CHAR_ENCODING ISOLatin1+
gmt gmtset TIME_LANGUAGE us

# Pull out a suitable region string in default  yyyy-mm-dd format

info=`gmt minmax -f0T -I50 -C -H RHAT_price.csv`
R="-R$info[1]/$info[2]/$info[3]/$info[4]"

# Lay down the basemap:

psbasemap $R -JX9iT/6i -Glightgreen -K -U"Example 21 in Cookbook" -Bs1Y/WSen \
    -Bpa3Of1o/50WSen:=\$::."RedHat (RHAT) Stock Price Trend since IPO": >! example_21.ps

# Plot main window with open price as red line over yellow envelope of low/highs
# use gmtconvert to stitch the envelope together (-I reverses outout) and
# set output date format to match the input date format.

gmtset OUTPUT_DATE_FORMAT dd-o-yy
gmtconvert -F0,2 -f0T -Hi RHAT_price.csv >! RHAT.env
gmtconvert -F0,3 -f0T -I -Hi RHAT_price.csv >> RHAT.env
psxy -R -J -Gyellow -O -K RHAT.env >> example_21.ps
psxy -R -J RHAT_price.csv -H -Wthin,red -O -K >> example_21.ps

# Draw P Wessel's purchase price as line and label it.  Note we temporary switch
# back to default yyyy-mm-dd format since that is what minmax gave us.

echo "05-May-00    0" >! RHAT.pw
echo "05-May-00    300" >> RHAT.pw
psxy -R -J RHAT.pw -Wthinner,- -O -K >> example_21.ps
echo "01-Jan-99    25" >! RHAT.pw
echo "01-Jan-05    25" >> RHAT.pw
psxy -R -J RHAT.pw -Wthick,- -O -K >> example_21.ps
gmtset INPUT_DATE_FORMAT yyyy-mm-dd
echo "$info[2] 25 12 0 17 RB Wessel purchase price" \
    | pstext -R -J -O -K -D-0.1i/0.05i -N >> example_21.ps
gmtset INPUT_DATE_FORMAT dd-o-yy

# Get smaller region for insert for trend since 2003

set R = "-R2003T/$info[2]/$info[3]/30"

# Lay down the basemap, using Finnish annotations and place the insert in the upper right:

gmtset TIME_LANGUAGE fi
psbasemap $R -JX6iT/3i -Bpa1Of3o/10:=\$:ESw -Bs1Y/ -Glightblue -O -K -X3i -Y3i >> example_21.ps
gmtset TIME_LANGUAGE us

# Again, plot close price as red line over yellow envelope of low/highs

psxy -R -J -Gyellow -O -K RHAT.env >> example_21.ps
psxy -R -J RHAT_price.csv -H -Wthin/red -O -K >> example_21.ps

# Draw P Wessel's purchase price as dashed line

psxy -R -J RHAT.pw -Wthick,- -O >> example_21.ps

# Clean up after ourselves:

\rm -f RHAT.* .gmtcommands4 .gmtdefaults4


