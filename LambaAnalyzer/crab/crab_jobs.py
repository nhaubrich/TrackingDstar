from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()
#############################################################
config.General.requestName = 'DATASETX_NAME' #just a name
##############################################################
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'trkeffanalyzer_MC_GeneralTracks_cfg.py'
###################################################
config.Data.inputDataset = 'DATASETX_BANE' #data set
###################################################
config.Data.useParent = True
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
#config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB()) # online community said this line needs to be removed
config.Data.publication = False
config.Site.storageSite = 'T3_US_FNALLPC' #output dir 

