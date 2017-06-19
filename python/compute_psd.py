#! /usr/bin/python

import datetime

def time_str2dt(time_str):
    # 09:280:80331
    cyr, doy, isec = map(int, time_str.split(':'))
    yr = cyr+2000 if datetime.datetime.now().year < cyr+2000 else cyr+1900
    dt = ( datetime.datetime(yr, 1, 1) + datetime.timedelta(doy - 1)
        + datetime.timedelta(seconds=isec) )
    return dt

def get_psd_model(line, cmp):
    assert( line[32] == cmp )
    model  = int(line[34])
    assert( model >= 0 and model < 5 )
    params = map(float, line[36:72].split())
    return model, params

def get_next_psd(fin):
    line = fin.readline()
    if line:
        sta_name = line[1:5]
        char_id  = line[7]
        domes    = line[9:18]
        dtime    = time_str2dt(line[19:31])
        modele, parame = get_psd_model(line, 'E')
        line = fin.readline()
        modeln, paramn = get_psd_model(line, 'N')
        line = fin.readline()
        modelu, paramu = get_psd_model(line, 'U')
        return modele, parame, modeln, paramn, modelu, paramu
    else: ## eof

filen = 'ITRF2014-psd-gnss.dat'
with open(filen) as fin:
    while (True):
        me, pe, mn, pn, mu, pu = get_next_psd(fin);
