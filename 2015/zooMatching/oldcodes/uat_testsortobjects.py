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
os.system('gethead f*o00.fits CMMTOBS OBJECT EXPTIME > junkfile2')
infile=open('junkfile2','r')
fnames=[]
ftype=[]
fobject=[]
exptime=[]
for line in infile:
    
    t=line.split()
    #print line
    #print len(t),t
    fnames.append(t[0])
    fobject.append(t[2].rstrip('\n').replace(' ',''))
    exptime.append(t[3].rstrip('\n').replace(' ',''))
    ftype.append(t[1])
    
infile.close()

filters=set(ftype)
  
ftype=np.array(ftype) # make into character array
fobject=np.array(fobject)
objecttypes=set(fobject)
exptime=np.array(exptime,'f')

for f in filters:
    #print 'filter = ',f
    for t in objecttypes:
        #print 'object = ',t
        objectgroup=str(t)+ '_' + str(f)
        #print 'objectgroup = ',objectgroup
        
        indices=np.where((f == ftype) & (t == fobject) & (exptime > 60))
        print 'indices = ',indices
        if len(indices[0]) > 0:
            outfile = open(objectgroup,'w')
            for i in indices[0]:
                #print fnames[i],ftype[i]
                outfile.write(fnames[i]+'\n')
            outfile.close()
            
                
os.remove('junkfile2')
