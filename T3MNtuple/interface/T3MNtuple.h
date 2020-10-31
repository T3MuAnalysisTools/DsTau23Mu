// -*- C++ -*-
//
// Package:    DsTau23Mu/T3MNtuple
// Class:      T3MNtuple
//
/**\class T3MNtuple T3MNtuple.cc DsTau23Mu/T3MNtuple/plugins/T3MNtuple.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Vladimir Cherepanov
//         Created:  Thu, 17 Jan 2019 15:04:27 GMT
//
//
//
#ifndef T3MNtuple_h
#define T3MNtuple_h


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonTime.h"
#include "DataFormats/MuonReco/interface/CaloMuon.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "RecoMuon/MuonIdentification/interface/MuonCaloCompatibility.h"
#include "DataFormats/MuonReco/interface/MuonShower.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetMatchInfo.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"
#include "CondFormats/DataRecord/interface/L1TUtmTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1TUtmTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1TGlobalPrescalesVetosRcd.h"
#include "CondFormats/L1TObjects/interface/L1TGlobalPrescalesVetos.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"
#include "TVector3.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "DsTau23Mu/T3MNtuple/interface/PDGInfo.h"
#include "DsTau23Mu/T3MNtuple/interface/DataMCType.h"
#include "TH2.h"
#include <TTree.h>

using namespace reco;
using namespace edm;
using namespace std;
using namespace l1t;
using namespace muon;
using namespace pat;
//
// class declaration
//

class T3MNtuple : public edm::EDAnalyzer {
public:
  explicit T3MNtuple(const edm::ParameterSet&);
  ~T3MNtuple();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;


  // ----------member data ---------------------------

  void ClearEvent();
  void fillEventInfo(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillTracks(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillMuons(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillMCTruth(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillTrigger(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillDsTree(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillBTagJets(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillPhotons(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  int  fillThreeMuons(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  int  fillTwoMuonsAndTracks(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void fillVertices(const edm::Event& iEvent, const edm::EventSetup& iSetup);

  std::vector<std::vector<unsigned int> > findThreeMuonsCandidates(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  std::vector<std::vector<unsigned int> > findTwoMuonsAndTrackCandidates(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  template<class T>
  void TriggerMatch(edm::Handle<trigger::TriggerEvent> &triggerSummary,  T obj, double drmax, float &match);
  template<class T>
  void TriggerMatch(edm::Handle<vector<TriggerObjectStandAlone>> &triggerObjects,  const TriggerNames& triggerNames, T obj, double drmax, float &match);
 
  bool isGoodTrack(const Track &track);
  bool AcceptedMuon(reco::MuonRef RefMuon);
  bool getTrackMatch(edm::Handle<std::vector<reco::Track> > &trackCollection, reco::TrackRef &refTrack, int &match);
  bool isGoodGenParticle(const reco::GenParticle &GenPar);
  bool SkipThisParticle(const reco::GenParticle &GenPar);
  std::vector<const reco::GenParticle* > TauDecayProducts(const reco::GenParticle *Tau);
  std::vector<int> SortByPt(std::vector<TLorentzVector> invec);


  double /*filterbadGlbMuon*/ gen_flavor, nmu_mom, hlt_doublemu4_lmnrt, hlt_doublemu3_tau3mu, l1_triplemu0, l1_doublemu0,
    prescale_triplemu0, prescale_doublemu_10_0, prescale_doublemu0_eta1p6,
    prescale_triplemu500, prescale_doublemu_11_4, prescale_doublemu0_eta1p6_os, prescale_doublemu0_eta1p4_os,
    l1_doublemu0_eta1p4_os, l1_doublemu0_eta1p6_os, l1_doublemu0_eta1p6, l1_doublemu_10_0, l1_doublemu_11_4, l1_triplemu500,
    p_reco[3], pt_reco[3], pt_max, pt_min, trigmat_reco[3], trigmat_new,
    eta_reco[3], pdgid_reco[3], momid_reco[3], vxy_reco[3],
    phi_reco[3], pt2mu_12, charge_reco[3], p_glb[3], eta_glb[3], phi_glb[3],
    p_in[3], eta_in[3], phi_in[3], p_out[3], eta_out[3], phi_out[3],  
    pt3mu_reco, m3mu_simp, m3mu_reco, p3mu_reco, eta3mu_reco, glbnC_reco[3],
    m3mu_refit, comp2d_reco[3], calocomp_reco[3], segmcomp_reco[3], trkhp_reco[3],
    segmcomp_0[3], segmcomp_1[3], segmcomp_2[3], segmcomp_3[3],
  //segmcomp_4[3], segmcomp_5[3], segmcomp_6[3], segmcomp_7[3],
    tLWM_min, pLWM_min, d0_reco[3], d0sig_reco[3], dr12_reco, dr23_reco, dr31_reco,
    dr_min, drtau_max, dz_reco[3], dzpv_reco[3],
    drtau_reco[3], dca12_reco, dca23_reco, dca31_reco, dca_max,
    fv_tC, fv_nC, fv_Prob, fv_dxy, fv_dxysig, fv_ppdl, fv_cosdphi,
    fvwo_tC[3], fvwo_nC[3], fv_d3D, fv_d3Dsig, fv_ppdl3D, fv_cosdphi3D,
    sv_d3D[30], sv_d3Dsig[30], sv_ppdl3D[30], sv_cosdphi3D[30],
    sv_mass[30], sv_pt[30], sv_nmu[30], sv_ntrk[30], sv_dz[30], sv_overlap[30],
    trkrel_tau, trkrel_tau05, ntrk_tau, ntrk_tau05, ntrk_tau_b,  mindca_iso, mindca_iso05,
    trkrel_reco[3], trkrel_max, ntrk_reco[3], ntrk_sum,
    Iso_sumPt[3], Iso_nTr[3], Iso_emEt[3],
    Iso_hadEt[3], Iso_eVE[3], Iso_hVE[3], nOMS_min, nOMS_reco[3], iTnC_reco[3],
    cschits_sta1[3], cschits_sta2[3], cscdxdz_sta1[3], cscdxdz_sta2[3],
    cscchi2_sta1[3], cscchi2_sta2[3], 
    cscdydz_sta1[3], cscdydz_sta2[3], cscnsegm_sta1[3], cscnsegm_sta2[3],
    uSta_reco[3], tKink_reco[3], gKink_reco[3], tRC2_reco[3], sRC2_reco[3], cLP_reco[3],
    cLM_reco[3], lDist_reco[3], gDEP_reco[3], tMat_reco[3], gTP_reco[3],
    ddz_12, m2mu_ss, m2mu_os1, m2mu_os2, m2mu_12, m2mu_23, m2mu_31, m2mu_min, m2mu_max,
    puN, n_vtx, eta_min, eta_max, 
    pf_reco[3], rpcmu_reco[3], rpchits_reco[3], nOVMH_reco[3], pterr_reco[3],
    pv_nmu, pv1_tC, pv1_nC, pv2_tC, pv2_nC, ntrk0p1, ntrk0p2, ntrk0p5, maxdxy_pv0, calomuon_3,
    jet_pt[30], jet_overlap[30], btagcvsb[30], btagcsv[30], btagmva[30],
    nOVPH_reco[3], iTvF_reco[3], pLWM_reco[3], tLWM_reco[3], ggm_reco[3],  tgm_reco[3],
    nOVTH_reco[3], nOLTH_reco[3], nOLTHin_reco[3], nOLTHout_reco[3],
    calem_reco[3], calemS9_reco[3], calemS25_reco[3], calhad_reco[3], calhadS9_reco[3],
    tma_reco[3], tmost_reco[3], tmosat_reco[3], tmlst_reco[3], 
    tmlsat_reco[3], tmlsolpt_reco[3], tmlsoblpt_reco[3],
    qprod_reco[3], vmuonhitcomb_reco[3], outerchi2_reco[3], timeatipinouterr_reco[3],
    mSWVH_reco[3], nOM_reco[3];


  //=======  Event ===
  int EventNumber;
  unsigned int Event_EventNumber;
  int Event_DataMC_Type;
  unsigned int Event_RunNumber;
  int Event_bunchCrossing;
  int Event_orbitNumber;
  unsigned int Event_luminosityBlock;
  bool Event_isRealData;
  unsigned int Event_nsignal_candidates;
  unsigned int Event_ndsphipi_candidate;
  //=======  Tracks === 
  std::vector<std::vector<double> > Track_p4;
  std::vector<double> Track_normalizedChi2;
  std::vector<double> Track_numberOfValidHits;
  std::vector<double> Track_charge;
  std::vector<double> Track_dxy;
  std::vector<double> Track_dz;
  std::vector<std::vector<double> > Track_poca;
  std::vector<double> Track_dxyError;
  std::vector<double> Track_dzError;
  std::vector<unsigned int> dump_track_index_to_fill;
  std::vector<unsigned int> dump_pv_index_to_fill;



  //=======  Gammas==
  std::vector<std::vector<float> > Gamma_P4;
  std::vector<int>                 Gamma_hasPixelSeed;
  std::vector<int>                 Gamma_hasConversionTracks;
  std::vector<float>               Gamma_e1x5;
  std::vector<float>               Gamma_e2x5;
  std::vector<float>               Gamma_e3x3;
  std::vector<float>               Gamma_e5x5;
  std::vector<int>               Gamma_isPFPhoton;


  //=======  Muons ===
  std::vector<std::vector<double> > Muon_p4;
  std::vector<std::vector<double> > Muon_Poca;

  std::vector<bool> Muon_isGlobalMuon;
  std::vector<bool> Muon_isStandAloneMuon;
  std::vector<bool> Muon_isTrackerMuon;
  std::vector<bool> Muon_isCaloMuon;
  std::vector<bool> Muon_isIsolationValid;
  std::vector<bool> Muon_isQualityValid;
  std::vector<bool> Muon_isTimeValid;
  std::vector<int>  Muon_expectedNnumberOfMatchedStations;
  std::vector<bool> Muon_isPFMuon;
  std::vector<bool> Muon_isRPCMuon;

  std::vector<float> Muon_emEt03;
  std::vector<float> Muon_emVetoEt03;
  std::vector<float> Muon_hadEt03;
  std::vector<float> Muon_hadVetoEt03;
  std::vector<int>   Muon_nJets03;
  std::vector<int>   Muon_nTracks03;
  std::vector<int>   Muon_ID; // bitwise 0 - Loose, 1 - Soft, 2 - Medium, 3 - Tight, 4 - HiPt
  std::vector<int>   Muon_StandardSelection;
  std::vector<float> Muon_sumPt03;
  std::vector<float> Muon_trackerVetoPt03;

  std::vector<float> Muon_emEt05;
  std::vector<float> Muon_emVetoEt05;
  std::vector<float> Muon_hadEt05;
  std::vector<float> Muon_hadVetoEt05;
  std::vector<int>   Muon_nJets05;
  std::vector<int>   Muon_nTracks05;
  std::vector<float> Muon_sumPt05;
  std::vector<float> Muon_trackerVetoPt05;

  std::vector<float> Muon_timeAtIpInOut;
  std::vector<float> Muon_timeAtIpInOutErr;



  std::vector<float> Muon_sumChargedHadronPt03;              // sum-pt of charged Hadron
  std::vector<float> Muon_sumChargedParticlePt03;            // sum-pt of charged Particles(inludes e/mu)
  std::vector<float> Muon_sumNeutralHadronEt03;              // sum pt of neutral hadrons
  std::vector<float> Muon_sumNeutralHadronEtHighThreshold03; // sum pt of neutral hadrons with a higher threshold
  std::vector<float> Muon_sumPhotonEt03;                     // sum pt of PF photons
  std::vector<float> Muon_sumPhotonEtHighThreshold03;        // sum pt of PF photons with a higher threshold
  std::vector<float> Muon_sumPUPt03;                         // sum pt of charged Particles not from PV (for Pu corrections)

  std::vector<float> Muon_sumChargedHadronPt04;              // sum-pt of charged Hadron
  std::vector<float> Muon_sumChargedParticlePt04;            // sum-pt of charged Particles(inludes e/mu)
  std::vector<float> Muon_sumNeutralHadronEt04;              // sum pt of neutral hadrons
  std::vector<float> Muon_sumNeutralHadronEtHighThreshold04; // sum pt of neutral hadrons with a higher threshold
  std::vector<float> Muon_sumPhotonEt04;                     // sum pt of PF photons
  std::vector<float> Muon_sumPhotonEtHighThreshold04;        // sum pt of PF photons with a higher threshold
  std::vector<float> Muon_sumPUPt04;                         // sum pt of charged Particles not from PV (for Pu corrections)

  std::vector<int>   Muon_numberOfChambers;
  std::vector<unsigned int> Muon_Track_idx;

  std::vector<bool>   Muon_combinedQuality_updatedSta;
  std::vector<double> Muon_combinedQuality_trkKink;
  std::vector<double> Muon_combinedQuality_glbKink;
  std::vector<double> Muon_combinedQuality_trkRelChi2;
  std::vector<double> Muon_combinedQuality_staRelChi2;
  std::vector<double> Muon_combinedQuality_chi2LocalPosition;
  std::vector<double> Muon_combinedQuality_chi2LocalMomentum;
  std::vector<double> Muon_combinedQuality_localDistance;
  std::vector<double> Muon_combinedQuality_globalDeltaEtaPhi;
  std::vector<bool>   Muon_combinedQuality_tightMatch;
  std::vector<double> Muon_combinedQuality_glbTrackProbability;


  std::vector<double>   Muon_prod_inner_outer_charge;
  std::vector<std::vector<double> > Muon_outerTrack_p4;
  std::vector<std::vector<double> > Muon_innerTrack_p4;
  std::vector<double>   Muon_innerTrack_quality;
  std::vector<double>   Muon_ptErrOverPt;
  std::vector<double>   Muon_calEnergy_hadS9;
  std::vector<double>   Muon_calEnergy_had;
  std::vector<double>   Muon_calEnergy_emS25;
  std::vector<double>   Muon_calEnergy_emS9;
  std::vector<double>   Muon_calEnergy_em;

  std::vector<int>      Muon_charge;
  std::vector<int>      Muon_trackCharge;
  std::vector<int>      Muon_pdgid;
  std::vector<double>   Muon_B;
  std::vector<double>   Muon_M;
  std::vector<std::vector<double> > Muon_par;
  std::vector<std::vector<double> > Muon_cov;


  std::vector<int> signalTau_charge;
  std::vector<int> signalTau_pdgid;
  std::vector<double> signalTau_B;
  std::vector<double> signalTau_M;
  std::vector<std::vector<double> > signalTau_lvp;
  std::vector<std::vector<double> > signalTau_cov;
  std::vector<int> signalTau_isLVP;

  std::vector<int>     Muon_hitPattern_pixelLayerwithMeas;
  std::vector<int>     Muon_numberOfMatchedStations;
  std::vector<float>   Muon_normChi2;
  std::vector<int>     Muon_hitPattern_numberOfValidMuonHits;
  std::vector<int>     Muon_innerTrack_numberofValidHits;
  std::vector<int>     Muon_numberofValidPixelHits;
  std::vector<int>     Muon_numberOfMatches;
  std::vector<int>     Muon_trackerLayersWithMeasurement;
  std::vector<double>  Muon_segmentCompatibility;
  std::vector<double>  Muon_caloCompatibility;


  std::vector<double> Muon_innerTrack_validFraction;
  std::vector<double> Muon_innerTrack_pixelLayersWithMeasurement;
  std::vector<double> Muon_innerTrack_numberOfValidTrackerHits;
  std::vector<double> Muon_innerTrack_numberOfLostTrackerHits;
  std::vector<double> Muon_innerTrack_numberOfLostTrackerInnerHits;
  std::vector<double> Muon_innerTrack_numberOfLostTrackerOuterHits;
  std::vector<double> Muon_innerTrack_normalizedChi2;

  std::vector<std::vector<float> > Muon_TrackX;
  std::vector<std::vector<float> > Muon_TrackY;
  std::vector<std::vector<float> > Muon_dDxDz;
  std::vector<std::vector<float> > Muon_dDyDz;
  std::vector<std::vector<float> > Muon_dX;
  std::vector<std::vector<float> > Muon_dY;
  std::vector<std::vector<float> > Muon_pullX;
  std::vector<std::vector<float> > Muon_pullY;
  std::vector<std::vector<float> > Muon_pullDxDz;
  std::vector<std::vector<float> > Muon_pullDyDz;
  std::vector<std::vector<float> > numberOfSegments;


  std::vector<double> Muon_ptError;
  std::vector<double> Muon_phiError;
  std::vector<double> Muon_etaError;

  std::vector<double> Muon_vmuonhitcomb_reco;
  std::vector<double> Muon_rpchits_reco;

  std::vector<double> Muon_outerTrack_normalizedChi2;
  std::vector<double> Muon_outerTrack_muonStationsWithValidHits;
  std::vector<bool>   Muon_isGoodMuon_TM2DCompatibility;
  std::vector<bool>   Muon_isGoodMuon_TrackerMuonArbitrated;
  std::vector<bool>   Muon_isGoodMuon_TMOneStationTight;
  std::vector<bool>   Muon_isGoodMuon_TMOneStationAngTight;
  std::vector<bool>   Muon_isGoodMuon_TMLastStationTight;
  std::vector<bool>   Muon_isGoodMuon_TMLastStationAngTight;
  std::vector<bool>   Muon_isGoodMuon_TMLastStationOptimizedLowPtTight;
  std::vector<bool>   Muon_isGoodMuon_TMLastStationOptimizedBarrelLowPtTight;


  //do All
  std::vector<std::vector<float> > MC_p4;
  std::vector<int> MC_pdgid;
  std::vector<std::vector<int> > MC_childpdgid;
  std::vector<std::vector<int> > MC_childidx;
  std::vector<int> MC_charge;
  std::vector<int> MC_midx;
  std::vector<int> MC_status;
  bool MC_isReco;
  // Signal particles Ds, B0, Bp
  std::vector<std::vector<float> > MCSignalParticle_p4;
  std::vector<std::vector<float> > MCSignalParticle_Vertex;
  std::vector<int> MCSignalParticle_pdgid;
  std::vector<std::vector<int> > MCSignalParticle_childpdgid;
  std::vector<std::vector<std::vector<float> > > MCSignalParticle_childp4;
  std::vector<std::vector<int> > MCSignalParticle_Sourcepdgid;
  std::vector<std::vector<std::vector<float> > > MCSignalParticle_Sourcep4;
  std::vector<std::vector<float> >  MCSignalParticle_SourceVertex;
  std::vector<int> MCSignalParticle_charge;
  std::vector<std::vector<float> > MCSignalParticle_Poca;
  std::vector<std::vector<unsigned int> > MCSignalParticle_Tauidx;

  // MC Tau Info
  std::vector<std::vector<std::vector<float> > > MCTauandProd_p4;
  std::vector<std::vector<std::vector<float> > > MCTauandProd_Vertex;
  std::vector<std::vector<int> > MCTauandProd_pdgid;
  std::vector<unsigned int>  MCTauandProd_midx;
  std::vector<std::vector<int> > MCTauandProd_charge;
  std::vector<unsigned int> MCTau_JAK;
  std::vector<unsigned int> MCTau_DecayBitMask;

  //  BTagged Jets
  std::vector<double>  Jet_BTagCVSB;
  std::vector<double>  Jet_BTagMVA;
  std::vector<double>  Jet_BTagCSV;
  std::vector<std::vector<double> > Jet_p4;

  //  Three Muons

  std::vector<std::vector<unsigned int> > ThreeMuons_idx;
  std::vector<std::vector<unsigned int> > ThreeMuons_index;
  std::vector<std::vector<float> > ThreeMuons_TriggerMatch_dR;
  std::vector<double>   ThreeMuons_SV_Chi2;
  std::vector<double>   ThreeMuons_SV_NDF;

  std::vector<std::vector<unsigned int> > TwoMuonsTrack_idx;
  std::vector<std::vector<unsigned int> > TwoMuonsTrack_Muonsindex;
  std::vector<std::vector<unsigned int> > TwoMuonsTrack_Trackindex;
  std::vector<double>   TwoMuonsTrack_SV_Chi2;
  std::vector<double>   TwoMuonsTrack_SV_NDF;
  std::vector<std::vector<float> > TwoMuonsTrack_TriggerMatch_dR;


  std::vector<std::vector<double > > Vertex_signal_dca_reco;
  std::vector<std::vector<double > > Vertex_signal_KF_pos;
  std::vector<int >                  Vertex_NMuonsAssocWithPV;
  std::vector<std::vector<double > > Vertex_signal_KF_cov;
  std::vector<double> Vertex_signal_KF_Chi2;
  std::vector<std::vector<std::vector<double > > > Vertex_signal_KF_refittedTracksP4;
  std::vector<double> Vertex_signal_KF_BS_2Ddistance;
  std::vector<double> Vertex_signal_KF_BS_error;
  std::vector<double> Vertex_signal_KF_BS_significance;

  std::vector<std::vector<double > > Vertex_signal_AF_pos;

  std::vector<double> Vertex_signal_AF_Chi2;
  std::vector<double> Vertex_signal_AF_Ndf;
  std::vector<std::vector<std::vector<double > > > Vertex_signal_AF_refittedTracksP4;

  std::vector<std::vector<double > > Vertex_pairfit_status;
  std::vector<std::vector<double > > Vertex_pair_quality;
  std::vector<std::vector<float  > > Vertex_Pair12_Pos;
  std::vector<std::vector<float  > > Vertex_Pair23_Pos;
  std::vector<std::vector<float  > > Vertex_Pair31_Pos;



  double Vertex_N_primary;
  std::vector<std::vector<double > > Vertex_MatchedPrimaryVertex;
  std::vector<std::vector<double > > Vertex_SecondBestPrimaryVertex;

  std::vector<bool>  Vertex_RefitPVisValid;
  std::vector<std::vector<double> > Vertex_MatchedRefitPrimaryVertex;
  std::vector<std::vector<double> > Vertex_MatchedRefitPrimaryVertex_covariance;
  std::vector<std::vector<double> > Vertex_d0_reco;
  std::vector<std::vector<double> > Vertex_dz_reco;
  std::vector<std::vector<double> > Vertex_d0SV_reco;
  std::vector<std::vector<double> > Vertex_dzSV_reco;
  std::vector<std::vector<double> > Vertex_d0sig_reco;
  std::vector<std::vector<double> > Vertex_d0sigSV_reco;
  std::vector<std::vector<double> > Vertex_d0BeamSpot_reco;
  std::vector<std::vector<double> > Vertex_d0BeamSpot_reco_sig;

  std::vector<std::vector<double> > Vertex_2Ddisplacement;
  std::vector<std::vector<double> > Vertex_3Ddisplacement;

  std::vector<std::vector<float> > Vertex_Isolation1;
  std::vector<std::vector<float> > Vertex_Isolation2;
  std::vector<std::vector<float> > Vertex_Isolation3;
  std::vector<std::vector<float> > Vertex_Isolation4;

  std::vector<std::vector<std::vector<float> > > IsolationBranch_Trackp4;


  std::vector<std::vector<std::vector<float> > > IsolationTrack_p4;
  std::vector<std::vector<int> > IsolationTrack_charge;
  std::vector<std::vector<float> > IsolationTrack_quality;

  std::vector<std::vector<float> > IsolationTrack_dxySV;
  std::vector<std::vector<float> > IsolationTrack_dzSV;

  std::vector<std::vector<float> > IsolationTrack_dxyPV;
  std::vector<std::vector<float> > IsolationTrack_dzPV;

  std::vector<std::vector<float> > IsolationTrack_DocaMu1;
  std::vector<std::vector<float> > IsolationTrack_DocaMu2;
  std::vector<std::vector<float> > IsolationTrack_DocaMu3;

  std::vector<string>  Trigger_l1name;
  std::vector<int> Trigger_l1decision;
  std::vector<int> Trigger_l1prescale;

  std::vector<string>  Trigger_hltname;
  std::vector<int> Trigger_hltdecision;

  std::vector<string> TriggerObject_name;
  std::vector<double> TriggerObject_pt;
  std::vector<double> TriggerObject_eta;
  std::vector<double> TriggerObject_phi;
  std::vector<std::vector<float> > SV_pos;
  std::vector<float> SV_Mass;
  std::vector<std::vector<int> > SV_TrackCharge;
  std::vector<std::vector<float> > SV_PosCovariance;
  std::vector<std::vector<std::vector<float> > >  SV_Track_P4;


  bool doMC_, doFullMC_, wideSB_, do2mu_, passhlt_, doTracks_, doMuons_, 
    do3mutuple_, doL1_, doThreeMuons_, doTwoMuonsAndTrack_, doBJets_, doPhotons_, miniAODRun_;
  double TriggerMuonMatchingdr_;
  string WhatData_;


  size_t mid_, n_reco, n_sv, njet20, ifar, ipv_gen, ipv1, ipv2;

  static double MuonPtCut_;
  static double MuonEtaCut_;
  static double TrackPtCut_;
  static double TrackEtaCut_;
  static double phimassmin_;
  static double phimassmax_;


  TTree *output_tree;
  TTree *output_former_tree;
  TH1F *h_n3mu, *h_step;
  TH1F *h_phimass;
  TH1F *h_svmass;
  TH1F *h_phimassj;
  InputTag algInputTag_;
  int cnt_;

  EDGetTokenT<MuonCollection> muonToken_;
  EDGetTokenT<JetTagCollection> btagCvsBToken_;
  EDGetTokenT<JetTagCollection> btagCSVToken_;
  EDGetTokenT<JetTagCollection> btagMVAToken_;
  EDGetTokenT<JetTagCollection> btagDeepCSVToken_;
  EDGetTokenT<VertexCollection> vtxToken_;
  EDGetTokenT<VertexCollection> svToken_;
  EDGetTokenT<PhotonCollection> photonToken_;
  EDGetTokenT<TrackCollection> trackToken_;
  EDGetTokenT<TriggerResults> triggerToken_;
  EDGetTokenT<trigger::TriggerEvent> trigeventToken_;
  EDGetTokenT<vector<TriggerObjectStandAlone>> triggerObjectToken_;
  EDGetToken algToken_;
  EDGetTokenT<BeamSpot> bsToken_;
  EDGetTokenT<vector<PileupSummaryInfo>> puToken_;
  EDGetTokenT<GenParticleCollection> genToken_;
  TString sampleType_;
  L1TGlobalUtil* gtUtil_;
};


#endif
