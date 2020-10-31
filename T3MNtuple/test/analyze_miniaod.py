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
                 100, #default value

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

#process.load('RecoMET.METFilters.badGlobalMuonTaggersAOD_cff')
#switch on tagging mode:
#process.badGlobalMuonTagger.taggingMode = cms.bool(True)
#process.cloneGlobalMuonTagger.taggingMode = cms.bool(True)

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

#process.source.fileNames = ['/store/data/Run2018B/DoubleMuonLowMass/AOD/17Sep2018-v1/60000/02CEEEBB-DCCB-9B46-84B2-7C99FB39C98A.root']
#process.source.fileNames = ['root://cms-xrd-global.cern.ch//store/data/Run2017D/DoubleMuonLowMass/AOD/17Nov2017-v1/70000/5A3F7AD8-36E9-E711-B981-1866DA85D72E.root']
#process.source.fileNames = ['/store/user/wangjian/DsToTau_TauTo3Mu_March2020/RunIIAutumn18DRPremix-102X/200323_083820/0000/BPH-RunIIAutumn18DRPremix-00158_323.root']
process.source.fileNames = ['file:/tmp/bjoshi/D7A24A95-28B8-824A-A9B2-1E643CA29B5F.root']

process.load("DsTau23Mu.T3MNtuple.DsTauNtuple_cfi")
process.load("DsTau23Mu.T3MNtuple.PhotonAndV0Skim_cfi")
process.load("DsTau23Mu.T3MNtuple.TrackCollectionProducer_cfi")

#process.tagger = cms.Path(process.badGlobalMuonTagger)
process.DsTauNtuple = cms.Sequence(process.T3MTree)
#process.DsTauNtuple = cms.Sequence(process.T3MTree)
process.p = cms.Path(process.TrackCollection * process.DsTauNtuple)
process.schedule = cms.Schedule(process.p)

process.T3MTree.muons = cms.InputTag("slimmedMuons")
process.T3MTree.pvs = cms.InputTag("offlineSlimmedPrimaryVertices")
process.T3MTree.svs = cms.InputTag("slimmedSecondaryVertices")
process.T3MTree.trks = cms.InputTag("TrackCollection:pfTracks:DsTauNtuple")
process.T3MTree.phos = cms.InputTag("slimmedPhotons")
process.T3MTree.btagsCvsB = cms.InputTag("none")
process.T3MTree.btagsMVA = cms.InputTag("none")
process.T3MTree.btagsCSV = cms.InputTag("none")
process.T3MTree.btagDeepCSV = cms.InputTag("none")  # absent in AOD
process.T3MTree.pfcands = cms.InputTag("packedPFCandidates")
process.T3MTree.triggerBitsH = cms.InputTag("TriggerResults", "", "HLT")
process.T3MTree.triggerSummary = cms.InputTag("none")
process.T3MTree.triggerObjects = cms.InputTag("slimmedPatTrigger")
process.T3MTree.beamSpotHandle = cms.InputTag("offlineBeamSpot")
process.T3MTree.pileupSummary = cms.InputTag("addPileupInfo")
process.T3MTree.genParticles = cms.InputTag("genParticles")
process.T3MTree.AlgInputTag = cms.InputTag( "gtStage2Digis" )
process.T3MTree.miniAODRun = cms.bool(True)
#process.T3MTree.BadGlbMuonFilter = cms.InputTag("none")
#process.output = cms.OutputModule("PoolOutputModule",
#                                   outputCommands = process.PhotonAndV0Skim_EventContent,
#                                   fileName = cms.string("DsT3MNtuple.root")
#                                   ) 
#process.this_is_the_end = cms.EndPath(process.output)
