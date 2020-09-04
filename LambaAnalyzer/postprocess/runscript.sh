#!/bin/sh

### This script runs the full postprocess ###

# Note: The argument of makeLambdaHists.py
#       is the input file for the postprocessor.
#       Change its path as necessary.

python makeLambdaHists.py '/uscms_data/d3/bbonham/TrackerProject/Input_for_Postprocess/output1.root'
python addBranchesForNNTraining.py
python converRootToPandas.py