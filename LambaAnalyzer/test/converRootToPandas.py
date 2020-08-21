from root_pandas import read_root
import sys

#cols = ['isSharedHit','trackPt','trackEta','trackPhi','nUniqueSimTracksInSharedHit','sharedHitContainsGenPion','sharedHitContainsGenProton','sharedHitContainsGenLambda','GenDeltaR']
cols = ['isSharedHit','trackPt','trackEta','trackPhi','nUniqueSimTracksInSharedHit','GenDeltaR']
for i in xrange(20*20):
    cols.append('pixel_%i' % i)
    
df = read_root(sys.argv[1], columns=cols)
print df
df['nUniqueSimTracksInSharedHit'] = df['nUniqueSimTracksInSharedHit'].str[0]
df['GenDeltaR'] = df['GenDeltaR'].str[0]
df.to_hdf("pixelTrain.h5",key='df',mode='w',encoding='utf-8')
print "done"
