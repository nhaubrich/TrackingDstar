# How to run the postprocessor

There are two ways to execute the postprocessor. 

## Method 1

### Run as a Jupyter Notebook

Change the path of the input file in the first cell and run all cells:
   
   ifile = 'path'

## Method 2

### Run as a .py script. 

Convert from .ipynb to .py, then run the script:

   $ jupyter nbconvert --to script PostProcessor.ipynb
   $ python PostProcessor.py