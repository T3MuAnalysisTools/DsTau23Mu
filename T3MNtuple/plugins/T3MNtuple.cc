#include "DsTau23Mu/T3MNtuple/interface/T3MNtuple.h"
//Simple Fits
#include "DsTau23Mu/T3MNtuple/interface/SimpleParticle.h"
#include "DsTau23Mu/T3MNtuple/interface/LorentzVectorParticle.h"
#include "DsTau23Mu/T3MNtuple/interface/TrackParticle.h"
#include "DsTau23Mu/T3MNtuple/interface/ParticleBuilder.h"

 
//
// constants, enums and typedefs
//

//
// static data member definitions
//
double T3MNtuple::MuonPtCut_(-1.);
double T3MNtuple::MuonEtaCut_(999);
double T3MNtuple::TrackPtCut_(-1.);
double T3MNtuple::TrackEtaCut_(999);
double T3MNtuple::phimassmin_(1.4);
double T3MNtuple::phimassmax_(3.0);


//
// constructors and destructor
//
T3MNtuple::T3MNtuple(const edm::ParameterSet& iConfig):
  TriggerMuonMatchingdr_(iConfig.getUntrackedParameter("TriggerMuonMatchingdr", (double) 0.3)),
  muonToken_(consumes<reco::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"))),
  btagCvsBToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagsCvsB"))),
  btagCSVToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagsCSV"))),
  btagMVAToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagsMVA"))),
  vtxToken_(consumes<VertexCollection>(iConfig.getParameter<InputTag>("pvs"))),
  svToken_(consumes<VertexCollection>(iConfig.getParameter<InputTag>("svs"))),
  trackToken_(consumes<TrackCollection>(iConfig.getParameter<InputTag>("trks"))),
  triggerToken_(consumes<TriggerResults>(iConfig.getParameter<InputTag>("triggerBitsH"))),
  trigeventToken_(consumes<trigger::TriggerEvent>(iConfig.getParameter<InputTag>("triggerSummary"))),
  algToken_(consumes<BXVector<GlobalAlgBlk>>(iConfig.getParameter<InputTag>("AlgInputTag"))),
  bsToken_(consumes<BeamSpot>(iConfig.getParameter<InputTag>("beamSpotHandle"))),
  puToken_(consumes<vector<PileupSummaryInfo> >(iConfig.getParameter<InputTag>("pileupSummary"))),
  genToken_(consumes<GenParticleCollection>(iConfig.getParameter<InputTag>("genParticles"))),
  sampleType_(iConfig.getUntrackedParameter<string>("DataMCType",""))
{
  gtUtil_ = new L1TGlobalUtil(iConfig, consumesCollector(), *this, algInputTag_, algInputTag_);
  doMC_ = iConfig.getParameter<bool>("doMC");
  doFullMC_ = iConfig.getParameter<bool>("doFullMC");
  wideSB_ = iConfig.getParameter<bool>("wideSB");
  do2mu_ = iConfig.getParameter<bool>("do2mu");
  passhlt_ = iConfig.getParameter<bool>("passhlt");
  mid_ = iConfig.getParameter<int>("mid");
  doTracks_ = iConfig.getParameter<bool>("doTracks");
  doMuons_ = iConfig.getParameter<bool>("doMuons");
  do3mutuple_ = iConfig.getParameter<bool>("do3mutuple");
  doL1_ = iConfig.getParameter<bool>("doL1");
  doBJets_ = iConfig.getParameter<bool>("doBJets");
  doThreeMuons_=  iConfig.getParameter<bool>("doThreeMuons");
  doTwoMuonsAndTrack_= iConfig.getParameter<bool>("doTwoMuonsAndTrack");
  MuonPtCut_ = iConfig.getParameter<double>("MuonPtCut"); //default: 2.0
  MuonEtaCut_ = iConfig.getParameter<double>("MuonEtaCut"); //default: 2.5

  TrackPtCut_ = iConfig.getParameter<double>("TrackPtCut"); //default: 1.5
  TrackEtaCut_ = iConfig.getParameter<double>("TrackEtaCut"); //default: 2.5

  DataMCType DMT;
  Event_DataMC_Type=DMT.GetType(sampleType_);
}


T3MNtuple::~T3MNtuple()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//


bool T3MNtuple::isGoodTrack(const Track &track) {
  if(track.pt()>TrackPtCut_){
    if(abs(track.eta())<TrackEtaCut_){
      if(track.hitPattern().trackerLayersWithMeasurement()>5){
	if(track.hitPattern().pixelLayersWithMeasurement()>1) return true;
      }
    }
  }
  return false;
}


bool T3MNtuple::AcceptedMuon(reco::MuonRef RefMuon) {
  if((RefMuon->pt() > MuonPtCut_) || (abs(RefMuon->eta()) < MuonEtaCut_)){
    if(RefMuon->isPFMuon() &&  ( RefMuon->isGlobalMuon() || RefMuon->isTrackerMuon()))  return true;
  }
  return false;
}


bool T3MNtuple::getTrackMatch(edm::Handle<std::vector<reco::Track> > &trackCollection, reco::TrackRef &refTrack, int &match) {
  match = -1;
  for (unsigned int iTrack = 0; iTrack < trackCollection->size(); iTrack++) {
    reco::TrackRef Track(trackCollection, iTrack);
    if (refTrack == Track) {
      match = iTrack;
      return true;
    }
  }
  return false;
}



bool T3MNtuple::SkipThisParticle(const reco::GenParticle &GenPar){

  int id = abs(GenPar.pdgId());
  if(id == 21 || id == 1 || id == 2 || id ==3 || id == 4 || id == 5 || id == 6) return true;
  if(id == 22 && GenPar.p4().Pt() < 0.3 ) return true;
  return false;


}

bool T3MNtuple::isGoodGenParticle(const reco::GenParticle &GenPar){


  int id = abs(GenPar.pdgId());
  //  std::cout<<" all gens pt:    "<< GenPar.p4().Pt()<< "   id:   "<< abs(GenPar.pdgId()) << std::endl;
  if (id == PDGInfo::Ds_plus) return true;
  if (id == PDGInfo::B_plus) return true;
  if (id == PDGInfo::B_0) return true;
  return false;
}

// ------------ method called for each event  ------------
void
T3MNtuple::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{ 
  //  std::cout<<" ========================  new event =============== "<< std::endl;
  cnt_++;
  ClearEvent();

  /*

  fillMCTruth(iEvent, iSetup);
  if(doThreeMuons_) Event_nsignal_candidates =   fillThreeMuons(iEvent, iSetup);
  if(Event_nsignal_candidates==0)
    {
      if(doTwoMuonsAndTrack_) Event_ndsphipi_candidate = fillTwoMuonsAndTracks(iEvent, iSetup);
    }
  //  std::cout<<"   Event_nsignal_candidates   "  << Event_nsignal_candidates <<"  Event_ndsphipi_candidate  "<< Event_ndsphipi_candidate <<std::endl;
  if(Event_nsignal_candidates!=0 or Event_ndsphipi_candidate!=0)
    {
      fillEventInfo(iEvent, iSetup);
      fillVertices(iEvent, iSetup);
      if(doTracks_)
	fillTracks(iEvent, iSetup);
      if(doBJets_)
	fillBTagJets(iEvent, iSetup);
      if(doMuons_)
	fillMuons(iEvent, iSetup);
      if(doMC_)
	fillMCTruth(iEvent, iSetup);
      if(doL1_)
	fillTrigger(iEvent, iSetup);
      output_tree->Fill();
    }
  */

  fillEventInfo(iEvent, iSetup);
  if(doMC_)
    fillMCTruth(iEvent, iSetup);

  if(doThreeMuons_) 
    Event_nsignal_candidates =   fillThreeMuons(iEvent, iSetup);

  if(Event_nsignal_candidates==0)
    {
      if(doTwoMuonsAndTrack_) Event_ndsphipi_candidate = fillTwoMuonsAndTracks(iEvent, iSetup);
    }
  //  std::cout<<"   Event_nsignal_candidates   "  << Event_nsignal_candidates <<"  Event_ndsphipi_candidate  "<< Event_ndsphipi_candidate <<std::endl;
  if(Event_nsignal_candidates!=0 or Event_ndsphipi_candidate!=0)
    {
      MC_isReco=1;
      fillVertices(iEvent, iSetup);
      if(doTracks_)
        fillTracks(iEvent, iSetup);
      if(doBJets_)
        fillBTagJets(iEvent, iSetup);
      if(doMuons_)
        fillMuons(iEvent, iSetup);
      if(doL1_)
        fillTrigger(iEvent, iSetup);
    }
  output_tree->Fill();
  //  fillDsTree(iEvent, iSetup); 
}
 


void T3MNtuple::fillVertices(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  Handle<VertexCollection> pvs;
  iEvent.getByToken(vtxToken_ , pvs);
  Vertex_N_primary = pvs->size();

  Handle<VertexCollection> svs;
  iEvent.getByToken(svToken_ , svs);

  Handle<TrackCollection> trackCollection;
  iEvent.getByToken(trackToken_, trackCollection);

  Handle<MuonCollection> muonCollection;
  iEvent.getByToken(muonToken_, muonCollection);

  std::vector<std::vector<std::vector<double> > > particles_p4;

  std::vector<std::vector<TransientTrack> > signalTracksCollection;
  ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
  if(ThreeMuons_idx.size()!=0){
    for ( auto &iThreeMuon :  ThreeMuons_idx ) {
      particles_p4.push_back(std::vector<std::vector<double> > ());
      vector<TransientTrack> isignalTracksCollection;
      ESHandle<TransientTrackBuilder> theB;
      iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
      for ( auto &iMuon :  iThreeMuon ) {
	reco::MuonRef Muon(muonCollection, iMuon);
	//	TrackRef MuonTrack = Muon->globalTrack();// change to a inner track
	TrackRef MuonTrack = Muon->innerTrack();
	isignalTracksCollection.push_back(theB->build(MuonTrack));
	std::vector<double> iiparticles_p4;
	iiparticles_p4.push_back(Muon->p4().E());
	iiparticles_p4.push_back(Muon->p4().Px());
	iiparticles_p4.push_back(Muon->p4().Py());
	iiparticles_p4.push_back(Muon->p4().Pz());
	particles_p4.at(particles_p4.size() -1).push_back(iiparticles_p4);
      }

      signalTracksCollection.push_back(isignalTracksCollection);
    }
  } else if(TwoMuonsTrack_idx.size()!=0){
    std::vector<std::vector<double> > iparticle_p4;
    for ( auto &iTwoMuonsTracks :  TwoMuonsTrack_idx ) {

      vector<TransientTrack> isignalTracksCollection;

      reco::MuonRef Muon1(muonCollection, iTwoMuonsTracks.at(0));
      reco::MuonRef Muon2(muonCollection, iTwoMuonsTracks.at(1));

      TrackRef track1 = Muon1->innerTrack();
      TrackRef track2 = Muon2->innerTrack();
      TrackRef track3 = TrackRef(trackCollection, iTwoMuonsTracks.at(2));

      isignalTracksCollection.push_back(theB->build(track1));
      isignalTracksCollection.push_back(theB->build(track2));
      isignalTracksCollection.push_back(theB->build(track3));
      signalTracksCollection.push_back(isignalTracksCollection);


      std::vector<double> particle1_p4;
      std::vector<double> particle2_p4;
      std::vector<double> particle3_p4;

      particle1_p4.push_back(Muon1->p4().E());       particle2_p4.push_back(Muon2->p4().E());       particle3_p4.push_back(sqrt(pow(track3->p(),2.0) + pow(PDGInfo::pi_mass(),2.0)));
      particle1_p4.push_back(Muon1->p4().Px());      particle2_p4.push_back(Muon2->p4().Px());      particle3_p4.push_back(track3->px());
      particle1_p4.push_back(Muon1->p4().Py());      particle2_p4.push_back(Muon2->p4().Py());      particle3_p4.push_back(track3->py());
      particle1_p4.push_back(Muon1->p4().Pz());      particle2_p4.push_back(Muon2->p4().Pz());      particle3_p4.push_back(track3->pz());
      iparticle_p4.push_back(particle1_p4);          iparticle_p4.push_back(particle2_p4);          iparticle_p4.push_back(particle3_p4);
      particles_p4.push_back(iparticle_p4);

    }
  }
  unsigned int index(0);
  for ( auto &iTransientTracks :  signalTracksCollection ) {
    Vertex_signal_KF_pos.push_back(std::vector<double> ());
    Vertex_signal_KF_cov.push_back(std::vector<double> ());
    Vertex_signal_KF_refittedTracksP4.push_back(std::vector<std::vector<double> >());

    Vertex_signal_AF_pos.push_back(std::vector<double> ());



    ClosestApproachInRPhi cApp12, cApp23, cApp31;
    cApp12.calculate(iTransientTracks[0].initialFreeState(), iTransientTracks[1].initialFreeState());
    cApp23.calculate(iTransientTracks[1].initialFreeState(), iTransientTracks[2].initialFreeState());
    cApp31.calculate(iTransientTracks[2].initialFreeState(), iTransientTracks[0].initialFreeState());
    std::vector<double> iVertex_signal_dca_reco;
    if((cApp12.status()&&cApp23.status()&&cApp31.status())) { 
      iVertex_signal_dca_reco.push_back(cApp12.distance());   // order 12,23,31, max
      iVertex_signal_dca_reco.push_back(cApp23.distance());
      iVertex_signal_dca_reco.push_back(cApp31.distance());
      iVertex_signal_dca_reco.push_back(TMath::Max(dca12_reco, TMath::Max(dca23_reco, dca31_reco)));
    } else {
      iVertex_signal_dca_reco.push_back(-1.);
      iVertex_signal_dca_reco.push_back(-1.);
      iVertex_signal_dca_reco.push_back(-1.);
      iVertex_signal_dca_reco.push_back(-1.);
    }
    Vertex_signal_dca_reco.push_back(iVertex_signal_dca_reco);
  
    TransientVertex transVtx;
    KalmanVertexFitter kvf(true);
    bool FitOk(true);
    try {
      transVtx = kvf.vertex(iTransientTracks); //KalmanVertexFitter
    } catch (...) {
      FitOk = false;
    }
    if (!transVtx.hasRefittedTracks())
      FitOk = false;
    if (transVtx.refittedTracks().size() != iTransientTracks.size())
      FitOk = false;
    TLorentzVector ThreeCandidate(0,0,0,0);

    math::XYZPoint TheSecondaryVertexPoint;
    if(FitOk){
      Vertex_signal_KF_Chi2.push_back(transVtx.totalChiSquared());
      Vertex_signal_KF_pos.at(index).push_back(transVtx.position().x());
      Vertex_signal_KF_pos.at(index).push_back(transVtx.position().y());
      Vertex_signal_KF_pos.at(index).push_back(transVtx.position().z());
  
      reco::Vertex secondaryVertex = transVtx;
      TMatrixTSym<double> svcov(LorentzVectorParticle::NVertex);
      math::Error<3>::type svCov;
      secondaryVertex.fill(svCov);

      for (int i = 0; i <LorentzVectorParticle::NVertex; i++){
	for (int j = 0; j < LorentzVectorParticle::NVertex; j++) {
	  svcov(i, j) = svCov(i, j);
	  svcov(j, i) = svCov(i, j);
	}
      }
      for (int i = 0; i < LorentzVectorParticle::NVertex; i++) {
	for (int j = i; j < LorentzVectorParticle::NVertex; j++) {
	  Vertex_signal_KF_cov.at(index).push_back(svcov(i, j));
	}
      }
   

      TheSecondaryVertexPoint = math::XYZPoint(transVtx.position().x(), transVtx.position().y(), transVtx.position().z());
      vector<TransientTrack>::const_iterator trkIt = transVtx.refittedTracks().begin();

      //      std::cout<<" refitted tracks size "<< transVtx.refittedTracks().size() << std::endl;
    
      for(; trkIt != transVtx.refittedTracks().end(); ++ trkIt) {
      std::vector<double> irefitted_tracks_p4;
	const Track & trkrefit = trkIt->track();
	irefitted_tracks_p4.push_back(sqrt(pow(trkrefit.p(),2.0) + pow(PDGInfo::mu_mass(),2.0)));
	irefitted_tracks_p4.push_back(trkrefit.px());
	irefitted_tracks_p4.push_back(trkrefit.py());
	irefitted_tracks_p4.push_back(trkrefit.pz());
	ThreeCandidate+=TLorentzVector(trkrefit.px(),trkrefit.py(),trkrefit.pz(),sqrt(pow(trkrefit.p(),2.0) + pow(PDGInfo::mu_mass(),2.0)));
	Vertex_signal_KF_refittedTracksP4.at(Vertex_signal_KF_refittedTracksP4.size() -1).push_back(irefitted_tracks_p4);
      }
    }


    bool AFitOk(true);
    AdaptiveVertexFitter avf;
    TransientVertex AdaptivetransVtx;
    avf.setWeightThreshold(0.1); //weight per track. allow almost every fit, else --> exception
    try {
	AdaptivetransVtx = avf.vertex(iTransientTracks);
    } catch (...) {
      AFitOk = false;
    }
    if(!AdaptivetransVtx.isValid()) 
      AFitOk = false;
    if (AFitOk){ 
      Vertex_signal_AF_Chi2.push_back(AdaptivetransVtx.totalChiSquared());
      Vertex_signal_AF_Ndf.push_back(AdaptivetransVtx.degreesOfFreedom());
      Vertex_signal_AF_pos.at(index).push_back(AdaptivetransVtx.position().x());
      Vertex_signal_AF_pos.at(index).push_back(AdaptivetransVtx.position().y());
      Vertex_signal_AF_pos.at(index).push_back(AdaptivetransVtx.position().z());


      vector<TransientTrack>::const_iterator trkIt = AdaptivetransVtx.refittedTracks().begin();
      for(; trkIt != AdaptivetransVtx.refittedTracks().end(); ++ trkIt) {
	//	std::cout<<"loop over tracks "<<std::endl; // no valid tracks by some reason while the vertex is valid.
      }
    } else {
      Vertex_signal_AF_Chi2.push_back(-1);
    }
  
  //--------------------  Fit each track pair 
  vector<TransientTrack> trackpair12, trackpair23, trackpair31;
  trackpair12.push_back(iTransientTracks.at(0)); trackpair12.push_back(iTransientTracks.at(1));
  trackpair23.push_back(iTransientTracks.at(1)); trackpair23.push_back(iTransientTracks.at(2));
  trackpair31.push_back(iTransientTracks.at(2)); trackpair31.push_back(iTransientTracks.at(0));
  KalmanVertexFitter kvf_trks12(true), kvf_trks23(true), kvf_trks31(true);
  TransientVertex fv_trks12 = kvf_trks12.vertex(trackpair12);
  TransientVertex fv_trks23 = kvf_trks23.vertex(trackpair23);
  TransientVertex fv_trks31 = kvf_trks31.vertex(trackpair31);


  bool Fit1Ok(true);
  try {
    fv_trks12 = kvf_trks12.vertex(trackpair12); 
  } catch (...) {
    Fit1Ok = false;
  }

  bool Fit2Ok(true);
  try {
    fv_trks23 = kvf_trks12.vertex(trackpair23); 
  } catch (...) {
    Fit2Ok = false;
  }

  bool Fit3Ok(true);
  try {
    fv_trks31 = kvf_trks12.vertex(trackpair31); 
  } catch (...) {
    Fit3Ok = false;
  }

  std::vector<double> iVertex_pair_quality;
  std::vector<double> iVertex_pairfit_status;
  iVertex_pairfit_status.push_back(Fit1Ok);
  iVertex_pairfit_status.push_back(Fit2Ok);
  iVertex_pairfit_status.push_back(Fit3Ok);

  if(Fit1Ok){iVertex_pair_quality.push_back(fv_trks12.totalChiSquared());}else{iVertex_pair_quality.push_back(-1);}
  if(Fit2Ok){iVertex_pair_quality.push_back(fv_trks23.totalChiSquared());}else{iVertex_pair_quality.push_back(-1);}
  if(Fit2Ok){iVertex_pair_quality.push_back(fv_trks31.totalChiSquared());}else{iVertex_pair_quality.push_back(-1);}
  
  Vertex_pair_quality.push_back(iVertex_pair_quality);
  Vertex_pairfit_status.push_back(iVertex_pairfit_status);
  
  ///////////////////////////////////////////
  //  find here the primary vertex with the best
  //  alignement to the tri-muon 
  double dphi_pv = -1.0;
  size_t primaryvertex_index;
  for(size_t vertex_index = 0; vertex_index  < pvs->size(); vertex_index++) {
    const Vertex & pvertex = (*pvs)[vertex_index];
    
    TVector3 Dv3D_reco(transVtx.position().x() - pvertex.x(), transVtx.position().y() - pvertex.y(), transVtx.position().z() - pvertex.z());
    double Cosdphi_3D = Dv3D_reco.Dot(ThreeCandidate.Vect())/(Dv3D_reco.Mag()*ThreeCandidate.Vect().Mag());
    if(Cosdphi_3D>dphi_pv){
      dphi_pv = Cosdphi_3D;
      primaryvertex_index=vertex_index;
    }
  }

  
  
  const Vertex & MatchedPrimaryVertex = (*pvs)[primaryvertex_index];
  dump_pv_index_to_fill.push_back(primaryvertex_index);

  std::vector<double>  iprimaryVertex_Pos;
  iprimaryVertex_Pos.push_back(MatchedPrimaryVertex.x());
  iprimaryVertex_Pos.push_back(MatchedPrimaryVertex.y());
  iprimaryVertex_Pos.push_back(MatchedPrimaryVertex.z());
  Vertex_MatchedPrimaryVertex.push_back(iprimaryVertex_Pos);


  vector<TransientTrack> primaryvertexTransientTracks;// remove muon candidate from the PV to perform refit
  for(Vertex::trackRef_iterator itk = MatchedPrimaryVertex.tracks_begin(); itk != MatchedPrimaryVertex.tracks_end(); itk++) {
    if((**itk).pt()>1) {
      if(deltaR(iTransientTracks.at(0).track().eta(), iTransientTracks.at(0).track().phi(), (**itk).eta(), (**itk).phi())<0.01)continue;
      if(deltaR(iTransientTracks.at(1).track().eta(), iTransientTracks.at(1).track().phi(), (**itk).eta(), (**itk).phi())<0.01)continue;
      if(deltaR(iTransientTracks.at(2).track().eta(), iTransientTracks.at(2).track().phi(), (**itk).eta(), (**itk).phi())<0.01)continue;

    }
    primaryvertexTransientTracks.push_back(theB->build(**itk));
  }

  KalmanVertexFitter pv_fit(true);
  bool FitPVOk(true);
  TransientVertex pvvertex;
  if(primaryvertexTransientTracks.size() >1){
    try {
      pvvertex =  pv_fit.vertex(trackpair12);
    } catch (...) {
      FitPVOk = false;
    }
  }

  Vertex_RefitPVisValid.push_back(pvvertex.isValid());
  std::vector<double> iRefitprimaryVertex_Pos;
  if(FitPVOk && pvvertex.isValid()){
    iRefitprimaryVertex_Pos.push_back(MatchedPrimaryVertex.position().x());
    iRefitprimaryVertex_Pos.push_back(MatchedPrimaryVertex.position().y());
    iRefitprimaryVertex_Pos.push_back(MatchedPrimaryVertex.position().z());
  }
  Vertex_MatchedRefitPrimaryVertex.push_back(iRefitprimaryVertex_Pos);


  TMatrixTSym<double> pvcov(LorentzVectorParticle::NVertex);

  math::Error<3>::type pvCov;
  MatchedPrimaryVertex.fill(pvCov);
  
  for (int i = 0; i <LorentzVectorParticle::NVertex; i++){
    for (int j = 0; j < LorentzVectorParticle::NVertex; j++) {
      pvcov(i, j) = pvCov(i, j);
      pvcov(j, i) = pvCov(i, j);
    }
  }
  std::vector<double>  pv_cov;     
  for (int i = 0; i < LorentzVectorParticle::NVertex; i++) {
    for (int j = i; j < LorentzVectorParticle::NVertex; j++) {
      pv_cov.push_back(pvcov(i, j));
    }
  }




  Vertex_MatchedRefitPrimaryVertex_covariance.push_back(pv_cov);

  Vertex final_pv = MatchedPrimaryVertex;  
  if(pvvertex.isValid()) final_pv = Vertex(pvvertex);
  math::XYZPoint pvPoint = math::XYZPoint(final_pv.x(), final_pv.y(), final_pv.z());

  std::vector<double> iVertex_d0_reco;
  iVertex_d0_reco.push_back(abs(iTransientTracks.at(0).track().dxy(TheSecondaryVertexPoint)));
  iVertex_d0_reco.push_back(abs(iTransientTracks.at(1).track().dxy(TheSecondaryVertexPoint)));
  iVertex_d0_reco.push_back(abs(iTransientTracks.at(2).track().dxy(TheSecondaryVertexPoint)));
  Vertex_d0_reco.push_back(iVertex_d0_reco);

  TLorentzVector LV1=TLorentzVector(particles_p4.at(index).at(0).at(1), particles_p4.at(index).at(0).at(2), particles_p4.at(index).at(0).at(3),particles_p4.at(index).at(0).at(0));
  TLorentzVector LV2=TLorentzVector(particles_p4.at(index).at(1).at(1), particles_p4.at(index).at(1).at(2), particles_p4.at(index).at(1).at(3),particles_p4.at(index).at(1).at(0));
  TLorentzVector LV3=TLorentzVector(particles_p4.at(index).at(2).at(1), particles_p4.at(index).at(2).at(2), particles_p4.at(index).at(2).at(3),particles_p4.at(index).at(2).at(0));
  TLorentzVector LVTau = LV1 + LV2 + LV3;

  GlobalVector dir1(particles_p4.at(index).at(0).at(1), particles_p4.at(index).at(0).at(2), particles_p4.at(index).at(0).at(3));
  GlobalVector dir2(particles_p4.at(index).at(1).at(1), particles_p4.at(index).at(1).at(2), particles_p4.at(index).at(1).at(3));
  GlobalVector dir3(particles_p4.at(index).at(2).at(1), particles_p4.at(index).at(2).at(2), particles_p4.at(index).at(2).at(3));

  std::pair<bool, Measurement1D> ip2d_1 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(0), dir1, final_pv);
  std::pair<bool, Measurement1D> ip2d_2 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(1), dir2, final_pv);
  std::pair<bool, Measurement1D> ip2d_3 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(2), dir3, final_pv);

  if(ip2d_1.first) d0sig_reco[0] = abs(ip2d_1.second.value()/ip2d_1.second.error());
  if(ip2d_2.first) d0sig_reco[1] = abs(ip2d_2.second.value()/ip2d_2.second.error());
  if(ip2d_3.first) d0sig_reco[2] = abs(ip2d_3.second.value()/ip2d_3.second.error());

  std::vector<double> iVertex_d0sig_reco;
  if(ip2d_1.first){ iVertex_d0sig_reco.push_back(abs(ip2d_1.second.value()/ip2d_1.second.error()));} else{iVertex_d0sig_reco.push_back(-1);}
  if(ip2d_2.first){ iVertex_d0sig_reco.push_back(abs(ip2d_2.second.value()/ip2d_2.second.error()));} else{iVertex_d0sig_reco.push_back(-1);}
  if(ip2d_3.first){ iVertex_d0sig_reco.push_back(abs(ip2d_3.second.value()/ip2d_3.second.error()));} else{iVertex_d0sig_reco.push_back(-1);}

  Vertex_d0sig_reco.push_back(iVertex_d0sig_reco);

  TVector3 dv2D_reco(-final_pv.position().x() + TheSecondaryVertexPoint.x(), -final_pv.position().y() + TheSecondaryVertexPoint.y(), 0);
  TVector3 vtauxy(ThreeCandidate.Px(), ThreeCandidate.Py(), 0);
  fv_cosdphi = dv2D_reco.Dot(vtauxy)/(dv2D_reco.Perp()*vtauxy.Perp());
  VertexDistanceXY vdistXY;
  Measurement1D distXY = vdistXY.distance(Vertex(transVtx), final_pv);
  std::vector<double> iVertex_2Ddisplacement;
  iVertex_2Ddisplacement.push_back(distXY.value());
  iVertex_2Ddisplacement.push_back(distXY.significance());
  iVertex_2Ddisplacement.push_back(distXY.value()*fv_cosdphi * m3mu_reco/vtauxy.Perp());
  Vertex_2Ddisplacement.push_back(iVertex_2Ddisplacement);

  //  std::cout<<"  "<< distXY.value() <<"  " << distXY.significance() << "   "<< distXY.value()*fv_cosdphi * m3mu_reco/vtauxy.Perp() <<std::endl;

  TVector3 vtauxyz(ThreeCandidate.Px(), ThreeCandidate.Py(), ThreeCandidate.Pz());
  TVector3 dv3D_reco(-final_pv.position().x() + TheSecondaryVertexPoint.x(), -final_pv.position().y() + TheSecondaryVertexPoint.y(), -final_pv.position().z() + TheSecondaryVertexPoint.z());
  fv_cosdphi3D = dv3D_reco.Dot(vtauxyz)/(dv3D_reco.Mag()*vtauxyz.Mag());
  VertexDistance3D dist;

  //  std::cout<<"  "<< dist.distance(Vertex(final_pv), final_pv).value() <<"  " << dist.distance(Vertex(final_pv), final_pv).significance() << "   "<< fv_d3D*fv_cosdphi3D*m3mu_reco/ThreeCandidate.P() <<std::endl;

  std::vector<double> iVertex_3Ddisplacement;
  iVertex_3Ddisplacement.push_back(dist.distance(Vertex(transVtx), final_pv).value());
  iVertex_3Ddisplacement.push_back(dist.distance(Vertex(transVtx), final_pv).significance());
  iVertex_3Ddisplacement.push_back(fv_d3D*fv_cosdphi3D*m3mu_reco/ThreeCandidate.P());



  Vertex_3Ddisplacement.push_back(iVertex_3Ddisplacement);



  ///////////////////////////////////////////////////////////////
  //    Here fill the isolation

  float minmuon_pt(999.), maxmuon_dr(0.);

  if(LV1.Pt()<minmuon_pt)minmuon_pt=LV1.Pt();
  if(LV2.Pt()<minmuon_pt)minmuon_pt=LV2.Pt();
  if(LV3.Pt()<minmuon_pt)minmuon_pt=LV3.Pt();

  float  drLV1Tau(deltaR(LV1.Eta(), LV1.Phi(), LVTau.Eta(), LVTau.Phi()));
  float  drLV2Tau(deltaR(LV2.Eta(), LV2.Phi(), LVTau.Eta(), LVTau.Phi()));
  float  drLV3Tau(deltaR(LV3.Eta(), LV3.Phi(), LVTau.Eta(), LVTau.Phi()));

  maxmuon_dr = TMath::Max(drLV1Tau, TMath::Max(drLV2Tau,drLV3Tau));

  float sumptalltracks(0.), sumalltracks(0.), mindist(99.);
  float sumptalltracks05(0.), sumalltracks05(0.), mindist05(99.), sumalltracks_b(0.);
  float pt_trk_1(0.), pt_trk_2(0.), pt_trk_3(0.);
  float N_trk_1(0.),N_trk_2(0.),N_trk_3(0.), N_trk_total(0.);
  float N_trk0p1(0.), N_trk0p2(0.), N_trk0p5(0.), maxdxy(0.);
  float relative_iso(0.), relative_iso05(0.), relative_mu1_iso(0.),relative_mu2_iso(0.),relative_mu3_iso(0.);
  float relative_maxiso(0.);


  for(size_t i = 0; i < trackCollection->size(); i++) {
    const Track & t = (*trackCollection)[i];

    if(!(t.quality(TrackBase::tight)))continue; //-- this might be weaker
    if(deltaR(LV1.Eta(), LV1.Phi(), t.eta(), t.phi())<0.01)continue;
    if(deltaR(LV2.Eta(), LV2.Phi(), t.eta(), t.phi())<0.01)continue;
    if(deltaR(LV3.Eta(), LV3.Phi(), t.eta(), t.phi())<0.01)continue;


    double dz = abs(t.dz(TheSecondaryVertexPoint));
    double dxy = abs(t.dxy(TheSecondaryVertexPoint));
    double dca_fv = sqrt(dz*dz+dxy*dxy);
    double dr_tau = deltaR(t.eta(), t.phi(), LVTau.Eta(), LVTau.Phi());

    ////////////////////////////////////////////////
    // Below is the isolation defined by Jian
    // iso no. 1b - using pt_min, drtau_max of the 3 mu
    if(t.pt() > 0.33*minmuon_pt && dr_tau < 3.*maxmuon_dr && dca_fv<0.05 ) {
      sumptalltracks += t.pt();
      sumalltracks++; // iso 3b
      if(dca_fv<mindist)mindist=dca_fv; // iso 4b
    } 

    if(t.pt()<1.0) continue;  // was 1.2
    // iso no. 1
    if(dr_tau < 0.5 && dca_fv<0.05 ) {
      sumptalltracks05 += t.pt();
      sumalltracks05++; // iso 3
      //if(dca_fv<mindca_iso05)mindca_iso05=dca_fv; // iso 4
    }
    if(dca_fv<mindca_iso05)mindist05=dca_fv; // iso 4
    if(dca_fv<0.05)sumalltracks_b++; // iso 3b



    TransientTrack trkiso = theB->build(t);
    ClosestApproachInRPhi cAppm1, cAppm2, cAppm3;
    cAppm1.calculate(trkiso.initialFreeState(), iTransientTracks.at(0).initialFreeState());
    cAppm2.calculate(trkiso.initialFreeState(), iTransientTracks.at(1).initialFreeState());
    cAppm3.calculate(trkiso.initialFreeState(), iTransientTracks.at(2).initialFreeState());
    if(!(cAppm1.status()&&cAppm2.status()&&cAppm3.status())) continue;


    // iso no. 2
    if(deltaR(t.eta(), t.phi(), LV1.Eta(), LV1.Phi()) < 0.3 && cAppm1.distance() < 0.1) {// && dz1 < .3) 
      N_trk_1++;
      pt_trk_1 += t.pt();
    }
    if(deltaR(t.eta(), t.phi(), LV2.Eta(), LV2.Phi()) < 0.3 && cAppm2.distance() < 0.1) {//&& dz2 < .3) 
      N_trk_2++;
      pt_trk_2 += t.pt();
    }
    if(deltaR(t.eta(), t.phi(), LV3.Eta(), LV3.Phi()) < 0.3 && cAppm3.distance() < 0.1) {//&& dz3 < .3) 
      N_trk_3++;
      pt_trk_3 += t.pt();
    }
    if( (deltaR(t.eta(), t.phi(), LV1.Eta(), LV1.Phi()) < 0.3 && cAppm1.distance() < 0.1 )
	||(deltaR(t.eta(), t.phi(), LV2.Eta(), LV2.Phi()) < 0.3 && cAppm2.distance() < 0.1 )
	||(deltaR(t.eta(), t.phi(), LV3.Eta(), LV3.Phi()) < 0.3 && cAppm3.distance() < 0.1 )
	) N_trk_total++;




    double dz_primaryvertex=abs(t.dz(pvPoint));
    if(!(dz_primaryvertex < 1))continue;
    double dxy_primaryvertex = abs(t.dxy(pvPoint));
    if(dxy_primaryvertex>0.1) N_trk0p1++;
    if(dxy_primaryvertex>0.2) N_trk0p2++;
    if(dxy_primaryvertex>0.5) N_trk0p5++;
    if(dxy_primaryvertex>maxdxy) maxdxy = dxy_primaryvertex;
  }

  relative_iso = sumptalltracks/LVTau.Pt();
  relative_iso05 = sumptalltracks05/LVTau.Pt();
  relative_mu1_iso = pt_trk_1/LV1.Pt(); relative_mu2_iso = pt_trk_2/LV2.Pt(); relative_mu3_iso = pt_trk_3/LV3.Pt();
  relative_maxiso = TMath::Max(relative_mu1_iso, TMath::Max(relative_mu2_iso,relative_mu3_iso ));

  std::vector<float> isolation1, isolation2, isolation3, isolation4;
  isolation1.push_back(relative_iso);
  isolation1.push_back(sumalltracks);
  isolation1.push_back(mindist);
  
  isolation2.push_back(relative_iso05);
  isolation2.push_back(sumalltracks05);
  isolation2.push_back(mindist05);
  
  isolation3.push_back(relative_mu1_iso);
  isolation3.push_back(relative_mu2_iso);
  isolation3.push_back(relative_mu3_iso);
  isolation3.push_back(relative_maxiso);

  isolation4.push_back(N_trk_1);
  isolation4.push_back(N_trk_2);
  isolation4.push_back(N_trk_3);

  isolation4.push_back(N_trk0p1);
  isolation4.push_back(N_trk0p2);
  isolation4.push_back(N_trk0p5);
  isolation4.push_back(maxdxy);

  Vertex_Isolation1.push_back(isolation1);
  Vertex_Isolation2.push_back(isolation2);
  Vertex_Isolation3.push_back(isolation3);
  Vertex_Isolation4.push_back(isolation4);
  index++;
  }



  //N_Sv = 0; // to be filled later
  for(size_t isv = 0; isv < svs->size(); isv++) {
    //    const Vertex & sv = (*svs)[isv];
    //    if(abs(sv.p4().M()-0.498)<.03 && sv.tracksSize()==2)continue; // no Ks
    //    std::cout<<"Secondary Vertex Mass  "<< sv.p4().M() << "    nSV   "<< isv <<std::endl;
  }
  return;
}

