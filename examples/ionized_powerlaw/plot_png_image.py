from radmc3dPy.image import *
from matplotlib import cm
a=readImage()
plotImage(a,log=True,maxlog=4,cmap=cm.hot,bunit='snu',dpc=1000,au=True)

