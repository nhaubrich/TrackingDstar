import ROOT
import sys

#ifile = ROOT.TFile(sys.argv[1])
ofile = ROOT.TFile("ofile.root","RECREATE")
#tree = ifile.Get("analyzer/tree1")

filechain = sys.argv[1].split(',')
tree = ROOT.TChain("analyzer/tree1")
for i in xrange(0,len(filechain)):
    if i >= len(filechain): break
    print "adding to chain: ",filechain[i]
    tree.Add(filechain[i])
   

hmass = ROOT.TH1F("hmass","hmass",40,1.07,1.17)
#hmass = ROOT.TH1F("hmass","hmass",40,0.3,0.7)
print "drawing mass"
#tree.Draw("LambdaMass>>hmass","PVOrder==1&&LambdaVtxLSig3D>10&&cosAlpha>0.99998&&LambdaVtxProb>0.05&&TrkPi1chi2<3.0&&TrkProtonchi2<3.0","ep")
########tree.Draw("K0Mass>>hmass","K0VtxLSig3D>10&&cosAlpha>0.99998&&K0VtxProb>0.05&&TrkPi1chi2<3.0&&TrkPi2chi2<3.0","ep")
print "done drawing mass"
hlt = ROOT.TH1F("hlt","hlt",400,0,20)
print "drawing flight length"
tree.Draw("flightLength>>hlt","(PVOrder==1||PVOrder==2)&&LambdaMass>1.11&&LambdaMass<1.13&&LambdaVtxLSig3D>10&&cosAlpha>0.99998&&LambdaVtxProb>0.05&&TrkPi1chi2<3.0&&TrkProtonchi2<3.0","ep")
#tree.Draw("flightLength>>hlt","abs((PVx*LambdaVtxPosx+PVy*LambdaVtxPosy)/((TMath::Sqrt(pow(PVx,2)+pow(PVy,2)))*(TMath::Sqrt(pow(LambdaVtxPosx,2)+pow(LambdaVtxPosy,2)))))<0.71&&(PVOrder==1||PVOrder==2)&&LambdaMass>1.11&&LambdaMass<1.13&&LambdaVtxLSig3D>10&&cosAlpha>0.99998&&LambdaVtxProb>0.05&&TrkPi1chi2<3.0&&TrkProtonchi2<3.0","ep")
#tree.Draw("flightLength>>hlt","K0Mass>0.48&&K0Mass<0.52&&K0VtxLSig3D>10&&cosAlpha>0.9998&&K0VtxProb>0.05&&TrkPi1chi2<3.0&&TrkPi2chi2<3.0","ep")
print "done drawing flight length"
ofile.cd()
hmass.Write()
hlt.Write()