void T3MNtuple::fillTracks(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  Handle<TrackCollection> trackCollection;
  iEvent.getByToken(trackToken_, trackCollection);
  unsigned int Track_index(0);
  unsigned int sel_track_index = 0;

  TwoMuonsTrack_Trackindex.resize(TwoMuonsTrack_idx.size());

  std::vector<reco::Track>::const_iterator trIt  = trackCollection->begin();
  std::vector<reco::Track>::const_iterator trEnd = trackCollection->end();

  for (; trIt != trEnd; ++trIt, Track_index++) 
    {
      if(find(dump_track_index_to_fill.begin(), dump_track_index_to_fill.end(), Track_index) !=  dump_track_index_to_fill.end())
	{
	  std::vector<double> iTrack_p4;
	  std::vector<double> iTrack_poca;
	  const reco::Track track = (*trIt);
	  if(isGoodTrack(track)){
    	    for(unsigned int iTwoMuonsTrack=0;  iTwoMuonsTrack < TwoMuonsTrack_idx.size(); iTwoMuonsTrack++){
	      if(find(TwoMuonsTrack_idx.at(iTwoMuonsTrack).begin(), TwoMuonsTrack_idx.at(iTwoMuonsTrack).end(), Track_index) !=  TwoMuonsTrack_idx.at(iTwoMuonsTrack).end()){
		TwoMuonsTrack_Trackindex.at(iTwoMuonsTrack).push_back(sel_track_index);
	      }
	    }

	    iTrack_p4.push_back(sqrt(pow(track.p(),2.0) + pow(PDGInfo::pi_mass(),2.0)));
	    iTrack_p4.push_back(track.px());
	    iTrack_p4.push_back(track.py());
	    iTrack_p4.push_back(track.pz());
	    Track_p4.push_back(iTrack_p4);
	    Track_normalizedChi2.push_back(track.normalizedChi2());
	    Track_numberOfValidHits.push_back(track.numberOfValidHits());
	    Track_charge.push_back(track.charge());
	    Track_dxy.push_back(track.dxy());
	    Track_dz.push_back(track.dz());
	    iTrack_poca.push_back(track.vx());
	    iTrack_poca.push_back(track.vy());
	    iTrack_poca.push_back(track.vz());
	    Track_poca.push_back(iTrack_poca);
	    
	    Track_dxyError.push_back(track.dxyError());
	    Track_dzError.push_back(track.dzError());
	    sel_track_index++;
	  }
	}
    }
}

