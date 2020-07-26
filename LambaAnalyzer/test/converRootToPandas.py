from root_pandas import read_root
import sys

cols = ['isSharedHit','trackPt','trackEta','trackPhi','nUniqueSimTracksInSharedHit', 'uniqueSharedSimTrackPDGIds',  'uniquePionSimTrackPDGIds', 'uniqueProtonSimTrackPDGIds', 'GenDeltaR', 'GenProtonDeltaR', 'GenPionDeltaR', 'sharedHitContainsGenLambda', 'sharedHitContainsGenProton', 'sharedHitContainsGenPion' ]
for i in xrange(20*20):
    cols.append('pixel_%i' % i)
    
df = read_root(sys.argv[1], columns=cols)
print df
df['nUniqueSimTracksInSharedHit'] = df['nUniqueSimTracksInSharedHit'].str[0]

df['uniqueSharedSimTrackPDGIds'] = df['uniqueSharedSimTrackPDGIds'].str[0]
df['uniquePionSimTrackPDGIds'] = df['uniquePionSimTrackPDGIds'].str[0]
df['uniqueProtonSimTrackPDGIds'] = df['uniqueProtonSimTrackPDGIds'].str[0]

df['GenDeltaR'] = df['GenDeltaR'].str[0]
df['GenProtonDeltaR'] = df['GenProtonDeltaR'].str[0]
df['GenPionDeltaR'] = df['GenPionDeltaR'].str[0]

df['sharedHitContainsGenLambda'] = df['sharedHitContainsGenLambda'].str[0]
df['sharedHitContainsGenProton'] = df['sharedHitContainsGenProton'].str[0]
df['sharedHitContainsGenPion'] = df['sharedHitContainsGenPion'].str[0]

df.to_hdf("pixelTrain.h5",key='df',mode='w',encoding='utf-8')
print "done"
