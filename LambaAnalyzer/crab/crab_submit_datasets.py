#!/usr/bin/env python

# This script loops over the list of data sets that you provide to submit crab jobs
# don't forget to activate your certificate

import sys
import os

# copying the MC list from the directory where it was produced
os.system("cp /uscms_data/d3/hichemb/princeton/project1/CMSSW_10_4_0/src/MClist_CMSSW_10_0_untaped.txt . ")

# copying the template crab config
os.system("cp crab_jobs.py crab_jobs_temp.py")

with open("MClist_CMSSW_10_0_untaped.txt") as file:
	i = 0 # data set order counter. e.g: i = 10 refers to the data set in the 10th line of MClist
	for line in file:
		#print line
		# resetting the temp config file for crab job submission
		os.system("cp crab_jobs.py crab_jobs_temp.py")
		#print "done 1 "
		f = open("crab_jobs_temp.py", "r")
		i += 1
		# specifying teh range of data set jobs to submit
		#		if i == 650 :
		#			break
		if i <650 :
			continue;
		print i

		#if i != 6 :
		#	continue
		#print "done 2"
		st = line[:-1] # get rid of \n at the end of the line
		c = str(i)
		p = st.replace("/", "__")
		s = p[0:92]+ "___" + c # replacing name since name is limited to 100 characters and adding the i counter to easily recognize the data set, rather than ekeping track of exact name
		bufer = f.read()
		#print bufer
		a = bufer.find("DATASETX_NAME")
		b = bufer.find("DATASETX_BANE")
		#t = str(a) + " , " + str(b)
		#print t
		if a != -1 :
			bufer = bufer.replace("DATASETX_NAME", s)
		if b != -1 :
			bufer = bufer.replace("DATASETX_BANE", st)
		#print bufer
		f.close()
		# replacing the temp file config with the current line data set then submitting job
		f = open("crab_jobs_temp.py", "w+")
		f.write(bufer)
		#print bufer
		f.close()
		os.system("crab submit crab_jobs_temp.py")
		print "Job submission done"