void T3MNtuple::fillMuons(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  Handle<TrackCollection> trackCollection;
  iEvent.getByToken(trackToken_, trackCollection);

  Handle<MuonCollection> muonCollection;
  iEvent.getByToken(muonToken_, muonCollection);

  Handle<VertexCollection> pvs;
  iEvent.getByToken(vtxToken_ , pvs);

  unsigned int Muon_index = 0;
  unsigned int sel_muon_index = 0;
  ThreeMuons_index.resize(ThreeMuons_idx.size());
  TwoMuonsTrack_Muonsindex.resize(TwoMuonsTrack_idx.size());


  for (reco::MuonCollection::const_iterator iMuon = muonCollection->begin(); iMuon != muonCollection->end(); ++iMuon, Muon_index++) {
    reco::MuonRef RefMuon(muonCollection, Muon_index);
    if(AcceptedMuon(RefMuon)){
      for(unsigned int iThreeMuons=0;  iThreeMuons < ThreeMuons_idx.size(); iThreeMuons++){
	if(find(ThreeMuons_idx.at(iThreeMuons).begin(), ThreeMuons_idx.at(iThreeMuons).end(), Muon_index) !=  ThreeMuons_idx.at(iThreeMuons).end()){
	  ThreeMuons_index.at(iThreeMuons).push_back(sel_muon_index);

	}
      }
      for(unsigned int iTwoMuons=0;  iTwoMuons < TwoMuonsTrack_idx.size(); iTwoMuons++){
	if(TwoMuonsTrack_idx.at(iTwoMuons).at(0) == Muon_index || TwoMuonsTrack_idx.at(iTwoMuons).at(1) == Muon_index){
	  TwoMuonsTrack_Muonsindex.at(iTwoMuons).push_back(sel_muon_index);
	}
      }
      
      std::vector<double> iMuon_Poca;
      iMuon_Poca.push_back(RefMuon->vx());
      iMuon_Poca.push_back(RefMuon->vy());
      iMuon_Poca.push_back(RefMuon->vz());
      Muon_Poca.push_back(iMuon_Poca);
      std::vector<double> iMuon_p4;
      iMuon_p4.push_back(RefMuon->p4().E());
      iMuon_p4.push_back(RefMuon->p4().Px());
      iMuon_p4.push_back(RefMuon->p4().Py());
      iMuon_p4.push_back(RefMuon->p4().Pz());
      Muon_p4.push_back(iMuon_p4);
      
      const reco::MuonIsolation Iso03 = RefMuon->isolationR03();
      const reco::MuonIsolation Iso05 = RefMuon->isolationR05();
      
      const reco::MuonPFIsolation PFIso03 = RefMuon->pfIsolationR03();
      const reco::MuonPFIsolation PFIso04 = RefMuon->pfIsolationR04();
      



      Muon_numberOfChambers.push_back(RefMuon->numberOfChambers());
      Muon_isGlobalMuon.push_back(RefMuon->isGlobalMuon());
      Muon_isPFMuon.push_back(RefMuon->isPFMuon());
      Muon_isRPCMuon.push_back(RefMuon->isRPCMuon());
      Muon_isStandAloneMuon.push_back(RefMuon->isStandAloneMuon());
      Muon_isTrackerMuon.push_back(RefMuon->isTrackerMuon());
      Muon_isCaloMuon.push_back(RefMuon->isCaloMuon());
      Muon_isQualityValid.push_back(RefMuon->isQualityValid());
      Muon_isTimeValid.push_back(RefMuon->isTimeValid());
      Muon_isIsolationValid.push_back(RefMuon->isIsolationValid());
      Muon_numberOfMatchedStations.push_back(RefMuon->numberOfMatchedStations());
      Muon_numberOfMatches.push_back(RefMuon->numberOfMatches(reco::Muon::SegmentArbitration));
      Muon_charge.push_back(RefMuon->charge());
      
      const Vertex & VertexMuonID = (*pvs)[dump_pv_index_to_fill.at(0)];
      int idbit(0);
      if(muon::isLooseMuon(*RefMuon)) idbit |= 1 << 0;
      if(muon::isSoftMuon(*RefMuon,VertexMuonID)) idbit |= 1 << 1;
      if(muon::isMediumMuon(*RefMuon)) idbit |= 1 << 2;
      if(muon::isTightMuon(*RefMuon,VertexMuonID)) idbit |= 1 << 3;
      if(muon::isHighPtMuon(*RefMuon,VertexMuonID)) idbit |= 1 << 4;
      Muon_ID.push_back(idbit);
      

      int ssbit(0);
      if(RefMuon->passed(reco::Muon::CutBasedIdLoose))ssbit|=1<<0;
      if(RefMuon->passed(reco::Muon::CutBasedIdMedium))ssbit|=1<<1;
      if(RefMuon->passed(reco::Muon::CutBasedIdMediumPrompt))ssbit|=1<<2;
      if(RefMuon->passed(reco::Muon::CutBasedIdTight))ssbit|=1<<3;
      if(RefMuon->passed(reco::Muon::CutBasedIdGlobalHighPt))ssbit|=1<<4;
      if(RefMuon->passed(reco::Muon::CutBasedIdTrkHighPt))ssbit|=1<<5;
      if(RefMuon->passed(reco::Muon::PFIsoVeryLoose))ssbit|=1<<6;
      if(RefMuon->passed(reco::Muon::PFIsoLoose))ssbit|=1<<7;
      if(RefMuon->passed(reco::Muon::PFIsoMedium))ssbit|=1<<8;
      if(RefMuon->passed(reco::Muon::PFIsoTight))ssbit|=1<<9;
      if(RefMuon->passed(reco::Muon::PFIsoVeryTight))ssbit|=1<<10;
      if(RefMuon->passed(reco::Muon::TkIsoLoose))ssbit|=1<<11;
      if(RefMuon->passed(reco::Muon::TkIsoTight))ssbit|=1<<12;
      if(RefMuon->passed(reco::Muon::SoftCutBasedId))ssbit|=1<<13;
      if(RefMuon->passed(reco::Muon::SoftMvaId))ssbit|=1<<14;
      if(RefMuon->passed(reco::Muon::MvaLoose))ssbit|=1<<15;
      if(RefMuon->passed(reco::Muon::MvaMedium))ssbit|=1<<16; 
      if(RefMuon->passed(reco::Muon::MvaTight))ssbit|=1<<17;
      if(RefMuon->passed(reco::Muon::MiniIsoLoose))ssbit|=1<<18;
      if(RefMuon->passed(reco::Muon::MiniIsoMedium))ssbit|=1<<19;
      if(RefMuon->passed(reco::Muon::MiniIsoTight))ssbit|=1<<20;
      if(RefMuon->passed(reco::Muon::MiniIsoVeryTight))ssbit|=1<<21;
      

      Muon_StandardSelection.push_back(ssbit);
      /////////////////////////////////////////////////////////////
      //here following guide given in:
      //https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2
      


      std::vector<double> iMuon_outerTrack_p4;
      std::vector<double> iMuon_innerTrack_p4;
      if (RefMuon->isGlobalMuon()) {
	Muon_normChi2.push_back(RefMuon->globalTrack()->normalizedChi2());
	Muon_hitPattern_numberOfValidMuonHits.push_back(RefMuon->globalTrack()->hitPattern().numberOfValidMuonHits());
	Muon_trackerLayersWithMeasurement.push_back(RefMuon->innerTrack()->hitPattern().trackerLayersWithMeasurement());
	Muon_numberofValidPixelHits.push_back(RefMuon->innerTrack()->hitPattern().numberOfValidPixelHits());
	
	iMuon_outerTrack_p4.push_back(RefMuon->outerTrack()->p());
	iMuon_outerTrack_p4.push_back(RefMuon->outerTrack()->eta());
	iMuon_outerTrack_p4.push_back(RefMuon->outerTrack()->phi());
	Muon_prod_inner_outer_charge.push_back(RefMuon->outerTrack()->charge()*RefMuon->innerTrack()->charge());
	Muon_outerTrack_normalizedChi2.push_back(RefMuon->outerTrack()->normalizedChi2());
	Muon_outerTrack_muonStationsWithValidHits.push_back(RefMuon->outerTrack()->hitPattern().muonStationsWithValidHits());
	
	
	unsigned int dt1(0),dt2(0),dt3(0),dt4(0);
	unsigned int rpc1(0),rpc2(0),rpc3(0),rpc4(0);
	unsigned int csc1(0),csc2(0),csc3(0),csc4(0);
	double comb(0);
	const reco::HitPattern &pattern = RefMuon->outerTrack()->hitPattern();
	for (int i=0;i<pattern.numberOfAllHits(reco::HitPattern::TRACK_HITS);i++)
	  { 
	    uint32_t hit = pattern.getHitPattern(reco::HitPattern::TRACK_HITS,i);
	    if (pattern.validHitFilter(hit) != 1) {continue;}
	    if (pattern.getMuonStation(hit) == 1)
	      { 
		if (pattern.muonDTHitFilter(hit))  dt1++;
		if (pattern.muonRPCHitFilter(hit)) rpc1++;
		if (pattern.muonCSCHitFilter(hit)) csc1++;
	      }
	    else if (pattern.getMuonStation(hit) == 2)
	      { 
		if (pattern.muonDTHitFilter(hit))  dt2++;
		if (pattern.muonRPCHitFilter(hit)) rpc2++;
		if (pattern.muonCSCHitFilter(hit)) csc2++;
	      }
	    else if (pattern.getMuonStation(hit) == 3)
	      { 
		if (pattern.muonDTHitFilter(hit))  dt3++;
		if (pattern.muonRPCHitFilter(hit)) rpc3++;
		if (pattern.muonCSCHitFilter(hit)) csc3++;
	      }
	    else if (pattern.getMuonStation(hit) == 4)
	      { 
		if (pattern.muonDTHitFilter(hit))  dt4++;
		if (pattern.muonRPCHitFilter(hit)) rpc4++;
		if (pattern.muonCSCHitFilter(hit)) csc4++;
	      }    
	  }
	comb = (dt1+dt2+dt3+dt4)/2. + (rpc1+rpc2+rpc3+rpc4);
	csc1>6 ? comb+=6 : comb+=csc1;
	csc2>6 ? comb+=6 : comb+=csc2;
	csc3>6 ? comb+=6 : comb+=csc3;
	csc4>6 ? comb+=6 : comb+=csc4;
	Muon_vmuonhitcomb_reco.push_back(comb);
	Muon_rpchits_reco.push_back(rpc1+rpc2+rpc3+rpc4);
	
	
      } else {
	Muon_normChi2.push_back(0);
	Muon_hitPattern_numberOfValidMuonHits.push_back(0);
	Muon_trackerLayersWithMeasurement.push_back(0);
	Muon_numberofValidPixelHits.push_back(0);
	Muon_prod_inner_outer_charge.push_back(0);
	Muon_outerTrack_normalizedChi2.push_back(0);
	Muon_outerTrack_muonStationsWithValidHits.push_back(0);
	Muon_vmuonhitcomb_reco.push_back(0);
	Muon_rpchits_reco.push_back(0);
      }
      
      if (RefMuon->isTrackerMuon()) {
	Muon_innerTrack_validFraction.push_back(RefMuon->innerTrack()->validFraction());
	Muon_innerTrack_pixelLayersWithMeasurement.push_back(RefMuon->innerTrack()->hitPattern().pixelLayersWithMeasurement() );
	Muon_innerTrack_numberOfValidTrackerHits.push_back(RefMuon->innerTrack()->hitPattern().numberOfValidTrackerHits() );
	Muon_innerTrack_numberOfLostTrackerHits.push_back(RefMuon->innerTrack()->hitPattern().numberOfLostTrackerHits(HitPattern::TRACK_HITS) );
	Muon_innerTrack_numberOfLostTrackerInnerHits.push_back(RefMuon->innerTrack()->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_INNER_HITS) );
	Muon_innerTrack_numberOfLostTrackerOuterHits.push_back(RefMuon->innerTrack()->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_OUTER_HITS) );
	Muon_innerTrack_normalizedChi2.push_back(RefMuon->innerTrack()->normalizedChi2() );
	
	Muon_innerTrack_numberofValidHits.push_back(RefMuon->innerTrack()->numberOfValidHits());
	Muon_hitPattern_pixelLayerwithMeas.push_back(RefMuon->innerTrack()->hitPattern().pixelLayersWithMeasurement());
	
	Muon_innerTrack_quality.push_back(RefMuon->innerTrack()->quality(TrackBase::highPurity));
	iMuon_innerTrack_p4.push_back(RefMuon->innerTrack()->p());
	iMuon_innerTrack_p4.push_back(RefMuon->innerTrack()->eta());
	iMuon_innerTrack_p4.push_back(RefMuon->innerTrack()->phi());
      } else {
	Muon_innerTrack_quality.push_back(0);
	Muon_innerTrack_numberofValidHits.push_back(0);
	Muon_hitPattern_pixelLayerwithMeas.push_back(0);
	Muon_innerTrack_validFraction.push_back(0);
	Muon_innerTrack_pixelLayersWithMeasurement.push_back(0);
	Muon_innerTrack_numberOfValidTrackerHits.push_back(0);
	Muon_innerTrack_numberOfLostTrackerHits.push_back(0);
	Muon_innerTrack_numberOfLostTrackerInnerHits.push_back(0);
	Muon_innerTrack_numberOfLostTrackerOuterHits.push_back(0);
	Muon_innerTrack_normalizedChi2.push_back(0);
      }
      Muon_outerTrack_p4.push_back(iMuon_outerTrack_p4);
      Muon_innerTrack_p4.push_back(iMuon_innerTrack_p4);
      
      
      if (RefMuon->isIsolationValid()) {
	Muon_emEt03.push_back(Iso03.emEt);
	Muon_emVetoEt03.push_back(Iso03.emVetoEt);
	Muon_hadEt03.push_back(Iso03.hadEt);
	Muon_hadVetoEt03.push_back(Iso03.hadVetoEt);
	Muon_nJets03.push_back(Iso03.nJets);
	Muon_nTracks03.push_back(Iso03.nTracks);
	Muon_sumPt03.push_back(Iso03.sumPt);
	Muon_trackerVetoPt03.push_back(Iso03.trackerVetoPt);
	
	Muon_emEt05.push_back(Iso05.emEt);
	Muon_emVetoEt05.push_back(Iso05.emVetoEt);
	Muon_hadEt05.push_back(Iso05.hadEt);
	Muon_hadVetoEt05.push_back(Iso05.hadVetoEt);
	Muon_nJets05.push_back(Iso05.nJets);
	Muon_nTracks05.push_back(Iso05.nTracks);
	Muon_sumPt05.push_back(Iso05.sumPt);
	Muon_trackerVetoPt05.push_back(Iso05.trackerVetoPt);
      } else { // if isolation is not valid use -1 as default
	Muon_emEt03.push_back(-1);
	Muon_emVetoEt03.push_back(-1);
	Muon_hadEt03.push_back(-1);
	Muon_hadVetoEt03.push_back(-1);
	Muon_nJets03.push_back(-1);
	Muon_nTracks03.push_back(-1);
	Muon_sumPt03.push_back(-1);
	Muon_trackerVetoPt03.push_back(-1);
	
	Muon_emEt05.push_back(-1);
	Muon_emVetoEt05.push_back(-1);
	Muon_hadEt05.push_back(-1);
	Muon_hadVetoEt05.push_back(-1);
	Muon_nJets05.push_back(-1);
	Muon_nTracks05.push_back(-1);
	Muon_sumPt05.push_back(-1);
	Muon_trackerVetoPt05.push_back(-1);
      }
      
      
      //--- Fill PFMuonIsolation -----
      if (RefMuon->isPFIsolationValid()) {
	Muon_sumChargedHadronPt03.push_back(PFIso03.sumChargedHadronPt);
	Muon_sumChargedParticlePt03.push_back(PFIso03.sumChargedParticlePt);
	Muon_sumNeutralHadronEt03.push_back(PFIso03.sumNeutralHadronEt);
	Muon_sumNeutralHadronEtHighThreshold03.push_back(PFIso03.sumNeutralHadronEtHighThreshold);
	Muon_sumPhotonEt03.push_back(PFIso03.sumPhotonEt);
	Muon_sumPhotonEtHighThreshold03.push_back(PFIso03.sumPhotonEtHighThreshold);
	Muon_sumPUPt03.push_back(PFIso03.sumPUPt);
	
	Muon_sumChargedHadronPt04.push_back(PFIso04.sumChargedHadronPt);
	Muon_sumChargedParticlePt04.push_back(PFIso04.sumChargedParticlePt);
	Muon_sumNeutralHadronEt04.push_back(PFIso04.sumNeutralHadronEt);
	Muon_sumNeutralHadronEtHighThreshold04.push_back(PFIso04.sumNeutralHadronEtHighThreshold);
	Muon_sumPhotonEt04.push_back(PFIso04.sumPhotonEt);
	Muon_sumPhotonEtHighThreshold04.push_back(PFIso04.sumPhotonEtHighThreshold);
	Muon_sumPUPt04.push_back(PFIso04.sumPUPt);
      } else { // if isolation is not valid use -1 as default
	Muon_sumChargedHadronPt03.push_back(-1);
	Muon_sumChargedParticlePt03.push_back(-1);
	Muon_sumNeutralHadronEt03.push_back(-1);
	Muon_sumNeutralHadronEtHighThreshold03.push_back(-1);
	Muon_sumPhotonEt03.push_back(-1);
	Muon_sumPhotonEtHighThreshold03.push_back(-1);
	Muon_sumPUPt03.push_back(-1);
	
	Muon_sumChargedHadronPt04.push_back(-1);
	Muon_sumChargedParticlePt04.push_back(-1);
	Muon_sumNeutralHadronEt04.push_back(-1);
	Muon_sumNeutralHadronEtHighThreshold04.push_back(-1);
	Muon_sumPhotonEt04.push_back(-1);
	Muon_sumPhotonEtHighThreshold04.push_back(-1);
	Muon_sumPUPt04.push_back(-1);
      }
      
      
      ///////////////////////////////////// Muon Combined Quality /////////////////////////////////////////////////////////////////////////////////////
      //   find more about combined Muon quality in http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_9_4_4/doc/html/d4/d52/structreco_1_1MuonQuality.html
      Muon_combinedQuality_updatedSta.push_back(RefMuon->combinedQuality().updatedSta);
      Muon_combinedQuality_trkKink.push_back(RefMuon->combinedQuality().trkKink);
      Muon_combinedQuality_glbKink.push_back(RefMuon->combinedQuality().glbKink);
      Muon_combinedQuality_trkRelChi2.push_back(RefMuon->combinedQuality().trkRelChi2);
      Muon_combinedQuality_staRelChi2.push_back(RefMuon->combinedQuality().staRelChi2);
      Muon_combinedQuality_chi2LocalPosition.push_back(RefMuon->combinedQuality().chi2LocalPosition);
      Muon_combinedQuality_chi2LocalMomentum.push_back(RefMuon->combinedQuality().chi2LocalMomentum);
      Muon_combinedQuality_localDistance.push_back(RefMuon->combinedQuality().localDistance);
      Muon_combinedQuality_globalDeltaEtaPhi.push_back(RefMuon->combinedQuality().globalDeltaEtaPhi);
      Muon_combinedQuality_tightMatch.push_back(RefMuon->combinedQuality().tightMatch);
      Muon_combinedQuality_glbTrackProbability.push_back(RefMuon->combinedQuality().glbTrackProbability);
      
      Muon_calEnergy_em.push_back(RefMuon->calEnergy().em);
      Muon_calEnergy_emS9.push_back(RefMuon->calEnergy().emS9);
      Muon_calEnergy_emS25.push_back(RefMuon->calEnergy().emS25);
      Muon_calEnergy_had.push_back(RefMuon->calEnergy().had);
      Muon_calEnergy_hadS9.push_back(RefMuon->calEnergy().hadS9);
      
      Muon_segmentCompatibility.push_back(muon::segmentCompatibility(*RefMuon));
      Muon_caloCompatibility.push_back(muon::caloCompatibility(*RefMuon));
      
      Muon_ptErrOverPt.push_back(RefMuon->muonBestTrack()->ptError()/RefMuon->muonBestTrack()->pt());
      
      Muon_ptError.push_back(RefMuon->muonBestTrack()->ptError());
      Muon_phiError.push_back(RefMuon->muonBestTrack()->phiError());
      Muon_etaError.push_back(RefMuon->muonBestTrack()->etaError());

      Muon_isGoodMuon_TM2DCompatibility.push_back(muon::isGoodMuon(*RefMuon, muon::TM2DCompatibilityTight));
      Muon_isGoodMuon_TrackerMuonArbitrated.push_back(muon::isGoodMuon(*RefMuon,muon::TrackerMuonArbitrated));
      Muon_isGoodMuon_TMOneStationTight.push_back(muon::isGoodMuon(*RefMuon,muon::TMOneStationTight));
      Muon_isGoodMuon_TMOneStationAngTight.push_back(muon::isGoodMuon(*RefMuon,muon::TMOneStationAngTight));
      Muon_isGoodMuon_TMLastStationTight.push_back(muon::isGoodMuon(*RefMuon,muon::TMLastStationTight));
      Muon_isGoodMuon_TMLastStationAngTight.push_back(muon::isGoodMuon(*RefMuon,muon::TMLastStationAngTight));
      Muon_isGoodMuon_TMLastStationOptimizedLowPtTight.push_back(muon::isGoodMuon(*RefMuon,muon::TMLastStationOptimizedLowPtTight));
      Muon_isGoodMuon_TMLastStationOptimizedBarrelLowPtTight.push_back(muon::isGoodMuon(*RefMuon,muon::TMLastStationOptimizedBarrelLowPtTight));
      
      
      reco::TrackRef Track = RefMuon->track();
      int ntp = Muon_par.size();
      Muon_par.push_back(std::vector<double>());
      Muon_cov.push_back(std::vector<double>());
      if (Track.isNonnull()) {
	GlobalPoint pvpoint(Track->vx(), Track->vy(), Track->vz());
	edm::ESHandle<TransientTrackBuilder> transTrackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", transTrackBuilder);
	reco::TransientTrack transTrk = transTrackBuilder->build(Track);
	TrackParticle trackparticle = ParticleBuilder::CreateTrackParticle(transTrk, transTrackBuilder, pvpoint, true, true);
	Muon_trackCharge.push_back(trackparticle.Charge());
	Muon_pdgid.push_back(trackparticle.PDGID());
	Muon_B.push_back(trackparticle.BField());
	Muon_M.push_back(trackparticle.Mass());
	for (int i = 0; i < trackparticle.NParameters(); i++) {
	  Muon_par.at(ntp).push_back(trackparticle.Parameter(i));
	  for (int j = i; j < trackparticle.NParameters(); j++) {
	    Muon_cov.at(ntp).push_back(trackparticle.Covariance(i, j));
	  }
	}
      } else {
	Muon_trackCharge.push_back(-999);
	Muon_pdgid.push_back(-999);
	Muon_B.push_back(-999);
	Muon_M.push_back(-999);
      }
      
      int match;
      getTrackMatch(trackCollection, Track, match);
      Muon_Track_idx.push_back(match);
      sel_muon_index++;
    }
  }
  
}

void 
T3MNtuple::fillMCTruth(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if (!iEvent.isRealData())
    {
      Handle<GenParticleCollection> genParticles;
      iEvent.getByToken(genToken_, genParticles);
      if(doFullMC_){
	
	for (reco::GenParticleCollection::const_iterator itr = genParticles->begin(); itr != genParticles->end(); ++itr) {
	  if(SkipThisParticle(*itr)) continue;
	  MC_pdgid.push_back(itr->pdgId());
	  MC_charge.push_back(itr->charge());
	  std::vector<float> iMC_p4;
	  iMC_p4.push_back(itr->p4().E());
	  iMC_p4.push_back(itr->p4().Px());
	  iMC_p4.push_back(itr->p4().Py());
	  iMC_p4.push_back(itr->p4().Pz());
	  
	  MC_p4.push_back(iMC_p4);
	  MC_midx.push_back(-1);
	  MC_status.push_back(itr->status());
	  MC_childpdgid.push_back(std::vector<int>());
	  MC_childidx.push_back(std::vector<int>());
	  
	}
	unsigned int i = 0;
	for (reco::GenParticleCollection::const_iterator itr = genParticles->begin(); itr != genParticles->end(); ++itr) {
	  if(SkipThisParticle(*itr)) continue;
	  for (unsigned int d = 0; d < itr->numberOfDaughters(); d++) {
	    const reco::GenParticle *dau = static_cast<const reco::GenParticle*>(itr->daughter(d));
	    unsigned int j = 0;
	    for (reco::GenParticleCollection::const_iterator jtr = genParticles->begin(); jtr != genParticles->end(); ++jtr){
	      if(SkipThisParticle(*jtr)) continue;
	      if (dau->status() == jtr->status() && dau->p4() == jtr->p4() && dau->pdgId() == jtr->pdgId() && dau->numberOfMothers() == jtr->numberOfMothers()
		  && dau->numberOfDaughters() == jtr->numberOfDaughters()) {
		MC_midx.at(j) = i;
		MC_childidx.at(i).push_back(j);
		MC_childpdgid.at(i).push_back(dau->pdgId());
	      }
	      j++;
	    }
	  }
	  i++;
	}
      }
      
      DataMCType DMT;
      unsigned int k(0);
      for (reco::GenParticleCollection::const_iterator itr = genParticles->begin(); itr != genParticles->end(); ++itr) {
	if (DMT.isSignalParticle(itr->pdgId())) {
	  MCSignalParticle_childpdgid.push_back(std::vector<int>());
	  MCSignalParticle_childp4.push_back(std::vector<std::vector<double> >());
	  MCSignalParticle_pdgid.push_back(itr->pdgId());
	  MCSignalParticle_charge.push_back(itr->charge());
	  MCSignalParticle_Tauidx.push_back(std::vector<unsigned int>());
	
	  std::vector<double> iSig_p4;
	  iSig_p4.push_back(itr->p4().E());
	  iSig_p4.push_back(itr->p4().Px());
	  iSig_p4.push_back(itr->p4().Py());
	  iSig_p4.push_back(itr->p4().Pz());
	  MCSignalParticle_p4.push_back(iSig_p4);
	  //	  std::cout<<"Signal particle  "<<   PDGInfo::pdgIdToName(itr->pdgId()) << std::endl;
	  // look for daughter tau
	  for (unsigned int i = 0; i < itr->numberOfDaughters(); i++){
	    const reco::Candidate *dau = itr->daughter(i);
	    std::vector<double> ichildp4;
	    ichildp4.push_back(dau->p4().E());
	    ichildp4.push_back(dau->p4().Px());
	    ichildp4.push_back(dau->p4().Py());
	    ichildp4.push_back(dau->p4().Pz());

	    MCSignalParticle_childpdgid.at(MCSignalParticle_childpdgid.size() - 1).push_back(dau->pdgId());
	    MCSignalParticle_childp4.at(MCSignalParticle_childpdgid.size() - 1).push_back(ichildp4);
	    //	    std::cout<<"Signal particles product  "<< PDGInfo::pdgIdToName(dau->pdgId())<< std::endl;
	    if (abs(dau->pdgId()) == PDGInfo::tau_minus) {
	      unsigned int tauidx = MCTauandProd_p4.size();
	      MCSignalParticle_Tauidx.at(MCSignalParticle_Tauidx.size() - 1).push_back(tauidx);
	      // Analysis the tau decay
	      std::vector<const reco::GenParticle*> TauProducts = TauDecayProducts(static_cast<const reco::GenParticle*>(dau));
	      //	      std::cout<<"k  :"<< k << std::endl;
	      MCTauandProd_midx.push_back(k);
	      MCTauandProd_pdgid.push_back(std::vector<int>());
	      MCTauandProd_charge.push_back(std::vector<int>());
	      MCTauandProd_p4.push_back(std::vector<std::vector<double> >());
	      //std::cout<<"In case of tau, tauidx: "<< tauidx << std::endl;
	      for (unsigned int i = 0; i < TauProducts.size(); i++) {
		MCTauandProd_pdgid.at(tauidx).push_back(TauProducts.at(i)->pdgId());
		MCTauandProd_charge.at(tauidx).push_back(TauProducts.at(i)->charge());
		std::vector<double> iTauandProd_p4;
		iTauandProd_p4.push_back(TauProducts.at(i)->p4().E());
		iTauandProd_p4.push_back(TauProducts.at(i)->p4().Px());
		iTauandProd_p4.push_back(TauProducts.at(i)->p4().Py());
		iTauandProd_p4.push_back(TauProducts.at(i)->p4().Pz());

		MCTauandProd_p4.at(tauidx).push_back(iTauandProd_p4);
		//std::cout<<"Tau Products:   "<< PDGInfo::pdgIdToName(TauProducts.at(i)->pdgId())<< std::endl;
	      }
	    }
	  }
	  k++;
	}
      }


      Handle<vector<PileupSummaryInfo> >  PupInfo;
      iEvent.getByToken(puToken_, PupInfo);
      puN = PupInfo->begin()->getTrueNumInteractions();
    }
}


std::vector<const reco::GenParticle* > T3MNtuple::TauDecayProducts(const reco::GenParticle *Tau){
  std::vector<const reco::GenParticle* > out;
  unsigned int pdgid=abs(Tau->pdgId());
  if(pdgid==PDGInfo::tau_minus){ // check that it is a tau
    out.push_back(Tau);
    for (unsigned int i=0; i< Tau->numberOfDaughters(); i++){
      const reco::Candidate *dau=Tau->daughter(i);
      out.push_back(static_cast<const reco::GenParticle*>(dau));
    }
  }
  return out;
}


int
T3MNtuple::fillTwoMuonsAndTracks(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  Handle<trigger::TriggerEvent> triggerSummary;
  iEvent.getByToken(trigeventToken_, triggerSummary);

  std::vector<std::vector<unsigned int> > PreselectedTwoMuonsTrackCollection  = findTwoMuonsAndTrackCandidates(iEvent, iSetup);
  if(PreselectedTwoMuonsTrackCollection.size()==0){
    return 0;          //No two muons + track candidate found!
  }


  Handle<TrackCollection> trackCollection;
  iEvent.getByToken(trackToken_, trackCollection);

  Handle<MuonCollection> muonCollection;
  iEvent.getByToken(muonToken_, muonCollection);
  
  for ( auto &iTwoMuTr :  PreselectedTwoMuonsTrackCollection ) {
    vector<TransientTrack> t_trks;
    TransientVertex transVtx;
    ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
    reco::MuonRef Muon1(muonCollection, iTwoMuTr.at(0));
    reco::MuonRef Muon2(muonCollection, iTwoMuTr.at(1));

    TLorentzVector mv1,mv2;

    mv1.SetPtEtaPhiM(Muon1->pt(), Muon1->eta(), Muon1->phi(), 0.106);
    mv2.SetPtEtaPhiM(Muon2->pt(), Muon2->eta(), Muon2->phi(), 0.106);
    TrackRef track1 = Muon1->innerTrack();
    TrackRef track2 = Muon2->innerTrack();
    TrackRef track3 = TrackRef(trackCollection, iTwoMuTr.at(2));
    t_trks.push_back(theB->build(track1));
    t_trks.push_back(theB->build(track2));
    t_trks.push_back(theB->build(track3));
    KalmanVertexFitter kvf(true);
    bool FitOk(true);
    try {
      transVtx = kvf.vertex(t_trks); //KalmanVertexFitter
    } catch (...) {
      FitOk = false;
    }
    if (!transVtx.hasRefittedTracks())
      FitOk = false;
    if (transVtx.refittedTracks().size() != t_trks.size())
      FitOk = false;

    if(FitOk){
      if(transVtx.totalChiSquared() < 30){
	if((mv1 + mv2).M() < phimassmin_ || (mv1 + mv2).M() > phimassmax_){ //----  di-muon mass constraint
      //if(transVtx.totalChiSquared() < 15 && Muon1->charge()*Muon2->charge()!=1){

	TwoMuonsTrack_idx.push_back(iTwoMuTr);
	TwoMuonsTrack_SV_Chi2.push_back(transVtx.totalChiSquared());
	TwoMuonsTrack_SV_NDF.push_back(transVtx.degreesOfFreedom());
	std::vector<float> iTrigMatchdR;
	
	for (unsigned int i=0; i < iTwoMuTr.size(); i++) {
	  float match;
	  if(i <2 ){	

	    reco::MuonRef TrackTriggMatch(muonCollection, iTwoMuTr.at(i));	
	    TriggerMatch(triggerSummary,  TrackTriggMatch , TriggerMuonMatchingdr_, match);
	  } else {
	    TrackRef TrackTriggMatch = TrackRef(trackCollection, iTwoMuTr.at(i));
	    TriggerMatch(triggerSummary,  TrackTriggMatch , TriggerMuonMatchingdr_, match);
	    dump_track_index_to_fill.push_back(iTwoMuTr.at(i));
	  }
	  iTrigMatchdR.push_back(match);
	}
	TwoMuonsTrack_TriggerMatch_dR.push_back(iTrigMatchdR);
	}
      }
    }
  }
  return TwoMuonsTrack_idx.size();
}


void T3MNtuple::fillBTagJets(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  Handle<JetTagCollection> btagsCvsB;
  iEvent.getByToken(btagCvsBToken_, btagsCvsB);
  Handle<JetTagCollection> btagsCSV;
  iEvent.getByToken(btagCSVToken_, btagsCSV);
  Handle<JetTagCollection> btagsMVA;
  iEvent.getByToken(btagMVAToken_, btagsMVA);

  for(size_t j = 0 ; j < btagsCvsB->size(); j++) {
    const JetTag & btag1 = (*btagsCvsB)[j];
    if(btag1.first->pt() > 20){
      std::vector<double> iJet_p4;
      iJet_p4.push_back(btag1.first->p4().e());
      iJet_p4.push_back(btag1.first->p4().px());
      iJet_p4.push_back(btag1.first->p4().py());
      iJet_p4.push_back(btag1.first->p4().pz());
      Jet_p4.push_back(iJet_p4);
      Jet_BTagCVSB.push_back(btag1.second);
      const JetTag & btag2 = (*btagsMVA)[j];
      Jet_BTagMVA.push_back(btag2.second);
      const JetTag & btag3 = (*btagsCSV)[j];
      Jet_BTagCSV.push_back(btag3.second<0 ? 0:btag3.second);
    }
  }
}


