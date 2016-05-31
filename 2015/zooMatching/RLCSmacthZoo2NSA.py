#!/usr/bin/env python

#  Written by Rose A. Finn, Feb 11, 2013
# 


import atpy, os
from pylab import *
from astropy.io import fits
from astropy.table import Table
from astropy.table import Column





homedir='/home/rfinn/'
# catalog with full agc

agcfile='/home/rfinn/idl/programs/idl_alfa/agctotal.fits'


def findnearest(x1,y1,x2,y2,delta):#use where command
	matchflag=1
	nmatch=0
	d=sqrt((x1-x2)**2 + (y1-y2)**2)#x2 and y2 are arrays
	index=arange(len(d))
	t=index[d<delta]
	matches=t
	if len(matches) > 0:
		nmatch=len(matches)
		if nmatch > 1:
			imatch=index[(d == min(d[t]))]
		else:
			imatch=matches[0]			
	else:
		imatch = 0
		matchflag = 0

	return imatch, matchflag,nmatch

class agc:
    def __init__(self,filename):

        self.agc=atpy.Table(filename,type='fits')
	    


    def match2zoo(self,delta):

        self.zoo_objid=[]
        self.zoo_nvote=zeros(len(self.agc.RA),'i')
        self.zoo_pel=zeros(len(self.agc.RA),'f')
        self.zoo_pcw=zeros(len(self.agc.RA),'f')
        self.zoo_pacw=zeros(len(self.agc.RA),'f')
        self.zoo_pedge=zeros(len(self.agc.RA),'f')
        self.zoo_pdk=zeros(len(self.agc.RA),'f')
        self.zoo_pmg=zeros(len(self.agc.RA),'f')
        self.zoo_pcs=zeros(len(self.agc.RA),'f')
        self.zoo_p_el_debiased=zeros(len(self.agc.RA),'f')
        self.zoo_p_cs_debiased=zeros(len(self.agc.RA),'f')
        self.zoo_spiral=zeros(len(self.agc.RA),'i')
        self.zoo_elliptical=zeros(len(self.agc.RA),'i')
        self.zoo_uncertain=zeros(len(self.agc.RA),'i')
        izoo_match=zeros(len(self.agc.RA),'i')
        izoo_match_flag=zeros(len(self.agc.RA),'i')
        izoophot_match=zeros(len(self.agc.RA),'i')
        izoophot_match_flag=zeros(len(self.agc.RA),'i')

        for i in range(len(self.agc.RA)):

            imatch,matchflag,nmatch=findnearest(self.agc.RA[i],self.agc.DEC[i],zoo.zRA,zoo.zDEC,delta)
            #matchflag=0
            #try:
            #    imatch=zoo.zoodict[self.sdss_objid[i]]
            #    matchflag=1
            #except:
            #    print 'no match using dictionary', i,matchflag

            if matchflag:
                #print i,self.agc.AGCNUMBER[i],' found match to galaxy zoo spec sample dat'
                izoo_match[i]=imatch
                izoo_match_flag[i]=matchflag
                self.zoo_objid.append(zoo.zdat.OBJID[imatch])
                self.zoo_nvote[i]=zoo.zdat.NVOTE[imatch]
                self.zoo_pel[i]=zoo.zdat.P_EL[imatch]
                self.zoo_pcw[i]=zoo.zdat.P_CW[imatch]
                self.zoo_pacw[i]=zoo.zdat.P_ACW[imatch]
                self.zoo_pedge[i]=zoo.zdat.P_EDGE[imatch]
                self.zoo_pdk[i]=zoo.zdat.P_DK[imatch]
                self.zoo_pmg[i]=zoo.zdat.P_MG[imatch]
                self.zoo_pcs[i]=zoo.zdat.P_CS[imatch]
                self.zoo_p_el_debiased[i]=zoo.zdat.P_EL_DEBIASED[imatch]
                self.zoo_p_cs_debiased[i]=zoo.zdat.P_CS_DEBIASED[imatch]
                self.zoo_spiral[i]=zoo.zdat.SPIRAL[imatch]
                self.zoo_elliptical[i]=zoo.zdat.ELLIPTICAL[imatch]
                self.zoo_uncertain[i]=zoo.zdat.UNCERTAIN[imatch]
            else:
                #imatch,matchflag,nmatch=findnearest(self.agc.RA[i],self.agc.DEC[i],zoo.zphotRA,zoo.zphotDEC,delta)
                #print 'got here',i,self.agc.AGCNUMBER[i]
		imatch,matchflag,nmatch=findnearest(self.agc.RA[i],self.agc.DEC[i],zoo.zphotRA,zoo.zphotDEC,delta)
                #try:
                #    imatch=zoo.zoophotdict[self.sdss_objid[i]]
                #    print 'found a match using phot dictionary'
                #    matchflag=1
                #except:
		#print 'no match using phot dictionary', i

                if matchflag:
                    izoophot_match[i]=imatch
                    izoophot_match_flag[i]=matchflag
                    self.zoo_objid.append(zoo.zphotdat.OBJID[imatch])
                    self.zoo_nvote[i]=zoo.zphotdat.NVOTE[imatch]
                    self.zoo_pel[i]=zoo.zphotdat.P_EL[imatch]
                    self.zoo_pcw[i]=zoo.zphotdat.P_CW[imatch]
                    self.zoo_pacw[i]=zoo.zphotdat.P_ACW[imatch]
                    self.zoo_pedge[i]=zoo.zphotdat.P_EDGE[imatch]
                    self.zoo_pdk[i]=zoo.zphotdat.P_DK[imatch]
                    self.zoo_pmg[i]=zoo.zphotdat.P_MG[imatch]
                    self.zoo_pcs[i]=zoo.zphotdat.P_CS[imatch]
                else:
                    self.zoo_objid.append('null')

        # write out results as a fits table that is line-matched to cluster NSA table
        ztab=atpy.Table()
        zooflag=izoo_match_flag | izoophot_match_flag
        ztab.add_column('match_flag',zooflag,dtype='bool')
        ztab.add_column('spec_match_flag',izoo_match_flag,dtype='bool')
        ztab.add_column('match_index',izoo_match)
        ztab.add_column('phot_match_flag',izoophot_match_flag,dtype='bool')
        ztab.add_column('phot_match_index',izoophot_match)
        #ztab.add_column('OBJID',self.zoo_objid,dtype='|S16')
        ztab.add_column('nvote',self.zoo_nvote)
        ztab.add_column('p_el',self.zoo_pel)
        ztab.add_column('p_cw',self.zoo_pcw)
        ztab.add_column('p_acw',self.zoo_pacw)
        ztab.add_column('p_edge',self.zoo_pedge)
        ztab.add_column('p_dk',self.zoo_pdk)
        ztab.add_column('p_mg',self.zoo_pmg)
        ztab.add_column('p_cs',self.zoo_pcs)
        ztab.add_column('p_el_debiased',self.zoo_p_el_debiased)
        ztab.add_column('p_cs_debiased',self.zoo_p_cs_debiased)
        ztab.add_column('p_spiral',self.zoo_spiral)
        ztab.add_column('p_elliptical',self.zoo_elliptical)
        ztab.add_column('p_uncertain',self.zoo_uncertain)



        outfile='/home/share/research/LocalClusters/AGCGalaxyZoo.fits'
        if os.path.exists(outfile):
            os.remove(outfile)
        ztab.write(outfile)

	zdat=Table()
        c1 = Column(name='match_flag',format='L',array=zooflag)
        c2 = Column(name='spec_match_flag',izoo_match_flag,dtype='bool')
        c3 = Column(name='match_index',izoo_match)
        c4 = Column(name='phot_match_flag',izoophot_match_flag,dtype='bool')
        c = Column(name='phot_match_index',izoophot_match)
        #c = Column(name=('OBJID',self.zoo_objid,dtype='|S16')
        c = Column(name='nvote',self.zoo_nvote)
        c = Column(name='p_el',self.zoo_pel)
        c = Column(name='p_cw',self.zoo_pcw)
        c = Column(name='p_acw',self.zoo_pacw)
        c = Column(name='p_edge',self.zoo_pedge)
        c = Column(name='p_dk',self.zoo_pdk)
        c = Column(name='p_mg',self.zoo_pmg)
        c = Column(name='p_cs',self.zoo_pcs)
        c = Column(name='p_el_debiased',self.zoo_p_el_debiased)
        c = Column(name='p_cs_debiased',self.zoo_p_cs_debiased)
        c = Column(name='p_spiral',self.zoo_spiral)
        c = Column(name='p_elliptical',self.zoo_elliptical)
        c = Column(name='p_uncertain',self.zoo_uncertain)
	zdat.add_column([c1,c2,c3 ])
	outfile2=''
	if os.path.exists(outfile2):
		os.remove(outfile2)
	zdat.write(outfile2)


