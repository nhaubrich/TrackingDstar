import ROOT
import sys
from math import pow,sqrt

ifile = ROOT.TFile(sys.argv[1])
ofile = ROOT.TFile("ofile.root","RECREATE")
tree = ifile.Get("tree1")

nentries = tree.GetEntries()
print "nentries = ",nentries
gridSize = 16
hist_shared = ROOT.TH2F("hist_shared","hist_shared",gridSize,0,gridSize,gridSize,0,gridSize)
hist_pion = ROOT.TH2F("hist_pion","hist_pion",gridSize,0,gridSize,gridSize,0,gridSize)
hist_proton = ROOT.TH2F("hist_proton","hist_proton",gridSize,0,gridSize,gridSize,0,gridSize)
#nentries = 1000

def getPixelHist(pixels,gridSize):
    xmin = -1
    xmax = -1
    ymin = -1
    ymax = -1
    xavg = 0.
    yavg = 0.
    tot_adc = 0.
    for x,y,adc in pixels:
        #print x,y,adc
        if x < xmin or xmin == -1:
            xmin = x
        if y < ymin or ymin == -1:
            ymin = y
        xavg += x*adc
        yavg += y*adc
        tot_adc += adc
    xavg = xavg / (tot_adc)
    yavg = yavg / (tot_adc)
    xavg_int = int(round(xavg))
    yavg_int = int(round(yavg))
    hist = ROOT.TH2F("hist_%i" % iEntry,"hist_%i" % iEntry,gridSize,0,gridSize,gridSize,0,gridSize)
    for x,y,adc in pixels:
        #print (x-xavg_int),(y-ymin),adc
        hist.Fill(x-xavg_int+gridSize/2.,y-yavg_int+gridSize/2.,adc)
    if hist.Integral() > 0:
       hist.Scale(1./hist.Integral())
    else:
       hist = ROOT.TH2F("hist_shared","hist_shared",gridSize,0,gridSize,gridSize,0,gridSize)
    return hist

for iEntry in xrange(nentries):
    if (iEntry % 1000 == 0): 
        print "processing entry: ",iEntry
    tree.GetEntry(iEntry)
    pixels_shared = []
    pixels_pion = []
    pixels_proton = []
    if len(tree.PionPixelHit_x)>0:
        for i in xrange(len(tree.PionPixelHit_x)):
            pixels_pion.append((tree.PionPixelHit_x[i],tree.PionPixelHit_y[i],tree.PionPixelHit_adc[i]))
        hist_pion.Add(getPixelHist(pixels_pion,gridSize))
    if len(tree.ProtonPixelHit_x)>0:
        for i in xrange(len(tree.ProtonPixelHit_x)):
            pixels_proton.append((tree.ProtonPixelHit_x[i],tree.ProtonPixelHit_y[i],tree.ProtonPixelHit_adc[i]))
        hist_proton.Add(getPixelHist(pixels_proton,gridSize))
    if tree.LambdaMass[0] > 0 and len(tree.LambdaSharedHitPixelHits_x) > 0 and tree.LambdaSharedHitLayer[0]==0 and tree.flightLength[0]<4.:
        for i in xrange(len(tree.LambdaSharedHitPixelHits_x)):
            pixels_shared.append((tree.LambdaSharedHitPixelHits_x[i],tree.LambdaSharedHitPixelHits_y[i],tree.LambdaSharedHitPixelHits_adc[i]))
        hist_shared.Add(getPixelHist(pixels_shared,gridSize))
    #dr = sqrt( pow((abs(tree.TrkProtonphi[0]-tree.TrkPi1phi[0])-(abs(tree.TrkProtonphi[0]-tree.TrkPi1phi[0])>3.14)*2*3.14),2) + pow((tree.TrkProtoneta[0]-tree.TrkPi1eta[0]),2) )
ofile.cd()
hist_shared.Write()
hist_pion.Write()
hist_proton.Write()
