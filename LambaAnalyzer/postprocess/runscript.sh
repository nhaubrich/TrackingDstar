#!/bin/sh

# renames your file (foo.root) to output1.root, be sure to edit the next line accordingly ! 
# if you don't want to rename your data file, just replace output1.root in line 6 by your file name
mv foo.root output1.root
python makeLambdaHists.py output1.root output2.root
python addBranchesForNNTraining.py output2.root output3.root
python converRootToPandas.py output3.root output_final.h5
