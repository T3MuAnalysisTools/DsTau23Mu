import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

import subprocess
import sys

options = VarParsing.VarParsing()
options.register('globalTag',
                 '92X_dataRun2_Prompt_v5', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Global Tag")

options.register('nEvents',
                 -1, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Maximum number of processed events")

options.register('eosInputFolder',
                 '/store/relval/CMSSW_8_0_20/RelValZMM_13/GEN-SIM-RECO/PU25ns_80X_mcRun2_asymptotic_2016_TrancheIV_v4_Tr4GT_v4-v1/00000', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "EOS folder with input files")

options.register('ntupleName',
                 './DsT3MNtuple.root', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Folder and name ame for output ntuple")

options.register('runOnMC',
                 True, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.bool,
                 "Run on DATA or MC")

options.register('hltPathFilter',
                 "all", #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Filter on paths (now only accepts all or IsoMu20)")

if options.hltPathFilter == "all" :
    pathCut   = "all"
    filterCut = "all"
elif options.hltPathFilter == "IsoMu20" :
    pathCut   = "HLT_IsoMu20_v"
    if options.runOnMC1 :
        filterCut = "hltL3crIsoL1sMu16L1f0L2f10QL3f20QL3trkIsoFiltered0p09"
    else :
        filterCut = "hltL3crIsoL1sMu18L1f0L2f10QL3f20QL3trkIsoFiltered0p09"
        
else :
    print "[" + sys.argv[0] + "]:", "hltPathFilter=", options.hltPathFilter, "is not a valid parameter!"
    sys.exit(100)


process = cms.Process("DsTauNtuple")



process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(500)

process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load('Configuration.Geometry.GeometrySimDB_cff')

process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag.globaltag = cms.string(options.globalTag)

process.load('RecoMET.METFilters.badGlobalMuonTaggersAOD_cff')
#switch on tagging mode:
process.badGlobalMuonTagger.taggingMode = cms.bool(True)
process.cloneGlobalMuonTagger.taggingMode = cms.bool(True)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.nEvents)  )
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(),
        secondaryFileNames = cms.untracked.vstring()
)


process.TFileService = cms.Service('TFileService',
                                   fileName = cms.string("DsT3MNtuple.root")
                                   )

#process.source.fileNames = ['root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_1.root',
 #                           'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_2.root',
  #                          'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_3.root',
   #                         'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_4.root',
#    #                        'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_5.root']
#process.source.fileNames = ['file:/afs/cern.ch/work/c/cherepan/InputTest/custom_DsTau3Mu_13TeV_RECO_crab350_9078.root']




#process.source.fileNames = ['file:MinimumBias_RECO_543.root']  # data file for debugs and test runs
process.source.fileNames = ['/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/EEC5ACC2-B802-E811-AF14-0CC47A0AD498.root']  # data file for debugs and test runs
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/825154B4-80FF-E711-ABFF-0025904CDDEC.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/F4E4B2B4-80FF-E711-805C-0025904C5DE0.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/EA5B3CB4-80FF-E711-8B32-0CC47AF9B32A.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/D8CD1229-80FF-E711-A13A-0025905C2CB8.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/C65FB9BB-80FF-E711-9508-0025905D1CB6.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/62A731B5-80FF-E711-B8D2-0025905C42A8.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/E25DD5BC-80FF-E711-A48C-0CC47AF9B2D2.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/FC51B9BB-80FF-E711-98EE-0025905D1CB6.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/D82226BB-80FF-E711-A033-0025904CDDEC.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/9EC743BA-80FF-E711-825B-0025904CDDEC.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/9C269DB8-80FF-E711-B9C9-0025904CDDEC.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/0EB224BB-80FF-E711-9762-0025904C540E.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/EC8D14B7-80FF-E711-9EE1-0025904C540E.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/84D46EB5-80FF-E711-BDAF-0025904C5DE0.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/B4D000BB-80FF-E711-B8F5-0025904C540E.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/C8201EB7-80FF-E711-B586-0025905BA736.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/7C2ED4B7-80FF-E711-952C-0025905D1D52.root',
#                            '/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/20000/10C7841D-2C02-E811-9DD1-F01FAFE5FB02.root']




process.load("DsTau23Mu.T3MNtuple.DsTauNtuple_cfi")
process.tagger = cms.Path(process.badGlobalMuonTagger)
process.DsTauNtuple = cms.Sequence(process.T3MTree)
process.p = cms.Path(process.DsTauNtuple)
process.schedule = cms.Schedule(process.tagger, process.p)
