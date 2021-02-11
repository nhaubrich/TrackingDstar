from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'crab1000' #just a name of the job
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True

config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'crab_MC1000.py'

config.Data.outputPrimaryDataset = 'MinBias'
config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 100
NJOBS = 10  # This is not a configuration parameter, but an auxiliary variable that we use in the next line.
config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.publication = True
config.Data.outputDatasetTag = 'CRAB3_tutorial_May2015_MC_generation'

config.Site.storageSite = 'T3_US_FNALLPC'
