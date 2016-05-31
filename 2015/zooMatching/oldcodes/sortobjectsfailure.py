#!/usr/bin/env python

'''
  GOAL:
  make lists of files that contain
  - dome flats with same filter
  - sky flats with same filter
  - bias frames
  - dark frames with the same exposure time


  mv c7133t014[3-9]f00.fits junk/.
'''
import glob
import os
import numpy as np

#filters=['V','R','ha8','ha12','ha16']
# group files using gethead
# OBSTYPE (object,flat,bias)
# CMMTOBS - filter
# bias frames
#os.system('ls *b00.fits > biasframes')

#object_files=glob.glob('*o00.fits')
#flat_files=glob.glob('*f00.fits')
#all_files=glob.glob('*.fits')
os.system('gethead *o00.fits CMMTOBS OBJECT > junkfile2')
infile=open('junkfile2','r')
fnames=[]
ffilter=[]
fobject=[]
for line in infile:
    t=line.split()
    fnames.append(t[0])
    fobject.append(t[2].rstrip('\n').replace(' ',''))
    ffilter.append(t[1])
infile.close()
info = {
    'Name' : fnames,
    'Filter' : ffilter,
    'Object' : fobject}

fobjectset=set(fobject)
ffilterset=set(ffilter)
#print info

ftype=np.array(ffilter) # make into character array
for files in fnames:
    for filters in ffilter:
        for objects in fobject:
            if 
#flattypes=['domeflat','skyflat']
#for f in ffilterset:
 #   for t in fobjectset:
  #      group = f+t
#print fnames, ffilter, fobject







     #   flatgroup=f #t+'_'+f
     #   print 'flatgroup = ',flatgroup
      #  indices=np.where(ftype == flatgroup)
       # #print indices , len(indices[0])
#
 #       if len(indices[0]) > 1:
  #          outfile = open(flatgroup,'w')
   #         for i in indices[0]:
                #print fnames[i],ftype[i]
    #            outfile.write(fnames[i]+'\n')
     #       outfile.close()
#os.remove('junkfile2')
                

