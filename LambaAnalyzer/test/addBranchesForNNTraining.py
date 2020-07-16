import ROOT
import sys
from math import pow,sqrt
import numpy as np

ifile = ROOT.TFile(sys.argv[1])
ofile = ROOT.TFile("ofile2.root","RECREATE")
tree = ifile.Get("tree1")
otree = tree.CloneTree(0)

nentries = tree.GetEntries()
print "nentries = ",nentries
gridSize = 20
#nentries = 1200000

isSharedHit = np.zeros(1,dtype=int)
otree.Branch("isSharedHit",isSharedHit,"isSharedHit/I")
pixel_references = [0.]*gridSize*gridSize
for i in xrange(gridSize*gridSize):
    pixel_references[i] = np.zeros(1,dtype=float)
    otree.Branch("pixel_%i" % i,pixel_references[i],"pixel_%i/D" %i )
trackPt = np.zeros(1,dtype=float)
trackEta = np.zeros(1,dtype=float)
trackPhi = np.zeros(1,dtype=float)
nUniqueSimTracksInSharedHit = np.zeros(1,dtype=int)
sharedHitContainsGenLambda = np.zeros(1,dtype=bool)
sharedHitContainsGenPion = np.zeros(1,dtype=bool)
sharedHitContainsGenProton = np.zeros(1,dtype=bool)
GenDeltaR =  np.zeros(1,dtype=float)

otree.Branch("trackPt",trackPt,"trackPt/D")
otree.Branch("trackEta",trackEta,"trackEta/D")
otree.Branch("trackPhi",trackPt,"trackPhi/D")
otree.Branch("nUniqueSimTracksInSharedHit",nUniqueSimTracksInSharedHit, "nUniqueSimTracksInSharedHit")
otree.Branch("sharedHitContainsGenLambda", sharedHitContainsGenLambda, "sharedHitContainsGenLambda")
otree.Branch("sharedHitContainsGenPion", sharedHitContainsGenPion, "sharedHitContainsGenPion")
otree.Branch("sharedHitContainsGenProton", sharedHitContainsGenProton, "sharedHitContainsGenProton")
otree.Branch("GenDeltaR",GenDeltaR,"GenDeltaR/D")

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
##for iEntry in xrange(1200000):
#for iEntry in xrange(1200000,nentries):
    if (iEntry % 1000 == 0): 
        print "processing entry: ",iEntry
    tree.GetEntry(iEntry)
    pixels_shared = []
    pixels_pion = []
    pixels_proton = []
    if len(tree.PionPixelHit_x)>0 and tree.PionPixelHitLayer==0 and iEntry%100==0:
        for i in xrange(len(tree.PionPixelHit_x)):
            pixels_pion.append((tree.PionPixelHit_x[i],tree.PionPixelHit_y[i],tree.PionPixelHit_adc[i]))
        hist = getPixelHist(pixels_pion,gridSize)
        isSharedHit[0] = 0
        for i in xrange(gridSize):
            for j in xrange(gridSize):
                pixel_references[i+gridSize*j][0] = hist.GetBinContent(i+1,j+1)
        trackPt[0] = tree.TrkPi1pt[0]
        trackEta[0] = tree.TrkPi1eta[0]
        trackPhi[0] = tree.TrkPi1phi[0]
        otree.Fill()
    if len(tree.ProtonPixelHit_x)>0 and tree.ProtonPixelHitLayer==0 and iEntry%100==0:
        for i in xrange(len(tree.ProtonPixelHit_x)):
            pixels_proton.append((tree.ProtonPixelHit_x[i],tree.ProtonPixelHit_y[i],tree.ProtonPixelHit_adc[i]))
        hist = getPixelHist(pixels_proton,gridSize)
        isSharedHit[0] = 0
        for i in xrange(gridSize):
            for j in xrange(gridSize):
                pixel_references[i+gridSize*j][0] = hist.GetBinContent(i+1,j+1)
        trackPt[0] = tree.TrkProtonpt[0]
        trackEta[0] = tree.TrkProtoneta[0]
        trackPhi[0] = tree.TrkProtonphi[0]
        otree.Fill()
    if tree.LambdaMass[0] > 0 and len(tree.LambdaSharedHitPixelHits_x) > 0 and tree.LambdaSharedHitLayer[0]==0 and tree.flightLength[0]<4.:
        for i in xrange(len(tree.LambdaSharedHitPixelHits_x)):
            pixels_shared.append((tree.LambdaSharedHitPixelHits_x[i],tree.LambdaSharedHitPixelHits_y[i],tree.LambdaSharedHitPixelHits_adc[i]))
        isSharedHit[0] = 1
        hist = getPixelHist(pixels_shared,gridSize)
        for i in xrange(gridSize):
            for j in xrange(gridSize):
                pixel_references[i+gridSize*j][0] = hist.GetBinContent(i+1,j+1)
        if tree.TrkPi1pt[0] > tree.TrkProtonpt[0]:
            trackPt[0] = tree.TrkPi1pt[0]
            trackEta[0] = tree.TrkPi1eta[0]
            trackPhi[0] = tree.TrkPi1phi[0]
        else:
            trackPt[0] = tree.TrkProtonpt[0]
            trackEta[0] = tree.TrkProtoneta[0]
            trackPhi[0] = tree.TrkProtonphi[0]
        #Fill in pixel matching info
        try:
            nUniqueSimTracksInSharedHit[0] = tree.nUniqueSimTracksInSharedHit[0]
        except:
            continue
        sharedHitContainsGenLambda[0] = tree.sharedHitContainsGenLambda[0]
        sharedHitContainsGenPion[0] = tree.sharedHitContainsGenPion[0]
        sharedHitContainsGenProton[0] =  tree.sharedHitContainsGenProton[0]
        GenDeltaR[0] = tree.GenDeltaR[0] #dR between best gen lambda and reco lambda, good match is dR<0.1
        otree.Fill()
    #dr = sqrt( pow((abs(tree.TrkProtonphi[0]-tree.TrkPi1phi[0])-(abs(tree.TrkProtonphi[0]-tree.TrkPi1phi[0])>3.14)*2*3.14),2) + pow((tree.TrkProtoneta[0]-tree.TrkPi1eta[0]),2) )
    
ofile.cd()
otree.Write()
