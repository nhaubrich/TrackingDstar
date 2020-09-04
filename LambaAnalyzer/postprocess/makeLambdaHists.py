print "Beginning makeLambdaHists"
import ROOT
import sys

# Manually give i/o files below
#ifile = '/uscms_data/d3/bbonham/TrackerProject/Input_for_Postprocess/output1.root'
ofile = ROOT.TFile("output2.root","RECREATE")

# OR Automatically give i/o files from arguments 
ifile = sys.argv[1]
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