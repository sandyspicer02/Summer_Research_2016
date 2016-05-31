#!/usr/bin/env python
import glob 
import ds9
import sys
import os

'''Usage - Run for command line
checkHAemission.py MKW11
'''
# open ds9  

# ask user if there is HA emission
# If no then place cutout in no emission folder
# if yes then place cutout in emission folder
# each galaxy will have its own folder containing Rband, Halpha with cont & w/o cont
#....................................
 
def displaycutout(prefix):
	
	d=ds9.ds9() #open new ds9 window 
	
	Rfiles = glob.glob('*R.fits') #seperate out each individual galaxy 
	#print Rfiles
	for rimage in Rfiles:
		d.set('frame delete all')
		t=rimage.split('R')
		csimage=t[0]+'Ha.fits'
		Haimage=t[0]+'Hawc.fits'
		continueflag=0
		t1=t[0].split('_') #obtain name of galaxy AGC number 
		#print rimage, os.path.isfile(rimage), os.path.isfile(csimage)
		if (os.path.isfile(rimage) & os.path.isfile(csimage) & os.path.isfile(Haimage)):
			
			s1='file new '+csimage #open Halpha cont subtracted and Rband images in ds9
			s2='file new '+rimage  #set settings for viewing in ds9
			try:
				d.set(s1)
				d.set('zscale')
				d.set(s2)
				d.set('zscale')
				
				
			except:
				print 'problem opening parent image'
                        

			while not(continueflag):
				flag=raw_input('Does this image have Halpha emission? (1 = yes, 0 = no, x = quit) \n') 
				if flag.find('x') > -1: 
					return
				try:
					flag=int(flag) # if valid command then continue to moving images
					continueflag=1
				except:
					print 'Error processing your input'
					

				spiral=raw_input('Is this a sprial galaxy? (1 = yes, 0 = no, x = quit) \n') 
				if spiral.find('x') > -1: 
					return
				try:
					spiral=int(spiral) # if valid command then continue to moving images
					continueflag=1
				except:
					print 'Error processing your input'
			
				
			if flag & spiral:
				soutdir=semissiondir+t1[0]+'/'
				try:
					if os.path.isdir(soutdir):
						print 'spiral emission directory exists'
					else:
						os.system('mkdir -p '+soutdir)

					os.rename(csimage,soutdir+csimage)# move images 
					os.rename(rimage,soutdir+rimage)
					os.rename(Haimage,soutdir+Haimage)
				except:
					print 'WARNING: problem moving image'
					
			elif flag & ~spiral:
				nsoutdir=nsemissiondir+t1[0]+'/'
				try:
					if os.path.isdir(nsoutdir):
						print 'non-spiral emission directory exists'
					else:
						os.system('mkdir -p '+nsoutdir)

					os.rename(csimage,nsoutdir+csimage)# move images 
					os.rename(rimage,nsoutdir+rimage)
					os.rename(Haimage,nsoutdir+Haimage)
				except:
					print 'WARNING: problem moving image'

			elif ~flag & spiral:
				
				sneoutdir=snoemissiondir+t1[0]+'/' #sne=spiral non emission (probably should change the name)
				if os.path.isdir(sneoutdir):
					print 'non-emission directory exists'
				else:
					os.system('mkdir -p '+sneoutdir)

				# leave images where they are
				os.rename(csimage,sneoutdir+csimage)
				os.rename(rimage,sneoutdir+rimage)
				os.rename(Haimage,sneoutdir+Haimage)

			
			else:
				nsneoutdir=nsnoemissiondir+t1[0]+'/'
				if os.path.isdir(nsneoutdir):
					print 'non-emission directory exists'
				else:
					os.system('mkdir -p '+nsneoutdir)

				# leave images where they are
				os.rename(csimage,nsneoutdir+csimage)
				os.rename(rimage,nsneoutdir+rimage)
				os.rename(Haimage,nsneoutdir+Haimage)
				

	return
prefix=sys.argv[1] 

semissiondir='/home/share/research/LocalClusters/Halpha/cutouts/' + prefix+'/emission/spiral/'
nsemissiondir='/home/share/research/LocalClusters/Halpha/cutouts/' + prefix+'/emission/nonspiral/'
snoemissiondir = '/home/share/research/LocalClusters/Halpha/cutouts/' + prefix+'/nonemission/spiral/'
nsnoemissiondir = '/home/share/research/LocalClusters/Halpha/cutouts/' + prefix+'/nonemission/nonspiral/'

if os.path.isdir(semissiondir): # check (and create) emssion/nonemssion directories
	print 'emission directory exists'
else:
	os.system('mkdir -p '+semissiondir)
if os.path.isdir(nsemissiondir): # check (and create) emssion/nonemssion directories
	print 'emission directory exists'
else:
	os.system('mkdir -p '+nsemissiondir)
if os.path.isdir(snoemissiondir):
	print 'non-emission directory exists'
else:
	os.system('mkdir -p '+snoemissiondir)
if os.path.isdir(nsnoemissiondir):
	print 'non-emission directory exists'
else:
	os.system('mkdir -p '+nsnoemissiondir)
	
displaycutout(prefix)


