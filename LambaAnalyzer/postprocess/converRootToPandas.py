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
cols = ['isSharedHit','trackPt','trackEta','trackPhi','nUniqueSimTracksInSharedHit','GenDeltaR','totalADCcount']
for i in xrange(20*20):
    cols.append('pixel_%i' % i)
    
df = read_root(ifile, columns=cols)
#print df
df['nUniqueSimTracksInSharedHit'] = df['nUniqueSimTracksInSharedHit'].str[0]
df['GenDeltaR'] = df['GenDeltaR'].str[0]
df.to_hdf(ofile,key='df',mode='w',encoding='utf-8')

print "Completed ConverRootToPandas\n"

print "POSTPROCESS COMPLETE"