int
T3MNtuple::fillThreeMuons(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  Handle<trigger::TriggerEvent> triggerSummary;
  iEvent.getByToken(trigeventToken_, triggerSummary);

  std::vector<std::vector<unsigned int> > PreselectedThreeMuonsCollection = findThreeMuonsCandidates(iEvent, iSetup);
  if(PreselectedThreeMuonsCollection.size()==0){
    return 0;            //No three muons candidate found! Skip the event
  }
  Handle<MuonCollection> muonCollection;
  iEvent.getByToken(muonToken_, muonCollection);
  for ( auto &iThreeMuon :  PreselectedThreeMuonsCollection ) {
    vector<TransientTrack> t_trks;   
    TransientVertex transVtx;
    ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
    reco::MuonRef Muon1(muonCollection, iThreeMuon.at(0));
    reco::MuonRef Muon2(muonCollection, iThreeMuon.at(1));
    reco::MuonRef Muon3(muonCollection, iThreeMuon.at(2));

    //    TLorentzVector mv1,mv2,mv3;

    //    mv1.SetPtEtaPhiM(Muon1->pt(), Muon1->eta(), Muon1->phi(), 0.106);
    //    mv2.SetPtEtaPhiM(Muon2->pt(), Muon2->eta(), Muon2->phi(), 0.106);
    //    mv3.SetPtEtaPhiM(Muon3->pt(), Muon3->eta(), Muon3->phi(), 0.106);


    //    TrackRef track1 = Muon1->globalTrack();
    //    TrackRef track2 = Muon2->globalTrack();
    //    TrackRef track3 = Muon3->globalTrack();


    TrackRef track1 = Muon1->innerTrack();
    TrackRef track2 = Muon2->innerTrack();
    TrackRef track3 = Muon3->innerTrack();

    t_trks.push_back(theB->build(track1));
    t_trks.push_back(theB->build(track2));
    t_trks.push_back(theB->build(track3));

    KalmanVertexFitter kvf(true);
    bool FitOk(true);
    try {
      transVtx = kvf.vertex(t_trks); //KalmanVertexFitter
    } catch (...) {
      FitOk = false;
    }
    if (!transVtx.hasRefittedTracks())
      FitOk = false;
    if (transVtx.refittedTracks().size() != t_trks.size())
      FitOk = false;

    //    if(FitOk);//  unused 


    if(transVtx.isValid()){
      if(transVtx.totalChiSquared() < 100.){ // very loose/ ndf =3

      int ntp = signalTau_lvp.size();
      signalTau_lvp.push_back(std::vector<double>());
      signalTau_cov.push_back(std::vector<double>());
      if(FitOk){
	signalTau_isLVP.push_back(1);
	LorentzVectorParticle  signalTau;
	GlobalPoint sv(transVtx.position().x(), transVtx.position().y(), transVtx.position().z());
	KinematicParticleFactoryFromTransientTrack kinFactory;
	float muMassSigma(sqrt(pow(10., -12.))), piChi(0.0), piNdf(0.0);
	std::vector<RefCountedKinematicParticle> muons;
	for (unsigned int i = 0; i <t_trks.size(); i++)
	muons.push_back(kinFactory.particle(t_trks.at(i), PDGInfo::mu_mass(), piChi, piNdf, sv, muMassSigma));
	KinematicParticleVertexFitter kpvFitter;
	RefCountedKinematicTree jpTree = kpvFitter.fit(muons);
	if(jpTree->isValid()){
	  jpTree->movePointerToTheTop();
	  const KinematicParameters parameters = jpTree->currentParticle()->currentState().kinematicParameters();
	  AlgebraicSymMatrix77 cov = jpTree->currentParticle()->currentState().kinematicParametersError().matrix();
	  
	  double c(0);
	  for (unsigned int i = 0; i < t_trks.size(); i++) {
	    c += t_trks.at(i).charge();
	  }
	  
	  TMatrixT<double> tau_par(LorentzVectorParticle::NLorentzandVertexPar, 1);
	  TMatrixTSym<double> tau_cov(LorentzVectorParticle::NLorentzandVertexPar);
	  for (int i = 0; i < LorentzVectorParticle::NLorentzandVertexPar; i++) {
	    tau_par(i, 0) = parameters(i);
	    for (int j = 0; j < LorentzVectorParticle::NLorentzandVertexPar; j++) {
	      tau_cov(i, j) = cov(i, j);
	    }
	  }
	  signalTau = LorentzVectorParticle(tau_par, tau_cov, abs(PDGInfo::tau_minus) * c, c, theB->field()->inInverseGeV(sv).z());
	  signalTau_charge.push_back(signalTau.Charge());
	  signalTau_pdgid.push_back(signalTau.PDGID());
	  signalTau_B.push_back(signalTau.BField());
	  signalTau_M.push_back(signalTau.Mass());
	  
	  for (int i = 0; i < signalTau.NParameters(); i++) {
	    signalTau_lvp.at(ntp).push_back(signalTau.Parameter(i));
	    for (int j = i; j < signalTau.NParameters(); j++) {
	      signalTau_cov.at(ntp).push_back(signalTau.Covariance(i, j));
	    }
	  }
	}
      }else{ signalTau_isLVP.push_back(-1);}
      
      ThreeMuons_idx.push_back(iThreeMuon);
      ThreeMuons_SV_Chi2.push_back(transVtx.totalChiSquared());
      ThreeMuons_SV_NDF.push_back(transVtx.degreesOfFreedom());
      std::vector<float> iTrigMatchdR;
      for ( auto &iMuon :  iThreeMuon ) {
	float match;
	reco::MuonRef MuonTriggMatch(muonCollection, iMuon);
	TLorentzVector mut;
	
	TriggerMatch(triggerSummary,  MuonTriggMatch, TriggerMuonMatchingdr_, match);
	iTrigMatchdR.push_back(match);
	
      }
      ThreeMuons_TriggerMatch_dR.push_back(iTrigMatchdR);
      }
    }
  }
  return ThreeMuons_idx.size();
}

template<class T>
void T3MNtuple::TriggerMatch(edm::Handle<trigger::TriggerEvent> &triggerSummary,  T obj, double drmax, float &match) {
  match = 999.;
  drmax = 1.;
  std::vector<trigger::TriggerObject> trgobjs = triggerSummary->getObjects();
  edm::InputTag MuonFilterTag = edm::InputTag("hltTau3muTkVertexFilter", "", "HLT"); 
  size_t MuonFilterIndex = (*triggerSummary).filterIndex(MuonFilterTag); 
  if(MuonFilterIndex < (*triggerSummary).sizeFilters()) {
    const trigger::Keys &KEYS = (*triggerSummary).filterKeys(MuonFilterIndex);
    for (unsigned int ipart = 0; ipart < KEYS.size(); ipart++) {
      double dr = reco::deltaR(trgobjs.at(KEYS.at(ipart)).eta(), trgobjs.at(KEYS.at(ipart)).phi(), obj->eta(), obj->phi());
      if (dr < drmax) {
	match = dr;
	drmax = dr;
      }
    }
  }
}



std::vector<std::vector<unsigned int> > 
T3MNtuple::findTwoMuonsAndTrackCandidates(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  BeamSpot bs;
  Handle<BeamSpot> beamSpotHandle;
  iEvent.getByToken(bsToken_, beamSpotHandle);
  bs = *beamSpotHandle;

  Handle<TrackCollection> trackCollection;
  iEvent.getByToken(trackToken_, trackCollection);

  Handle<MuonCollection> muonCollection;
  iEvent.getByToken(muonToken_, muonCollection);
  int Muon_index = 0;

  std::vector<unsigned int> preselected_muon_idx;
  std::vector<std::vector<unsigned int> > TwoMuonsPlusTrackCollection; // note that the track index goes last

  for (reco::MuonCollection::const_iterator iMuon = muonCollection->begin(); iMuon != muonCollection->end(); ++iMuon, Muon_index++) {
    reco::MuonRef RefMuon(muonCollection, Muon_index);
    if((RefMuon->pt() < MuonPtCut_) || (std::fabs(RefMuon->eta()) > MuonEtaCut_)) continue;
    if(RefMuon->isPFMuon() && (RefMuon->isTrackerMuon() || RefMuon->isGlobalMuon()))  preselected_muon_idx.push_back(Muon_index);
  }
  
  if(preselected_muon_idx.size() > 1){
    for(size_t i = 0; i < preselected_muon_idx.size()-1; ++ i){
      std::vector<unsigned int> dump_index;
      reco::MuonRef  Muon1(muonCollection, preselected_muon_idx.at(i));
      for(size_t j = i+1; j < preselected_muon_idx.size(); ++ j){
	reco::MuonRef  Muon2(muonCollection, preselected_muon_idx.at(j));

        double dz_12 = abs(Muon2->innerTrack()->dz(beamSpotHandle->position())-Muon1->innerTrack()->dz(beamSpotHandle->position()));  //   Check that two muons are
        double dr_12 = deltaR(Muon1->eta(), Muon1->phi(), Muon2->eta(), Muon2->phi());                                                //   not far from each othe

	if(dz_12>0.5 ||  dr_12>0.8)continue; // - to be checked
	unsigned int Track_index = 0;
	for (reco::TrackCollection::const_iterator iTrack = trackCollection->begin(); iTrack != trackCollection->end(); ++iTrack, Track_index++)
	  {
	    const reco::Track track = (*iTrack);
	    if(isGoodTrack(track)){
	      if(!(abs(track.dxy(beamSpotHandle->position())) < .3)  ||  !(abs(track.dz(beamSpotHandle->position())) < 20)) continue;   // check if the tracks is far from the BS.   
	          
	      double dz23 = abs(track.dz(beamSpotHandle->position())-Muon2->innerTrack()->dz(beamSpotHandle->position()));  // if the POCA of the track candidate is far from the muons - continue
	      double dz31 = abs(track.dz(beamSpotHandle->position())-Muon1->innerTrack()->dz(beamSpotHandle->position()));

	      if(dz23 > 0.5 || dz31 > 0.5)  continue;

	      double dr23 = deltaR(track.eta(), track.phi(), Muon2->eta(), Muon2->phi());
	      double dr31 = deltaR(track.eta(), track.phi(), Muon1->eta(), Muon1->phi());

	      if(dr23 > 1.2 || dr31 > 1.2)    continue;
	      if(dr23 < 0.02 || dr31 < 0.02)  continue;
	      
	      if( abs(Muon1->charge() + Muon2->charge() + track.charge())>1.1 ) continue;  // check the charge

	      dump_index.push_back(preselected_muon_idx.at(i));
	      dump_index.push_back(preselected_muon_idx.at(j));
	      dump_index.push_back(Track_index);
	      TwoMuonsPlusTrackCollection.push_back(dump_index);
	      dump_index.clear();
	    }
	  }
      }
    }
  }
  return TwoMuonsPlusTrackCollection;
}




std::vector<std::vector<unsigned int> > 
T3MNtuple::findThreeMuonsCandidates(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  BeamSpot bs;
  Handle<BeamSpot> beamSpotHandle;
  iEvent.getByToken(bsToken_, beamSpotHandle);
  bs = *beamSpotHandle;

  Handle<TrackCollection> trackCollection;
  iEvent.getByToken(trackToken_, trackCollection);

  Handle<MuonCollection> muonCollection;
  iEvent.getByToken(muonToken_, muonCollection);
  int Muon_index = 0;
  std::vector<unsigned int> preselected_muon_idx;
  std::vector<std::vector<unsigned int> > ThreeMuonsCollection;
  for (reco::MuonCollection::const_iterator iMuon = muonCollection->begin(); iMuon != muonCollection->end(); ++iMuon, Muon_index++) {
    reco::MuonRef RefMuon(muonCollection, Muon_index);
    if(AcceptedMuon(RefMuon))preselected_muon_idx.push_back(Muon_index);
    //    if((RefMuon->pt() < MuonPtCut_) || (abs(RefMuon->eta()) > MuonEtaCut_)) continue;
    //    if(RefMuon->isPFMuon() &&  ( RefMuon->isGlobalMuon() || RefMuon->isTrackerMuon()))  preselected_muon_idx.push_back(Muon_index);
  }
  if(preselected_muon_idx.size() > 2){
    for(size_t i = 0; i < preselected_muon_idx.size()-1; ++ i){
      std::vector<unsigned int> dump_index;
      reco::MuonRef  Muon1(muonCollection, preselected_muon_idx.at(i));
      for(size_t j = i+1; j < preselected_muon_idx.size(); ++ j){
	reco::MuonRef  Muon2(muonCollection, preselected_muon_idx.at(j));
	double dz_12 = abs(Muon2->innerTrack()->dz(beamSpotHandle->position())-Muon1->innerTrack()->dz(beamSpotHandle->position()));  //   Check that two muons are 
	double dr_12 = deltaR(Muon1->eta(), Muon1->phi(), Muon2->eta(), Muon2->phi());                                                //   not far from each other
	if(dz_12>0.5 ||  dr_12>0.8)continue; // - to be checked  -  this is previsou req.
	if(j<preselected_muon_idx.size()-1){
	  for(size_t k = j+1; k < preselected_muon_idx.size(); ++ k){
	    reco::MuonRef  Muon3(muonCollection, preselected_muon_idx.at(k));
	    size_t number_of_muons_pt2p5 = 0;
	    if(Muon1->pt()>2.5)number_of_muons_pt2p5++;
	    if(Muon2->pt()>2.5)number_of_muons_pt2p5++;
	    if(Muon3->pt()>2.5)number_of_muons_pt2p5++;
	    if(number_of_muons_pt2p5<2)continue;  //  Not sure it is needed; Commented.
	    double dz_23 = abs(Muon3->innerTrack()->dz(beamSpotHandle->position())-Muon2->innerTrack()->dz(beamSpotHandle->position()));
	    double dz_31 = abs(Muon3->innerTrack()->dz(beamSpotHandle->position())-Muon1->innerTrack()->dz(beamSpotHandle->position()));
	    double dr_23 = deltaR(Muon3->eta(), Muon3->phi(), Muon2->eta(), Muon2->phi());
	    double dr_31 = deltaR(Muon3->eta(), Muon3->phi(), Muon1->eta(), Muon1->phi());
	    
	    if(dr_23>0.8 || dr_31>0.8)continue; // - to be checked  -  this is previsou req.
	    if(dz_23>0.5 || dz_31>0.5)continue; // - to be checked  -  this is previsou req.
	    if(abs(Muon1->charge()+Muon2->charge()+Muon3->charge())>1.1)continue;
	    dump_index.push_back(preselected_muon_idx.at(i));
	    dump_index.push_back(preselected_muon_idx.at(j));
	    dump_index.push_back(preselected_muon_idx.at(k));
	    ThreeMuonsCollection.push_back(dump_index);
	    dump_index.clear();

	  }
	}
      }
    }
  }

  return ThreeMuonsCollection;
}




void T3MNtuple::fillTrigger(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  gtUtil_->retrieveL1(iEvent, iSetup, algToken_);
  const vector<pair<string, bool> > initialDecisions = gtUtil_->decisionsInitial();

  if (!iEvent.isRealData())
    {
      for (size_t i_l1t = 0; i_l1t < initialDecisions.size(); i_l1t++) 
	{
    	  string l1tName = (initialDecisions.at(i_l1t)).first;
	  if(l1tName.find("DoubleMu") != string::npos || l1tName.find("TripleMu") != string::npos){
	    Trigger_l1name.push_back( l1tName );
	    Trigger_l1decision.push_back( initialDecisions.at(i_l1t).second );
	    Trigger_l1prescale.push_back( 1 );
	  }
	}
    }
  else
    {
      ESHandle<L1TGlobalPrescalesVetos> psAndVetos;
      auto psRcd = iSetup.tryToGet<L1TGlobalPrescalesVetosRcd>();
      if(psRcd) psRcd->get(psAndVetos);
      int columnN= gtUtil_->prescaleColumn();
      for (size_t i_l1t = 0; i_l1t < initialDecisions.size(); i_l1t++) {
        string l1tName = (initialDecisions.at(i_l1t)).first;
	if(l1tName.find("DoubleMu") != string::npos || l1tName.find("TripleMu") != string::npos){
	  	  Trigger_l1name.push_back( l1tName );
	  	  Trigger_l1decision.push_back( initialDecisions.at(i_l1t).second );
	  	  Trigger_l1prescale.push_back( (psAndVetos->prescale_table_)[columnN][i_l1t]);
	}
      }
    } 

  Handle<TriggerResults> triggerBitsH;
  iEvent.getByToken(triggerToken_, triggerBitsH);
  const TriggerNames &triggerNames = iEvent.triggerNames( *triggerBitsH );
  for (size_t i_hlt = 0; i_hlt != triggerBitsH->size(); ++i_hlt)
    {
      string hltName = triggerNames.triggerName(i_hlt);
      if(hltName.find("HLT_DoubleMu") != string::npos){
	Trigger_hltname.push_back(hltName);
	Trigger_hltdecision.push_back(triggerBitsH->accept(i_hlt ));
      }
    }
}


void T3MNtuple::fillEventInfo(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  Event_EventNumber = iEvent.id().event();
  Event_RunNumber = iEvent.id().run();
  Event_bunchCrossing = iEvent.bunchCrossing();
  Event_orbitNumber = iEvent.orbitNumber();
  Event_luminosityBlock = iEvent.luminosityBlock();
  Event_isRealData = iEvent.isRealData();

  DataMCType DMT;
  Event_DataMC_Type = DMT.GetType();
  //  std::cout<<"Event_DataMC_Type  "<<Event_DataMC_Type << std::endl;
  if (Event_isRealData) {
    Event_DataMC_Type = DataMCType::Data;
  }
} 


