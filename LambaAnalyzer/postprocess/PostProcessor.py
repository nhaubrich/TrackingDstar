#########################
### Make Lambda Hists ###
#########################

print "Beginning makeLambdaHists"
import ROOT
import sys

# Manually give i/o files below
ifile = sys.argv[1]
ofile = ROOT.TFile("output2.root","RECREATE")
# Automatically give i/o files from arguments 
#ifile = sys.argv[1]
#ofile = ROOT.TFile(sys.argv[2],"RECREATE")

filechain = ifile.split(',')
tree = ROOT.TChain("analyzer/tree1")
for i in xrange(0,len(filechain)):
    if i >= len(filechain): break
    print "adding to chain: ",filechain[i]
    tree.Add(filechain[i])
otree = tree.CloneTree(0)

#nentries = tree.GetEntries()
nentries = 10000
print "Total entries to loop over: ",nentries

curr_event = -1 # keep track of current event to remove lambda duplicates
LambdaMasses = []
curr_event_entries = [] # entry numbers for the current event 
for iEntry in xrange(nentries):
    tree.GetEntry(iEntry)
    if (iEntry%10000 == 0): print "processing entry: ",iEntry
    #print tree.event_n,curr_event
    
    # Skip entries with more than one Lambda
    if len(tree.nUniqueSimTracksInSharedHit)>1: 
        continue

    if tree.event_n != curr_event:
        curr_event = tree.event_n
        #print LambdaMasses
        #print "curr_event_entries = ",curr_event_entries
        for writeEntry in curr_event_entries:
            tree.GetEntry(writeEntry)
            for lm in LambdaMasses:
                if lm[1] != writeEntry: continue
                tree.LambdaMass[lm[2]] = lm[0]
            #print "filling: ",tree.event_n,tree.LambdaMass[0],tree.LamdaVertexCAz[0]
            otree.Fill()
        LambdaMasses = []
        curr_event_entries = []
        tree.GetEntry(iEntry)
    for iLambda in xrange(len(tree.LambdaMass)):
        dz = abs(tree.LamdaVertexCAz[iLambda]-tree.PVz)
        foundCopy = False
        for j in xrange(len(LambdaMasses)):
            if (abs(tree.LambdaMass[iLambda] - LambdaMasses[j][0]) < 0.00001):
                # found a duplicate, take the one with closest dz to vertex
                foundCopy = True
                #print "found a duplicate, ",LambdaMasses[j][0]
                tree.GetEntry(LambdaMasses[j][1])
                #print dz,tree.LamdaVertexCAz[LambdaMasses[j][2]]
                if (dz < abs(tree.LamdaVertexCAz[LambdaMasses[j][2]]-tree.PVz)):
                    LambdaMasses[j] = (-999,LambdaMasses[j][1],LambdaMasses[j][2])
                    tree.GetEntry(iEntry)
                    LambdaMasses.append((tree.LambdaMass[iLambda],iEntry,iLambda))
                    #print "replacing: ",dz
                else:
                    #print "keep old one"
                    LambdaMasses.append((-999,iEntry,iLambda))
                tree.GetEntry(iEntry)
                break
        if not foundCopy:
            LambdaMasses.append((tree.LambdaMass[iLambda],iEntry,iLambda))
    curr_event_entries.append(iEntry)
    #otree.Fill()

ofile.cd()
otree.Write()
ofile.Close()
print "Completed makeLambdaHists\n"

####################################
### Add Branches For NN Training ###
####################################

print "Beginning addBranchesForNNTraining"
import ROOT
import sys
from math import pow,sqrt
import numpy as np

# Manually give i/o files below
ifile = ROOT.TFile("output2.root")
ofile = ROOT.TFile("output3.root","RECREATE")
# Automatically give i/o files from arguments 
#ifile = ROOT.TFile(sys.argv[1])  
#ofile = ROOT.TFile(sys.argv[2],"RECREATE")

tree = ifile.Get("tree1")
otree = tree.CloneTree(0)
nentries = tree.GetEntries()
print "nentries = ",nentries
gridSize = 20

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
GenDeltaR = np.zeros(1,dtype=float)
totalADCcount = np.zeros(1,dtype=float)

