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
                 '/tmp/bjoshi/DsT3MNtuple.root', #default value
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
                                   fileName = cms.string("DsT3MNtuple_run2017F.root")
                                   )




#DsPhiPiList = []
#for n in range(1,301):
#    DsPhiPiList.append('root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_'+str(n)+'.root')

#process.source.fileNames =DsPhiPiList


#process.source.fileNames = ['file:TSG-RunIIAutumn18DR-00006_84.root']




#process.source.fileNames =['/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/AC2BE19D-A946-E911-B9D1-0CC47A5FBE31.root',
#                           '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/B854E6AE-A946-E911-A2BE-0CC47A5FA3B9.root',
#                           '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/92305211-F247-E911-9D1D-0CC47A5FC679.root',
#                           '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/16D24F4C-1C47-E911-BE09-549F351EDC46.root',
#                           '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/8446C82F-6B46-E911-B9A7-0242AC1C0502.root']
 
#process.source.fileNames =['/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/AC2BE19D-A946-E911-B9D1-0CC47A5FBE31.root']
    




#process.source.fileNames = ['/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/AC2BE19D-A946-E911-B9D1-0CC47A5FBE31.root']
#process.source.fileNames = ['/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/AC2BE19D-A946-E911-B9D1-0CC47A5FBE31.root']
 #                           '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/B854E6AE-A946-E911-A2BE-0CC47A5FA3B9.root',
  #                          '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/92305211-F247-E911-9D1D-0CC47A5FC679.root',
   #                         '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/270000/16D24F4C-1C47-E911-BE09-549F351EDC46.root',
    #                        '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/8446C82F-6B46-E911-B9A7-0242AC1C0502.root']
     #                       '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/06DEA852-A346-E911-B18F-0242AC1C0502.root',
      #                      '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/C6C25394-9E46-E911-B8A3-0242AC1C0500.root',
       #                     '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/8AAD234B-A046-E911-9879-0242AC1C0501.root',
        #                    '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/BEF998F1-AE46-E911-AE47-0242AC1C0500.root',
         #                   '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/0AA0AF89-9F46-E911-9AC5-0242AC1C0501.root',
          #                  '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/C6527937-A546-E911-9F0B-0242AC1C0501.root',
           #                 '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/5AE65B11-BC46-E911-B568-0242AC1C0505.root',
            #                '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/2E3DDB4B-B446-E911-B98A-0242AC1C0501.root',
             #               '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/7ACE84F3-BC46-E911-AE4D-0242AC1C0501.root',
              #              '/store/mc/RunIIFall17DRPremix/BdToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/2710000/2AA39780-CA46-E911-8E8D-0242AC1C0500.root']




#process.source.fileNames = ['root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_1.root',
 #                           'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_2.root',
  #                          'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_3.root',
   #                         'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_4.root',
#    #                        'root://cms-xrd-global.cern.ch//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_5.root']
#process.source.fileNames = ['file:/afs/cern.ch/work/c/cherepan/InputTest/custom_DsTau3Mu_13TeV_RECO_crab350_9078.root']





#process.source.fileNames = ['root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_520.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_942.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_763.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_122.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_405.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_332.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_986.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_635.root',
#                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_27.root',
##                            'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_988.root',
 #                           'root://xrootd-cms.infn.it//store/user/fsimone/DsPhiPi/crab_crab_DsPhiPi_13TeV_RECO/190225_141742/0000/DsPhiPi_13TeV_RECO_219.root']



#process.source.fileNames = ['/store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/110000/08CD692D-A747-E911-AED9-3CFDFE6366E0.root',
#                            '/store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/110000/4604F75D-A647-E911-863A-1866DA85D9A3.root',
#                            '/store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/3E402898-4E46-E911-8D40-1866DA85DACC.root',
#                            '/store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/BC22FAED-ED46-E911-A0BB-F01FAFDC49B6.root',
#                            '/store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/5C95DF7B-2048-E911-B14C-008CFA0A5844.root']



#process.source.fileNames = ['file:MinimumBias_RECO_543.root']  # data file for debugs and test runs
#process.source.fileNames = ['/store/data/Run2017F/DoubleMuonLowMass/AOD/17Nov2017-v1/10000/EEC5ACC2-B802-E811-AF14-0CC47A0AD498.root']  # data file for debugs and test runs
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

process.source.fileNames = ['root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/110000/08CD692D-A747-E911-AED9-3CFDFE6366E0.root']
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/110000/4604F75D-A647-E911-863A-1866DA85D9A3.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/3E402898-4E46-E911-8D40-1866DA85DACC.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/BC22FAED-ED46-E911-A0BB-F01FAFDC49B6.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/108D0360-3147-E911-9C47-B083FECFF52D.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/7464B23D-A547-E911-B749-C0BFC0E5685E.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/1A44C63F-A547-E911-B78D-90E2BACBAB00.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/5C95DF7B-2048-E911-B14C-008CFA0A5844.root',
#'root://cmsio5.rc.ufl.edu//store/mc/RunIIFall17DRPremix/DsToTau_To3Mu_MuFilter_TuneCUEP8M1_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/70000/0E182ABE-8148-E911-AE5F-A0369FF882E0.root']

process.load("DsTau23Mu.T3MNtuple.DsTauNtuple_cfi")
process.tagger = cms.Path(process.badGlobalMuonTagger)
process.DsTauNtuple = cms.Sequence(process.T3MTree)
process.T3MTree.doMC = cms.bool(False)
process.p = cms.Path(process.DsTauNtuple)
process.schedule = cms.Schedule(process.tagger, process.p)