void T3MNtuple::fillDsTree(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
 
  h_step->Fill(0);

  BeamSpot bs;
  Handle<BeamSpot> beamSpotHandle;
  iEvent.getByToken(bsToken_, beamSpotHandle);
  bs = *beamSpotHandle;


  Handle<VertexCollection> pvs;
  iEvent.getByToken(vtxToken_ , pvs);
  n_vtx = pvs->size();

  Handle<VertexCollection> svs;
  iEvent.getByToken(svToken_ , svs);


  Handle<MuonCollection> muons;
  iEvent.getByToken(muonToken_, muons);
  if(muons->size() < (do2mu_?2:3))return;
  h_step->Fill(1);


  //Handle<ValueMap<MuonShower> > muonShowerInformationValueMapH_;
  //iEvent.getByToken(MuonShowerInformationValueMapToken_, muonShowerInformationValueMapH_);

  //Handle<MuonCollection> badmuons;
  //iEvent.getByToken(badmuonToken_, badmuons);


  Handle<TrackCollection> trks;
  iEvent.getByToken(trackToken_, trks);

  //Handle<JetCollection> jets;
  //iEvents.getByToken(jetToken_, jets);
  Handle<JetTagCollection> btagsCvsB;
  iEvent.getByToken(btagCvsBToken_, btagsCvsB);
  Handle<JetTagCollection> btagsCSV;
  iEvent.getByToken(btagCSVToken_, btagsCSV);
  Handle<JetTagCollection> btagsMVA;
  iEvent.getByToken(btagMVAToken_, btagsMVA);

  Handle<TriggerResults> triggerBitsH;
  iEvent.getByToken(triggerToken_, triggerBitsH);

  Handle<trigger::TriggerEvent> triggerSummary;
  iEvent.getByToken(trigeventToken_, triggerSummary);

  Handle<BXVector<GlobalAlgBlk>> alg;
  iEvent.getByToken(algToken_,alg);





  //////////////////////////////
  // HLT

  hlt_doublemu4_lmnrt = 0;
  hlt_doublemu3_tau3mu = 0;

  const TriggerNames &triggerNames = iEvent.triggerNames( *triggerBitsH );
  for (size_t i_hlt = 0; i_hlt != triggerBitsH->size(); ++i_hlt)
    {
      string hltName = triggerNames.triggerName(i_hlt);

      if(!(hltName.find("HLT_DoubleMu4_LowMassNonResonantTrk_Displaced_v") == string::npos)){
        if( triggerBitsH->wasrun(i_hlt) && !triggerBitsH->error(i_hlt) && triggerBitsH->accept(i_hlt )) hlt_doublemu4_lmnrt = 1;
      }
      if(!(hltName.find("HLT_DoubleMu3_Trk_Tau3mu_") == string::npos)){
        if( triggerBitsH->wasrun(i_hlt) && !triggerBitsH->error(i_hlt) && triggerBitsH->accept(i_hlt )) hlt_doublemu3_tau3mu = 1;
      }
    }
  if(passhlt_ && hlt_doublemu3_tau3mu<0.1) return;
  h_step->Fill(2);


  //////////////////////
  // b, c origin ?
  gen_flavor=1; nmu_mom=0;
  double gen_pv = 0;
  size_t ndsgen = 0;

  if(doMC_) {

    Handle<GenParticleCollection> genParticles;
    iEvent.getByToken(genToken_, genParticles);
    const GenParticle & gen1st = (*genParticles)[2];
    gen_pv = gen1st.vz();

    for(size_t i = 2; i < genParticles->size(); ++ i) {

      const GenParticle & p = (*genParticles)[i];
      //const Candidate * mom = p.mother();
      //if(i==2)cout<<setprecision(3)<< setiosflags(ios::showpoint)
      //  <<"id: "<<p.pdgId()<<"\tstatus: "<<p.status()
      //  <<"\tpt: "<<p.pt()<<"\teta: "<<p.eta()
      //  <<"\tphi: "<<p.phi()<<"\tmass: "<<p.mass()
      //  <<"\tvx: "<<p.vx()<<"\tvy "<<p.vy()
      //  <<"\tvz: "<<p.vz()<<"\tMOTHER id: "<<mom->pdgId()
      //  <<"\tMOTHER pt:"<<mom->pt()<<endl<<endl;

      if(abs(p.pdgId())==4 && gen_flavor==1){gen_flavor=4;}
      if(abs(p.pdgId())==5 && gen_flavor==1){gen_flavor=5;}

      if(abs(p.pdgId())==431) ndsgen++; // Ds
      if(abs(p.pdgId())!=13) continue; // mu
      if(abs(p.mother()->pdgId())!=mid_) continue; // phi (norm. channel), or tau (signal channel)
      nmu_mom++;
    }
    //if(ndsgen>1)return;
    //if(nmu_mom>3)return; // why ???
  }

  //h_step->Fill(2);

  ///////////////////////////
  // start to loop over muons
  n_reco = 0;
  pdgid_reco[0] = 0; pdgid_reco[1] = 0; pdgid_reco[2] = 0;
  momid_reco[0] = 0; momid_reco[1] = 0; momid_reco[2] = 0;
  vxy_reco[0] = 0; vxy_reco[1] = 0; vxy_reco[2] = 0;
  size_t j1 = 0, j2 = 0, j3 = 0;
  TLorentzVector vtau, vm12 ;
  double min_fvnC_2mu1tk = 10, min_fvnC = 100;
  int iTrk = 9999;
  vector<size_t> kinematic_muon_index;

  for(size_t i = 0; i < muons->size(); ++ i) 
    {
      const Muon & mu = (*muons)[i];
      if(!(mu.pt() > MuonPtCut_) || !(abs(mu.eta()) < MuonEtaCut_))continue;
      //    bool isID = false;
      if(mu.isPFMuon() && mu.isGlobalMuon()) kinematic_muon_index.push_back(i);// isID=true;
      //if(mu.isPFMuon() && (mu.isGlobalMuon()||mu.isTrackerMuon())) isID=true;
      
      //if( muon::isGoodMuon(mu, muon::TMOneStationTight)
      //  && mu.innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5
      //  && mu.innerTrack()->hitPattern().pixelLayersWithMeasurement() > 0
      //  //if(!(m_1.innerTrack()->quality(TrackBase::highPurity)))continue;
      //  && abs(mu.innerTrack()->dxy(beamSpotHandle->position())) < 0.3
      //  && abs(mu.innerTrack()->dz(beamSpotHandle->position())) < 20
      //) isID=true;
      
      //    if(isID)kinematic_muon_index.push_back(i);
    }
  if(kinematic_muon_index.size() < (do2mu_?2:3)) return;  //  if kinematic_muon < 3  - continue
  h_step->Fill(3);



  for(size_t i = 0; i < kinematic_muon_index.size()-1; ++ i)      //  loop over muons passed kinematic cuts
    {
      const Muon & m_1 = (*muons)[kinematic_muon_index[i]];


      for(size_t j = i+1; j < kinematic_muon_index.size(); ++ j)   // second loop to find a muon pair
	{
	  const Muon & m_2 = (*muons)[kinematic_muon_index[j]];

	  double dz12 = abs(m_2.innerTrack()->dz(beamSpotHandle->position())-m_1.innerTrack()->dz(beamSpotHandle->position()));  //   DeltaZ
	  double dr12 = deltaR(m_1.eta(), m_1.phi(), m_2.eta(), m_2.phi());                                                      //   DeltaR

	  if(dz12>0.5 ||  dr12>0.8)continue; // if Delta POCA_Z of two muon candiadate  > 0.5 cm or large deltaR  - skip the pair  candidate
	  if(j<kinematic_muon_index.size()-1) 
	    {
	      for(size_t k = j+1; k < kinematic_muon_index.size(); ++ k) 
		{
		  const Muon & m_3 = (*muons)[kinematic_muon_index[k]];

		  size_t n_muons_pt2p5 = 0;
		  if(m_1.pt()>2.5)n_muons_pt2p5++;
		  if(m_2.pt()>2.5)n_muons_pt2p5++;
		  if(m_3.pt()>2.5)n_muons_pt2p5++;
		  if(n_muons_pt2p5<2)continue;  //  ?? only to reduce the ntuple size ?

		  double dz23 = abs(m_3.innerTrack()->dz(beamSpotHandle->position())-m_2.innerTrack()->dz(beamSpotHandle->position()));
		  double dz31 = abs(m_3.innerTrack()->dz(beamSpotHandle->position())-m_1.innerTrack()->dz(beamSpotHandle->position()));
		  if(dz23>0.5 || dz31>0.5)continue;
		  double dr23 = deltaR(m_3.eta(), m_3.phi(), m_2.eta(), m_2.phi());
		  double dr31 = deltaR(m_3.eta(), m_3.phi(), m_1.eta(), m_1.phi());
		  if(dr23>0.8 || dr31>0.8)continue;



		  if(abs(m_1.charge()+m_2.charge()+m_3.charge())>1.1)continue;

		  vector<TransientTrack> t_trks;   
		  ESHandle<TransientTrackBuilder> theB;
		  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
		  TrackRef trk1 = m_1.innerTrack();
		  TrackRef trk2 = m_2.innerTrack();
		  TrackRef trk3 = m_3.innerTrack();
		  t_trks.push_back(theB->build(trk1));
		  t_trks.push_back(theB->build(trk2));
		  t_trks.push_back(theB->build(trk3));
		  KalmanVertexFitter kvf;
		  TransientVertex fv = kvf.vertex(t_trks);

		  if(!fv.isValid()) continue; // eff ? 
		  double fvnC_tmp = fv.totalChiSquared()/fv.degreesOfFreedom();
		  //		  std::cout<<" fvnC_tmp  " <<  fv.totalChiSquared() <<std::endl;
		  //vtau.SetPxPyPzE(m_1.px()+m_2.px()+m_3.px(), m_1.py()+m_2.py()+m_3.py(), m_1.pz()+m_2.pz()+m_3.pz(), m_1.energy()+m_2.energy()+m_3.energy());
	  
		  if(n_reco==0)n_reco=3;
		  else {n_reco++;}

	  
		  //if(vtau.M() >  max_mtau){ // keep the max mass
		  if(fvnC_tmp < min_fvnC)  // check the loose quality of the SV
		    {
		      if(m_1.p()>m_2.p())  // sort muons by momentum
			{
			  if(m_2.p()>m_3.p())
			    {
			      j1=kinematic_muon_index[i]; j2=kinematic_muon_index[j]; j3=kinematic_muon_index[k];
			    }
			  else if(m_1.p()>m_3.p())
			    {
			      j1=kinematic_muon_index[i]; j2=kinematic_muon_index[k]; j3=kinematic_muon_index[j];
			    }
			  else
			    {
			      j1=kinematic_muon_index[k]; j2=kinematic_muon_index[i]; j3=kinematic_muon_index[j];
			    }
			}
		      else 
			{
			  if(m_1.p()>m_3.p())
			    {
			      j1=kinematic_muon_index[j]; j2=kinematic_muon_index[i]; j3=kinematic_muon_index[k];
			    }
			  else if(m_2.p()>m_3.p())
			    {
			      j1=kinematic_muon_index[j]; j2=kinematic_muon_index[k]; j3=kinematic_muon_index[i];
			    }
			  else 
			    {
			      j1=kinematic_muon_index[k]; j2=kinematic_muon_index[j]; j3=kinematic_muon_index[i];
			    }
			}
	      
		      min_fvnC = fvnC_tmp; // select finally the three_muon candidate by the best vertex;
		    }
		}
	    }

	  if(n_reco<3 && do2mu_ && m_1.pt() > 2.5 && m_2.pt() > 2.5)  ////////// if do 2mu+1trk
	    {
	      for(size_t itk = 0; itk < trks->size(); itk++)
		{
		  const Track & t = (*trks)[itk];

		  if(!isGoodTrack(t)) continue;
		  if(!(abs(t.dxy(beamSpotHandle->position())) < .3)  ||  !(abs(t.dz(beamSpotHandle->position())) < 20)) continue;   // check if the tracks is far from the BS.


	    
		  double dz23 = abs(t.dz(beamSpotHandle->position())-m_2.innerTrack()->dz(beamSpotHandle->position()));  // if the POCA of the track candidate is far from the muons - continue
		  double dz31 = abs(t.dz(beamSpotHandle->position())-m_1.innerTrack()->dz(beamSpotHandle->position()));

		  if(dz23 > 0.5 || dz31 > 0.5)  continue;

		  double dr23 = deltaR(t.eta(), t.phi(), m_2.eta(), m_2.phi());
		  double dr31 = deltaR(t.eta(), t.phi(), m_1.eta(), m_1.phi());

		  if(dr23 > 1.2 || dr31 > 1.2)    continue;
		  if(dr23 < 0.02 || dr31 < 0.02)  continue;

		  if( abs(m_1.charge()+m_2.charge()+t.charge())>1.1 ) continue;  // check the charge
	    
		  vector<TransientTrack> t_trks;
		  ESHandle<TransientTrackBuilder> theB;
		  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
		  TrackRef trk1 = m_1.innerTrack();
		  TrackRef trk2 = m_2.innerTrack();
		  TrackRef trk3 = TrackRef(trks, itk);
		  t_trks.push_back(theB->build(trk1));
		  t_trks.push_back(theB->build(trk2));
		  t_trks.push_back(theB->build(trk3));
		  KalmanVertexFitter kvf;
		  TransientVertex fv = kvf.vertex(t_trks);
		  if(!fv.isValid()) continue;
		  double fv_tC = fv.totalChiSquared();
		  double fv_dOF = fv.degreesOfFreedom();
		  double fv_nC = fv_tC/fv_dOF;
		  if(fv_nC > 5) continue;  // why 5 ?
		  if(fv_nC < min_fvnC_2mu1tk){
	      
		    //double t_energy = sqrt(0.140*0.140 + t.p()*t.p()); // pion mass
		    //vtau.SetPxPyPzE(m_1.px()+m_2.px()+t.px(), m_1.py()+m_2.py()+t.py(), m_1.pz()+m_2.pz()+t.pz(), m_1.energy()+m_2.energy()+t_energy);
		    //vm12.SetPxPyPzE(m_1.px()+m_2.px(), m_1.py()+m_2.py(), m_1.pz()+m_2.pz(), m_1.energy()+m_2.energy());
		    //if(min_fvnC_2mu1tk>9.99)cout<<endl;
		    //cout<<eventN<<"\t"<<fv_nC<<"\t"<<vm12.M()<<"\t"<<vtau.M()<<endl;
		    //if(vtau.M() >  max_mtau_2mu[0]t) // keep the max mass
	      
		    iTrk=itk; // index of the survived candidate
		    n_reco=2; //  2mu + track category
	      
		    if(m_1.p()>m_2.p()) // sort muons by p
		      {
			j1=kinematic_muon_index[i]; j2=kinematic_muon_index[j];
		      }
		    else 
		      {
			j1=kinematic_muon_index[j]; j2=kinematic_muon_index[i];
		      }

		    //max_mtau_2mu[0]t = vtau.M();
		    min_fvnC_2mu1tk = fv_nC;
	      
		  }
	    
		} // loop of tracks
	
	    } // if (n_reco<3)
      
	}
    
    }

  h_n3mu->Fill(n_reco);
  if(n_reco < (do2mu_?2:3)) return; 
  h_step->Fill(4);

  vector<Muon> mu; 
  mu.push_back((*muons)[j1]);
  mu.push_back((*muons)[j2]);
  mu.push_back((*muons)[j3]);


  ///////////////////
  // Trigger Objects
  edm::InputTag MuonFilterTag = edm::InputTag("hltTau3muTkVertexFilter", "", "HLT"); 
  size_t MuonFilterIndex = (*triggerSummary).filterIndex(MuonFilterTag); 
  trigger::TriggerObjectCollection allTriggerObjects = triggerSummary->getObjects();
  trigger::TriggerObjectCollection MuonLegObjects;
  if(MuonFilterIndex < (*triggerSummary).sizeFilters()) {
    const trigger::Keys &keysMuons = (*triggerSummary).filterKeys(MuonFilterIndex);
    for(size_t j = 0; j < keysMuons.size(); j++ ){
      trigger::TriggerObject foundObject = (allTriggerObjects)[keysMuons[j]];
      MuonLegObjects.push_back(foundObject);
    }
  }

  //cout<<MuonLegObjects.size()<<"\t"<<n_reco<<endl;

  if(n_reco >= 3 && doMC_){

    Handle<GenParticleCollection> genParticles2;
    iEvent.getByToken(genToken_, genParticles2);

    double dr = 0.03;

    //cout<<endl<<genParticles2->size()<<endl;
    for(size_t i = 2; i < genParticles2->size(); ++ i) {

      const GenParticle & p = (*genParticles2)[i];
      //const Candidate * mom = p.mother();

      //if(abs(p.pdgId())==13)cout<<p.status()<<" mom: "<<mom->pdgId()<<endl;
      if(p.charge()==0)continue;
      if(p.status()!=1)continue;
      if(p.p()<2.5)continue;
      if(abs(p.eta())>2.45)continue;
      //double dpt=3*(0.6+abs(p.eta()))/100;

      const Candidate * mom = p.mother();

      for(int j = 0; j < 3; j++) {

	if(pdgid_reco[j]==0){
	  double dpt = 3*mu[j].muonBestTrack()->ptError();
	  double dr1 = deltaR(mu[j].eta(), mu[j].phi(), p.eta(), p.phi());
	  double dpt1 = abs(mu[j].pt()-p.pt());
	  if(dr1 < dr && mu[j].charge()==p.charge() && dpt1 < dpt) {
	    pdgid_reco[j] = p.pdgId();
	    momid_reco[j] = mom->pdgId();
	    vxy_reco[j] = sqrt(p.vx()*p.vx() + p.vy()*p.vy());
	  }
	}

      }
    }
  }

  TrackRef t3 = TrackRef(trks, 0);
  if(n_reco>2) t3 = mu[2].innerTrack();
  else t3 = TrackRef(trks, iTrk);


  double t3_energy = sqrt(0.140*0.140 + t3->p()*t3->p()); // pion
  if(n_reco>2) t3_energy = sqrt(0.106*0.106 + t3->p()*t3->p()); // muon

  vtau.SetPxPyPzE(mu[0].px()+mu[1].px()+t3->px(), mu[0].py()+mu[1].py()+t3->py(), mu[0].pz()+mu[1].pz()+t3->pz(), mu[0].energy()+mu[1].energy()+t3_energy);


  m3mu_reco = vtau.M();



  double pt12 = (mu[0].pt()+mu[1].pt());
  double eta12 = (mu[0].eta()*mu[0].pt() + mu[1].eta()*mu[1].pt())/pt12;
  double phi12 = (mu[0].phi()*mu[0].pt() + mu[1].phi()*mu[1].pt())/pt12;
  m3mu_simp = sqrt(2*pt12*t3->pt()*( cosh(eta12-t3->eta()) - cos(phi12-t3->phi())));

  if(wideSB_) {
    if(m3mu_reco>3.2||m3mu_reco<1.)return;
  } else {
    if(m3mu_reco>2.05||m3mu_reco<1.6)return;
  }

  h_step->Fill(5);


  ////////////////////
  // 2 mu mass
  TLorentzVector mv1, mv2, mv3;
  vector<TLorentzVector> mv;
  mv1.SetPtEtaPhiM(mu[0].pt(), mu[0].eta(), mu[0].phi(), 0.106);
  mv2.SetPtEtaPhiM(mu[1].pt(), mu[1].eta(), mu[1].phi(), 0.106);
  mv3.SetPtEtaPhiM(t3->pt(), t3->eta(), t3->phi(), n_reco>2?0.106:0.140);

  if(n_reco> 2){
    //  std::cout<<"Ds Tree  "<< std::endl;
  //  mv1.Print();
    //  mv2.Print();
    //  mv3.Print();
  }



  m2mu_12 = (mv1+mv2).M();
  if(n_reco==2 && abs(m2mu_12-1.02)>0.02)return;
  m2mu_23 = (mv2+mv3).M();
  m2mu_31 = (mv3+mv1).M();
  m2mu_min= TMath::Min(m2mu_12, TMath::Min(m2mu_23, m2mu_31));
  m2mu_max= TMath::Max(m2mu_12, TMath::Max(m2mu_23, m2mu_31));

  if(abs(mu[0].charge()+mu[1].charge())>1) {
    mv.push_back(mv3); mv.push_back(mv1); mv.push_back(mv2);
  }
  else if (abs(mu[1].charge()+t3->charge())>1) {
    mv.push_back(mv1); mv.push_back(mv2); mv.push_back(mv3);
  }
  else if (abs(t3->charge()+mu[0].charge())>1) {
    mv.push_back(mv2); mv.push_back(mv3); mv.push_back(mv1);
  }
  double m2mu_osa=(mv[0]+mv[1]).M();
  double m2mu_osb=(mv[0]+mv[2]).M();
  m2mu_ss = (mv[1]+mv[2]).M();
  m2mu_os1 = (TMath::Max(m2mu_osa, m2mu_osb));
  m2mu_os2 = (TMath::Min(m2mu_osa, m2mu_osb));

  pt3mu_reco = vtau.Pt();
  p3mu_reco = vtau.P();
  eta3mu_reco = vtau.Eta();
  pt2mu_12 = sqrt((mu[0].px()+mu[1].px())*(mu[0].px()+mu[1].px()) + (mu[0].py()+mu[1].py())*(mu[0].py()+mu[1].py()));

  pt_max = 0; pt_min = 999;
  if(mu[0].pt()>pt_max)pt_max=mu[0].pt();
  if(mu[1].pt()>pt_max)pt_max=mu[1].pt();
  if(t3->pt()>pt_max)pt_max=t3->pt();
  if(mu[0].pt()<pt_min)pt_min=mu[0].pt();
  if(mu[1].pt()<pt_min)pt_min=mu[1].pt();
  if(t3->pt()<pt_min)pt_min=t3->pt();


  // Event based trigger matching
  trigmat_new = 0;
  for(size_t it = 0; it < MuonLegObjects.size(); it = it+3) {
    const trigger::TriggerObject & to1 = MuonLegObjects[it];
    const trigger::TriggerObject & to2 = MuonLegObjects[it+1];
    const trigger::TriggerObject & to3 = MuonLegObjects[it+2];
    bool mat1 = false, mat2 = false, mat3 = false;
    for(int i = 0; i < 2 ; i++) {
      if(n_reco> 2){
	//      std::cout<<"  dr1  "<< deltaR(mu[i].eta(), mu[i].phi(), to1.eta(), to1.phi()) << std::endl;
	//      std::cout<<"  dr2  "<< deltaR(mu[i].eta(), mu[i].phi(), to2.eta(), to2.phi()) << std::endl;
	//      std::cout<<"  dr3  "<< deltaR(mu[i].eta(), mu[i].phi(), to3.eta(), to3.phi()) << std::endl;
      }

      if(deltaR(mu[i].eta(), mu[i].phi(), to1.eta(), to1.phi())<0.03 && abs(mu[i].pt()-to1.pt())/mu[i].pt()<0.1) mat1=true;
      if(deltaR(mu[i].eta(), mu[i].phi(), to2.eta(), to2.phi())<0.03 && abs(mu[i].pt()-to2.pt())/mu[i].pt()<0.1) mat2=true;
      if(n_reco>2)if(deltaR(mu[i].eta(), mu[i].phi(), to3.eta(), to3.phi())<0.03 && abs(mu[i].pt()-to3.pt())/mu[i].pt()<0.1) mat3=true;
    }

    if(n_reco>2) {
      if(deltaR(t3->eta(), t3->phi(), to1.eta(), to1.phi())<0.03 && abs(t3->pt()-to1.pt())/t3->pt()<0.1) mat1=true;
      if(deltaR(t3->eta(), t3->phi(), to2.eta(), to2.phi())<0.03 && abs(t3->pt()-to2.pt())/t3->pt()<0.1) mat2=true;
    }
    if(deltaR(t3->eta(), t3->phi(), to3.eta(), to3.phi())<0.03 && abs(t3->pt()-to3.pt())/t3->pt()<0.1) mat3=true;

    if(mat1 && mat2 && mat3) {trigmat_new = 1; break;}
  }
  std::cout<<"trigmat_new  "<< trigmat_new << std::endl;



  for(int i = 0; i < 3; i++) {  // loop muon

    //if(hlt_doublemu3_tau3mu)cout<<"reco mu"<<i<<"  "<<mu[i].pdgId()<<" "<<mu[i].pt()<<" "<<mu[i].eta()<<" "<<mu[i].phi()<<endl;

    // global muon variables
    p_out[i] = 0; eta_out[i] = -10;
    phi_out[i] = -10; p_glb[i] = 0;
    eta_glb[i] = -10; phi_glb[i] = -10;
    glbnC_reco[i] = 100; nOVMH_reco[i] = 0;
    mSWVH_reco[i] = 0, qprod_reco[i] = 0;
    outerchi2_reco[i] = 1000; vmuonhitcomb_reco[i] = 0;
    rpchits_reco[i] = 0;
    cschits_sta1[i] = 0;
    cscchi2_sta1[i] = 100;
    cschits_sta2[i] = 0;
    cscchi2_sta2[i] = 100;
    cscdxdz_sta1[i] = 0;
    cscdxdz_sta2[i] = 0;
    cscdydz_sta1[i] = 0;
    cscdydz_sta2[i] = 0;
    cscnsegm_sta1[i] = 0;
    cscnsegm_sta2[i] = 0;


    if(i==2 && n_reco<3) {

      uSta_reco[i] = 0;
      tKink_reco[i] = 0;
      gKink_reco[i] = 0;
      tRC2_reco[i] = 0;
      sRC2_reco[i] = 0;
      cLP_reco[i] = 0;
      cLM_reco[i] = 0;
      lDist_reco[i] = 0;
      gDEP_reco[i] = 0;
      tMat_reco[i] = 0;
      gTP_reco[i] = 0;
      calem_reco[i] = 0;
      calemS9_reco[i] = 0;
      calemS25_reco[i] = 0;
      calhad_reco[i] = 0;
      calhadS9_reco[i] = 0;
      nOMS_reco[i] = 99;
      nOM_reco[i] = 0;
      comp2d_reco[i] = 0;
      calocomp_reco[i] = 0;
      segmcomp_reco[i] = 0;
      segmcomp_0[i] = 0;
      segmcomp_1[i] = 0;
      segmcomp_2[i] = 0;
      segmcomp_3[i] = 0;
      //segmcomp_4[i] = 0;
      //segmcomp_5[i] = 0;
      //segmcomp_6[i] = 0;
      //segmcomp_7[i] = 0;
      pf_reco[i] = 0;
      rpcmu_reco[i] = 0;
      Iso_sumPt[i] = 0;
      Iso_nTr[i] = 0;
      Iso_emEt[i] = 0;
      Iso_hadEt[i] = 0;
      Iso_eVE[i] = 0;
      Iso_hVE[i] = 0;
      tma_reco[i] = 0;
      tmost_reco[i] = 0;
      tmosat_reco[i] = 0;
      tmlst_reco[i] = 0;
      tmlsat_reco[i] = 0;
      tmlsolpt_reco[i] = 0;
      tmlsoblpt_reco[i] = 0;
      timeatipinouterr_reco[i] = -1;

      pterr_reco[i] = t3->ptError()/t3->pt();
      trkhp_reco[i] = t3->quality(TrackBase::highPurity);
      charge_reco[i] = t3->charge();
      p_in[i] = t3->p();
      eta_in[i] = t3->eta();
      phi_in[i] = t3->phi();
      p_reco[i] = t3->p();
      pt_reco[i] = t3->pt();
      eta_reco[i] = t3->eta();
      phi_reco[i] = t3->phi();
      nOVPH_reco[i] = t3->hitPattern().numberOfValidPixelHits();
      iTvF_reco[i] = t3->validFraction();
      tLWM_reco[i] = t3->hitPattern().trackerLayersWithMeasurement();
      pLWM_reco[i] = t3->hitPattern().pixelLayersWithMeasurement();
      nOVTH_reco[i] = t3->hitPattern().numberOfValidTrackerHits();
      nOLTH_reco[i] = t3->hitPattern().numberOfLostTrackerHits(HitPattern::TRACK_HITS);
      nOLTHin_reco[i] = t3->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_INNER_HITS);
      nOLTHout_reco[i] = t3->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_OUTER_HITS);
      drtau_reco[i] = deltaR(t3->eta(), t3->phi(), vtau.Eta(), vtau.Phi());
      iTnC_reco[i] = t3->normalizedChi2();
      //	    cout<<" p_in  "  << p_in[i] << " i  "<<i << endl;
      trigmat_reco[i] = 0;
      for(size_t it = 0; it < MuonLegObjects.size(); it ++) {
	const trigger::TriggerObject & to = MuonLegObjects[it];
	if(deltaR(eta_reco[i], phi_reco[i], to.eta(), to.phi())<0.03 && abs(pt_reco[i]-to.pt())/pt_reco[i]<0.1)trigmat_reco[i] = to.id();  // was 0.05 and 0.3
      }

      continue;
    }




    ///////////////////////////////////// Muon Combined Quality /////////////////////////////////////////////////////////////////////////////////////
    //   find more about combined Muon quality in http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_9_4_4/doc/html/d4/d52/structreco_1_1MuonQuality.html


    charge_reco[i] = mu[i].charge();
    uSta_reco[i] = mu[i].combinedQuality().updatedSta;
    tKink_reco[i] = mu[i].combinedQuality().trkKink;
    gKink_reco[i] = TMath::Log(2+mu[i].combinedQuality().glbKink);
    tRC2_reco[i] = mu[i].combinedQuality().trkRelChi2;
    sRC2_reco[i] = mu[i].combinedQuality().staRelChi2;
    cLP_reco[i] = mu[i].combinedQuality().chi2LocalPosition;
    cLM_reco[i] = mu[i].combinedQuality().chi2LocalMomentum;
    lDist_reco[i] = mu[i].combinedQuality().localDistance;
    gDEP_reco[i] = mu[i].combinedQuality().globalDeltaEtaPhi;
    tMat_reco[i] = mu[i].combinedQuality().tightMatch;
    gTP_reco[i] = mu[i].combinedQuality().glbTrackProbability;
    calem_reco[i] = mu[i].calEnergy().em;
    calemS9_reco[i] = mu[i].calEnergy().emS9;
    calemS25_reco[i] = mu[i].calEnergy().emS25;
    calhad_reco[i] = mu[i].calEnergy().had;
    calhadS9_reco[i] = mu[i].calEnergy().hadS9;
    nOMS_reco[i] = mu[i].numberOfMatchedStations();
    nOM_reco[i] = mu[i].numberOfMatches(reco::Muon::SegmentArbitration);
    comp2d_reco[i] = muon::isGoodMuon(mu[i], muon::TM2DCompatibilityTight);
    calocomp_reco[i] = muon::caloCompatibility(mu[i]);
    segmcomp_reco[i] = muon::segmentCompatibility(mu[i]);
    //segmcomp_0[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 0);
    //segmcomp_1[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 1);
    //segmcomp_2[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 2);
    //segmcomp_3[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 3);
    //segmcomp_4[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 4);
    //segmcomp_5[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 5);
    //segmcomp_6[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 6);
    //segmcomp_7[i] = scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 7);

    //if(mu[i].pt()<8 && abs(mu[i].eta())<1.2 && mu[i].isGlobalMuon()) 
    //
    //if(mu[i].isGlobalMuon()) {
    //  cout<<endl<<"mu "<<i<<"  p: "<<mu[i].p()<<"  eta: "<<mu[i].eta()<<"  nOMS: "<<nOMS_reco[i]
    //      <<"  pdgid: "<<pdgid_reco[i]<<"  momid: "<<momid_reco[i]<<endl;
    //  cout<<"segmComp  Old: "<<segmcomp_reco[i]<<"  New: "<<scnew(mu[i], reco::Muon::SegmentAndTrackArbitration, 7)<<endl;
    //}
    trkhp_reco[i] = mu[i].innerTrack()->quality(TrackBase::highPurity);
    pf_reco[i] = mu[i].isPFMuon();
    rpcmu_reco[i] = mu[i].isRPCMuon();
    p_reco[i] = mu[i].p();
    pt_reco[i] = mu[i].pt();
    eta_reco[i] = mu[i].eta();
    phi_reco[i] = mu[i].phi();
    pterr_reco[i] = mu[i].muonBestTrack()->ptError()/mu[i].muonBestTrack()->pt();
    p_in[i] = mu[i].innerTrack()->p();
    eta_in[i] = mu[i].innerTrack()->eta();
    phi_in[i] = mu[i].innerTrack()->phi();
    nOVPH_reco[i] = mu[i].innerTrack()->hitPattern().numberOfValidPixelHits();
    iTvF_reco[i] = mu[i].innerTrack()->validFraction();
    tLWM_reco[i] = mu[i].innerTrack()->hitPattern().trackerLayersWithMeasurement();
    pLWM_reco[i] = mu[i].innerTrack()->hitPattern().pixelLayersWithMeasurement();
    nOVTH_reco[i] = mu[i].innerTrack()->hitPattern().numberOfValidTrackerHits();
    nOLTH_reco[i] = mu[i].innerTrack()->hitPattern().numberOfLostTrackerHits(HitPattern::TRACK_HITS);
    nOLTHin_reco[i] = mu[i].innerTrack()->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_INNER_HITS);
    nOLTHout_reco[i] = mu[i].innerTrack()->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_OUTER_HITS);
    drtau_reco[i] = deltaR(mu[i].eta(), mu[i].phi(), vtau.Eta(), vtau.Phi());
    Iso_sumPt[i] = mu[i].isolationR03().sumPt;
    Iso_nTr[i] = mu[i].isolationR03().nTracks;
    Iso_emEt[i] = mu[i].isolationR03().emEt;
    Iso_hadEt[i] = mu[i].isolationR03().hadEt;
    Iso_eVE[i] = mu[i].isolationR03().emVetoEt;
    Iso_hVE[i] = mu[i].isolationR03().hadVetoEt;
    iTnC_reco[i] = mu[i].innerTrack()->normalizedChi2();
    tma_reco[i] = muon::isGoodMuon(mu[i], muon::TrackerMuonArbitrated);
    tmost_reco[i] = muon::isGoodMuon(mu[i], muon::TMOneStationTight);
    tmosat_reco[i] = muon::isGoodMuon(mu[i], muon::TMOneStationAngTight);
    tmlst_reco[i] = muon::isGoodMuon(mu[i], muon::TMLastStationTight);
    tmlsat_reco[i] = muon::isGoodMuon(mu[i], muon::TMLastStationAngTight);
    tmlsolpt_reco[i] = muon::isGoodMuon(mu[i], muon::TMLastStationOptimizedLowPtTight);
    tmlsoblpt_reco[i] = muon::isGoodMuon(mu[i], muon::TMLastStationOptimizedBarrelLowPtTight);
    timeatipinouterr_reco[i] = mu[i].time().timeAtIpInOutErr;


    trigmat_reco[i] = 0;
    for(size_t it = 0; it < MuonLegObjects.size(); it ++) {
      const trigger::TriggerObject & to = MuonLegObjects[it];
      //if(i==0)cout<<"hlt  mu"<<it<<"  "<<to.id()<<" "<<to.pt()<<" "<<to.eta()<<" "<<to.phi()<<endl;
      if(deltaR(eta_reco[i], phi_reco[i], to.eta(), to.phi())<0.03 && abs(pt_reco[i]-to.pt())/pt_reco[i]<0.1)trigmat_reco[i] = to.id();  // was 0.05 and 0.3
    } 

  
    // global muon variables
    if(!mu[i].isGlobalMuon()) {
      continue;
    }

    p_out[i] = mu[i].outerTrack()->p();
    eta_out[i] = mu[i].outerTrack()->eta();
    phi_out[i] = mu[i].outerTrack()->phi();
    outerchi2_reco[i] = mu[i].outerTrack()->normalizedChi2();
    qprod_reco[i] = mu[i].outerTrack()->charge()*mu[i].innerTrack()->charge();

    p_glb[i] = mu[i].globalTrack()->p();
    eta_glb[i] = mu[i].globalTrack()->eta();
    phi_glb[i] = mu[i].globalTrack()->phi();
    glbnC_reco[i] = mu[i].globalTrack()->normalizedChi2();
    nOVMH_reco[i] = mu[i].globalTrack()->hitPattern().numberOfValidMuonHits();
    mSWVH_reco[i] = mu[i].outerTrack()->hitPattern().muonStationsWithValidHits();

    unsigned int dt1(0),dt2(0),dt3(0),dt4(0);
    unsigned int rpc1(0),rpc2(0),rpc3(0),rpc4(0);
    unsigned int csc1(0),csc2(0),csc3(0),csc4(0);
    double comb(0);
    const reco::HitPattern &pattern = mu[i].outerTrack()->hitPattern();
    for (int i=0;i<pattern.numberOfAllHits(reco::HitPattern::TRACK_HITS);i++)
      { 
	uint32_t hit = pattern.getHitPattern(reco::HitPattern::TRACK_HITS,i);
	if (pattern.validHitFilter(hit) != 1) {continue;}
	if (pattern.getMuonStation(hit) == 1)
	  { 
	    if (pattern.muonDTHitFilter(hit)) {dt1++;}
	    if (pattern.muonRPCHitFilter(hit)) {rpc1++;}
	    if (pattern.muonCSCHitFilter(hit)) {csc1++;}
	  }
	else if (pattern.getMuonStation(hit) == 2)
	  { 
	    if (pattern.muonDTHitFilter(hit)) {dt2++;}
	    if (pattern.muonRPCHitFilter(hit)) {rpc2++;}
	    if (pattern.muonCSCHitFilter(hit)) {csc2++;}
	  }
	else if (pattern.getMuonStation(hit) == 3)
	  { 
	    if (pattern.muonDTHitFilter(hit)) {dt3++;}
	    if (pattern.muonRPCHitFilter(hit)) {rpc3++;}
	    if (pattern.muonCSCHitFilter(hit)) {csc3++;}
	  }
	else if (pattern.getMuonStation(hit) == 4)
	  { 
	    if (pattern.muonDTHitFilter(hit)) {dt4++;}
	    if (pattern.muonRPCHitFilter(hit)) {rpc4++;}
	    if (pattern.muonCSCHitFilter(hit)) {csc4++;}
	  }    
      }
    comb = (dt1+dt2+dt3+dt4)/2. + (rpc1+rpc2+rpc3+rpc4);
    csc1>6 ? comb+=6 : comb+=csc1;
    csc2>6 ? comb+=6 : comb+=csc2;
    csc3>6 ? comb+=6 : comb+=csc3;
    csc4>6 ? comb+=6 : comb+=csc4;
    vmuonhitcomb_reco[i] = comb;
    rpchits_reco[i] = rpc1+rpc2+rpc3+rpc4;

  } // loop muon


  // some handy max, min variables
  drtau_max = TMath::Max(drtau_reco[0], TMath::Max(drtau_reco[1], drtau_reco[2] ));
  tLWM_min = TMath::Min(tLWM_reco[0], TMath::Min(tLWM_reco[1], tLWM_reco[2]));
  nOMS_min = TMath::Min(nOMS_reco[0], TMath::Min(nOMS_reco[1], nOMS_reco[2]));
  eta_min = TMath::Min(abs(eta_reco[0]), TMath::Min(abs(eta_reco[1]), abs(eta_reco[2])));
  eta_max = TMath::Max(abs(eta_reco[0]), TMath::Max(abs(eta_reco[1]), abs(eta_reco[2])));

  //MuonShower muonShowerInformation1 = (*muonShowerInformationValueMapH_)[muonRef1];
  //nSCH_reco[0] = muonShowerInformation1.nStationCorrelatedHits.at(1);

  dr12_reco = deltaR(mu[0].eta(), mu[0].phi(), mu[1].eta(), mu[1].phi());
  dr23_reco = deltaR(t3->eta(), t3->phi(), mu[1].eta(), mu[1].phi());
  dr31_reco = deltaR(mu[0].eta(), mu[0].phi(), t3->eta(), t3->phi());

  dr_min = 99;
  if(dr12_reco<dr_min) {dr_min = dr12_reco; ifar=2;}
  if(dr23_reco<dr_min) {dr_min = dr23_reco; ifar=0;}
  if(dr31_reco<dr_min) {dr_min = dr31_reco; ifar=1;}


  ////////////////////
  // Fit 3mu vertex
  vector<TransientTrack> t_trks;
  TrackRef trk1 = mu[0].innerTrack();
  TrackRef trk2 = mu[1].innerTrack();
  ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
  t_trks.push_back(theB->build(trk1));
  t_trks.push_back(theB->build(trk2));
  t_trks.push_back(theB->build(t3));


  //TrajectoryStateClosestToPoint mu1TS = t_trks[0].impactPointTSCP();
  //TrajectoryStateClosestToPoint mu2TS = t_trks[1].impactPointTSCP();
  ClosestApproachInRPhi cApp12, cApp23, cApp31;
  cApp12.calculate(t_trks[0].initialFreeState(), t_trks[1].initialFreeState());
  cApp23.calculate(t_trks[1].initialFreeState(), t_trks[2].initialFreeState());
  cApp31.calculate(t_trks[2].initialFreeState(), t_trks[0].initialFreeState());
  //cApp.calculate(mu1TS.theState(), mu2TS.theState());
  if(!(cApp12.status()&&cApp23.status()&&cApp31.status())) { return; cout<<"DCA unvalid!"<<endl; }
  dca12_reco = cApp12.distance();
  dca23_reco = cApp23.distance();
  dca31_reco = cApp31.distance();
  dca_max = TMath::Max(dca12_reco, TMath::Max(dca23_reco, dca31_reco));

  KalmanVertexFitter kvf(true);
  TransientVertex fv = kvf.vertex(t_trks);
  if(!fv.isValid()) { return; cout<<"Vertex Fit unvalid!"<<endl; }

  TLorentzVector vtau_refit, vmu_refit;
  vtau_refit.SetPtEtaPhiM(0, 0, 0, 0);
  vector<TransientTrack>::const_iterator trkIt = fv.refittedTracks().begin();
  for(; trkIt != fv.refittedTracks().end(); ++ trkIt) {
    const Track & trkrefit = trkIt->track();
    vmu_refit.SetPtEtaPhiM(trkrefit.pt(), trkrefit.eta(), trkrefit.phi(), (n_reco>2?0.106:0.140));
    vtau_refit += vmu_refit;
  }
  m3mu_refit = vtau_refit.M();

  fv_tC = fv.totalChiSquared();
  int fv_dOF = fv.degreesOfFreedom();
  fv_nC = fv_tC/fv_dOF;
  fv_Prob = TMath::Prob(fv_tC,(int)fv_dOF);

  //GlobalError err = fv.positionError(); //(verr.At(0,0), verr.At(1,0), verr.At(1,1), verr.At(2,0), verr.At(2,1), verr.At(2,2) );
  //GlobalPoint displacementFromBeamspot( -1*((bs.x0() -  fv.position().x()) +  (fv.position().z() - bs.z0()) * bs.dxdz()),
  //-1*((bs.y0() - fv.position().y())+  (fv.position().z() - bs.z0()) * bs.dydz()), 0);
  //fv_dxy = displacementFromBeamspot.perp();
  //double fv_dxyerr = sqrt(err.rerr(displacementFromBeamspot));
  //fv_dxysig = fv_dxy/fv_dxyerr;

  vector<TransientTrack> t_trks12, t_trks23, t_trks31;
  t_trks12.push_back(theB->build(trk1)); t_trks12.push_back(theB->build(trk2));
  t_trks23.push_back(theB->build(trk2)); t_trks23.push_back(theB->build(t3));
  t_trks31.push_back(theB->build(t3)); t_trks31.push_back(theB->build(trk1));
  KalmanVertexFitter kvf_trks12, kvf_trks23, kvf_trks31;
  TransientVertex fv_trks12 = kvf_trks12.vertex(t_trks12);
  TransientVertex fv_trks23 = kvf_trks23.vertex(t_trks23);
  TransientVertex fv_trks31 = kvf_trks31.vertex(t_trks31);
  fvwo_tC[0] = fv_trks23.totalChiSquared();
  fvwo_nC[0] = fvwo_tC[0]/fv_trks23.degreesOfFreedom();
  fvwo_tC[1] = fv_trks31.totalChiSquared();
  fvwo_nC[1] = fvwo_tC[1]/fv_trks31.degreesOfFreedom();
  fvwo_tC[2] = fv_trks12.totalChiSquared();
  fvwo_nC[2] = fvwo_tC[2]/fv_trks12.degreesOfFreedom();


  TVector3 vtauxyz(vtau.Px(), vtau.Py(), vtau.Pz());

  ////////////////////
  // find the good PV
  double PVZ = fv.position().z()-fv_dxy*vtau.Pz()/vtau.Pt();
  double dispv1 = 99, dispvgen=99, dphi_pv = -1;
  //int ipvPVZ = 99, ipvgen = 99;
  for(size_t jpv = 0; jpv < pvs->size(); jpv++) {
    const Vertex & vi = (*pvs)[jpv];

    if(abs(vi.position().Z()-PVZ)<dispv1){
      dispv1=abs(vi.position().Z()-PVZ);
      ipv1=jpv;
    }
    if(abs(vi.position().Z()-gen_pv)<dispvgen){
      dispvgen=abs(vi.position().Z()-gen_pv);
      ipv_gen=jpv;
    }
    TVector3 Dv3D_reco(fv.position().x() - vi.x(), fv.position().y() - vi.y(), fv.position().z() - vi.z());
    double Cosdphi_3D = Dv3D_reco.Dot(vtauxyz)/(Dv3D_reco.Mag()*vtauxyz.Mag());
    if(Cosdphi_3D>dphi_pv){
      dphi_pv = Cosdphi_3D;
      ipv2=jpv;
    }

  }
  const Vertex & pv0 = (*pvs)[ipv2];


  //////////////////////////////////////////////////
  // refit PV with and w.o. the 3 mu
  pv1_tC = 999; pv1_nC = 99; pv2_tC = 999; pv2_nC = 99;
  vector<TransientTrack> pv_trks;
  TransientVertex pv2, pv1;

  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
  for(Vertex::trackRef_iterator itk = pv0.tracks_begin(); itk != pv0.tracks_end(); itk++) {
    if((**itk).pt()>1) {
      if(deltaR(mu[0].eta(), mu[0].phi(), (**itk).eta(), (**itk).phi())<0.01)continue;
      if(deltaR(mu[1].eta(), mu[1].phi(), (**itk).eta(), (**itk).phi())<0.01)continue;
      if(deltaR(t3->eta(), t3->phi(), (**itk).eta(), (**itk).phi())<0.01)continue;
    }
    pv_trks.push_back(theB->build(**itk));
  }
  if(pv_trks.size()>1) {
    KalmanVertexFitter kvf_pv;
    pv1 = kvf_pv.vertex(pv_trks);
    if(pv1.isValid()){
      pv1_tC = pv1.totalChiSquared();
      pv1_nC = pv1_tC/pv1.degreesOfFreedom();
    }

    // adding the 3 mu tracks
    pv_trks.push_back(theB->build(trk1));
    pv_trks.push_back(theB->build(trk2));
    pv_trks.push_back(theB->build(t3));
    pv2 = kvf_pv.vertex(pv_trks);
    if(pv2.isValid()){
      pv2_tC = pv2.totalChiSquared();
      pv2_nC = pv2_tC/pv2.degreesOfFreedom();
    }
  }
  //cout<<pv0.x()-pv1.position().x()<<"  "<<pv0.y()-pv1.position().y()<<"  "<<pv0.z()-pv1.position().z()<<"  "<<endl;

  Vertex pvv = pv0;  // the final PV
  if(pv1.isValid()) pvv = Vertex(pv1);
  math::XYZPoint pv1P = math::XYZPoint(pvv.x(), pvv.y(), pvv.z());


  d0_reco[0] = abs(mu[0].innerTrack()->dxy(pv1P));
  d0_reco[1] = abs(mu[1].innerTrack()->dxy(pv1P));
  d0_reco[2] = abs(t3->dxy(pv1P));
  d0sig_reco[0] = -1; d0sig_reco[1] = -1; d0sig_reco[2] = -1;
  GlobalVector dir1(mu[0].px(), mu[0].py(), mu[0].pz());
  GlobalVector dir2(mu[1].px(), mu[1].py(), mu[1].pz());
  GlobalVector dir3(t3->px(), t3->py(), t3->pz());
  std::pair<bool, Measurement1D> ip2d_1 = IPTools::signedTransverseImpactParameter(t_trks[0], dir1, pvv);
  std::pair<bool, Measurement1D> ip2d_2 = IPTools::signedTransverseImpactParameter(t_trks[1], dir2, pvv);
  std::pair<bool, Measurement1D> ip2d_3 = IPTools::signedTransverseImpactParameter(t_trks[2], dir3, pvv);
  if(ip2d_1.first) d0sig_reco[0] = abs(ip2d_1.second.value()/ip2d_1.second.error());
  if(ip2d_2.first) d0sig_reco[1] = abs(ip2d_2.second.value()/ip2d_2.second.error());
  if(ip2d_3.first) d0sig_reco[2] = abs(ip2d_3.second.value()/ip2d_3.second.error());


  ////////////////////
  // displacement 2D
  TVector3 dv2D_reco(fv.position().x() - pv1P.x(), fv.position().y() - pv1P.y(), 0);
  TVector3 vtauxy(vtau.Px(), vtau.Py(), 0);
  fv_cosdphi = dv2D_reco.Dot(vtauxy)/(dv2D_reco.Perp()*vtauxy.Perp());
  VertexDistanceXY vdistXY;
  Measurement1D distXY = vdistXY.distance(Vertex(fv), pvv);
  fv_dxy = distXY.value();
  fv_dxysig = distXY.significance();
  fv_ppdl = distXY.value()*fv_cosdphi * m3mu_reco/vtauxy.Perp();


  ////////////////////
  // displacement 3D
  TVector3 dv3D_reco(fv.position().x() - pv1P.x(), fv.position().y() - pv1P.y(), fv.position().z() - pv1P.z());
  //TVector3 vtauxyz(vtau.Px(), vtau.Py(), vtau.Pz());
  fv_cosdphi3D = dv3D_reco.Dot(vtauxyz)/(dv3D_reco.Mag()*vtauxyz.Mag());
  VertexDistance3D dist;
  fv_d3D = dist.distance(Vertex(fv), pvv).value(); // = dv_reco.Mag() ??
  fv_d3Dsig = dist.distance(Vertex(fv), pvv).significance();
  fv_ppdl3D = fv_d3D*fv_cosdphi3D*m3mu_reco/vtau.P();



  vector<double> softmueta, softmuphi;
  pv_nmu = 0;

  for(size_t i = 0; i < muons->size(); ++ i) {
    if(i==j1)continue;
    if(i==j2)continue;
    if(i==j3)continue;
    const Muon & m_1 = (*muons)[i];
    if(!(abs(m_1.eta())<2.4)) continue;
    if(!(muon::isGoodMuon(m_1, muon::TMOneStationTight))) continue;
    if(!(m_1.innerTrack()->hitPattern().trackerLayersWithMeasurement()>5))continue;
    if(!(m_1.innerTrack()->hitPattern().pixelLayersWithMeasurement()>0))continue;
    //if(!(abs(m_1.innerTrack()->dxy(pv0.position())) < .3))continue;
    if(!(abs(m_1.innerTrack()->dz(pv1P)) < 1))continue;
    pv_nmu++;
    softmueta.push_back(m_1.eta());
    softmuphi.push_back(m_1.phi());
  }

  //for(Vertex::trackRef_iterator itk = pv0.tracks_begin(); itk != pv0.tracks_end(); itk++) {
  //  for(size_t imu = 0; imu < softmueta.size(); imu ++) {
  //    if(deltaR(softmueta[imu], softmuphi[imu], (**itk).eta(), (**itk).phi())<0.01)
  //      pv_nmu++;
  //  }
  //}

  ////////////////////
  // secondary vertices
  n_sv = 0 ;
  for(size_t isv = 0; isv < svs->size(); isv++) {
    const Vertex & sv = (*svs)[isv];
    if(abs(sv.p4().M()-0.498)<.03 && sv.tracksSize()==2)continue; // no Ks

    double dx = sv.x()-pv1P.x();
    double dy = sv.y()-pv1P.y();
    double dz = sv.z()-pv1P.z();
    TVector3 sv_reco(dx, dy, dz);
    sv_overlap[n_sv]=deltaR(sv_reco.Eta(), sv_reco.Phi(), dv3D_reco.Eta(), dv3D_reco.Phi());

    TVector3 svxyz(sv.p4().Px(), sv.p4().Py(), sv.p4().Pz());
    sv_cosdphi3D[n_sv] = sv_reco.Dot(svxyz)/(sv_reco.Mag()*svxyz.Mag());
    VertexDistance3D distsv;
    sv_d3D[n_sv] = distsv.distance(sv, pvv).value();
    sv_d3Dsig[n_sv] = distsv.distance(sv, pvv).significance();
    sv_ppdl3D[n_sv] = sv_d3D[n_sv]*sv_cosdphi3D[n_sv]*sv.p4().M()/sv.p4().P();

    sv_nmu[n_sv] = 0;
    for(Vertex::trackRef_iterator itk = sv.tracks_begin(); itk != sv.tracks_end(); itk++) {
      for(size_t imu = 0; imu < softmueta.size(); imu ++) {
	if(deltaR(softmueta[imu], softmuphi[imu], (**itk).eta(), (**itk).phi())<0.01)
	  sv_nmu[n_sv] ++;
      }
    }

    sv_mass[n_sv] = sv.p4().M();
    sv_pt[n_sv] = sv.p4().Pt();
    sv_dz[n_sv] = abs(dz);
    sv_ntrk[n_sv] = sv.tracksSize();
    n_sv++;
  }

  dzpv_reco[0] = 1;
  dzpv_reco[1] = 1;
  dzpv_reco[2] = 1;

  dz_reco[0] = abs(mu[0].innerTrack()->dz(pv1P));
  dz_reco[1] = abs(mu[1].innerTrack()->dz(pv1P));
  dz_reco[2] = abs(t3->dz(pv1P));

  for(size_t jpv = 0; jpv < pvs->size(); jpv++) {
    if(jpv==ipv2)continue;
    const Vertex & vi = (*pvs)[jpv];
    if(abs(mu[0].innerTrack()->dz(vi.position()))<dz_reco[0]) dzpv_reco[0]=-1;
    if(abs(mu[1].innerTrack()->dz(vi.position()))<dz_reco[1]) dzpv_reco[1]=-1;
    if(abs(t3->dz(vi.position()))<dz_reco[2]) dzpv_reco[2]=-1;
  }


  ////////////////////
  // Track Isolation
  // How to decide if a track is associated with a certain PV ?
  double pttrk_tau = 0, pttrk_tau05 = 0,  pttrk_m1 = 0, pttrk_m2 = 0, pttrk_m3 = 0;
  mindca_iso = 99; mindca_iso05 = 99;
  ntrk_tau = 0; ntrk_tau05 = 0; ntrk_tau_b = 0; ntrk_sum = 0;
  ntrk_reco[0] = 0;  ntrk_reco[1] = 0;  ntrk_reco[2] = 0;
  ntrk0p1 = 0; ntrk0p2 = 0; ntrk0p5 = 0; maxdxy_pv0 =0;

  math::XYZPoint fvP = math::XYZPoint(fv.position().x(), fv.position().y(), fv.position().z());
  for(size_t i = 0; i < trks->size(); i++) {
    const Track & t = (*trks)[i];
    if(!(t.quality(TrackBase::tight)))continue;
    if(deltaR(mu[0].eta(), mu[0].phi(), t.eta(), t.phi())<0.01)continue;
    if(deltaR(mu[1].eta(), mu[1].phi(), t.eta(), t.phi())<0.01)continue;
    if(deltaR(t3->eta(), t3->phi(), t.eta(), t.phi())<0.01)continue;

    double dz = abs(t.dz(fvP));
    double dxy = abs(t.dxy(fvP));
    double dca_fv = sqrt(dz*dz+dxy*dxy);
    double dr_tau = deltaR(t.eta(), t.phi(), vtau.Eta(), vtau.Phi());

    // iso no. 1b - using pt_min, drtau_max of the 3 mu
    if(t.pt() > 0.33*pt_min && dr_tau < 3.*drtau_max && dca_fv<0.05 ) {
      pttrk_tau += t.pt();
      ntrk_tau++; // iso 3b
      if(dca_fv<mindca_iso)mindca_iso=dca_fv; // iso 4b
    } 

    if(t.pt()<1.0) continue;  // was 1.2
    // iso no. 1
    if(dr_tau < 0.5 && dca_fv<0.05 ) {
      pttrk_tau05 += t.pt();
      ntrk_tau05++; // iso 3
      //if(dca_fv<mindca_iso05)mindca_iso05=dca_fv; // iso 4
    }

    if(dca_fv<0.05)ntrk_tau_b++; // iso 3b
    if(dca_fv<mindca_iso05)mindca_iso05=dca_fv; // iso 4


    TransientTrack trkiso = theB->build(t);
    ClosestApproachInRPhi cAppm1, cAppm2, cAppm3;
    cAppm1.calculate(trkiso.initialFreeState(), t_trks[0].initialFreeState());
    cAppm2.calculate(trkiso.initialFreeState(), t_trks[1].initialFreeState());
    cAppm3.calculate(trkiso.initialFreeState(), t_trks[2].initialFreeState());
    if(!(cAppm1.status()&&cAppm2.status()&&cAppm3.status())) continue;

    // iso no. 2
    if(deltaR(t.eta(), t.phi(), mu[0].eta(), mu[0].phi()) < 0.3 && cAppm1.distance() < 0.1) {// && dz1 < .3) 
      ntrk_reco[0]++;
      pttrk_m1 += t.pt();
    }
    if(deltaR(t.eta(), t.phi(), mu[1].eta(), mu[1].phi()) < 0.3 && cAppm2.distance() < 0.1) {//&& dz2 < .3) 
      ntrk_reco[1]++;
      pttrk_m2 += t.pt();
    }
    if(deltaR(t.eta(), t.phi(), t3->eta(), t3->phi()) < 0.3 && cAppm3.distance() < 0.1) {//&& dz3 < .3) 
      ntrk_reco[2]++;
      pttrk_m3 += t.pt();
    }
    if( (deltaR(t.eta(), t.phi(), mu[0].eta(), mu[0].phi()) < 0.3 && cAppm1.distance() < 0.1 )
	||(deltaR(t.eta(), t.phi(), mu[1].eta(), mu[1].phi()) < 0.3 && cAppm2.distance() < 0.1 )
	||(deltaR(t.eta(), t.phi(), t3->eta(), t3->phi()) < 0.3 && cAppm3.distance() < 0.1 )
	) ntrk_sum++;


    // displaced track counting
    // only tracks consistent with PV
    double dz_pv0=abs(t.dz(pv1P));
    if(!(dz_pv0 < 1))continue;
    double dxy_pv0 = abs(t.dxy(pv1P));
    if(dxy_pv0>0.1) ntrk0p1++;
    if(dxy_pv0>0.2) ntrk0p2++;
    if(dxy_pv0>0.5) ntrk0p5++;
    if(dxy_pv0>maxdxy_pv0) maxdxy_pv0 = dxy_pv0;

  }

  trkrel_tau = pttrk_tau/vtau.Pt();
  trkrel_tau05 = pttrk_tau05/vtau.Pt();
  trkrel_reco[0] = pttrk_m1/mu[0].pt(); trkrel_reco[1] = pttrk_m2/mu[1].pt(); trkrel_reco[2] = pttrk_m3/t3->pt();
  trkrel_max = TMath::Max(trkrel_reco[0], TMath::Max( trkrel_reco[1], trkrel_reco[2]));


  // Good Global Muon, Tight Global Muon
  ggm_reco[0] = (glbnC_reco[0]<3 && cLP_reco[0]<12 && tKink_reco[0]<20 && segmcomp_reco[0]>0.303)?1:0;
  tgm_reco[0] = (glbnC_reco[0]<10 && pf_reco[0] && nOVMH_reco[0]>0 && nOMS_reco[0]>1
		 && d0_reco[0]<0.2 && dz_reco[0]<0.5 && nOVPH_reco[0]>0 && tLWM_reco[0]>5)?1:0;
  ggm_reco[1] = (glbnC_reco[1]<3 && cLP_reco[1]<12 && tKink_reco[1]<20 && segmcomp_reco[1]>0.303)?1:0;
  tgm_reco[1] = (glbnC_reco[1]<10 && pf_reco[1] && nOVMH_reco[1]>0 && nOMS_reco[1]>1
		 && d0_reco[1]<0.2 && dz_reco[1]<0.5 && nOVPH_reco[1]>0 && tLWM_reco[1]>5)?1:0;
  ggm_reco[2] = (glbnC_reco[2]<3 && cLP_reco[2]<12 && tKink_reco[2]<20 && segmcomp_reco[2]>0.303)?1:0;
  tgm_reco[2] = (glbnC_reco[2]<10 && pf_reco[2] && nOVMH_reco[2]>0 && nOMS_reco[2]>1
		 && d0_reco[2]<0.2 && dz_reco[2]<0.5 && nOVPH_reco[2]>0 && tLWM_reco[2]>5)?1:0;




  /////////////////
  // b tag
  //  to add Jet ID ??
  njet20 = 0;
  for(size_t j = 0 ; j < btagsCvsB->size(); j++) {
    const JetTag & btag1 = (*btagsCvsB)[j];

    if(btag1.first->pt()<20) break;
    if(deltaR(vtau.Eta(), vtau.Phi(), btag1.first->eta(), btag1.first->phi())<0.4)jet_overlap[njet20]=1;
    else jet_overlap[njet20]=0;

    jet_pt[njet20] = btag1.first->pt();
    btagcvsb[njet20] = btag1.second;
    //cout<<"jet"<<j<<"  pt: "<<btag.first->pt()<<"  btag: "<<btag.second<<endl;
    const JetTag & btag2 = (*btagsMVA)[j];
    btagmva[njet20] = btag2.second;
    const JetTag & btag3 = (*btagsCSV)[j];
    btagcsv[njet20] = (btag3.second<0 ? 0:btag3.second);

    njet20 ++;
  }

  output_former_tree->Fill();
  h_step->Fill(6);
}





