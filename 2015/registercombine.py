#!/usr/bin/env python

'''
    GOAL:
    -read in a file containing images of the same object in a given filter.
    -register the images with the iraf command "wregister."
    -combine the registered images.

    REQUIREMENTS:
    -Must have a file that contains images of one object  in one filter. Should     be named in the format object_filter
    -must run in folder containing files that will be inputted
    -will create a combined output image named object_filter.fits

'''
from pyraf import iraf
import sys
import os


#read in list of files
infile=sys.argv[1]
imfile=open(infile,'r')
images=[]

for line in imfile:
    images.append(line.rstrip('\n'))
imfile.close()
                  
#run wregister
for image in images:
    iraf.wregister(input=image,reference=images[0],output='r'+image)
#write registered files into an output file
outfile=open('imageFile','w')
for im in images:
    outfile.write(im+'\n')
outfile.close()

#combine registered images
iraf.imcombine(input='@imageFile', output=infile+'.fits')


#Written by Tiffany Flood and Kelly Whalen

     

