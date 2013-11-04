import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTHiggsOfflineAnalysis")

process.load("HLTriggerOffline.Higgs.HiggsValidation_cff")
process.load("DQMServices.Components.MEtoEDMConverter_cfi")


##############################################################################
##### Templates to change parameters in hltMuonValidator #####################
# process.hltMuonValidator.hltPathsToCheck = ["HLT_IsoMu3"]
# process.hltMuonValidator.genMuonCut = "abs(mother.pdgId) == 24"
# process.hltMuonValidator.recMuonCut = "isGlobalMuon && eta < 1.2"
##############################################################################

hltProcessName = "HLT"
process.hltHiggsValidator.hltProcessName = hltProcessName
#process.hltHiggsValidator.HWW.hltPathsToCheck = cms.vstring(
#		"HLT_Photon26",
		#		"HLT_Mu30_eta2p1_v",
		#		"HLT_IsoMu24_eta2p1_v",
		#"HLT_Ele27_WP80_v",
#		)

process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = cms.string(autoCond['startup'])

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
	#Ztautau
	'/store/relval/CMSSW_6_2_0/RelValZTT/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/5693CC6D-51EC-E211-B87E-003048D3740A.root',
	#Zee
#	'/store/relval/CMSSW_6_2_0/RelValZEE/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/50169250-40EC-E211-964F-02163E008C2D.root',
#	'/store/relval/CMSSW_6_2_0/RelValZEE/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/D656BB27-4CEC-E211-807F-003048D4988C.root',
	#Zmumu
#	'/store/relval/CMSSW_6_2_0/RelValZMM/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/42161FB1-4CEC-E211-BA55-003048F236DC.root',
#	'/store/relval/CMSSW_6_2_0/RelValZMM/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/F4EB068C-6BEC-E211-A604-001D09F24E20.root',
	#H130Gluonfusion
#	'/store/relval/CMSSW_6_2_0/RelValH130GGgluonfusion/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/2CCFE179-46EC-E211-8398-0025901D5E00.root',
#	'/store/relval/CMSSW_6_2_0/RelValH130GGgluonfusion/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/2E1B16F6-4CEC-E211-ABE9-003048F1C420.root',
	#TTbarLepton
#	'/store/relval/CMSSW_6_2_0/RelValTTbarLepton/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/44754556-57EC-E211-A3EE-003048FEB9EE.root',
#	'/store/relval/CMSSW_6_2_0/RelValTTbarLepton/GEN-SIM-RECO/PRE_ST62_V8-v3/00000/704D9C8F-4FEC-E211-9FFE-001E67396E9B.root'
    ),
    secondaryFileNames = cms.untracked.vstring(
	    #'file:/afs/cern.ch/user/d/duarte/scratch0/H130GGgluonfusion_cfi_py_GEN_SIM_DIGI_L1_DIGI2RAW_HLT.root',
#           '/store/relval/CMSSW_5_2_0/RelValZMM/GEN-SIM-DIGI-RAW-HLTDEBUG/START52_V4A-v1/0248/C493C02B-1569-E111-B86D-0030486792AC.root', 
#           '/store/relval/CMSSW_5_2_0/RelValZMM/GEN-SIM-DIGI-RAW-HLTDEBUG/START52_V4A-v1/0000/E61E0884-0469-E111-8AF4-001BFCDBD160.root', 
#           '/store/relval/CMSSW_5_2_0/RelValZMM/GEN-SIM-DIGI-RAW-HLTDEBUG/START52_V4A-v1/0000/56FCB9F3-0869-E111-85DA-001A928116EA.root', 
#           '/store/relval/CMSSW_5_2_0/RelValZMM/GEN-SIM-DIGI-RAW-HLTDEBUG/START52_V4A-v1/0000/4E7E8D82-0469-E111-8839-002354EF3BCE.root', 
#           '/store/relval/CMSSW_5_2_0/RelValZMM/GEN-SIM-DIGI-RAW-HLTDEBUG/START52_V4A-v1/0000/083CE9EB-0869-E111-979C-001A928116C4.root',
    )
)

process.DQMStore = cms.Service("DQMStore")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 2000
process.MessageLogger.destinations += ['HiggsValidationMessages']
process.MessageLogger.categories   += ['HiggsValidation']
process.MessageLogger.debugModules += ['*']#HLTHiggsValidator','HLTHiggsSubAnalysis','HLTHiggsPlotter']
process.MessageLogger.HiggsValidationMessages = cms.untracked.PSet(
    threshold       = cms.untracked.string('DEBUG'),
    default         = cms.untracked.PSet(limit = cms.untracked.int32(0)),
    HiggsValidation = cms.untracked.PSet(limit = cms.untracked.int32(1000))
    )

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring(
        'drop *', 
        'keep *_MEtoEDMConverter_*_HLTHiggsOfflineAnalysis'),
    fileName = cms.untracked.string('hltHiggsValidator.root')
)


process.analyzerpath = cms.Path(
    process.hltHiggsValidator *
    process.MEtoEDMConverter
)


process.outpath = cms.EndPath(process.out)
