#!/usr/bin/env python

from pylab import *
#import pylab

#plot a parabola
figure(1)
x=arange(10)
y=x**2
plot(x,y,'b^')
xlabel('time (sec)',fontsize=20)
ylabel('distance (m)',fontsize=20)

savefig('/home/share/research/HalphaGroups/kelly.png')
