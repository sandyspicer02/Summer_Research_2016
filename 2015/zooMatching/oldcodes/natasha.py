#!/usr/bin/env python

from pylab import *
#import pylab

#plot a porabola

figure(1)
clf()
x=arange(10)
y=x**2
plot(x,y,'b^',markersize=12)
xlabel('time(sec)',fontsize=20)
ylabel('distance(m)',fontsize=20)
savefig('/home/share/research/HalphaGroups/natasha.png')