class Gzoo:
    def __init__(self):
        # infile=homedir+'research/NSA/nsa_v0_1_2.fits'
        infile=homedir+'research/GalaxyZooTables/GalaxyZoo1_DR_table2.fits'
        self.zdat=atpy.Table(infile,type='fits')
        infile=homedir+'research/GalaxyZooTables/GalaxyZoo1_DR_table3.fits'
        self.zphotdat=atpy.Table(infile,type='fits')
        self.zRA=zeros(len(self.zdat.RA),'d')
        self.zDEC=zeros(len(self.zdat.RA),'d')
        for i in range(len(self.zdat.RA)):
            r=self.zdat.RA[i].split(':')
            self.zRA[i]=(float(r[0])+float(r[1])/60.+float(r[2])/3600.)*15
            d=self.zdat.DEC[i].split(':')
            self.zDEC[i]=(float(d[0])+float(d[1])/60.+float(d[2])/3600.)
        self.zphotRA=zeros(len(self.zphotdat.RA),'d')
        self.zphotDEC=zeros(len(self.zphotdat.RA),'d')
        for i in range(len(self.zphotdat.RA)):
            r=self.zphotdat.RA[i].split(':')
            self.zphotRA[i]=(float(r[0])+float(r[1])/60.+float(r[2])/3600.)*15
            d=self.zphotdat.DEC[i].split(':')
            self.zphotDEC[i]=(float(d[0])+float(d[1])/60.+float(d[2])/3600.)
        self.zoodict=dict((a,b) for a,b in zip(self.zdat.OBJID,arange(len(self.zdat.OBJID))))
        self.zoophotdict=dict((a,b) for a,b in zip(self.zphotdat.OBJID,arange(len(self.zphotdat.OBJID))))
zoo=Gzoo()
# match radius = 3"/3600 -> deg
delta=3./3600. 
a=agc(agcfile)
a.match2zoo(delta)
#mkw11=cluster('MKW11')
#mkw11.match2zoo(delta)
#myclusternames=['MKW11', 'MKW8', 'AWM4', 'A2063', 'A2052', 'NGC6107', 'Coma', 'A1367', 'Hercules']
#myclusternames=['MKW11']
#for cname in myclusternames:
#    cl=cluster(cname)
#    print '\n',cl.prefix, '\n'
#    cl.match2zoo(delta)

