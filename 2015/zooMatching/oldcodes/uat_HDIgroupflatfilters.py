#!/usr/bin/env python

'''
  GOAL:
  make lists of files that contain
  - dome flats with same filter
  - sky flats with same filter

  PROCEDURE:
  -User should move junk files to a junk subdirectory before starting
   - Junk files include initial bias frames pointing and other garbage frames
  - Use gethead to pull relevant header data
  - Overscan subtract and trim images (we assume these image names begin with 'tr'
  - Combine flats according to flat type (dome vs sky) and filter

  USAGE:
  
  REQUIRED MODULES:
  pyraf

  NOTES:
  in junkfile ftr flats still show. We changed the gethead requirements to only bring in files that start with tr but the ftr files will not go away! =(
  
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
os.system('gethead tr*f00.fits CMMTOBS > junkfile')
#we assume that the flat images are trimmed and the file name starts with 'tr'
infile=open('junkfile','r')
fnames=[]
filter=[]
ftype=[]   #skyflat or domeflat
for line in infile:
    t=line.split()
    fnames.append(t[0])
    ftype.append(t[1]+t[2])
    filter.append(t[3].rstrip('\n'))
infile.close()
set_filter=set(filter)
set_ftype=set(ftype)
array_ftype=np.array(ftype)
array_filter=np.array(filter)

for f in set_ftype:
    print "flat type=",f
    for element in set_filter:
        ftype_filter = str(f)+'_'+str(element)
        print 'filter type = ',element
        indices=np.where((array_ftype == f)&(array_filter == element))
        print indices, len(indices[0])
        if len(indices[0]) > 0:
            outfile = open(ftype_filter,'w')
            for i in indices[0]:
                #print fnames[i],ftype[i]
                outfile.write(fnames[i]+'\n')
            outfile.close()
os.remove('junkfile')            
            
                

