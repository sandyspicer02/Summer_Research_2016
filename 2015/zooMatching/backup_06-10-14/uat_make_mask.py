#!/usr/bin/env python

import os
import sys
from astropy.io import fits
import numpy as np
from pyraf import iraf

# pass in image and catalog

if len(sys.argv) == 1:
    sys.exit('Usage:  uat_make_mask.py image optional_default.sex_file')
elif len(sys.argv) == 2:
    image=sys.argv[1]
    defaultcat='default.sex.sdss'
elif len(sys.argv) == 3:
    image=sys.argv[1]
    defaultcat=sys.argv[2]
else:
    sys.exit('Too many command-line arguments \n  Usage:  uat_make_mask.py image catalog(optional)')
#
# assume galaxy is at the center of the image
fdulist = fits.open(image)
t=fdulist[0].data
n2,n1=t.shape
xc=n1/2.
yc=n2/2.
fdulist.close()

# run sextractor to generate a list of objects in the image
# generate 'segmentation image'
try:
    os.system('ln -s /home/share/research/LocalClusters/sextractor/* .')
except:
    print 'could not link to /home/share/research/LocalClusters/sextractor'
    print defaultcat,' should be in the current directory'
os.system('sex %s -c %s -CATALOG_NAME test.cat -CATALOG_TYPE ASCII'%(image,defaultcat))

#   parse sextractor output to get x,y coords of objects        

sexout=np.loadtxt('test.cat',usecols=[0,1,2])
sexnumber=sexout[:,0]
xsex=sexout[:,1]
ysex=sexout[:,2]
dist=np.sqrt((yc-ysex)**2+(xc-xsex)**2)

#   find object ID
objIndex=np.where(dist == min(dist))
objNumber=sexnumber[objIndex]
objNumber=objNumber[0] # not sure why above line returns a list
mask_image=image.split('.fits')[0]+'mask.fits'


fdulist = fits.open('segmentation.fits',mode='update')
t=fdulist[0].data
# replace object ID values with zero
f = (t == objNumber)
replace_values = np.zeros(t.shape)
t=t*(~f) + replace_values*f

fdulist.flush()
# write out mask image
if os.path.isfile(mask_image):
    os.remove(mask_image)

hdu=fits.PrimaryHDU(t)
hdu.writeto(mask_image)

fdulist.close()

#   use iraf imexam to replace object ID values with zero
#iraf.imreplace(working_dir+mask_image,value=0,lower=objNumber-.5,upper=objNumber+.5)
# convert segmentation image to object mask by replacing the object ID of target galaxy with zeros
