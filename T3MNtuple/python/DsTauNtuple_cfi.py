import FWCore.ParameterSet.Config as cms


T3MTree = cms.EDAnalyzer('T3MNtuple',
                         mid = cms.int32(15),
                         MuonPtCut = cms.double(2.0),
                         MuonEtaCut = cms.double(2.4),
                         TrackPtCut = cms.double(1.0),
                         TrackEtaCut = cms.double(2.4),
                         passhlt = cms.bool(True),
                         wideSB = cms.bool(False),
                         do2mu = cms.bool(True), # do 2 mu category or not
                         doTracks = cms.bool(True), # do fillTracks
                         doMuons = cms.bool(True), # do fillMuons
                         do3mutuple = cms.bool(True),
                         doMC = cms.bool(False),  # Set in other instance, here only for local tests
                         doFullMC = cms.bool(False),   # Set in other instance, here only for local tests
                         doL1 = cms.bool(True),
                         doBJets = cms.bool(True),
                         doPhotons = cms.bool(True),
                         doThreeMuons = cms.bool(True),
                         doTwoMuonsAndTrack = cms.bool(True),
                         miniAODRun = cms.bool(False),
                         TriggerMuonMatchingdr = cms.untracked.double(0.3),
                         WhatData = cms.untracked.string("2018"),
                         DataMCType    = cms.untracked.string("ds_tau"), #Defaut: data. Have a look at src/DataMCType.cc for available types   Set in other instance, here only for local tests
                         muons = cms.InputTag("muons"),
                         pvs = cms.InputTag("offlinePrimaryVertices"),
                         svs = cms.InputTag("inclusiveSecondaryVertices"),
                         trks = cms.InputTag("generalTracks"),
                         phos = cms.InputTag("photons"),
                         btagsCvsB = cms.InputTag("pfCombinedCvsBJetTags"),
                         btagsMVA = cms.InputTag("pfCombinedMVAV2BJetTags"),
                         btagsCSV = cms.InputTag("pfCombinedSecondaryVertexV2BJetTags"),
                         btagDeepCSV = cms.InputTag("pfDeepCSVJetTags"),  # absent in AOD
                         pfcands = cms.InputTag("particleFlow"),
                         triggerBitsH = cms.InputTag("TriggerResults", "", "HLT"),
                         triggerSummary = cms.InputTag("hltTriggerSummaryAOD", "", "HLT"),
                         beamSpotHandle = cms.InputTag("offlineBeamSpot"),
                         pileupSummary = cms.InputTag("addPileupInfo"),
                         genParticles = cms.InputTag("genParticles"),
                         AlgInputTag = cms.InputTag( "gtStage2Digis" ),
                         BadGlbMuonFilter = cms.InputTag("badGlobalMuonTagger"),
                         )
