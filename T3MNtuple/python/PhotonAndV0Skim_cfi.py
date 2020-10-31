import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Reconstruction_cff import *

# make photon candidate conversions for P-wave studies
from HeavyFlavorAnalysis.Onia2MuMu.OniaPhotonConversionProducer_cfi import PhotonCandidates as oniaPhotonCandidates

# add v0 with tracks embed
from HeavyFlavorAnalysis.Onia2MuMu.OniaAddV0TracksProducer_cfi import *

# Pick branches you want to keep
PhotonAndV0Skim_EventContent = cms.PSet(
     outputCommands = cms.untracked.vstring(
                     'drop *',
                     'keep *_oniaPhotonCandidates_*_*',
                     'keep *_oniaV0Tracks_*_*',
     )
)

PhotonAndV0SkimSequence = cms.Sequence(
            oniaPhotonCandidates *
            oniaV0Tracks )
