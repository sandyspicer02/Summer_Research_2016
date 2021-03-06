#!/usr/bin/env python

'''
  GOAL:
  make lists of files that contain
  - dome flats with same filter
  - sky flats with same filter

  PROCEDURE:
  -user should move junk files to to a junk subdirectory before starting
  -use gethead to pull relavent header data
  - overscan subtract and trim images
  -combine flats according to flat type (dome or sky) and filter

  USAGE:

  REQUIRED MODULES:
  -pyraf

  NOTES:
  
  developed to reduce HDI data from April 2015

  WRITTEN BY:
  Rose A. Finn
  EDITED BY:
  Natasha Collova, Tiffany Flood, and Kaitlyn Hoag 5/29/15
  
  UPDATES:
  
'''
import glob
import os
import numpy as np

# group files using gethead
# OBSTYPE (object,flat,bias)
# CMMTOBS - filter
# bias frames
#os.system('ls *b00.fits > biasframes')

#object_files=glob.glob('*o00.fits')
#flat_files=glob.glob('*f00.fits')
#all_files=glob.glob('*.fits')
os.system('gethead *f00.fits CMMTOBS > junkfile')
infile=open('junkfile','r')
fnames=[]
ftype=[]
for line in infile:
    t=line.split('.fits')
    fnames.append(t[0]+'.fits')
    ftype.append(t[1].rstrip('\n').replace(' ',''))
infile.close()

filters=set(ftype)
  
ftype=np.array(ftype) # make into character array
flattypes=['domeflat','skyflat']
for t in flattypes:
    for f in filters:
        flatgroup=t+f
        print 'flatgroup = ',flatgroup
        indices=np.where(f == filters and t == flattypes)
        #print indices, len(indices[0])
        if len(indices[0]) > 0:
            outfile = open(flatgroup,'w')
            for i in indices[0]:
                #print fnames[i],ftype[i]
                outfile.write(fnames[i]+'\n')
            outfile.close()
os.remove('junkfile')