// ------------ method called once each job just before starting event loop  ------------
void 
T3MNtuple::beginJob()
{

  std::cout<<" ------------------------>>>>    T3MNtuple begin Job "<<std::endl;
  cnt_=0;

  Service<TFileService> fs;
  h_n3mu = fs->make<TH1F>("n3mu", "", 10, 0, 10);
  h_step = fs->make<TH1F>("step", "", 10, 0, 10);


  output_former_tree = fs->make<TTree>("ft3mtree", "");


  //output_tree->Branch("filterbadGlbMuon", &filterbadGlbMuon, "filterbadGlbMuon/D");
  output_former_tree->Branch("gen_flavor", &gen_flavor, "gen_flavor/D");
  output_former_tree->Branch("nmu_mom", &nmu_mom, "nmu_mom/D");
  output_former_tree->Branch("hlt_doublemu4_lmnrt", &hlt_doublemu4_lmnrt, "hlt_doublemu4_lmnrt/D");
  output_former_tree->Branch("hlt_doublemu3_tau3mu", &hlt_doublemu3_tau3mu, "hlt_doublemu3_tau3mu/D");
  output_former_tree->Branch("l1_triplemu0", &l1_triplemu0, "l1_triplemu0/D");
  output_former_tree->Branch("l1_doublemu0", &l1_doublemu0, "l1_doublemu0/D");
  output_former_tree->Branch("l1_doublemu0_eta1p6_os", &l1_doublemu0_eta1p6_os, "l1_doublemu0_eta1p6_os/D");
  output_former_tree->Branch("l1_doublemu0_eta1p4_os", &l1_doublemu0_eta1p4_os, "l1_doublemu0_eta1p4_os/D");
  output_former_tree->Branch("l1_doublemu0_eta1p6", &l1_doublemu0_eta1p6, "l1_doublemu0_eta1p6/D");
  output_former_tree->Branch("l1_doublemu_10_0", &l1_doublemu_10_0, "l1_doublemu_10_0/D");
  output_former_tree->Branch("l1_doublemu_11_4", &l1_doublemu_11_4, "l1_doublemu_11_4/D");
  output_former_tree->Branch("l1_triplemu500", &l1_triplemu500, "l1_triplemu500/D");
  output_former_tree->Branch("prescale_triplemu0", &prescale_triplemu0, "prescale_triplemu0/D");
  output_former_tree->Branch("prescale_doublemu0_eta1p6", &prescale_doublemu0_eta1p6, "prescale_doublemu0_eta1p6/D");
  output_former_tree->Branch("prescale_doublemu_10_0", &prescale_doublemu_10_0, "prescale_doublemu_10_0/D");
  output_former_tree->Branch("prescale_triplemu500", &prescale_triplemu500, "prescale_triplemu500/D");
  output_former_tree->Branch("prescale_doublemu0_eta1p6_os", &prescale_doublemu0_eta1p6_os, "prescale_doublemu0_eta1p6_os/D");
  output_former_tree->Branch("prescale_doublemu0_eta1p4_os", &prescale_doublemu0_eta1p4_os, "prescale_doublemu0_eta1p4_os/D");
  output_former_tree->Branch("prescale_doublemu_11_4", &prescale_doublemu_11_4, "prescale_doublemu_11_4/D");
  output_former_tree->Branch("n_reco", &n_reco, "n_reco/I");
  output_former_tree->Branch("ifar", &ifar, "ifar/I");
  output_former_tree->Branch("ipv_gen", &ipv_gen, "ipv_gen/I");
  output_former_tree->Branch("ipv1", &ipv1, "ipv1/I");
  output_former_tree->Branch("ipv2", &ipv2, "ipv2/I");
  output_former_tree->Branch("pdgid_reco", pdgid_reco, "pdgid_reco[3]/D");
  output_former_tree->Branch("momid_reco", momid_reco, "momid_reco[3]/D");
  output_former_tree->Branch("vxy_reco", vxy_reco, "vxy_reco[3]/D");
  output_former_tree->Branch("n_vtx", &n_vtx, "n_vtx/D");
  output_former_tree->Branch("trigmat_reco", trigmat_reco, "trigmat_reco[3]/D");
  output_former_tree->Branch("trigmat_new", &trigmat_new, "trigmat_new/D");
  output_former_tree->Branch("p_reco", p_reco, "p_reco[3]/D");
  output_former_tree->Branch("pt_reco", pt_reco, "pt_reco[3]/D");
  output_former_tree->Branch("pt_max", &pt_max, "pt_max/D");
  output_former_tree->Branch("pt_min", &pt_min, "pt_min/D");
  output_former_tree->Branch("eta_reco", eta_reco, "eta_reco[3]/D");
  output_former_tree->Branch("eta_min", &eta_min, "eta_min/D");
  output_former_tree->Branch("eta_max", &eta_max, "eta_max/D");
  output_former_tree->Branch("phi_reco", phi_reco, "phi_reco[3]/D");
  output_former_tree->Branch("p_in", p_in, "p_in[3]/D");
  output_former_tree->Branch("eta_in", eta_in, "eta_in[3]/D");
  output_former_tree->Branch("phi_in", phi_in, "phi_in[3]/D");
  output_former_tree->Branch("p_out", p_out, "p_out[3]/D");
  output_former_tree->Branch("eta_out", eta_out, "eta_out[3]/D");
  output_former_tree->Branch("phi_out", phi_out, "phi_out[3]/D");
  output_former_tree->Branch("p_glb", p_glb, "p_glb[3]/D");
  output_former_tree->Branch("eta_glb", eta_glb, "eta_glb[3]/D");
  output_former_tree->Branch("phi_glb", phi_glb, "phi_glb[3]/D");
  output_former_tree->Branch("pt3mu_reco", &pt3mu_reco, "pt3mu_reco/D");
  output_former_tree->Branch("pt2mu_12", &pt2mu_12, "pt2mu_12/D");
  output_former_tree->Branch("p3mu_reco", &p3mu_reco, "p3mu_reco/D");
  output_former_tree->Branch("eta3mu_reco", &eta3mu_reco, "eta3mu_reco/D");
  output_former_tree->Branch("m3mu_reco", &m3mu_reco, "m3mu_reco/D");
  output_former_tree->Branch("m3mu_simp", &m3mu_simp, "m3mu_simp/D");
  output_former_tree->Branch("m3mu_refit", &m3mu_refit, "m3mu_refit/D");
  output_former_tree->Branch("pf_reco", pf_reco, "pf_reco[3]/D");
  output_former_tree->Branch("rpcmu_reco", rpcmu_reco, "rpcmu_reco[3]/D");
  output_former_tree->Branch("rpchits_reco", rpchits_reco, "rpchits_reco[3]/D");
  output_former_tree->Branch("comp2d_reco", comp2d_reco, "comp2d_reco[3]/D");
  output_former_tree->Branch("tma_reco", tma_reco, "tma_reco[3]/D");
  output_former_tree->Branch("tmost_reco", tmost_reco, "tmost_reco[3]/D");
  output_former_tree->Branch("tmosat_reco", tmosat_reco, "tmosat_reco[3]/D");
  output_former_tree->Branch("tmlst_reco", tmlst_reco, "tmlst_reco[3]/D");
  output_former_tree->Branch("tmlsat_reco", tmlsat_reco, "tmlsat_reco[3]/D");
  output_former_tree->Branch("tmlsolpt_reco", tmlsolpt_reco, "tmlsolpt_reco[3]/D");
  output_former_tree->Branch("tmlsoblpt_reco", tmlsoblpt_reco, "tmlsoblpt_reco[3]/D");
  output_former_tree->Branch("calocomp_reco", calocomp_reco, "calocomp_reco[3]/D");
  output_former_tree->Branch("segmcomp_reco", segmcomp_reco, "segmcomp_reco[3]/D");
  output_former_tree->Branch("segmcomp_0", segmcomp_0, "segmcomp_0[3]/D");
  output_former_tree->Branch("segmcomp_1", segmcomp_1, "segmcomp_1[3]/D");
  output_former_tree->Branch("segmcomp_2", segmcomp_2, "segmcomp_2[3]/D");
  output_former_tree->Branch("segmcomp_3", segmcomp_3, "segmcomp_3[3]/D");
  //output_former_tree->Branch("segmcomp_4", segmcomp_4, "segmcomp_4[3]/D");
  //output_former_tree->Branch("segmcomp_5", segmcomp_5, "segmcomp_5[3]/D");
  //output_former_tree->Branch("segmcomp_6", segmcomp_6, "segmcomp_6[3]/D");
  //output_former_tree->Branch("segmcomp_7", segmcomp_7, "segmcomp_7[3]/D");
  output_former_tree->Branch("trkhp_reco", trkhp_reco, "trkhp_reco[3]/D");
  output_former_tree->Branch("glbnC_reco", glbnC_reco, "glbnC_reco[3]/D");
  output_former_tree->Branch("nOVMH_reco", nOVMH_reco, "nOVMH_reco[3]/D");
  output_former_tree->Branch("nOVPH_reco", nOVPH_reco, "nOVPH_reco[3]/D");
  output_former_tree->Branch("nOVTH_reco", nOVTH_reco, "nOVTH_reco[3]/D");
  output_former_tree->Branch("nOLTH_reco", nOLTH_reco, "nOLTH_reco[3]/D");
  output_former_tree->Branch("nOLTHin_reco", nOLTHin_reco, "nOLTHin_reco[3]/D");
  output_former_tree->Branch("nOLTHout_reco", nOLTHout_reco, "nOLTHout_reco[3]/D");
  //output_former_tree->Branch("nSCH_reco", nSCH_reco, "nSCH_reco[3]/D");
  output_former_tree->Branch("iTvF_reco", iTvF_reco, "iTvF_reco[3]/D");
  output_former_tree->Branch("tLWM_reco", tLWM_reco, "tLWM_reco[3]/D");
  output_former_tree->Branch("pLWM_reco", pLWM_reco, "pLWM_reco[3]/D");
  output_former_tree->Branch("ggm_reco", ggm_reco, "ggm_reco[3]/D");
  output_former_tree->Branch("tgm_reco", tgm_reco, "tgm_reco[3]/D");
  output_former_tree->Branch("pterr_reco", pterr_reco, "pterr_reco[3]/D");
  output_former_tree->Branch("Iso_sumPt", Iso_sumPt, "Iso_sumPt[3]/D");
  output_former_tree->Branch("Iso_nTr", Iso_nTr, "Iso_nTr[3]/D");
  output_former_tree->Branch("Iso_emEt", Iso_emEt, "Iso_emEt[3]/D");
  output_former_tree->Branch("Iso_hadEt", Iso_hadEt, "Iso_hadEt[3]/D");
  output_former_tree->Branch("Iso_eVE", Iso_eVE, "Iso_eVE[3]/D");
  output_former_tree->Branch("Iso_hVE", Iso_hVE, "Iso_hVE[3]/D");
  output_former_tree->Branch("nOMS_reco", nOMS_reco, "nOMS_reco[3]/D");
  output_former_tree->Branch("nOM_reco", nOM_reco, "nOM_reco[3]/D");
  output_former_tree->Branch("mSWVH_reco", mSWVH_reco, "mSWVH_reco[3]/D");
  output_former_tree->Branch("cschits_sta1", cschits_sta1, "cschits_sta1[3]/D");
  output_former_tree->Branch("cscchi2_sta1", cscchi2_sta1, "cscchi2_sta1[3]/D");
  output_former_tree->Branch("cschits_sta2", cschits_sta2, "cschits_sta2[3]/D");
  output_former_tree->Branch("cscchi2_sta2", cscchi2_sta2, "cscchi2_sta2[3]/D");
  output_former_tree->Branch("cscdxdz_sta1", cscdxdz_sta1, "cscdxdz_sta1[3]/D");
  output_former_tree->Branch("cscdxdz_sta2", cscdxdz_sta2, "cscdxdz_sta2[3]/D");
  output_former_tree->Branch("cscdydz_sta1", cscdydz_sta1, "cscdydz_sta1[3]/D");
  output_former_tree->Branch("cscdydz_sta2", cscdydz_sta2, "cscdydz_sta2[3]/D");
  output_former_tree->Branch("cscnsegm_sta1", cscnsegm_sta1, "cscnsegm_sta1[3]/D");
  output_former_tree->Branch("cscnsegm_sta2", cscnsegm_sta2, "cscnsegm_sta2[3]/D");
  output_former_tree->Branch("nOMS_min", &nOMS_min, "nOMS_min/D");
  output_former_tree->Branch("tLWM_min", &tLWM_min, "tLWM_min/D");
  output_former_tree->Branch("d0_reco", d0_reco, "d0_reco[3]/D");
  output_former_tree->Branch("d0sig_reco", d0sig_reco, "d0sig_reco[3]/D");
  output_former_tree->Branch("dz_reco", dz_reco, "dz_reco[3]/D");
  output_former_tree->Branch("dzpv_reco", dzpv_reco, "dzpv_reco[3]/D");
  output_former_tree->Branch("dr12_reco", &dr12_reco, "dr12_reco/D");
  output_former_tree->Branch("dr23_reco", &dr23_reco, "dr23_reco/D");
  output_former_tree->Branch("dr31_reco", &dr31_reco, "dr31_reco/D");
  output_former_tree->Branch("dr_min", &dr_min, "dr_min/D");
  output_former_tree->Branch("drtau_max", &drtau_max, "drtau_max/D");
  output_former_tree->Branch("charge_reco", charge_reco, "charge_reco[3]/D");
  output_former_tree->Branch("drtau_reco", drtau_reco, "drtau_reco[3]/D");
  output_former_tree->Branch("dca12_reco", &dca12_reco, "dca12_reco/D");
  output_former_tree->Branch("dca23_reco", &dca23_reco, "dca23_reco/D");
  output_former_tree->Branch("dca31_reco", &dca31_reco, "dca31_reco/D");
  output_former_tree->Branch("dca_max", &dca_max, "dca_max/D");
  output_former_tree->Branch("trkrel_reco", trkrel_reco, "trkrel_reco[3]/D");
  output_former_tree->Branch("trkrel_max", &trkrel_max, "trkrel_max/D");
  output_former_tree->Branch("trkrel_tau", &trkrel_tau, "trkrel_tau/D");
  output_former_tree->Branch("trkrel_tau05", &trkrel_tau05, "trkrel_tau05/D");
  output_former_tree->Branch("ntrk_reco", ntrk_reco, "ntrk_reco[3]/D");
  output_former_tree->Branch("ntrk_sum", &ntrk_sum, "ntrk_sum/D");
  output_former_tree->Branch("ntrk_tau", &ntrk_tau, "ntrk_tau/D");
  output_former_tree->Branch("mindca_iso", &mindca_iso, "mindca_iso/D");
  output_former_tree->Branch("ntrk_tau05", &ntrk_tau05, "ntrk_tau05/D");
  output_former_tree->Branch("ntrk_tau_b", &ntrk_tau_b, "ntrk_tau_b/D");
  output_former_tree->Branch("mindca_iso05", &mindca_iso05, "mindca_iso05/D");
  output_former_tree->Branch("fv_tC", &fv_tC, "fv_tC/D");
  output_former_tree->Branch("fv_nC", &fv_nC, "fv_nC/D");
  output_former_tree->Branch("fvwo_tC", fvwo_tC, "fvwo_tC[3]/D");
  output_former_tree->Branch("fvwo_nC", fvwo_nC, "fvwo_nC[3]/D");
  output_former_tree->Branch("fv_Prob", &fv_Prob, "fv_Prob/D");
  output_former_tree->Branch("fv_dxy", &fv_dxy, "fv_dxy/D");
  output_former_tree->Branch("fv_dxysig", &fv_dxysig, "fv_dxysig/D");
  output_former_tree->Branch("fv_ppdl", &fv_ppdl, "fv_ppdl/D");
  output_former_tree->Branch("fv_cosdphi", &fv_cosdphi, "fv_cosdphi/D");
  output_former_tree->Branch("fv_d3D", &fv_d3D, "fv_d3D/D");
  output_former_tree->Branch("fv_d3Dsig", &fv_d3Dsig, "fv_d3Dsig/D");
  output_former_tree->Branch("fv_ppdl3D", &fv_ppdl3D, "fv_ppdl3D/D");
  output_former_tree->Branch("fv_cosdphi3D", &fv_cosdphi3D, "fv_cosdphi3D/D");
  output_former_tree->Branch("iTnC_reco", iTnC_reco, "iTnC1_reco[3]/D");
  output_former_tree->Branch("cLP_reco", cLP_reco, "cLP_reco[3]/D");
  output_former_tree->Branch("cLM_reco", cLM_reco, "cLM_reco[3]/D");
  output_former_tree->Branch("tKink_reco", tKink_reco, "tKink_reco[3]/D");
  output_former_tree->Branch("gKink_reco", gKink_reco, "gKink_reco[3]/D");
  output_former_tree->Branch("qprod_reco", qprod_reco, "qprod_reco[3]/D");
  output_former_tree->Branch("vmuonhitcomb_reco", vmuonhitcomb_reco, "vmuonhitcomb_reco[3]/D");
  output_former_tree->Branch("outerchi2_reco", outerchi2_reco, "outerchi2_reco[3]/D");
  output_former_tree->Branch("timeatipinouterr_reco", timeatipinouterr_reco, "timeatipinouterr_reco[3]/D");
  output_former_tree->Branch("uSta_reco", uSta_reco, "uSta_reco[3]/D");
  output_former_tree->Branch("tRC2_reco", tRC2_reco, "tRC2_reco[3]/D");
  output_former_tree->Branch("sRC2_reco", sRC2_reco, "sRC2_reco[3]/D");
  output_former_tree->Branch("lDist_reco", lDist_reco, "lDist_reco[3]/D");
  output_former_tree->Branch("gDEP_reco", gDEP_reco, "gDEP_reco[3]/D");
  output_former_tree->Branch("tMat_reco", tMat_reco, "tMat_reco[3]/D");
  output_former_tree->Branch("gTP_reco", gTP_reco, "gTP_reco[3]/D");
  output_former_tree->Branch("calem_reco", calem_reco, "calem_reco[3]/D");
  output_former_tree->Branch("calemS9_reco", calemS9_reco, "calemS9_reco[3]/D");
  output_former_tree->Branch("calemS25_reco", calemS25_reco, "calemS25_reco[3]/D");
  output_former_tree->Branch("calhad_reco", calhad_reco, "calhad_reco[3]/D");
  output_former_tree->Branch("calhadS9_reco", calhadS9_reco, "calhadS9_reco[3]/D");
  output_former_tree->Branch("ddz_12", &ddz_12, "ddz_12/D");
  output_former_tree->Branch("calomuon_3", &calomuon_3, "calomuon_3/D");
  output_former_tree->Branch("n_sv", &n_sv, "n_sv/I");
  output_former_tree->Branch("sv_mass", sv_mass, "sv_mass[n_sv]/D");
  output_former_tree->Branch("sv_nmu", sv_nmu, "sv_nmu[n_sv]/D");
  output_former_tree->Branch("sv_d3D", sv_d3D, "sv_d3D[n_sv]/D");
  output_former_tree->Branch("sv_d3Dsig", sv_d3Dsig, "sv_d3Dsig[n_sv]/D");
  output_former_tree->Branch("sv_ppdl3D", sv_ppdl3D, "sv_ppdl3D[n_sv]/D");
  output_former_tree->Branch("sv_ntrk", sv_ntrk, "sv_ntrk[n_sv]/D");
  output_former_tree->Branch("sv_pt", sv_pt, "sv_pt[n_sv]/D");
  output_former_tree->Branch("sv_dz", sv_dz, "sv_dz[n_sv]/D");
  output_former_tree->Branch("sv_cosdphi3D", sv_cosdphi3D, "sv_cosdphi3D[n_sv]/D");
  output_former_tree->Branch("sv_overlap", sv_overlap, "sv_overlap[n_sv]/D");
  output_former_tree->Branch("pv_nmu", &pv_nmu, "pv_nmu/D");
  output_former_tree->Branch("pv1_tC", &pv1_tC, "pv1_tC/D");
  output_former_tree->Branch("pv1_nC", &pv1_nC, "pv1_nC/D");
  output_former_tree->Branch("pv2_tC", &pv2_tC, "pv2_tC/D");
  output_former_tree->Branch("pv2_nC", &pv2_nC, "pv2_nC/D");
  output_former_tree->Branch("ntrk0p1", &ntrk0p1, "ntrk0p1/D");
  output_former_tree->Branch("ntrk0p2", &ntrk0p2, "ntrk0p2/D");
  output_former_tree->Branch("ntrk0p5", &ntrk0p5, "ntrk0p5/D");
  output_former_tree->Branch("maxdxy_pv0", &maxdxy_pv0, "maxdxy_pv0/D");
  output_former_tree->Branch("njet20", &njet20, "njet20/I");
  output_former_tree->Branch("jet_pt", jet_pt, "jet_pt[njet20]/D");
  output_former_tree->Branch("jet_overlap", jet_overlap, "jet_overlap[njet20]/D");
  output_former_tree->Branch("btagcvsb", btagcvsb, "btagcvsb[njet20]/D");
  output_former_tree->Branch("btagcsv", btagcsv, "btagcsv[njet20]/D");
  output_former_tree->Branch("btagmva", btagmva, "btagmva[njet20]/D");
  output_former_tree->Branch("m2mu_ss", &m2mu_ss, "m2mu_ss/D");
  output_former_tree->Branch("m2mu_os1", &m2mu_os1, "m2mu_os1/D");
  output_former_tree->Branch("m2mu_os2", &m2mu_os2, "m2mu_os2/D");
  output_former_tree->Branch("m2mu_12", &m2mu_12, "m2mu_12/D");
  output_former_tree->Branch("m2mu_23", &m2mu_23, "m2mu_23/D");
  output_former_tree->Branch("m2mu_31", &m2mu_31, "m2mu_31/D");
  output_former_tree->Branch("m2mu_max", &m2mu_max, "m2mu_max/D");
  output_former_tree->Branch("m2mu_min", &m2mu_min, "m2mu_min/D");


  output_tree = fs->make<TTree>("t3mtree", "");

  //=============== Event Block ==============
  output_tree->Branch("Event_EventNumber", &Event_EventNumber);
  output_tree->Branch("Event_RunNumber", &Event_RunNumber);
  output_tree->Branch("Event_bunchCrossing", &Event_bunchCrossing);
  output_tree->Branch("Event_orbitNumber", &Event_orbitNumber);
  output_tree->Branch("Event_luminosityBlock", &Event_luminosityBlock);
  output_tree->Branch("Event_isRealData", &Event_isRealData);
  output_tree->Branch("Event_nsignal_candidates", &Event_nsignal_candidates);
  output_tree->Branch("Event_ndsphipi_candidate", &Event_ndsphipi_candidate);
  output_tree->Branch("Event_DataMC_Type" ,&Event_DataMC_Type);


  output_tree->Branch("puN", &puN, "puN/D");

  output_tree->Branch("Track_p4", &Track_p4);
  output_tree->Branch("Track_normalizedChi2", &Track_normalizedChi2);
  output_tree->Branch("Track_numberOfValidHits", &Track_numberOfValidHits);
  output_tree->Branch("Track_charge", &Track_charge);
  output_tree->Branch("Track_dxy", &Track_dxy);
  output_tree->Branch("Track_dz", &Track_dz);
  output_tree->Branch("Track_poca", &Track_poca);
  output_tree->Branch("Track_dxyError", &Track_dxyError);
  output_tree->Branch("Track_dzError", &Track_dzError);


  //=============  Muon Block ====
  output_tree->Branch("Muon_p4", &Muon_p4);
  output_tree->Branch("Muon_Poca", &Muon_Poca);
  output_tree->Branch("Muon_isGlobalMuon", &Muon_isGlobalMuon);
  output_tree->Branch("Muon_isStandAloneMuon", &Muon_isStandAloneMuon);
  output_tree->Branch("Muon_isTrackerMuon", &Muon_isTrackerMuon);
  output_tree->Branch("Muon_isCaloMuon", &Muon_isCaloMuon);
  output_tree->Branch("Muon_isIsolationValid", &Muon_isIsolationValid);
  output_tree->Branch("Muon_isQualityValid", &Muon_isQualityValid);
  output_tree->Branch("Muon_isTimeValid", &Muon_isTimeValid);
  output_tree->Branch("Muon_emEt03", &Muon_emEt03);
  output_tree->Branch("Muon_emVetoEt03", &Muon_emVetoEt03);
  output_tree->Branch("Muon_hadEt03", &Muon_hadEt03);
  output_tree->Branch("Muon_hadVetoEt03", &Muon_hadVetoEt03);
  output_tree->Branch("Muon_nJets03", &Muon_nJets03);
  output_tree->Branch("Muon_nTracks03", &Muon_nTracks03);
  output_tree->Branch("Muon_sumPt03", &Muon_sumPt03);
  output_tree->Branch("Muon_trackerVetoPt03", &Muon_trackerVetoPt03);
  output_tree->Branch("Muon_emEt05", &Muon_emEt05);
  output_tree->Branch("Muon_emVetoEt05", &Muon_emVetoEt05);
  output_tree->Branch("Muon_hadEt05", &Muon_hadEt05);
  output_tree->Branch("Muon_hadVetoEt05", &Muon_hadVetoEt05);
  output_tree->Branch("Muon_nJets05", &Muon_nJets05);
  output_tree->Branch("Muon_nTracks05", &Muon_nTracks05);
  output_tree->Branch("Muon_sumPt05", &Muon_sumPt05);
  output_tree->Branch("Muon_trackerVetoPt05", &Muon_trackerVetoPt05);
  output_tree->Branch("Muon_sumChargedHadronPt03", &Muon_sumChargedHadronPt03);
  output_tree->Branch("Muon_sumChargedParticlePt03", &Muon_sumChargedParticlePt03);
  output_tree->Branch("Muon_sumNeutralHadronEt03", &Muon_sumNeutralHadronEt03);
  output_tree->Branch("Muon_sumNeutralHadronEtHighThreshold03", &Muon_sumNeutralHadronEtHighThreshold03);
  output_tree->Branch("Muon_sumPhotonEt03", &Muon_sumPhotonEt03);
  output_tree->Branch("Muon_sumPhotonEtHighThreshold03", &Muon_sumPhotonEtHighThreshold03);
  output_tree->Branch("Muon_sumPUPt03", &Muon_sumPUPt03);
  output_tree->Branch("Muon_sumChargedHadronPt04", &Muon_sumChargedHadronPt04);
  output_tree->Branch("Muon_sumChargedParticlePt04", &Muon_sumChargedParticlePt04);
  output_tree->Branch("Muon_sumNeutralHadronEt04", &Muon_sumNeutralHadronEt04);
  output_tree->Branch("Muon_sumNeutralHadronEtHighThreshold04", &Muon_sumNeutralHadronEtHighThreshold04);
  output_tree->Branch("Muon_sumPhotonEt04", &Muon_sumPhotonEt04);
  output_tree->Branch("Muon_sumPhotonEtHighThreshold04", &Muon_sumPhotonEtHighThreshold04);
  output_tree->Branch("Muon_sumPUPt04", &Muon_sumPUPt04);
  output_tree->Branch("Muon_Track_idx", &Muon_Track_idx);
  output_tree->Branch("Muon_hitPattern_pixelLayerwithMeas", &Muon_hitPattern_pixelLayerwithMeas);
  output_tree->Branch("Muon_numberOfMatchedStations", &Muon_numberOfMatchedStations);
  output_tree->Branch("Muon_normChi2", &Muon_normChi2);
  output_tree->Branch("Muon_hitPattern_numberOfValidMuonHits", &Muon_hitPattern_numberOfValidMuonHits);
  output_tree->Branch("Muon_innerTrack_numberofValidHits", &Muon_innerTrack_numberofValidHits);
  output_tree->Branch("Muon_numberOfMatches", &Muon_numberOfMatches);
  output_tree->Branch("Muon_numberOfChambers", &Muon_numberOfChambers);
  output_tree->Branch("Muon_isPFMuon", &Muon_isPFMuon);
  output_tree->Branch("Muon_isRPCMuon", &Muon_isRPCMuon);
  output_tree->Branch("Muon_numberofValidPixelHits", &Muon_numberofValidPixelHits);
  output_tree->Branch("Muon_trackerLayersWithMeasurement", &Muon_trackerLayersWithMeasurement);

  output_tree->Branch("Muon_combinedQuality_updatedSta",&Muon_combinedQuality_updatedSta);
  output_tree->Branch("Muon_combinedQuality_trkKink",&Muon_combinedQuality_trkKink);
  output_tree->Branch("Muon_combinedQuality_glbKink",&Muon_combinedQuality_glbKink);
  output_tree->Branch("Muon_combinedQuality_trkRelChi2",&Muon_combinedQuality_trkRelChi2);
  output_tree->Branch("Muon_combinedQuality_staRelChi2",&Muon_combinedQuality_staRelChi2);
  output_tree->Branch("Muon_combinedQuality_chi2LocalPosition",&Muon_combinedQuality_chi2LocalPosition);
  output_tree->Branch("Muon_combinedQuality_chi2LocalMomentum",&Muon_combinedQuality_chi2LocalMomentum);
  output_tree->Branch("Muon_combinedQuality_localDistance",&Muon_combinedQuality_localDistance);
  output_tree->Branch("Muon_combinedQuality_globalDeltaEtaPhi",&Muon_combinedQuality_globalDeltaEtaPhi);
  output_tree->Branch("Muon_combinedQuality_tightMatch",&Muon_combinedQuality_tightMatch);
  output_tree->Branch("Muon_combinedQuality_glbTrackProbability",&Muon_combinedQuality_glbTrackProbability);

  output_tree->Branch("Muon_prod_inner_outer_charge",&Muon_prod_inner_outer_charge);
  output_tree->Branch("Muon_outerTrack_p4",&Muon_outerTrack_p4);
  output_tree->Branch("Muon_innerTrack_p4",&Muon_innerTrack_p4);
  output_tree->Branch("Muon_innerTrack_quality",&Muon_innerTrack_quality);
  output_tree->Branch("Muon_ptErrOverPt",&Muon_ptErrOverPt);
  output_tree->Branch("Muon_calEnergy_hadS9",&Muon_calEnergy_hadS9);
  output_tree->Branch("Muon_calEnergy_had",&Muon_calEnergy_had);
  output_tree->Branch("Muon_calEnergy_emS25",&Muon_calEnergy_emS25);
  output_tree->Branch("Muon_calEnergy_emS9",&Muon_calEnergy_emS9);
  output_tree->Branch("Muon_calEnergy_em",&Muon_calEnergy_em);

  output_tree->Branch("Muon_ptError",&Muon_ptError);
  output_tree->Branch("Muon_phiError",&Muon_phiError);
  output_tree->Branch("Muon_etaError",&Muon_etaError);

  output_tree->Branch("Muon_segmentCompatibility",&Muon_segmentCompatibility);
  output_tree->Branch("Muon_caloCompatibility",&Muon_caloCompatibility);
  output_tree->Branch("Muon_isGoodMuon_TM2DCompatibility",&Muon_isGoodMuon_TM2DCompatibility);

  output_tree->Branch("Muon_innerTrack_validFraction",&Muon_innerTrack_validFraction );
  output_tree->Branch("Muon_innerTrack_pixelLayersWithMeasurement",&Muon_innerTrack_pixelLayersWithMeasurement );
  output_tree->Branch("Muon_innerTrack_numberOfValidTrackerHits",&Muon_innerTrack_numberOfValidTrackerHits );
  output_tree->Branch("Muon_innerTrack_numberOfLostTrackerHits",&Muon_innerTrack_numberOfLostTrackerHits );
  output_tree->Branch("Muon_innerTrack_numberOfLostTrackerInnerHits",&Muon_innerTrack_numberOfLostTrackerInnerHits );
  output_tree->Branch("Muon_innerTrack_numberOfLostTrackerOuterHits",&Muon_innerTrack_numberOfLostTrackerOuterHits );
  output_tree->Branch("Muon_innerTrack_normalizedChi2",&Muon_innerTrack_normalizedChi2 );
  
  output_tree->Branch("Muon_outerTrack_normalizedChi2",&Muon_outerTrack_normalizedChi2 );
  output_tree->Branch("Muon_outerTrack_muonStationsWithValidHits",&Muon_outerTrack_muonStationsWithValidHits );
  output_tree->Branch("Muon_isGoodMuon_TrackerMuonArbitrated",&Muon_isGoodMuon_TrackerMuonArbitrated );
  output_tree->Branch("Muon_isGoodMuon_TMOneStationTight",&Muon_isGoodMuon_TMOneStationTight );
  output_tree->Branch("Muon_isGoodMuon_TMOneStationAngTight",&Muon_isGoodMuon_TMOneStationAngTight );
  output_tree->Branch("Muon_isGoodMuon_TMLastStationTight",&Muon_isGoodMuon_TMLastStationTight );
  output_tree->Branch("Muon_isGoodMuon_TMLastStationAngTight",&Muon_isGoodMuon_TMLastStationAngTight );
  output_tree->Branch("Muon_isGoodMuon_TMLastStationOptimizedLowPtTight",&Muon_isGoodMuon_TMLastStationOptimizedLowPtTight );
  output_tree->Branch("Muon_isGoodMuon_TMLastStationOptimizedBarrelLowPtTight",&Muon_isGoodMuon_TMLastStationOptimizedBarrelLowPtTight );



  output_tree->Branch("Muon_vmuonhitcomb_reco",&Muon_vmuonhitcomb_reco);
  output_tree->Branch("Muon_rpchits_reco",&Muon_rpchits_reco);

  output_tree->Branch("Muon_ID",&Muon_ID);
  output_tree->Branch("Muon_StandardSelection",&Muon_StandardSelection);

  output_tree->Branch("Muon_charge", &Muon_charge);
  output_tree->Branch("Muon_trackCharge", &Muon_trackCharge);
  output_tree->Branch("Muon_pdgid", &Muon_pdgid);
  output_tree->Branch("Muon_B", &Muon_B);
  output_tree->Branch("Muon_M", &Muon_M);
  output_tree->Branch("Muon_par", &Muon_par);
  output_tree->Branch("Muon_cov", &Muon_cov);

  if(doMC_){
    if(doFullMC_){
      output_tree->Branch("MC_p4", &MC_p4);
      output_tree->Branch("MC_pdgid", &MC_pdgid);
      output_tree->Branch("MC_charge", &MC_charge);
      output_tree->Branch("MC_midx", &MC_midx);
      output_tree->Branch("MC_childpdgid", &MC_childpdgid);
      output_tree->Branch("MC_childidx", &MC_childidx);
      output_tree->Branch("MC_status", &MC_status);
    }
      output_tree->Branch("MC_isReco", &MC_isReco);
      output_tree->Branch("MCSignalParticle_p4", &MCSignalParticle_p4);
      output_tree->Branch("MCSignalParticle_pdgid", &MCSignalParticle_pdgid);
      output_tree->Branch("MCSignalParticle_childpdgid", &MCSignalParticle_childpdgid);
      output_tree->Branch("MCSignalParticle_childp4", &MCSignalParticle_childp4);
      output_tree->Branch("MCSignalParticle_charge", &MCSignalParticle_charge);
      output_tree->Branch("MCSignalParticle_Tauidx", &MCSignalParticle_Tauidx);
      output_tree->Branch("MCTauandProd_p4", &MCTauandProd_p4);
      output_tree->Branch("MCTauandProd_pdgid", &MCTauandProd_pdgid);
      output_tree->Branch("MCTauandProd_midx", &MCTauandProd_midx);
      output_tree->Branch("MCTauandProd_charge", &MCTauandProd_charge);
  }


  //================  Three Muonss block
  output_tree->Branch("ThreeMuons_index",&ThreeMuons_index);
  output_tree->Branch("ThreeMuons_SV_Chi2",&ThreeMuons_SV_Chi2);
  output_tree->Branch("ThreeMuons_SV_NDF",&ThreeMuons_SV_NDF);
  output_tree->Branch("ThreeMuons_TriggerMatch_dR",&ThreeMuons_TriggerMatch_dR);



  output_tree->Branch("signalTau_charge",&signalTau_charge);
  output_tree->Branch("signalTau_isLVP",&signalTau_isLVP);
  output_tree->Branch("signalTau_pdgid",&signalTau_pdgid);
  output_tree->Branch("signalTau_B", &signalTau_B);
  output_tree->Branch("signalTau_M",&signalTau_M);
  output_tree->Branch("signalTau_lvp",&signalTau_lvp);
  output_tree->Branch("signalTau_cov",&signalTau_cov);




  output_tree->Branch("TwoMuonsTrack_Muonsindex",&TwoMuonsTrack_Muonsindex);
  output_tree->Branch("TwoMuonsTrack_Trackindex",&TwoMuonsTrack_Trackindex);
  output_tree->Branch("TwoMuonsTrack_SV_Chi2",&TwoMuonsTrack_SV_Chi2);
  output_tree->Branch("TwoMuonsTrack_SV_NDF",&TwoMuonsTrack_SV_NDF);
  output_tree->Branch("TwoMuonsTrack_TriggerMatch_dR",&TwoMuonsTrack_TriggerMatch_dR);

  output_tree->Branch("Jet_BTagCVSB", &Jet_BTagCVSB);
  output_tree->Branch("Jet_BTagMVA", &Jet_BTagMVA);
  output_tree->Branch("Jet_BTagCSV", &Jet_BTagCSV);
  output_tree->Branch("Jet_p4",&Jet_p4);

  output_tree->Branch("Vertex_N_primary", &Vertex_N_primary);
  output_tree->Branch("Vertex_signal_dca_reco", &Vertex_signal_dca_reco);
  output_tree->Branch("Vertex_signal_KF_pos", &Vertex_signal_KF_pos);
  output_tree->Branch("Vertex_signal_KF_cov", &Vertex_signal_KF_cov);
  output_tree->Branch("Vertex_signal_KF_refittedTracksP4", &Vertex_signal_KF_refittedTracksP4);
  output_tree->Branch("Vertex_signal_KF_Chi2", &Vertex_signal_KF_Chi2);
  output_tree->Branch("Vertex_signal_AF_pos", &Vertex_signal_AF_pos);
  output_tree->Branch("Vertex_signal_AF_Chi2", &Vertex_signal_AF_Chi2);
  output_tree->Branch("Vertex_signal_AF_Ndf", &Vertex_signal_AF_Ndf);
  output_tree->Branch("Vertex_pair_quality", &Vertex_pair_quality);
  output_tree->Branch("Vertex_pairfit_status", &Vertex_pairfit_status);
  output_tree->Branch("Vertex_MatchedPrimaryVertex",&Vertex_MatchedPrimaryVertex);
  output_tree->Branch("Vertex_RefitPVisValid",&Vertex_RefitPVisValid);
  output_tree->Branch("Vertex_MatchedRefitPrimaryVertex",&Vertex_MatchedRefitPrimaryVertex);
  output_tree->Branch("Vertex_MatchedRefitPrimaryVertex_covariance",&Vertex_MatchedRefitPrimaryVertex_covariance);
  output_tree->Branch("Vertex_d0_reco",&Vertex_d0_reco);
  output_tree->Branch("Vertex_d0sig_reco",&Vertex_d0sig_reco);
  output_tree->Branch("Vertex_2Ddisplacement",&Vertex_2Ddisplacement);
  output_tree->Branch("Vertex_3Ddisplacement",&Vertex_3Ddisplacement);
  output_tree->Branch("Vertex_Isolation1",&Vertex_Isolation1);
  output_tree->Branch("Vertex_Isolation2",&Vertex_Isolation2);
  output_tree->Branch("Vertex_Isolation3",&Vertex_Isolation3);
  output_tree->Branch("Vertex_Isolation4",&Vertex_Isolation4);




  output_tree->Branch("Trigger_l1name",&Trigger_l1name);
  output_tree->Branch("Trigger_l1decision",&Trigger_l1decision);
  output_tree->Branch("Trigger_l1prescale",&Trigger_l1prescale);

  output_tree->Branch("Trigger_hltname",&Trigger_hltname);
  output_tree->Branch("Trigger_hltdecision",&Trigger_hltdecision);

  //refitter_.setServices(iSetup);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
T3MNtuple::endJob() 
{
  std::cout << " No Of event processed: " << cnt_ << std::endl;
}

// ------------ method called when starting to processes a run  ------------
/*
  void 
  T3MNtuple::beginRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a run  ------------
/*
  void 
  T3MNtuple::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
  void 
  T3MNtuple::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
  void 
  T3MNtuple::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
T3MNtuple::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}




void T3MNtuple::ClearEvent() {


  puN = 0;

  l1_doublemu0 = 0; l1_triplemu0 = 0; l1_triplemu500 = 0;
  l1_doublemu_10_0 = 0; l1_doublemu_11_4 = 0;
  l1_doublemu0_eta1p6 = 0; l1_doublemu0_eta1p6_os = 0; l1_doublemu0_eta1p4_os = 0;
  prescale_triplemu0 = 0; prescale_doublemu_10_0 = 0; prescale_doublemu0_eta1p6 = 0;
  prescale_triplemu500 = 0; prescale_doublemu_11_4 = 0; prescale_doublemu0_eta1p6_os = 0;
  prescale_doublemu0_eta1p4_os = 0;

  Track_p4.clear();
  Track_normalizedChi2.clear();
  Track_numberOfValidHits.clear();
  Track_charge.clear();
  Track_dxy.clear();
  Track_dz.clear();
  Track_poca.clear();
  Track_dxyError.clear();
  Track_dzError.clear();

  dump_track_index_to_fill.clear();
  dump_pv_index_to_fill.clear();


  Event_nsignal_candidates=0;
  Event_ndsphipi_candidate=0;
 
  //=======  Muons ===
  Muon_p4.clear();
  Muon_Poca.clear();
  Muon_isGlobalMuon.clear();
  Muon_isPFMuon.clear();
  Muon_isRPCMuon.clear();
  Muon_isStandAloneMuon.clear();
  Muon_isTrackerMuon.clear();
  Muon_isCaloMuon.clear();
  Muon_isIsolationValid.clear();
  Muon_isQualityValid.clear();
  Muon_isTimeValid.clear();

  Muon_emEt03.clear();
  Muon_emVetoEt03.clear();
  Muon_hadEt03.clear();
  Muon_hadVetoEt03.clear();
  Muon_nJets03.clear();
  Muon_nTracks03.clear();
  Muon_sumPt03.clear();
  Muon_trackerVetoPt03.clear();
  Muon_ID.clear();
  Muon_StandardSelection.clear();
  Muon_emEt05.clear();
  Muon_emVetoEt05.clear();
  Muon_hadEt05.clear();
  Muon_hadVetoEt05.clear();
  Muon_nJets05.clear();
  Muon_nTracks05.clear();
  Muon_sumPt05.clear();
  Muon_trackerVetoPt05.clear();

  Muon_sumChargedHadronPt03.clear();
  Muon_sumChargedParticlePt03.clear();
  Muon_sumNeutralHadronEt03.clear();
  Muon_sumNeutralHadronEtHighThreshold03.clear();
  Muon_sumPhotonEt03.clear();
  Muon_sumPhotonEtHighThreshold03.clear();
  Muon_sumPUPt03.clear();

  Muon_sumChargedHadronPt04.clear();
  Muon_sumChargedParticlePt04.clear();
  Muon_sumNeutralHadronEt04.clear();
  Muon_sumNeutralHadronEtHighThreshold04.clear();
  Muon_sumPhotonEt04.clear();
  Muon_sumPhotonEtHighThreshold04.clear();
  Muon_sumPUPt04.clear();

  Muon_numberOfChambers.clear();
  Muon_Track_idx.clear();

  Muon_charge.clear();
  Muon_trackCharge.clear();
  Muon_pdgid.clear();
  Muon_B.clear();
  Muon_M.clear();
  Muon_par.clear();
  Muon_cov.clear();

  Muon_hitPattern_pixelLayerwithMeas.clear();
  Muon_numberOfMatchedStations.clear();
  Muon_normChi2.clear();
  Muon_hitPattern_numberOfValidMuonHits.clear();
  Muon_innerTrack_numberofValidHits.clear();
  Muon_numberOfMatches.clear();
  Muon_numberofValidPixelHits.clear();
  Muon_trackerLayersWithMeasurement.clear();

  Muon_vmuonhitcomb_reco.clear();
  Muon_rpchits_reco.clear();

  Muon_combinedQuality_updatedSta.clear();
  Muon_combinedQuality_trkKink.clear();
  Muon_combinedQuality_glbKink.clear();
  Muon_combinedQuality_trkRelChi2.clear();
  Muon_combinedQuality_staRelChi2.clear();
  Muon_combinedQuality_chi2LocalPosition.clear();
  Muon_combinedQuality_chi2LocalMomentum.clear();
  Muon_combinedQuality_localDistance.clear();
  Muon_combinedQuality_globalDeltaEtaPhi.clear();
  Muon_combinedQuality_tightMatch.clear();
  Muon_combinedQuality_glbTrackProbability.clear();

  Muon_prod_inner_outer_charge.clear();
  Muon_outerTrack_p4.clear();
  Muon_innerTrack_p4.clear();
  Muon_innerTrack_quality.clear();
  Muon_ptErrOverPt.clear();
  Muon_calEnergy_hadS9.clear();
  Muon_calEnergy_had.clear();
  Muon_calEnergy_emS25.clear();
  Muon_calEnergy_emS9.clear();
  Muon_calEnergy_em.clear();

  Muon_segmentCompatibility.clear();
  Muon_caloCompatibility.clear();

  Muon_ptError.clear();
  Muon_phiError.clear();
  Muon_etaError.clear();

  Muon_innerTrack_validFraction.clear();
  Muon_innerTrack_pixelLayersWithMeasurement.clear();
  Muon_innerTrack_numberOfValidTrackerHits.clear();
  Muon_innerTrack_numberOfLostTrackerHits.clear();
  Muon_innerTrack_numberOfLostTrackerInnerHits.clear();
  Muon_innerTrack_numberOfLostTrackerOuterHits.clear();
  Muon_innerTrack_normalizedChi2.clear();
  
  Muon_outerTrack_normalizedChi2.clear();
  Muon_outerTrack_muonStationsWithValidHits.clear();
  Muon_isGoodMuon_TM2DCompatibility.clear();
  Muon_isGoodMuon_TrackerMuonArbitrated.clear();
  Muon_isGoodMuon_TMOneStationTight.clear();
  Muon_isGoodMuon_TMOneStationAngTight.clear();
  Muon_isGoodMuon_TMLastStationTight.clear();
  Muon_isGoodMuon_TMLastStationAngTight.clear();
  Muon_isGoodMuon_TMLastStationOptimizedLowPtTight.clear();
  Muon_isGoodMuon_TMLastStationOptimizedBarrelLowPtTight.clear();


  if (doMC_) {
    MC_isReco=0;
    MC_p4.clear();
    MC_pdgid.clear();
    MC_charge.clear();
    MC_midx.clear();
    MC_status.clear();
    MC_childpdgid.clear();
    MC_childidx.clear();
    MCSignalParticle_p4.clear();
    MCSignalParticle_pdgid.clear();
    MCSignalParticle_charge.clear();
    MCSignalParticle_Poca.clear();
    MCSignalParticle_Tauidx.clear();
    MCTauandProd_p4.clear();
    MCTauandProd_Vertex.clear();
    MCTauandProd_pdgid.clear();
    MCTauandProd_midx.clear();
    MCTauandProd_charge.clear();
    MCTau_JAK.clear();
    MCTau_DecayBitMask.clear();
    MCSignalParticle_childpdgid.clear();
    MCSignalParticle_childp4.clear();
  }


  ThreeMuons_idx.clear();
  ThreeMuons_index.clear();
  ThreeMuons_SV_Chi2.clear();
  ThreeMuons_SV_NDF.clear();
  ThreeMuons_TriggerMatch_dR.clear();

  signalTau_charge.clear();
  signalTau_isLVP.clear();
  signalTau_pdgid.clear();
  signalTau_B.clear();
  signalTau_M.clear();
  signalTau_lvp.clear();
  signalTau_cov.clear();

  TwoMuonsTrack_idx.clear();
  TwoMuonsTrack_Muonsindex.clear();
  TwoMuonsTrack_Trackindex.clear();
  TwoMuonsTrack_SV_Chi2.clear();
  TwoMuonsTrack_SV_NDF.clear();
  TwoMuonsTrack_TriggerMatch_dR.clear();

  Jet_BTagCVSB.clear();
  Jet_BTagMVA.clear();
  Jet_BTagCSV.clear();
  Jet_p4.clear();

  Vertex_signal_dca_reco.clear();
  Vertex_signal_KF_pos.clear();
  Vertex_signal_KF_cov.clear();
  Vertex_signal_KF_refittedTracksP4.clear();
  Vertex_signal_KF_Chi2.clear();
  Vertex_signal_AF_pos.clear();
  Vertex_signal_AF_Chi2.clear();
  Vertex_signal_AF_Ndf.clear();
  Vertex_pair_quality.clear();
  Vertex_pairfit_status.clear();
  Vertex_MatchedPrimaryVertex.clear();
  Vertex_RefitPVisValid.clear();
  Vertex_MatchedRefitPrimaryVertex.clear();
  Vertex_MatchedRefitPrimaryVertex_covariance.clear();
  Vertex_d0_reco.clear();
  Vertex_d0sig_reco.clear();
  Vertex_2Ddisplacement.clear();
  Vertex_3Ddisplacement.clear();
  Vertex_Isolation1.clear();
  Vertex_Isolation2.clear();
  Vertex_Isolation3.clear();
  Vertex_Isolation4.clear();





  Trigger_l1name.clear();
  Trigger_l1decision.clear();
  Trigger_l1prescale.clear();
  Trigger_hltname.clear();
  Trigger_hltdecision.clear();



}

//define this as a plug-in
DEFINE_FWK_MODULE(T3MNtuple);
