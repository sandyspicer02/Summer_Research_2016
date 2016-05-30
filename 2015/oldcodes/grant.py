#!/usr/bin/env python
from pylab import *

figure(1)
clf()
x = arange(10)
y = x**2
plot(x,y,'co',markersize=13)
xlabel('time(sec)',fontsize=13)
ylabel('distance(m)',fontsize=13)

savefig('/home/share/research/HalphaGroups/grant.png')