otree.Branch("trackPt",trackPt,"trackPt/D")
otree.Branch("trackEta",trackEta,"trackEta/D")
otree.Branch("trackPhi",trackPt,"trackPhi/D")
otree.Branch("nUniqueSimTracksInSharedHit",nUniqueSimTracksInSharedHit, "nUniqueSimTracksInSharedHit")
otree.Branch("sharedHitContainsGenLambda", sharedHitContainsGenLambda, "sharedHitContainsGenLambda")
otree.Branch("sharedHitContainsGenPion", sharedHitContainsGenPion, "sharedHitContainsGenPion")
otree.Branch("sharedHitContainsGenProton", sharedHitContainsGenProton, "sharedHitContainsGenProton")
otree.Branch("GenDeltaR",GenDeltaR,"GenDeltaR/D")
otree.Branch("totalADCcount",totalADCcount,"totalADCcount/D")

def getPixelHist(pixels,gridSize):
    xmin = -1
    xmax = -1
    ymin = -1
    ymax = -1
    xavg = 0.
    yavg = 0.
    global tot_adc
    tot_adc = 0.
    for x,y,adc in pixels:
        # print x,y,adc
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
    hist.SetDirectory(0)
    for x,y,adc in pixels:
        hist.Fill(x-xavg_int+gridSize/2.,y-yavg_int+gridSize/2.,adc)
    if hist.Integral() > 0:
        # Save the total charge
        totalADCcount[0] = hist.Integral()
        # Normalize the charge
        hist.Scale(1./hist.Integral())
        # Include the absolute charge
        #hist.Scale(1./10**4)
    else:
        hist = ROOT.TH2F("hist_shared","hist_shared",gridSize,0,gridSize,gridSize,0,gridSize)
    return hist

for iEntry in xrange(nentries):
    if (iEntry % 10000 == 0): 
        print "processing entry: ",iEntry
    tree.GetEntry(iEntry)
    pixels_shared = []
    pixels_pion = []
    pixels_proton = []
    
    ### Fill Pion Non-Shared Hits ###
    if ( len(tree.PionPixelHit_x)+len(tree.PionPixelHit_y) > 2 # eleminate single pixel events
        and tree.PionPixelHitLayer==0 and tree.LambdaMass[0]>0 and tree.flightLength[0]<4. 
        and tree.GenDeltaR.size()>0 and tree.GenDeltaR[0]<0.1 ):
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
            
    ### Fill Proton Non-Shared Hits ###
    if ( len(tree.ProtonPixelHit_x)+len(tree.ProtonPixelHit_y) > 2 # eleminate single pixel events
        and tree.ProtonPixelHitLayer==0 and tree.LambdaMass[0]>0 and tree.flightLength[0]<4.
        and tree.GenDeltaR.size()>0 and tree.GenDeltaR[0]<0.1 ):
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
            
    ### Fill Shared Hits ###
    if ( len(tree.LambdaSharedHitPixelHits_x)+len(tree.LambdaSharedHitPixelHits_y) > 2 # eleminate single pixel events
        and tree.LambdaSharedHitLayer[0]==0 and tree.LambdaMass[0]>0 and tree.flightLength[0]<4.
        and tree.GenDeltaR.size()>0 and tree.GenDeltaR[0]<0.1 ):
        for i in xrange(len(tree.LambdaSharedHitPixelHits_x)):
            pixels_shared.append((tree.LambdaSharedHitPixelHits_x[i], tree.LambdaSharedHitPixelHits_y[i], tree.LambdaSharedHitPixelHits_adc[i]))
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

ofile.cd()
otree.Write()
ofile.Close()
print "Completed addBranchesForNNTraining\n"

##############################
### Convert ROOT to Pandas ###
##############################

print "Beginning ConverRootToPandas"

from root_pandas import read_root
import sys

# Manually give i/o files below
ifile = "output3.root"
ofile = "output_final.h5"
# Automatically give i/o files from arguments 
#ifile = sys.argv[1]
#ofile = sys.argv[2]

#cols = ['isSharedHit','trackPt','trackEta','trackPhi','nUniqueSimTracksInSharedHit','sharedHitContainsGenPion','sharedHitContainsGenProton','sharedHitContainsGenLambda','GenDeltaR']
cols = ['isSharedHit','trackPt','trackEta','trackPhi','nUniqueSimTracksInSharedHit','GenDeltaR','totalADCcount','event_n']
for i in xrange(20*20):
    cols.append('pixel_%i' % i)
    
df = read_root(ifile, columns=cols)
#print df
df['nUniqueSimTracksInSharedHit'] = df['nUniqueSimTracksInSharedHit'].str[0]
df['GenDeltaR'] = df['GenDeltaR'].str[0]
df.to_hdf(ofile,key='df',mode='w',encoding='utf-8')

print "Completed ConverRootToPandas\n"

print "POSTPROCESS COMPLETE"
