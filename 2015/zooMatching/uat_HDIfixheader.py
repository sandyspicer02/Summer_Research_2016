#!/usr/bin/env python

'''
USE:
Changes certain header keywords to the standard.
Updates CMMTOBS --> FILTER
        RASTRNG --> CRVAL1
        DECSTRNG --> CRVAL2
Adds CRPIX1, CRPIX2, CD1_1, CD2_2, CTYPE1, CTYPE2

INPUT: all ftr*.fits in directory
OUTPUT: hftr*.fits

EXAMPLE:
   In the directory containing all flattened objects with incorrect headers type in the command line:
      '/home/share/research/pythonCode/uat_HDIfixheader.py'(or whatever the path is to where this program is stored)
   

NOTES: can be used on dome flattened images. To do so type '--filestring "dtr*.fits"' after the command


TO DO:
- convert FILTER1 and FILTER2 to regular FILTER header
- rename FILTER1 FILTER2 to FWHEEL1, FWHEEL2


FILTER1 = 0 = U band
FILTER1 = 1 = U band

FILTER 1 102 V
FILTER 1 103 R
FILTER 2 202 ha8
FILTER 2 203 ha12
FILTER 2 204 ha16 6740



'''

import argparse
import glob
from astropy import coordinates as coord
from astropy import units as u
from astropy.io import fits

filter1 = {102:'V',103:'R'}
filter2 = {202:'ha8',203:'ha12',204:'ha12'}

parser = argparse.ArgumentParser(description ='Edit image headers to include basic WCS information to the HDI image headers')
parser.add_argument('--filestring', dest='filestring', default='c*.fits', help='match string for input files (default =  ftr*.fits)')
parser.add_argument('--pixscalex', dest='pixelscalex', default='0.00011808', help='pixel scale in x (default = 0.00011808)')
parser.add_argument('--pixscaley', dest='pixelscaley', default='0.00011808', help='pixel scale in y (default = 0.00011808)')
args = parser.parse_args()
files = sorted(glob.glob(args.filestring))
nfiles=len(files)
i=1
for f in files:
    print 'FIXING HEADER FOR FILE %i OF %i'%(i,nfiles)
    data, header = fits.getdata(f,header=True)
    f1=header['FILTER1']
    f2=header['FILTER2']
    try:
        newfilter=filter1[f1]
    except KeyError:
        try:
            newfilter=filter2[f2]
        except KeyError:
            print 'WARNING: could not find filter2 = ',f2
    header.rename_key('FILTER1','FWHEEL1')
    header.rename_key('FILTER2','FWHEEL2')

    FILTER = header['CMMTOBS']
    if FILTER != newfilter:
        print 'WARNING: Filter wheel positions do not match CCMTOBS keyword'
    header.append(card=('FILTER',newfilter,'FILTER'))

    RASTRNG = header['RASTRNG']
    RA = coord.Angle(RASTRNG,unit=u.hour)
    header.append(card=('CRVAL1',RA.degree,'RA of reference point'))
    DECSTRNG = header['DECSTRNG']
    DEC = coord.Angle(DECSTRNG,unit=u.degree)
    header.append(card=('CRVAL2',DEC.degree,'DEC of reference point'))

    header.append(card=('CRPIX1','2048.','X reference pixel'))
    header.append(card=('CRPIX2','2048.','Y reference pixel'))
    header.append(card=('CD1_1',args.pixelscalex,'Pixel scale in X'))
    header.append(card=('CD2_2',args.pixelscaley,'Pixel scale in Y'))
    header.append(card=('CTYPE1','RA---TAN-SIP',''))
    header.append(card=('CTYPE2','DEC--TAN-SIP',''))
    header.append(card=('GAIN','1.3','gain (e-/ADU)'))
    header.append(card=('EPOCH',header['EQUINOX'],'EPOCH OF OBSERVATIONS'))
    
    print 'WRITING UPDATED FILE'
    fits.writeto('h'+f,data,header,clobber=True)
    i += 1
    print '\n'

    
    


                   