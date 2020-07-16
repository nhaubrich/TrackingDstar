#!/bin/sh

#mv output*.root output1.root
python makeLambdaHists.py output1.root output2.root
python addBranchesForNNTraining.py output2.root output3.root
python converRootToPandas.py output3.root output_final20.h5
