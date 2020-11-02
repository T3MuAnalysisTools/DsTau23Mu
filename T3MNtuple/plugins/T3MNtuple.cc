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
double T3MNtuple::phimassmin_(1.7);
double T3MNtuple::phimassmax_(3.0);


//
// constructors and destructor
//
T3MNtuple::T3MNtuple(const edm::ParameterSet& iConfig):
   TriggerMuonMatchingdr_(iConfig.getUntrackedParameter("TriggerMuonMatchingdr", (double) 0.3)),
   WhatData_(iConfig.getUntrackedParameter<string>("WhatData","2017")),
   muonToken_(consumes<MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"))),
   btagCvsBToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagsCvsB"))),
   btagCSVToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagsCSV"))),
   btagMVAToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagsMVA"))),
   //   btagDeepCSVToken_(consumes<reco::JetTagCollection>(iConfig.getParameter<edm::InputTag>("btagDeepCSV"))), ## absent in AOD
   vtxToken_(consumes<VertexCollection>(iConfig.getParameter<InputTag>("pvs"))),
   //ksToken_(consumes<CompositeVertexPtrCollection>(iConfig.getParameter<InputTag>("kshort"))),
   //conversionToken_(consumes<CompositeiCandidateCollection>(iConfig.getParameter<InputTag>("conversions"))),
   svToken_(consumes<VertexCollection>(iConfig.getParameter<InputTag>("svs"))),
   photonToken_(consumes<PhotonCollection>(iConfig.getParameter<edm::InputTag>("phos"))),
   trackToken_(consumes<TrackCollection>(iConfig.getParameter<InputTag>("trks"))),
   triggerToken_(consumes<TriggerResults>(iConfig.getParameter<InputTag>("triggerBitsH"))),
   trigeventToken_(consumes<trigger::TriggerEvent>(iConfig.getParameter<InputTag>("triggerSummary"))),
   triggerObjectToken_(consumes<vector<pat::TriggerObjectStandAlone>>(iConfig.getParameter<InputTag>("triggerObjects"))),
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
   doPhotons_ = iConfig.getParameter<bool>("doPhotons");
   doThreeMuons_=  iConfig.getParameter<bool>("doThreeMuons");
   doTwoMuonsAndTrack_= iConfig.getParameter<bool>("doTwoMuonsAndTrack");
   MuonPtCut_ = iConfig.getParameter<double>("MuonPtCut"); //default: 1.0
   MuonEtaCut_ = iConfig.getParameter<double>("MuonEtaCut"); //default: 2.4
   TrackPtCut_ = iConfig.getParameter<double>("TrackPtCut"); //default: 1.0
   TrackEtaCut_ = iConfig.getParameter<double>("TrackEtaCut"); //default: 2.4
   miniAODRun_ = iConfig.getParameter<bool>("miniAODRun"); // true if running over MiniAOD

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
      if(fabs(track.eta())<TrackEtaCut_){return true;
         if(track.hitPattern().trackerLayersWithMeasurement()>5){
            if(track.hitPattern().pixelLayersWithMeasurement()>1) return true;
         }
      }
   }
   return false;
}


std::vector<int> T3MNtuple::SortByPt(std::vector<TLorentzVector> invec){
   double pt1=invec.at(0).Pt();
   double pt2=invec.at(1).Pt();
   double pt3=invec.at(2).Pt();

   std::vector<int> out;
   int i1,i2,i3;


   if(pt1>pt2)
   {
      if(pt2>pt3)
      {
         i1=0; i2 = 1; i3 = 2;
      }
      else if(pt1>pt3)
      {
         i1=0; i2 = 2; i3 = 1;
      }
      else
      {
         i1=2; i2 = 0; i3 = 1;
      }
   }
   else
   {
      if(pt1>pt3)
      {
         i1=1; i2 = 0; i3 = 2;
      }
      else if(pt2>pt3)
      {
         i1=1; i2 = 2; i3 = 0;
      }
      else
      {
         i1=2; i2 = 1; i3 = 0;
      }
   }

   out.push_back(i1);  out.push_back(i2);  out.push_back(i3);

   return out;
}



bool T3MNtuple::AcceptedMuon(MuonRef RefMuon) {
   if((RefMuon->pt() > MuonPtCut_) && (abs(RefMuon->eta()) < MuonEtaCut_)){
      //    if(   /*RefMuon->isPFMuon() &&*/  ( RefMuon->isGlobalMuon() || RefMuon->isTrackerMuon()))   return true;
      //    if(   /*RefMuon->isPFMuon() &&*/  ( RefMuon->isGlobalMuon() || RefMuon->isTrackerMuon()) and 
      //	  RefMuon->innerTrack().isNonnull() && RefMuon->innerTrack()->hitPattern().numberOfValidPixelHits() > 0)   return true;

      //    if( RefMuon->innerTrack().isNonnull() && RefMuon->innerTrack()->hitPattern().numberOfValidPixelHits() > 0)   return true;


      if(RefMuon->innerTrack().isNonnull() && RefMuon->innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && 
            ( RefMuon->isGlobalMuon() || RefMuon->isTrackerMuon()))      return true;


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
   if(id == 22 && GenPar.p4().Pt() < 0.1 ) return true;
   return false;


}

bool T3MNtuple::isGoodGenParticle(const reco::GenParticle &GenPar){

   int id = abs(GenPar.pdgId());
   if (id == PDGInfo::Ds_plus) return true;
   if (id == PDGInfo::B_plus) return true;
   if (id == PDGInfo::B_0) return true;
   return false;
}

// ------------ method called for each event  ------------
   void
T3MNtuple::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{ 
  //   std::cout<<" ========================  new event =============== "<< std::endl;
   cnt_++;
   ClearEvent();

   fillEventInfo(iEvent, iSetup);
   if(doMC_)
      fillMCTruth(iEvent, iSetup);

   if(doThreeMuons_) {
      Event_nsignal_candidates =   fillThreeMuons(iEvent, iSetup);
      //cout<<"Number of signal candidates = "<<Event_nsignal_candidates<<endl;
      //cout<<"Number of dsphipi candidates = "<<Event_ndsphipi_candidate<<endl;
   }

   if(doTwoMuonsAndTrack_)     Event_ndsphipi_candidate = fillTwoMuonsAndTracks(iEvent, iSetup);
   //  std::cout<<"  "<< Event_nsignal_candidates << "   "<< Event_ndsphipi_candidate << std::endl;
   if(Event_nsignal_candidates!=0 or Event_ndsphipi_candidate!=0)
   {
      MC_isReco=1;
      fillVertices(iEvent, iSetup);
      if(doBJets_ && !miniAODRun_)
         fillBTagJets(iEvent, iSetup);
      if(doPhotons_)
         fillPhotons(iEvent, iSetup);
      if(doMuons_)
         fillMuons(iEvent, iSetup);
      if(doTracks_)
         fillTracks(iEvent, iSetup);
      output_tree->Fill();
   }
<<<<<<< HEAD

   if(doL1_)
      fillTrigger(iEvent, iSetup);
   //fillDsTree(iEvent, iSetup); 
=======
   output_tree->Fill();
>>>>>>> 97eb3193a8388ffe0ca375fa2a1660fe9402cc67
}



void T3MNtuple::fillVertices(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   Handle<VertexCollection> pvs;
   iEvent.getByToken(vtxToken_ , pvs);
   Vertex_N_primary = pvs->size();

   Handle<VertexCollection> svs;
   iEvent.getByToken(svToken_ , svs);

   Handle<MuonCollection> muonCollection;
   iEvent.getByToken(muonToken_, muonCollection);

   Handle<TrackCollection> trackCollection;
   iEvent.getByToken(trackToken_, trackCollection);

   BeamSpot bs;
   Handle<BeamSpot> beamSpotHandle;
   iEvent.getByToken(bsToken_, beamSpotHandle);
   bs = *beamSpotHandle;

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
            MuonRef Muon(muonCollection, iMuon);
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
   }

   if(TwoMuonsTrack_idx.size()!=0){
      std::vector<std::vector<double> > iparticle_p4;
      for ( auto &iTwoMuonsTracks :  TwoMuonsTrack_idx ) {

         vector<TransientTrack> isignalTracksCollection;

         MuonRef Muon1(muonCollection, iTwoMuonsTracks.at(0));
         MuonRef Muon2(muonCollection, iTwoMuonsTracks.at(1));

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

   //   std::cout<<"size  "<< signalTracksCollection.size() << std::endl;
   unsigned int index(0);
   for ( auto &iTransientTracks :  signalTracksCollection ){
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
         transVtx = kvf.vertex(iTransientTracks); 
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

         reco::Vertex TripletVtx = reco::Vertex(TheSecondaryVertexPoint, svCov, transVtx.totalChiSquared(), 3, 3);

         VertexState BSstate(bs);
         VertexDistanceXY vertTool2D;
         double BSdistance2D = vertTool2D.distance(BSstate, TripletVtx).value();
         double BSdist_err2D = vertTool2D.distance(BSstate, TripletVtx).error();
         double BSdist_sign2D =vertTool2D.distance(BSstate, TripletVtx).significance();

         Vertex_signal_KF_BS_2Ddistance.push_back(BSdistance2D);
         Vertex_signal_KF_BS_error.push_back(BSdist_err2D);
         Vertex_signal_KF_BS_significance.push_back(BSdist_sign2D);

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

      } else {
         Vertex_signal_AF_Chi2.push_back(-1);
      }

      //--------------------  Fit each track pair 
      vector<TransientTrack> trackpair12, trackpair23, trackpair31;
      trackpair12.push_back(iTransientTracks.at(0)); trackpair12.push_back(iTransientTracks.at(1));
      trackpair23.push_back(iTransientTracks.at(1)); trackpair23.push_back(iTransientTracks.at(2));
      trackpair31.push_back(iTransientTracks.at(2)); trackpair31.push_back(iTransientTracks.at(0));
      KalmanVertexFitter kvf_trks12(true), kvf_trks23(true), kvf_trks31(true);
      TransientVertex fv_trks12;// = kvf_trks12.vertex(trackpair12);
      TransientVertex fv_trks23;// = kvf_trks23.vertex(trackpair23);
      TransientVertex fv_trks31;// = kvf_trks31.vertex(trackpair31);


      bool Fit1Ok(true);
      try {
         fv_trks12 = kvf_trks12.vertex(trackpair12); 
      } catch (...) {
         Fit1Ok = false;
      }

      bool Fit2Ok(true);
      try {
         fv_trks23 = kvf_trks23.vertex(trackpair23); 
      } catch (...) {
         Fit2Ok = false;
      }

      bool Fit3Ok(true);
      try {
         fv_trks31 = kvf_trks31.vertex(trackpair31); 
      } catch (...) {
         Fit3Ok = false;
      }

      std::vector<double> iVertex_pair_quality;
      std::vector<double> iVertex_pairfit_status;
      iVertex_pairfit_status.push_back(Fit1Ok);
      iVertex_pairfit_status.push_back(Fit2Ok);
      iVertex_pairfit_status.push_back(Fit3Ok);

      std::vector<float> iVertex_Pair12_Pos;
      std::vector<float> iVertex_Pair23_Pos;
      std::vector<float> iVertex_Pair31_Pos;
      if(fv_trks12.isValid())
      {
         iVertex_pair_quality.push_back(fv_trks12.totalChiSquared());
         iVertex_Pair12_Pos.push_back( fv_trks12.position().x());
         iVertex_Pair12_Pos.push_back( fv_trks12.position().y());
         iVertex_Pair12_Pos.push_back( fv_trks12.position().z());
      }

      else
      {
         iVertex_pair_quality.push_back(-1);
         iVertex_Pair12_Pos.push_back( 99.);
         iVertex_Pair12_Pos.push_back( 99.);
         iVertex_Pair12_Pos.push_back( 99.);
      }
      if(fv_trks23.isValid())
      {
         iVertex_pair_quality.push_back(fv_trks23.totalChiSquared());
         iVertex_Pair23_Pos.push_back( fv_trks23.position().x());
         iVertex_Pair23_Pos.push_back( fv_trks23.position().y());
         iVertex_Pair23_Pos.push_back( fv_trks23.position().z());
      }
      else
      {
         iVertex_pair_quality.push_back(-1);
         iVertex_Pair23_Pos.push_back( 99.);
         iVertex_Pair23_Pos.push_back( 99.);
         iVertex_Pair23_Pos.push_back( 99.);
      }
      if(fv_trks31.isValid())
      {
         iVertex_pair_quality.push_back(fv_trks31.totalChiSquared());
         iVertex_Pair31_Pos.push_back( fv_trks31.position().x());
         iVertex_Pair31_Pos.push_back( fv_trks31.position().y());
         iVertex_Pair31_Pos.push_back( fv_trks31.position().z());
      }
      else
      {
         iVertex_pair_quality.push_back(-1);
         iVertex_Pair31_Pos.push_back( 99.);
         iVertex_Pair31_Pos.push_back( 99.);
         iVertex_Pair31_Pos.push_back( 99.);
      }

      Vertex_Pair12_Pos.push_back( iVertex_Pair12_Pos);
      Vertex_Pair23_Pos.push_back( iVertex_Pair23_Pos);
      Vertex_Pair31_Pos.push_back( iVertex_Pair31_Pos);


      Vertex_pair_quality.push_back(iVertex_pair_quality);
      Vertex_pairfit_status.push_back(iVertex_pairfit_status);

      ///////////////////////////////////////////
      //  find here the primary vertex with the best
      //  alignement to the tri-muon 
      double dphi_pv = -1.0;
      unsigned int primaryvertex_index;
      for(unsigned int vertex_index = 0; vertex_index  < pvs->size(); vertex_index++) {
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

      double tempdz(99.);
      unsigned int secondbest_primaryvertex_index(0);
      for(unsigned int vertex_index = 0; vertex_index  < pvs->size(); vertex_index++) {
         if(vertex_index == primaryvertex_index) continue;
         const Vertex & temp_pv = (*pvs)[vertex_index];
         if(fabs(temp_pv.z() -  MatchedPrimaryVertex.z()) < tempdz ){
            tempdz = fabs(temp_pv.z() -  MatchedPrimaryVertex.z());
            secondbest_primaryvertex_index = vertex_index;
         }
      }


      const Vertex & SecondBestPrimaryVertex = (*pvs)[secondbest_primaryvertex_index];
      std::vector<double> iSecondBestprimaryVertex_Pos;
      if( pvs->size()>1){
         iSecondBestprimaryVertex_Pos.push_back(SecondBestPrimaryVertex.x());
         iSecondBestprimaryVertex_Pos.push_back(SecondBestPrimaryVertex.y());
         iSecondBestprimaryVertex_Pos.push_back(SecondBestPrimaryVertex.z());
      }else{
         iSecondBestprimaryVertex_Pos.push_back(-99.);
         iSecondBestprimaryVertex_Pos.push_back(-99.);
         iSecondBestprimaryVertex_Pos.push_back(-99.);
      }
      Vertex_SecondBestPrimaryVertex.push_back(iSecondBestprimaryVertex_Pos);


      int NParticlesComingFromPV(0);
      vector<TransientTrack> primaryvertexTransientTracks;// remove muon candidates from the PV to perform refit
      for(Vertex::trackRef_iterator itk = MatchedPrimaryVertex.tracks_begin(); itk != MatchedPrimaryVertex.tracks_end(); itk++) {
         if((**itk).pt()>1) {
            if(deltaR(iTransientTracks.at(0).track().eta(), iTransientTracks.at(0).track().phi(), (**itk).eta(), (**itk).phi())<0.01){NParticlesComingFromPV++;continue;}
            if(deltaR(iTransientTracks.at(1).track().eta(), iTransientTracks.at(1).track().phi(), (**itk).eta(), (**itk).phi())<0.01){NParticlesComingFromPV++;continue;}
            if(deltaR(iTransientTracks.at(2).track().eta(), iTransientTracks.at(2).track().phi(), (**itk).eta(), (**itk).phi())<0.01){NParticlesComingFromPV++;continue;}
         }
         primaryvertexTransientTracks.push_back(theB->build(**itk));
      }
      Vertex_NMuonsAssocWithPV.push_back(NParticlesComingFromPV);
      KalmanVertexFitter pv_fit(true);
      bool FitPVOk(true);
      TransientVertex pvvertex;

      if(primaryvertexTransientTracks.size() >1){
         try {
            pvvertex =  pv_fit.vertex(primaryvertexTransientTracks);
         } catch (...) {
            FitPVOk = false;
         }
      }

      Vertex_RefitPVisValid.push_back(pvvertex.isValid());
      std::vector<double> iRefitprimaryVertex_Pos;
      if(FitPVOk && pvvertex.isValid()){
         iRefitprimaryVertex_Pos.push_back(pvvertex.position().x());
         iRefitprimaryVertex_Pos.push_back(pvvertex.position().y());
         iRefitprimaryVertex_Pos.push_back(pvvertex.position().z());
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
      math::XYZPoint bsPoint = math::XYZPoint(beamSpotHandle->position().x(), beamSpotHandle->position().y(), beamSpotHandle->position().z());



      std::vector<double> iVertex_d0BeamSpot_reco;
      iVertex_d0BeamSpot_reco.push_back(abs(iTransientTracks.at(0).track().dxy(bsPoint)));
      iVertex_d0BeamSpot_reco.push_back(abs(iTransientTracks.at(1).track().dxy(bsPoint)));
      iVertex_d0BeamSpot_reco.push_back(abs(iTransientTracks.at(2).track().dxy(bsPoint)));
      Vertex_d0BeamSpot_reco.push_back(iVertex_d0BeamSpot_reco);


      std::vector<double> iVertex_d0BeamSpot_reco_sig;
      double d0ErrorToBs_1  = sqrt( iTransientTracks.at(0).track().d0Error() * iTransientTracks.at(0).track().d0Error() +
            0.5*  beamSpotHandle->BeamWidthX()* beamSpotHandle->BeamWidthX()+
            0.5*  beamSpotHandle->BeamWidthY()* beamSpotHandle->BeamWidthY() );

      double d0ErrorToBs_2  = sqrt( iTransientTracks.at(1).track().d0Error() * iTransientTracks.at(1).track().d0Error() +
            0.5*  beamSpotHandle->BeamWidthX()* beamSpotHandle->BeamWidthX()+
            0.5*  beamSpotHandle->BeamWidthY()* beamSpotHandle->BeamWidthY() );

      double d0ErrorToBs_3  = sqrt( iTransientTracks.at(2).track().d0Error() * iTransientTracks.at(2).track().d0Error() +
            0.5*  beamSpotHandle->BeamWidthX()* beamSpotHandle->BeamWidthX()+
            0.5*  beamSpotHandle->BeamWidthY()* beamSpotHandle->BeamWidthY() );



      if(d0ErrorToBs_1!=0){  iVertex_d0BeamSpot_reco_sig.push_back( abs(iTransientTracks.at(0).track().dxy(bsPoint)) / d0ErrorToBs_1);} else {iVertex_d0BeamSpot_reco_sig.push_back(-1);}
      if(d0ErrorToBs_2!=0){  iVertex_d0BeamSpot_reco_sig.push_back( abs(iTransientTracks.at(1).track().dxy(bsPoint)) / d0ErrorToBs_2);} else {iVertex_d0BeamSpot_reco_sig.push_back(-1);}
      if(d0ErrorToBs_3!=0){  iVertex_d0BeamSpot_reco_sig.push_back( abs(iTransientTracks.at(1).track().dxy(bsPoint)) / d0ErrorToBs_3);} else {iVertex_d0BeamSpot_reco_sig.push_back(-1);}

      Vertex_d0BeamSpot_reco_sig.push_back(iVertex_d0BeamSpot_reco_sig);


      std::vector<double> iVertex_d0SV_reco;
      iVertex_d0SV_reco.push_back(abs(iTransientTracks.at(0).track().dxy(TheSecondaryVertexPoint)));
      iVertex_d0SV_reco.push_back(abs(iTransientTracks.at(1).track().dxy(TheSecondaryVertexPoint)));
      iVertex_d0SV_reco.push_back(abs(iTransientTracks.at(2).track().dxy(TheSecondaryVertexPoint)));
      Vertex_d0SV_reco.push_back(iVertex_d0SV_reco);


      std::vector<double> iVertex_dzSV_reco;
      iVertex_dzSV_reco.push_back(abs(iTransientTracks.at(0).track().dz(TheSecondaryVertexPoint)));
      iVertex_dzSV_reco.push_back(abs(iTransientTracks.at(1).track().dz(TheSecondaryVertexPoint)));
      iVertex_dzSV_reco.push_back(abs(iTransientTracks.at(2).track().dz(TheSecondaryVertexPoint)));
      Vertex_dzSV_reco.push_back(iVertex_dzSV_reco);



      std::vector<double> iVertex_d0_reco;
      iVertex_d0_reco.push_back(abs(iTransientTracks.at(0).track().dxy(pvPoint)));
      iVertex_d0_reco.push_back(abs(iTransientTracks.at(1).track().dxy(pvPoint)));
      iVertex_d0_reco.push_back(abs(iTransientTracks.at(2).track().dxy(pvPoint)));
      Vertex_d0_reco.push_back(iVertex_d0_reco);


      std::vector<double> iVertex_dz_reco;
      iVertex_dz_reco.push_back(abs(iTransientTracks.at(0).track().dz(pvPoint)));
      iVertex_dz_reco.push_back(abs(iTransientTracks.at(1).track().dz(pvPoint)));
      iVertex_dz_reco.push_back(abs(iTransientTracks.at(2).track().dz(pvPoint)));
      Vertex_dz_reco.push_back(iVertex_dz_reco);



      TLorentzVector LV1=TLorentzVector(particles_p4.at(index).at(0).at(1), particles_p4.at(index).at(0).at(2), particles_p4.at(index).at(0).at(3),particles_p4.at(index).at(0).at(0));
      TLorentzVector LV2=TLorentzVector(particles_p4.at(index).at(1).at(1), particles_p4.at(index).at(1).at(2), particles_p4.at(index).at(1).at(3),particles_p4.at(index).at(1).at(0));
      TLorentzVector LV3=TLorentzVector(particles_p4.at(index).at(2).at(1), particles_p4.at(index).at(2).at(2), particles_p4.at(index).at(2).at(3),particles_p4.at(index).at(2).at(0));
      TLorentzVector LVTau = LV1 + LV2 + LV3;
      m3mu_reco = LVTau.M();

      GlobalVector dir1(particles_p4.at(index).at(0).at(1), particles_p4.at(index).at(0).at(2), particles_p4.at(index).at(0).at(3));
      GlobalVector dir2(particles_p4.at(index).at(1).at(1), particles_p4.at(index).at(1).at(2), particles_p4.at(index).at(1).at(3));
      GlobalVector dir3(particles_p4.at(index).at(2).at(1), particles_p4.at(index).at(2).at(2), particles_p4.at(index).at(2).at(3));

      std::pair<bool, Measurement1D> ip2d_1 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(0), dir1, final_pv);
      std::pair<bool, Measurement1D> ip2d_2 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(1), dir2, final_pv);
      std::pair<bool, Measurement1D> ip2d_3 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(2), dir3, final_pv);


      std::pair<bool, Measurement1D> ip2dSV_1 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(0), dir1, transVtx);
      std::pair<bool, Measurement1D> ip2dSV_2 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(1), dir2, transVtx);
      std::pair<bool, Measurement1D> ip2dSV_3 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(2), dir3, transVtx);


      //  std::pair<bool, Measurement1D> ipBS2d_1 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(0), dir1, );
      //  std::pair<bool, Measurement1D> ipBS2d_2 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(1), dir2, final_pv);
      //  std::pair<bool, Measurement1D> ipBS2d_3 = IPTools::signedTransverseImpactParameter(iTransientTracks.at(2), dir3, final_pv);

      std::vector<double> iVertex_d0sig_reco;
      if(ip2d_1.first){ iVertex_d0sig_reco.push_back(abs(ip2d_1.second.value()/ip2d_1.second.error()));} else{iVertex_d0sig_reco.push_back(-1);}
      if(ip2d_2.first){ iVertex_d0sig_reco.push_back(abs(ip2d_2.second.value()/ip2d_2.second.error()));} else{iVertex_d0sig_reco.push_back(-1);}
      if(ip2d_3.first){ iVertex_d0sig_reco.push_back(abs(ip2d_3.second.value()/ip2d_3.second.error()));} else{iVertex_d0sig_reco.push_back(-1);}
      Vertex_d0sig_reco.push_back(iVertex_d0sig_reco);


      std::vector<double> iVertex_d0sigSV_reco;
      if(ip2dSV_1.first){ iVertex_d0sigSV_reco.push_back(abs(ip2dSV_1.second.value()/ip2dSV_1.second.error()));} else{iVertex_d0sigSV_reco.push_back(-1);}
      if(ip2dSV_2.first){ iVertex_d0sigSV_reco.push_back(abs(ip2dSV_2.second.value()/ip2dSV_2.second.error()));} else{iVertex_d0sigSV_reco.push_back(-1);}
      if(ip2dSV_3.first){ iVertex_d0sigSV_reco.push_back(abs(ip2dSV_3.second.value()/ip2dSV_3.second.error()));} else{iVertex_d0sigSV_reco.push_back(-1);}
      Vertex_d0sigSV_reco.push_back(iVertex_d0sigSV_reco);


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


      TVector3 vtauxyz(ThreeCandidate.Px(), ThreeCandidate.Py(), ThreeCandidate.Pz());
      TVector3 dv3D_reco(-final_pv.position().x() + TheSecondaryVertexPoint.x(), -final_pv.position().y() + TheSecondaryVertexPoint.y(), -final_pv.position().z() + TheSecondaryVertexPoint.z());
      fv_cosdphi3D = dv3D_reco.Dot(vtauxyz)/(dv3D_reco.Mag()*vtauxyz.Mag());
      VertexDistance3D dist;

      std::vector<double> iVertex_3Ddisplacement;
      iVertex_3Ddisplacement.push_back(dist.distance(Vertex(transVtx), final_pv).value());
      iVertex_3Ddisplacement.push_back(dist.distance(Vertex(transVtx), final_pv).significance());
      iVertex_3Ddisplacement.push_back(fv_d3D*fv_cosdphi3D*m3mu_reco/ThreeCandidate.P());
      Vertex_3Ddisplacement.push_back(iVertex_3Ddisplacement);


      ///////////////////////////////////////////////////////////////
      //    Here fill the isolation


      IsolationBranch_Trackp4.push_back(std::vector<std::vector<float> >());
      for(Vertex::trackRef_iterator itk = MatchedPrimaryVertex.tracks_begin(); itk != MatchedPrimaryVertex.tracks_end(); itk++) {
         if(deltaR(iTransientTracks.at(0).track().eta(), iTransientTracks.at(0).track().phi(), (**itk).eta(), (**itk).phi())<0.001)continue;
         if(deltaR(iTransientTracks.at(1).track().eta(), iTransientTracks.at(1).track().phi(), (**itk).eta(), (**itk).phi())<0.001)continue;
         if(deltaR(iTransientTracks.at(2).track().eta(), iTransientTracks.at(2).track().phi(), (**itk).eta(), (**itk).phi())<0.001)continue;

         std::vector<float> iIsolationBranch_Track_p4;

         iIsolationBranch_Track_p4.push_back(sqrt(pow((**itk).p(),2.0) + pow(PDGInfo::pi_mass(),2.0)));
         iIsolationBranch_Track_p4.push_back((**itk).px());
         iIsolationBranch_Track_p4.push_back((**itk).py());
         iIsolationBranch_Track_p4.push_back((**itk).pz());

         IsolationBranch_Trackp4.at(IsolationBranch_Trackp4.size() - 1).push_back(iIsolationBranch_Track_p4);
      }


      //  former Isolation
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


      //--------------------------- Isolation Branch

      std::vector<TLorentzVector>  MuLVs;
      MuLVs.push_back(LV1);
      MuLVs.push_back(LV2);
      MuLVs.push_back(LV3);


      std::vector<int> sortedindices = SortByPt(MuLVs);


      IsolationTrack_p4.push_back(std::vector<std::vector<float> >());
      IsolationTrack_VertexWithSignalMuonIsValid.push_back(std::vector<std::vector<int> >());
      IsolationTrack_VertexWithSignalMuonChi2.push_back(std::vector<std::vector<float> >());
      IsolationTrack_VertexWithSignalMuonPosition.push_back(std::vector<std::vector<float> >());

      std::vector<float> iIsolationTrack_dxySV;
      std::vector<float> iIsolationTrack_dzSV;
      std::vector<float> iIsolationTrack_dxyPV;
      std::vector<float> iIsolationTrack_dzPV;
      std::vector<float> iIsolationTrack_DocaMu1;
      std::vector<float> iIsolationTrack_DocaMu2;
      std::vector<float> iIsolationTrack_DocaMu3;
      std::vector<int>   iIsolationTrack_charge;
      std::vector<int>   iIsolationTrack_isHighPurity;
      std::vector<int>   IsolationTracksIndices;


      vector<TransientTrack> iIsolationTracksCollection;
      ESHandle<TransientTrackBuilder> TheB;
      std::vector<std::vector<int> >  VertexWithSignalMuonIsValid;
      iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",TheB);



      //      std::cout<<" sig   " << ThreeMuons_idx.size() << " two mu plus track   "<< TwoMuonsTrack_idx.size() <<  " signaltrack collect     "<< signalTracksCollection.size() <<  " signal Counter  "<<index <<std::endl;




      for(size_t i = 0; i < trackCollection->size(); i++) {
         const Track & t = (*trackCollection)[i];


         if(!(t.quality(TrackBase::tight)))continue; //-- this might be weaker
         if(deltaR(LV1.Eta(), LV1.Phi(), t.eta(), t.phi())<0.01)continue;
         if(deltaR(LV2.Eta(), LV2.Phi(), t.eta(), t.phi())<0.01)continue;
         if(deltaR(LV3.Eta(), LV3.Phi(), t.eta(), t.phi())<0.01)continue;
	 if(index>=ThreeMuons_idx.size()) break;  // <----- fillinf isolation vertexing only for a signal candidate
         //if(abs(t.dz(pvPoint))< 0.5 && t.quality(TrackBase::tight) && sqrt(t.px()*t.px() + t.py()*t.py() ) > 0.5){//  && deltaR(t.eta(), t.phi(), LVTau.Eta(), LVTau.Phi()) < 1.){
         if(abs(t.dz(pvPoint))< 0.5 && sqrt(t.px()*t.px() + t.py()*t.py() ) > 0.4  && deltaR(t.eta(), t.phi(), LVTau.Eta(), LVTau.Phi()) < 1.){

    	    std::vector<int>   iIsolationTrack_VertexWithSignalMuonIsValid;
	    std::vector<float> iIsolationTrack_VertexWithSignalMuonChi2;
	    std::vector<float> iIsolationTrack_VertexWithSignalMuonPosition;
   	    IsolationTracksIndices.push_back(i);
            std::vector<float> iIsolation_Track_p4;
            iIsolation_Track_p4.push_back(sqrt(pow(t.p(),2.0) + pow(PDGInfo::pi_mass(),2.0)));
            iIsolation_Track_p4.push_back(t.px());
            iIsolation_Track_p4.push_back(t.py());
            iIsolation_Track_p4.push_back(t.pz());

            iIsolationTrack_charge.push_back(t.charge());
            iIsolationTrack_isHighPurity.push_back(t.quality(TrackBase::highPurity));

            IsolationTrack_p4.at(IsolationTrack_p4.size() - 1).push_back(iIsolation_Track_p4);

            iIsolationTrack_dxySV.push_back(t.dxy(TheSecondaryVertexPoint));
            iIsolationTrack_dzSV.push_back(t.dz(TheSecondaryVertexPoint));

            iIsolationTrack_dxyPV.push_back(t.dxy(pvPoint));
            iIsolationTrack_dzPV.push_back(t.dz(pvPoint));


            ClosestApproachInRPhi DocaMuon1, DocaMuon2, DocaMuon3;

            DocaMuon1.calculate(theB->build(t).initialFreeState(), iTransientTracks.at(sortedindices.at(0)).initialFreeState());
            DocaMuon2.calculate(theB->build(t).initialFreeState(), iTransientTracks.at(sortedindices.at(1)).initialFreeState());
            DocaMuon3.calculate(theB->build(t).initialFreeState(), iTransientTracks.at(sortedindices.at(2)).initialFreeState());


            if(DocaMuon1.status()){ iIsolationTrack_DocaMu1.push_back(DocaMuon1.distance());}
            else iIsolationTrack_DocaMu1.push_back(-1);

            if(DocaMuon2.status()){ iIsolationTrack_DocaMu2.push_back(DocaMuon2.distance());}
            else iIsolationTrack_DocaMu2.push_back(-1);

            if(DocaMuon3.status()){ iIsolationTrack_DocaMu3.push_back(DocaMuon3.distance());}
            else iIsolationTrack_DocaMu3.push_back(-1);
	    iIsolationTracksCollection.push_back(TheB->build(t));

	    vector<TransientTrack> IsolationTrack_Muon1;
	    vector<TransientTrack> IsolationTrack_Muon2;
	    vector<TransientTrack> IsolationTrack_Muon3;


	    IsolationTrack_Muon1.push_back(iTransientTracks.at(0));IsolationTrack_Muon1.push_back(TheB->build(t));
	    IsolationTrack_Muon2.push_back(iTransientTracks.at(1));IsolationTrack_Muon2.push_back(TheB->build(t));
	    IsolationTrack_Muon3.push_back(iTransientTracks.at(2));IsolationTrack_Muon3.push_back(TheB->build(t));

	    KalmanVertexFitter KVF_IsoTrack_Mu1(true),KVF_IsoTrack_Mu2(true),KVF_IsoTrack_Mu3(true); 
	    TransientVertex V_IsoTrack_Mu1,V_IsoTrack_Mu2,V_IsoTrack_Mu3;
	    bool FitTrackMu1Ok(true);
	    bool FitTrackMu2Ok(true);
	    bool FitTrackMu3Ok(true);

	    try {
	      V_IsoTrack_Mu1=KVF_IsoTrack_Mu1.vertex(IsolationTrack_Muon1);
	    } catch (...) {
	      FitTrackMu1Ok = false;
	    }

	    try {
	      V_IsoTrack_Mu2=KVF_IsoTrack_Mu2.vertex(IsolationTrack_Muon2);
	    } catch (...) {
	      FitTrackMu2Ok = false;
	    }

	    try {
	      V_IsoTrack_Mu3=KVF_IsoTrack_Mu3.vertex(IsolationTrack_Muon3);
	    } catch (...) {
	      FitTrackMu3Ok = false;
	    }

	    iIsolationTrack_VertexWithSignalMuonIsValid.push_back(V_IsoTrack_Mu1.isValid());
	    iIsolationTrack_VertexWithSignalMuonIsValid.push_back(V_IsoTrack_Mu2.isValid());
	    iIsolationTrack_VertexWithSignalMuonIsValid.push_back(V_IsoTrack_Mu3.isValid());
	    if(FitTrackMu1Ok&&FitTrackMu2Ok&&FitTrackMu3Ok);// dummy; to make it compile
	    if(V_IsoTrack_Mu1.isValid())
	      {
		iIsolationTrack_VertexWithSignalMuonChi2.push_back(V_IsoTrack_Mu1.totalChiSquared());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu1.position().x());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu1.position().y());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu1.position().z());
	      }
	    else 
	      {
		iIsolationTrack_VertexWithSignalMuonChi2.push_back(-1);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
	      }

	    if(V_IsoTrack_Mu2.isValid())
	      {	    
		iIsolationTrack_VertexWithSignalMuonChi2.push_back(V_IsoTrack_Mu2.totalChiSquared());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu2.position().x());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu2.position().y());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu2.position().z());
	      }
	    else 
	      {
		iIsolationTrack_VertexWithSignalMuonChi2.push_back(-1);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
	      }

	    if(V_IsoTrack_Mu3.isValid())
	      {	    
		iIsolationTrack_VertexWithSignalMuonChi2.push_back(V_IsoTrack_Mu3.totalChiSquared());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu3.position().x());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu3.position().y());
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(V_IsoTrack_Mu3.position().z());
	      }
	    else 
	      {
		iIsolationTrack_VertexWithSignalMuonChi2.push_back(-1);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
		iIsolationTrack_VertexWithSignalMuonPosition.push_back(-99.);
	      }


	    IsolationTrack_VertexWithSignalMuonIsValid.at(IsolationTrack_VertexWithSignalMuonIsValid.size() - 1).push_back(iIsolationTrack_VertexWithSignalMuonIsValid);
	    IsolationTrack_VertexWithSignalMuonChi2.at(IsolationTrack_VertexWithSignalMuonChi2.size() - 1).push_back(iIsolationTrack_VertexWithSignalMuonChi2);
	    IsolationTrack_VertexWithSignalMuonPosition.at(IsolationTrack_VertexWithSignalMuonPosition.size() - 1).push_back(iIsolationTrack_VertexWithSignalMuonPosition);

  

         }
      
            

         //--------------------------- Isolation Branch

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
            if(dca_fv<mindca_iso05)mindca_iso05=dca_fv; // iso 4
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
      //********************************************************************************
      //Here reconstruct the vertices of all signal candidates with all isolation tracks 


      IsolationTrack_dxySV.push_back(iIsolationTrack_dxySV);
      IsolationTrack_dzSV.push_back(iIsolationTrack_dzSV);

      IsolationTrack_dxyPV.push_back(iIsolationTrack_dxyPV);
      IsolationTrack_dzPV.push_back(iIsolationTrack_dzPV);
      IsolationTrack_DocaMu1.push_back(iIsolationTrack_DocaMu1);
      IsolationTrack_DocaMu2.push_back(iIsolationTrack_DocaMu2);
      IsolationTrack_DocaMu3.push_back(iIsolationTrack_DocaMu3);
      IsolationTrack_charge.push_back(iIsolationTrack_charge);
      IsolationTrack_isHighPurity.push_back(iIsolationTrack_isHighPurity);

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
      isolation2.push_back(mindist);
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
      for(size_t isv = 0; isv < svs->size(); isv++) {
         const Vertex & sv = (*svs)[isv];
         SV_Track_P4.push_back(std::vector<std::vector<float> >());
         std::vector<float> iSV_pos;
         iSV_pos.push_back(sv.x());
         iSV_pos.push_back(sv.y());
         iSV_pos.push_back(sv.z());
         SV_pos.push_back(iSV_pos);
         SV_Mass.push_back(sv.p4().M());

         TMatrixTSym<float> sv_cov(LorentzVectorParticle::NVertex);

         math::Error<3>::type sv_Cov;
         sv.fill(sv_Cov);

         for (int i = 0; i <LorentzVectorParticle::NVertex; i++){
            for (int j = 0; j < LorentzVectorParticle::NVertex; j++) {
               sv_cov(i, j) = sv_Cov(i, j);
               sv_cov(j, i) = sv_Cov(i, j);
            }
         }

         std::vector<float>  sv_covariance;
         for (int i = 0; i < LorentzVectorParticle::NVertex; i++) {
            for (int j = i; j < LorentzVectorParticle::NVertex; j++) {
               sv_covariance.push_back(sv_cov(i, j));
            }
         }
         SV_PosCovariance.push_back(sv_covariance);
         std::vector<int>    iSV_Trackcharge;
         for(Vertex::trackRef_iterator itk = sv.tracks_begin(); itk != sv.tracks_end(); itk++) {
            std::vector<float>  iSV_TrackP4;

            iSV_TrackP4.push_back(sqrt(pow((**itk).p(),2.0) + pow(PDGInfo::pi_mass(),2.0)));
            iSV_TrackP4.push_back((**itk).px());
            iSV_TrackP4.push_back((**itk).py());
            iSV_TrackP4.push_back((**itk).pz());
            SV_Track_P4.at(SV_Track_P4.size()-1).push_back(iSV_TrackP4);
            iSV_Trackcharge.push_back((**itk).charge());

         }
         SV_TrackCharge.push_back(iSV_Trackcharge);
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

         //Handle<VertexCompositePtrCollection> kshorts;
         //if (miniAODRun_) iEvent.getByToken(ksToken_, kshorts);


         unsigned int Muon_index = 0;
         unsigned int sel_muon_index = 0;
         ThreeMuons_index.resize(ThreeMuons_idx.size());
         TwoMuonsTrack_Muonsindex.resize(TwoMuonsTrack_idx.size());


         for (MuonCollection::const_iterator iMuon = muonCollection->begin(); iMuon != muonCollection->end(); ++iMuon, Muon_index++) {
            MuonRef RefMuon(muonCollection, Muon_index);
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

               const MuonIsolation Iso03 = RefMuon->isolationR03();
               const MuonIsolation Iso05 = RefMuon->isolationR05();

               const MuonPFIsolation PFIso03 = RefMuon->pfIsolationR03();
               const MuonPFIsolation PFIso04 = RefMuon->pfIsolationR04();

               //Muon_matchedToKs.push_back(MuonMatchedKs(RefMuon, const std::vector<reco::VertexCompositePtrCandidate>& kshort);

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
               Muon_numberOfMatches.push_back(RefMuon->numberOfMatches(Muon::SegmentArbitration));
               Muon_charge.push_back(RefMuon->charge());

               Muon_expectedNnumberOfMatchedStations.push_back(RefMuon->expectedNnumberOfMatchedStations());


               const Vertex & VertexMuonID = (*pvs)[dump_pv_index_to_fill.at(0)];
               int idbit(0);
               if(muon::isLooseMuon(*RefMuon)) idbit |= 1 << 0;
               if(muon::isSoftMuon(*RefMuon,VertexMuonID)) idbit |= 1 << 1;
               if(muon::isMediumMuon(*RefMuon)) idbit |= 1 << 2;
               if(muon::isTightMuon(*RefMuon,VertexMuonID)) idbit |= 1 << 3;
               if(muon::isHighPtMuon(*RefMuon,VertexMuonID)) idbit |= 1 << 4;
               Muon_ID.push_back(idbit);
               //	       std::cout<<"muonsoftmva  "<< RefMuon->softMvaValue() << std::endl;  // only in Mini AOD

               int ssbit(0);
               if(RefMuon->passed(Muon::CutBasedIdLoose))ssbit|=1<<0;
               if(RefMuon->passed(Muon::CutBasedIdMedium))ssbit|=1<<1;
               if(RefMuon->passed(Muon::CutBasedIdMediumPrompt))ssbit|=1<<2;
               if(RefMuon->passed(Muon::CutBasedIdTight))ssbit|=1<<3;
               if(RefMuon->passed(Muon::CutBasedIdGlobalHighPt))ssbit|=1<<4;
               if(RefMuon->passed(Muon::CutBasedIdTrkHighPt))ssbit|=1<<5;
               if(RefMuon->passed(Muon::PFIsoVeryLoose))ssbit|=1<<6;
               if(RefMuon->passed(Muon::PFIsoLoose))ssbit|=1<<7;
               if(RefMuon->passed(Muon::PFIsoMedium))ssbit|=1<<8;
               if(RefMuon->passed(Muon::PFIsoTight))ssbit|=1<<9;
               if(RefMuon->passed(Muon::PFIsoVeryTight))ssbit|=1<<10;
               if(RefMuon->passed(Muon::TkIsoLoose))ssbit|=1<<11;
               if(RefMuon->passed(Muon::TkIsoTight))ssbit|=1<<12;
               if(RefMuon->passed(Muon::SoftCutBasedId))ssbit|=1<<13;
               if(RefMuon->passed(Muon::SoftMvaId))ssbit|=1<<14;
               if(RefMuon->passed(Muon::MvaLoose))ssbit|=1<<15;
               if(RefMuon->passed(Muon::MvaMedium))ssbit|=1<<16; 
               if(RefMuon->passed(Muon::MvaTight))ssbit|=1<<17;
               if(RefMuon->passed(Muon::MiniIsoLoose))ssbit|=1<<18;
               if(RefMuon->passed(Muon::MiniIsoMedium))ssbit|=1<<19;
               if(RefMuon->passed(Muon::MiniIsoTight))ssbit|=1<<20;
               if(RefMuon->passed(Muon::MiniIsoVeryTight))ssbit|=1<<21;


               Muon_StandardSelection.push_back(ssbit);
               /////////////////////////////////////////////////////////////
               //here following guide given in:
               //https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2
               /////////////////////////////////////////////////////////////


               std::vector<float>  iMuon_TrackX;
               std::vector<float>  iMuon_TrackY;
               std::vector<float>  iMuon_dDxDz;
               std::vector<float>  iMuon_dDyDz;
               std::vector<float>  iMuon_dX;
               std::vector<float>  iMuon_dY;
               std::vector<float>  iMuon_pullX;
               std::vector<float>  iMuon_pullY;
               std::vector<float>  iMuon_pullDxDz;
               std::vector<float>  iMuon_pullDyDz;
               std::vector<float>  inumberOfSegments;


               for(int it = 1; it <= 8; ++it) { // loop over stations, 1,2,3,4 are DT, 5,6,7,8 are CSC
                  if(it <=4){ // we are in DT
                     iMuon_TrackX.push_back(RefMuon->trackX(it,1));
                     iMuon_TrackY.push_back(RefMuon->trackY(it,1));
                     iMuon_dDxDz.push_back(RefMuon->dDxDz(it,1));
                     iMuon_dDyDz.push_back(RefMuon->dDyDz(it,1));
                     iMuon_dX.push_back(RefMuon->dX(it,1));
                     iMuon_dY.push_back(RefMuon->dY(it,1));
                     iMuon_pullX.push_back(RefMuon->pullX(it,1));
                     iMuon_pullY.push_back(RefMuon->pullY(it,1));
                     iMuon_pullDxDz.push_back(RefMuon->pullDxDz(it,1));
                     iMuon_pullDyDz.push_back(RefMuon->pullDyDz(it,1));
                     inumberOfSegments.push_back(RefMuon->numberOfSegments(it,1));
                  }
                  if(it > 4){  // now in csc
                     iMuon_TrackX.push_back(RefMuon->trackX(it - 4,2));
                     iMuon_TrackY.push_back(RefMuon->trackY(it - 4,2));
                     iMuon_dDxDz.push_back(RefMuon->dDxDz(it - 4,2));
                     iMuon_dDyDz.push_back(RefMuon->dDyDz(it - 4,2));
                     iMuon_dX.push_back(RefMuon->dX(it - 4,2));
                     iMuon_dY.push_back(RefMuon->dY(it - 4,2));
                     iMuon_pullX.push_back(RefMuon->pullX(it - 4,2));
                     iMuon_pullY.push_back(RefMuon->pullY(it - 4,2));
                     iMuon_pullDxDz.push_back(RefMuon->pullDxDz(it - 4,2));
                     iMuon_pullDyDz.push_back(RefMuon->pullDyDz(it - 4,2));
                     inumberOfSegments.push_back(RefMuon->numberOfSegments(it - 4,2));
                  }
               }


               Muon_TrackX.push_back(iMuon_TrackX);
               Muon_TrackY.push_back(iMuon_TrackY);
               Muon_dDxDz.push_back(iMuon_dDxDz);
               Muon_dDyDz.push_back(iMuon_dDyDz);
               Muon_dX.push_back(iMuon_dX);
               Muon_dY.push_back(iMuon_dY);
               Muon_pullX.push_back(iMuon_pullX);
               Muon_pullY.push_back(iMuon_pullY);
               Muon_pullDxDz.push_back(iMuon_pullDxDz);
               Muon_pullDyDz.push_back(iMuon_pullDyDz);
               numberOfSegments.push_back(inumberOfSegments);

               std::vector<double> iMuon_outerTrack_p4;
               std::vector<double> iMuon_innerTrack_p4;
               if (RefMuon->isGlobalMuon()) {
                  Muon_hitPattern_numberOfValidMuonHits.push_back(RefMuon->globalTrack()->hitPattern().numberOfValidMuonHits());
                  Muon_normChi2.push_back(RefMuon->globalTrack()->normalizedChi2());

                  iMuon_outerTrack_p4.push_back(RefMuon->outerTrack()->eta());
                  iMuon_outerTrack_p4.push_back(RefMuon->outerTrack()->phi());
                  Muon_prod_inner_outer_charge.push_back(RefMuon->outerTrack()->charge()*RefMuon->innerTrack()->charge());

                  Muon_outerTrack_normalizedChi2.push_back(RefMuon->outerTrack()->normalizedChi2());
                  Muon_outerTrack_muonStationsWithValidHits.push_back(RefMuon->outerTrack()->hitPattern().muonStationsWithValidHits());


                  unsigned int dt1(0),dt2(0),dt3(0),dt4(0);
                  unsigned int rpc1(0),rpc2(0),rpc3(0),rpc4(0);
                  unsigned int csc1(0),csc2(0),csc3(0),csc4(0);
                  double comb(0);
                  const reco::HitPattern &pattern = RefMuon->globalTrack()->hitPattern();
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
                  Muon_prod_inner_outer_charge.push_back(0);
                  Muon_outerTrack_normalizedChi2.push_back(0);
                  Muon_outerTrack_muonStationsWithValidHits.push_back(0);
                  Muon_vmuonhitcomb_reco.push_back(0);
                  Muon_rpchits_reco.push_back(0);
               }

               if (RefMuon->isTrackerMuon() || RefMuon->isGlobalMuon()) {
                  Muon_numberofValidPixelHits.push_back(RefMuon->innerTrack()->hitPattern().numberOfValidPixelHits());
                  Muon_trackerLayersWithMeasurement.push_back(RefMuon->innerTrack()->hitPattern().trackerLayersWithMeasurement());
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
                  Muon_trackerLayersWithMeasurement.push_back(0);
                  Muon_numberofValidPixelHits.push_back(0);
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


               Muon_timeAtIpInOut.push_back(RefMuon->time().timeAtIpInOut);
               Muon_timeAtIpInOutErr.push_back(RefMuon->time().timeAtIpInOutErr);


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


               //--- Fill1 PFMuonIsolation -----
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
                        //	    Muon_cov.at(ntp).push_back(trackparticle.Covariance(i, j)); // comment out to keep sizee low, this is unused variable
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
         T3MNtuple::fillPhotons(const edm::Event& iEvent, const edm::EventSetup& iSetup)
         {

            edm::Handle<std::vector<reco::Photon> > photons;
            iEvent.getByToken(photonToken_, photons);

            for(unsigned int iPhoton = 0; iPhoton < photons->size(); iPhoton++){
               reco::PhotonRef photon(photons,iPhoton);
               std::vector<float>  iGammaP4;
               iGammaP4.push_back(photon->p4().E());
               iGammaP4.push_back(photon->p4().Px());
               iGammaP4.push_back(photon->p4().Py());
               iGammaP4.push_back(photon->p4().Pz());
               Gamma_P4.push_back(iGammaP4);

               Gamma_hasPixelSeed.push_back(photon->hasPixelSeed());
               Gamma_hasConversionTracks.push_back(photon->hasConversionTracks());
               Gamma_e1x5.push_back(photon->e1x5());
               Gamma_e2x5.push_back(photon->e2x5());
               Gamma_e3x3.push_back(photon->e3x3());
               Gamma_e5x5.push_back(photon->e5x5());

               Gamma_isPFPhoton.push_back(photon->isPFlowPhoton());

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
                     MCSignalParticle_childp4.push_back(std::vector<std::vector<float> >());
                     MCSignalParticle_Sourcepdgid.push_back(std::vector<int>());
                     MCSignalParticle_Sourcep4.push_back(std::vector<std::vector<float> >());
                     MCSignalParticle_pdgid.push_back(itr->pdgId());
                     MCSignalParticle_charge.push_back(itr->charge());
                     MCSignalParticle_Tauidx.push_back(std::vector<unsigned int>());

                     std::vector<float> iSig_p4;
                     iSig_p4.push_back(itr->p4().E());
                     iSig_p4.push_back(itr->p4().Px());
                     iSig_p4.push_back(itr->p4().Py());
                     iSig_p4.push_back(itr->p4().Pz());
                     MCSignalParticle_p4.push_back(iSig_p4);

                     std::vector<float> iSig_Vertex;
                     iSig_Vertex.push_back(itr->vx());
                     iSig_Vertex.push_back(itr->vy());
                     iSig_Vertex.push_back(itr->vz());
                     MCSignalParticle_Vertex.push_back(iSig_Vertex);

                     std::vector<float> iSourceVtx;

                     if(itr->numberOfMothers()!=0){
                        iSourceVtx.push_back(itr->mother(0)->vx());
                        iSourceVtx.push_back(itr->mother(0)->vy());
                        iSourceVtx.push_back(itr->mother(0)->vz());
                     }
                     MCSignalParticle_SourceVertex.push_back(iSourceVtx);

                     for (unsigned int i = 0; i < itr->numberOfMothers(); i++){
                        const reco::Candidate *mot = itr->mother(i);
                        std::vector<float> iSourcep4;
                        iSourcep4.push_back(mot->p4().E());
                        iSourcep4.push_back(mot->p4().Px());
                        iSourcep4.push_back(mot->p4().Py());
                        iSourcep4.push_back(mot->p4().Pz());



                        MCSignalParticle_Sourcepdgid.at(MCSignalParticle_Sourcepdgid.size() - 1).push_back(mot->pdgId());
                        MCSignalParticle_Sourcep4.at(MCSignalParticle_Sourcepdgid.size() - 1).push_back(iSourcep4);
                     }

                     // look for daughter tau
                     for (unsigned int i = 0; i < itr->numberOfDaughters(); i++){
                        const reco::Candidate *dau = itr->daughter(i);
                        std::vector<float> ichildp4;
                        ichildp4.push_back(dau->p4().E());
                        ichildp4.push_back(dau->p4().Px());
                        ichildp4.push_back(dau->p4().Py());
                        ichildp4.push_back(dau->p4().Pz());

                        MCSignalParticle_childpdgid.at(MCSignalParticle_childpdgid.size() - 1).push_back(dau->pdgId());
                        MCSignalParticle_childp4.at(MCSignalParticle_childpdgid.size() - 1).push_back(ichildp4);
                        if (abs(dau->pdgId()) == PDGInfo::tau_minus) {
                           unsigned int tauidx = MCTauandProd_p4.size();
                           MCSignalParticle_Tauidx.at(MCSignalParticle_Tauidx.size() - 1).push_back(tauidx);
                           // Analysis the tau decay
                           std::vector<const reco::GenParticle*> TauProducts = TauDecayProducts(static_cast<const reco::GenParticle*>(dau));
                           MCTauandProd_midx.push_back(k);
                           MCTauandProd_pdgid.push_back(std::vector<int>());
                           MCTauandProd_charge.push_back(std::vector<int>());
                           MCTauandProd_p4.push_back(std::vector<std::vector<float> >());
                           MCTauandProd_Vertex.push_back(std::vector<std::vector<float> >());
                           for (unsigned int i = 0; i < TauProducts.size(); i++) {
                              MCTauandProd_pdgid.at(tauidx).push_back(TauProducts.at(i)->pdgId());
                              MCTauandProd_charge.at(tauidx).push_back(TauProducts.at(i)->charge());
                              std::vector<float> iTauandProd_p4;
                              std::vector<float> iTauandProd_vertex;
                              iTauandProd_p4.push_back(TauProducts.at(i)->p4().E());
                              iTauandProd_p4.push_back(TauProducts.at(i)->p4().Px());
                              iTauandProd_p4.push_back(TauProducts.at(i)->p4().Py());
                              iTauandProd_p4.push_back(TauProducts.at(i)->p4().Pz());

                              iTauandProd_vertex.push_back(TauProducts.at(i)->vx());
                              iTauandProd_vertex.push_back(TauProducts.at(i)->vy());
                              iTauandProd_vertex.push_back(TauProducts.at(i)->vz());
                              //		std::cout<<"  iTauandProd_p4   "<<iTauandProd_p4.size() << std::endl;
                              MCTauandProd_p4.at(tauidx).push_back(iTauandProd_p4);
                              MCTauandProd_Vertex.at(tauidx).push_back(iTauandProd_vertex);
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
            Handle<vector<TriggerObjectStandAlone>> triggerObjects;
   
            Handle<TriggerResults> triggerBitsH;
            iEvent.getByToken(triggerToken_, triggerBitsH);
            const TriggerNames &triggerNames = iEvent.triggerNames( *triggerBitsH );

            if (miniAODRun_) iEvent.getByToken(triggerObjectToken_, triggerObjects);
            else iEvent.getByToken(trigeventToken_, triggerSummary);

            std::vector<std::vector<unsigned int> > PreselectedTwoMuonsTrackCollection  = findTwoMuonsAndTrackCandidates(iEvent, iSetup);
            //  std::cout<<" PreselectedTwoMuonsTrackCollection  "<< PreselectedTwoMuonsTrackCollection.size()<< std::endl;
            if(PreselectedTwoMuonsTrackCollection.size()==0){
               return 0;          //No two muons + track candidate found!
            }


            Handle<TrackCollection> trackCollection;
            iEvent.getByToken(trackToken_, trackCollection);

            edm::Handle<MuonCollection> muonCollection;
            iEvent.getByToken(muonToken_, muonCollection);

            for ( auto &iTwoMuTr :  PreselectedTwoMuonsTrackCollection ) {
               vector<TransientTrack> t_trks;
               TransientVertex transVtx;
               ESHandle<TransientTrackBuilder> theB;
               iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
               MuonRef Muon1(muonCollection, iTwoMuTr.at(0));
               MuonRef Muon2(muonCollection, iTwoMuTr.at(1));

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
                  if(transVtx.totalChiSquared() < 100){
                     if( (mv1 + mv2).M() < phimassmin_)// || (mv1 + mv2).M() > phimassmax_)   // renmove that
                     {
                        TwoMuonsTrack_idx.push_back(iTwoMuTr);
                        TwoMuonsTrack_SV_Chi2.push_back(transVtx.totalChiSquared());
                        TwoMuonsTrack_SV_NDF.push_back(transVtx.degreesOfFreedom());
                        std::vector<float> iTrigMatchdR;

                        for (unsigned int i=0; i < iTwoMuTr.size(); i++) {
                           float match;
                           if(i <2 ){	
                              MuonRef TrackTriggMatch(muonCollection, iTwoMuTr.at(i));	
                              if (miniAODRun_) TriggerMatch(triggerObjects, triggerNames, TrackTriggMatch, TriggerMuonMatchingdr_, match);
                              else TriggerMatch(triggerSummary,  TrackTriggMatch , TriggerMuonMatchingdr_, match);
                           } else {
                              TrackRef TrackTriggMatch = TrackRef(trackCollection, iTwoMuTr.at(i));
                              if (miniAODRun_) TriggerMatch(triggerObjects, triggerNames, TrackTriggMatch, TriggerMuonMatchingdr_, match);
                              else TriggerMatch(triggerSummary,  TrackTriggMatch , TriggerMuonMatchingdr_, match);
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
            if(btag1.first->pt() > 5){
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
            Handle<vector<TriggerObjectStandAlone>> triggerObjects;
   
            Handle<TriggerResults> triggerBitsH;
            iEvent.getByToken(triggerToken_, triggerBitsH);
            const TriggerNames &triggerNames = iEvent.triggerNames( *triggerBitsH );

            if (miniAODRun_) iEvent.getByToken(triggerObjectToken_, triggerObjects);
            else iEvent.getByToken(trigeventToken_, triggerSummary);

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
               MuonRef Muon1(muonCollection, iThreeMuon.at(0));
               MuonRef Muon2(muonCollection, iThreeMuon.at(1));
               MuonRef Muon3(muonCollection, iThreeMuon.at(2));

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

               if(transVtx.isValid()){
                  if(transVtx.totalChiSquared() < 100.)   //remove for sync
                  { // very loose/ ndf =3

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
                                 //	      signalTau_cov.at(ntp).push_back(signalTau.Covariance(i, j));// comment out to keep size low
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
                        MuonRef MuonTriggMatch(muonCollection, iMuon);
                        TLorentzVector mut;

                        if (miniAODRun_) TriggerMatch(triggerObjects,  triggerNames, MuonTriggMatch, TriggerMuonMatchingdr_, match);
                        else TriggerMatch(triggerSummary,  MuonTriggMatch, TriggerMuonMatchingdr_, match);
                        iTrigMatchdR.push_back(match);
                        //	std::cout<<" match  "<< match << std::endl;
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
            edm::InputTag MuonFilterTag;

            if(WhatData_=="2017")  MuonFilterTag = edm::InputTag("hltTau3muTkVertexFilter", "", "HLT"); 
            if(WhatData_=="2018")  MuonFilterTag = edm::InputTag("hltdstau3muDisplaced3muFltr", "", "HLT"); 
            //  if(WhatData_=="Parked")  edm::InputTag MuonFilterTag = edm::InputTag("hltdstau3muDisplaced3muFltr", "", "HLT"); 


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

      template<class T>
         void T3MNtuple::TriggerMatch(edm::Handle<vector<TriggerObjectStandAlone>>& triggerObjects, const TriggerNames& triggerNames, T obj, double drmax, float &match){

            std::string MuonFilterTag;

            if(WhatData_=="2017")  MuonFilterTag = "hltTau3muTkVertexFilter"; 
            if(WhatData_=="2018")  MuonFilterTag = "hltdstau3muDisplaced3muFltr"; 

            match = 999.;
            drmax = 1.;

            for (TriggerObjectStandAlone to: *triggerObjects){
               
               to.unpackPathNames(triggerNames);
               trigger::size_type nFilters = to.filterLabels().size();
               for (trigger::size_type iFilter=0; iFilter<nFilters; iFilter++){
                  std::string filterName = to.filterLabels()[iFilter];

                  if (filterName.compare(MuonFilterTag)!=0) continue;

                  double dr = reco::deltaR(to.eta(), to.phi(), obj->eta(), obj->phi());

                  if (dr < drmax){
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

            Handle<MuonCollection> muonCollection;
            iEvent.getByToken(muonToken_, muonCollection);
            int Muon_index = 0;

            std::vector<unsigned int> preselected_muon_idx;
            std::vector<std::vector<unsigned int> > TwoMuonsPlusTrackCollection; // note that the track index goes last

            for (MuonCollection::const_iterator iMuon = muonCollection->begin(); iMuon != muonCollection->end(); ++iMuon, Muon_index++) {
               MuonRef RefMuon(muonCollection, Muon_index);
               if(AcceptedMuon(RefMuon)) preselected_muon_idx.push_back(Muon_index);
            }
            if(preselected_muon_idx.size() > 1){
               for(size_t i = 0; i < preselected_muon_idx.size()-1; ++ i){
                  std::vector<unsigned int> dump_index;
                  MuonRef  Muon1(muonCollection, preselected_muon_idx.at(i));
                  for(size_t j = i+1; j < preselected_muon_idx.size(); ++ j){
                     MuonRef  Muon2(muonCollection, preselected_muon_idx.at(j));

                     double dz_12 = abs(Muon2->vz()-Muon1->vz());  // like NFN for sync
                     // double dz_12 = abs(Muon2->innerTrack()->dz(beamSpotHandle->position())-Muon1->innerTrack()->dz(beamSpotHandle->position()));  //   Check that two muons are
                     double dr_12 = deltaR(Muon1->eta(), Muon1->phi(), Muon2->eta(), Muon2->phi());                                                //   not far from each othe
                     // if(abs(Muon1->charge() + Muon2->charge()) !=0 ) continue;

                     if(dz_12<0.5 &&  dr_12<0.8)
                     { // - to be checked
                        unsigned int Track_index = 0;
                        iEvent.getByToken(trackToken_, trackCollection);
                        for (reco::TrackCollection::const_iterator iTrack = trackCollection->begin(); iTrack != trackCollection->end(); ++iTrack, Track_index++)
                        {
                           const reco::Track track = (*iTrack);
                           if(isGoodTrack(track))
                           {
                              double dz23 = fabs(track.vz()  - Muon2->vz());  // like INFN
                              double dz31 = fabs(track.vz()  - Muon1->vz());  // like INFN

                              double dr23 = deltaR(track.eta(), track.phi(), Muon2->eta(), Muon2->phi());
                              double dr31 = deltaR(track.eta(), track.phi(), Muon1->eta(), Muon1->phi());

                              if(dr23 > 0.8  || dr31 > 0.8 )  continue;
                              if(dr23 < 0.01 || dr31 < 0.01)  continue;
                              if(dz23 > 0.5  || dz31 > 0.5 )  continue;

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
            for (MuonCollection::const_iterator iMuon = muonCollection->begin(); iMuon != muonCollection->end(); ++iMuon, Muon_index++) {
               MuonRef RefMuon(muonCollection, Muon_index);
               if(AcceptedMuon(RefMuon))preselected_muon_idx.push_back(Muon_index);
            }

            if(preselected_muon_idx.size() > 2){
               for(size_t i = 0; i < preselected_muon_idx.size()-1; ++ i){

                  std::vector<unsigned int> dump_index;
                  MuonRef  Muon1(muonCollection, preselected_muon_idx.at(i));
                  for(size_t j = i+1; j < preselected_muon_idx.size(); ++ j){
                     MuonRef  Muon2(muonCollection, preselected_muon_idx.at(j));

                     double dz_12 = abs(Muon2->vz()-Muon1->vz());  //  INFN
                     double dr_12 = deltaR(Muon1->eta(), Muon1->phi(), Muon2->eta(), Muon2->phi());                                                //   not far from each other
                     if(dz_12>0.5 ||  dr_12>0.8)continue; // - to be checked  -  this is previsou req.

                     if(j<preselected_muon_idx.size()-1){
                        for(size_t k = j+1; k < preselected_muon_idx.size(); ++ k){
                           MuonRef  Muon3(muonCollection, preselected_muon_idx.at(k));
                           size_t number_of_muons_pt2p5 = 0;
                           if(Muon1->pt()>2.5)number_of_muons_pt2p5++;
                           if(Muon2->pt()>2.5)number_of_muons_pt2p5++;
                           if(Muon3->pt()>2.5)number_of_muons_pt2p5++;

                           if(Muon1->pt() < 1 or Muon2->pt() <1 or Muon3->pt()<1)std::cout<<"Wrong pt!!!"<< std::endl;


                           //	    if(number_of_muons_pt2p5<2)continue; 
                           //	    double dz_23 = abs(Muon3->innerTrack()->dz(beamSpotHandle->position())-Muon2->innerTrack()->dz(beamSpotHandle->position()));
                           //	    double dz_31 = abs(Muon3->innerTrack()->dz(beamSpotHandle->position())-Muon1->innerTrack()->dz(beamSpotHandle->position()));

                           double dz_23 = abs(Muon3->vz() - Muon2->vz());
                           double dz_31 = abs(Muon3->vz() - Muon1->vz());

                           double dr_23 = deltaR(Muon3->eta(), Muon3->phi(), Muon2->eta(), Muon2->phi());
                           double dr_31 = deltaR(Muon3->eta(), Muon3->phi(), Muon1->eta(), Muon1->phi());

                           if(dr_23>0.8 || dr_31>0.8)continue; 
                           if(dz_23>0.5 || dz_31>0.5)continue; 
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
            for (size_t i_l1t = 0; i_l1t < initialDecisions.size(); i_l1t++){

               string l1tName = (initialDecisions.at(i_l1t)).first;
               if(l1tName.find("DoubleMu") != string::npos || l1tName.find("TripleMu") != string::npos)
               {
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
               if(l1tName.find("DoubleMu") != string::npos || l1tName.find("TripleMu") != string::npos)
               {
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
            if(hltName.find("HLT_DoubleMu") != string::npos  or hltName.find("HLT_Mu") != string::npos )
            {
               Trigger_hltname.push_back(hltName);
               Trigger_hltdecision.push_back(triggerBitsH->accept(i_hlt ));
            }
         }

         // Fill Trigger object info
         if (miniAODRun_){

            Handle<std::vector<TriggerObjectStandAlone> > triggerObjects;
            iEvent.getByToken(triggerObjectToken_, triggerObjects);

            for (TriggerObjectStandAlone to: *triggerObjects){
               
               to.unpackPathNames(triggerNames);
               trigger::size_type nFilters = to.filterLabels().size();
               for (trigger::size_type iFilter=0; iFilter<nFilters; iFilter++){
                  std::string filterName = to.filterLabels()[iFilter];

                  if (filterName.compare("hltTau3muTkVertexFilter")!=0 || 
                        filterName.compare("hltdstau3muDisplaced3muFltr")!=0 ) continue;
                  TriggerObject_name.push_back(filterName);
                  TriggerObject_pt.push_back(to.pt());
                  TriggerObject_eta.push_back(to.eta());
                  TriggerObject_phi.push_back(to.phi());
               }
            }

         } else {

            edm::Handle<trigger::TriggerEvent> triggerSummary;
            iEvent.getByToken(trigeventToken_, triggerSummary);

            std::vector<trigger::TriggerObject> trgobjs = triggerSummary->getObjects();

            edm::InputTag MuonFilterTag2017("hltTau3muTkVertexFilter","","HLT");
            edm::InputTag MuonFilterTag2018("hltdstau3muDisplaced3muFltr","","HLT");

            size_t MuonFilterIndex = (*triggerSummary).filterIndex(MuonFilterTag2017);
            if(MuonFilterIndex < (*triggerSummary).sizeFilters()) {
               const trigger::Keys &KEYS = (*triggerSummary).filterKeys(MuonFilterIndex);
               for (unsigned int ipart = 0; ipart < KEYS.size(); ipart++) {
                  TriggerObject_name.push_back("hltTau3muTkVertexFilter");
                  TriggerObject_pt.push_back(trgobjs.at(KEYS.at(ipart)).pt());
                  TriggerObject_eta.push_back(trgobjs.at(KEYS.at(ipart)).eta());
                  TriggerObject_phi.push_back(trgobjs.at(KEYS.at(ipart)).phi());
               }  
            }

            MuonFilterIndex = (*triggerSummary).filterIndex(MuonFilterTag2018);
            if(MuonFilterIndex < (*triggerSummary).sizeFilters()) {
               const trigger::Keys &KEYS = (*triggerSummary).filterKeys(MuonFilterIndex);
               for (unsigned int ipart = 0; ipart < KEYS.size(); ipart++) {
                  TriggerObject_name.push_back("hltdstau3muDisplaced3muFltr");
                  TriggerObject_pt.push_back(trgobjs.at(KEYS.at(ipart)).pt());
                  TriggerObject_eta.push_back(trgobjs.at(KEYS.at(ipart)).eta());
                  TriggerObject_phi.push_back(trgobjs.at(KEYS.at(ipart)).phi());
               }  
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
         if (Event_isRealData) {
            Event_DataMC_Type = DataMCType::Data;
         }
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
               output_tree->Branch("Muon_expectedNnumberOfMatchedStations", &Muon_expectedNnumberOfMatchedStations);
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
               output_tree->Branch("Muon_timeAtIpInOut", &Muon_timeAtIpInOut);
               output_tree->Branch("Muon_timeAtIpInOutErr", &Muon_timeAtIpInOutErr);


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

               output_tree->Branch("Muon_TrackX",&Muon_TrackX);
               output_tree->Branch("Muon_TrackY",&Muon_TrackY);
               output_tree->Branch("Muon_dDxDz",&Muon_dDxDz);
               output_tree->Branch("Muon_dDyDz",&Muon_dDyDz);
               output_tree->Branch("Muon_dX",&Muon_dX);
               output_tree->Branch("Muon_dY",&Muon_dY);
               output_tree->Branch("Muon_pullX",&Muon_pullX);
               output_tree->Branch("Muon_pullY",&Muon_pullY);
               output_tree->Branch("Muon_pullDxDz",&Muon_pullDxDz);
               output_tree->Branch("Muon_pullDyDz",&Muon_pullDyDz);
               output_tree->Branch("numberOfSegments",&numberOfSegments);



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

               if(doPhotons_){
                  output_tree->Branch("Gamma_P4",&Gamma_P4);
                  output_tree->Branch("Gamma_hasPixelSeed",&Gamma_hasPixelSeed);
                  output_tree->Branch("Gamma_hasConversionTracks",&Gamma_hasConversionTracks);
                  output_tree->Branch("Gamma_e1x5",&Gamma_e1x5);
                  output_tree->Branch("Gamma_e2x5",&Gamma_e2x5);
                  output_tree->Branch("Gamma_e3x3",&Gamma_e3x3);
                  output_tree->Branch("Gamma_e5x5",&Gamma_e5x5);
                  output_tree->Branch("Gamma_isPFPhoton",&Gamma_isPFPhoton);
               }
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
                  output_tree->Branch("MCSignalParticle_Vertex", &MCSignalParticle_Vertex);
                  output_tree->Branch("MCSignalParticle_pdgid", &MCSignalParticle_pdgid);
                  output_tree->Branch("MCSignalParticle_childpdgid", &MCSignalParticle_childpdgid);
                  output_tree->Branch("MCSignalParticle_childp4", &MCSignalParticle_childp4);
                  output_tree->Branch("MCSignalParticle_Sourcepdgid", &MCSignalParticle_Sourcepdgid);
                  output_tree->Branch("MCSignalParticle_Sourcep4", &MCSignalParticle_Sourcep4);
                  output_tree->Branch("MCSignalParticle_charge", &MCSignalParticle_charge);
                  output_tree->Branch("MCSignalParticle_Tauidx", &MCSignalParticle_Tauidx);
                  output_tree->Branch("MCSignalParticle_SourceVertex", &MCSignalParticle_SourceVertex);
                  output_tree->Branch("MCTauandProd_p4", &MCTauandProd_p4);
                  output_tree->Branch("MCTauandProd_Vertex", &MCTauandProd_Vertex);
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
               output_tree->Branch("Vertex_signal_KF_BS_2Ddistance", &Vertex_signal_KF_BS_2Ddistance);
               output_tree->Branch("Vertex_signal_KF_BS_error", &Vertex_signal_KF_BS_error);
               output_tree->Branch("Vertex_signal_KF_BS_significance", &Vertex_signal_KF_BS_significance);
               output_tree->Branch("Vertex_pair_quality", &Vertex_pair_quality);

               output_tree->Branch("Vertex_Pair12_Pos", &Vertex_Pair12_Pos);
               output_tree->Branch("Vertex_Pair23_Pos", &Vertex_Pair23_Pos);
               output_tree->Branch("Vertex_Pair31_Pos", &Vertex_Pair31_Pos);

               output_tree->Branch("Vertex_pairfit_status", &Vertex_pairfit_status);
               output_tree->Branch("Vertex_MatchedPrimaryVertex",&Vertex_MatchedPrimaryVertex);
               output_tree->Branch("Vertex_SecondBestPrimaryVertex",&Vertex_SecondBestPrimaryVertex);
               output_tree->Branch("Vertex_RefitPVisValid",&Vertex_RefitPVisValid);
               output_tree->Branch("Vertex_MatchedRefitPrimaryVertex",&Vertex_MatchedRefitPrimaryVertex);
               output_tree->Branch("Vertex_MatchedRefitPrimaryVertex_covariance",&Vertex_MatchedRefitPrimaryVertex_covariance);
               output_tree->Branch("Vertex_d0_reco",&Vertex_d0_reco);
               output_tree->Branch("Vertex_dz_reco",&Vertex_dz_reco);
               output_tree->Branch("Vertex_d0SV_reco",&Vertex_d0SV_reco);
               output_tree->Branch("Vertex_dzSV_reco",&Vertex_dzSV_reco);
               output_tree->Branch("Vertex_d0BeamSpot_reco",&Vertex_d0BeamSpot_reco);
               output_tree->Branch("Vertex_d0BeamSpot_reco_sig",&Vertex_d0BeamSpot_reco_sig);
               output_tree->Branch("Vertex_d0sig_reco",&Vertex_d0sig_reco);
               output_tree->Branch("Vertex_d0sigSV_reco",&Vertex_d0sigSV_reco);
               output_tree->Branch("Vertex_2Ddisplacement",&Vertex_2Ddisplacement);
               output_tree->Branch("Vertex_3Ddisplacement",&Vertex_3Ddisplacement);
               output_tree->Branch("Vertex_Isolation1",&Vertex_Isolation1);
               output_tree->Branch("Vertex_Isolation2",&Vertex_Isolation2);
               output_tree->Branch("Vertex_Isolation3",&Vertex_Isolation3);
               output_tree->Branch("Vertex_Isolation4",&Vertex_Isolation4);

               output_tree->Branch("Vertex_NMuonsAssocWithPV",&Vertex_NMuonsAssocWithPV);
               output_tree->Branch("TriggerObject_pt",&TriggerObject_pt);
               output_tree->Branch("TriggerObject_phi",&TriggerObject_phi);
               output_tree->Branch("TriggerObject_eta",&TriggerObject_eta);
               output_tree->Branch("TriggerObject_name",&TriggerObject_name);

               //  output_tree->Branch("IsolationBranch_Trackp4", &IsolationBranch_Trackp4);



               output_tree->Branch("IsolationTrack_p4", &IsolationTrack_p4);

	       output_tree->Branch("IsolationTrack_VertexWithSignalMuonIsValid", &IsolationTrack_VertexWithSignalMuonIsValid);
	       output_tree->Branch("IsolationTrack_VertexWithSignalMuonChi2", &IsolationTrack_VertexWithSignalMuonChi2);
	       output_tree->Branch("IsolationTrack_VertexWithSignalMuonPosition", &IsolationTrack_VertexWithSignalMuonPosition);


               output_tree->Branch("IsolationTrack_charge",&IsolationTrack_charge);
	       output_tree->Branch("IsolationTrack_isHighPurity",&IsolationTrack_isHighPurity);
               output_tree->Branch("IsolationTrack_quality",&IsolationTrack_quality);

               output_tree->Branch("IsolationTrack_dxySV",&IsolationTrack_dxySV);
               output_tree->Branch("IsolationTrack_dzSV",&IsolationTrack_dzSV);

               output_tree->Branch("IsolationTrack_dxyPV",&IsolationTrack_dxyPV);
               output_tree->Branch("IsolationTrack_dzPV",&IsolationTrack_dzPV);

               output_tree->Branch("IsolationTrack_DocaMu1",&IsolationTrack_DocaMu1);
               output_tree->Branch("IsolationTrack_DocaMu2",&IsolationTrack_DocaMu2);
               output_tree->Branch("IsolationTrack_DocaMu3",&IsolationTrack_DocaMu3);

               output_tree->Branch("SV_Track_P4",&SV_Track_P4);
               output_tree->Branch("SV_pos",&SV_pos);
               output_tree->Branch("SV_TrackCharge",&SV_TrackCharge);
               output_tree->Branch("SV_Mass",&SV_Mass);
               output_tree->Branch("SV_PosCovariance",&SV_PosCovariance);



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

            //=======  Gammas ===
            Gamma_P4.clear();
            Gamma_hasPixelSeed.clear();
            Gamma_hasConversionTracks.clear();

            Gamma_e1x5.clear();
            Gamma_e2x5.clear();
            Gamma_e3x3.clear();
            Gamma_e5x5.clear();

            Gamma_isPFPhoton.clear();


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

            Muon_expectedNnumberOfMatchedStations.clear();


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
            Muon_timeAtIpInOut.clear();
            Muon_timeAtIpInOutErr.clear();


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

            Muon_TrackX.clear();
            Muon_TrackY.clear();
            Muon_dDxDz.clear();
            Muon_dDyDz.clear();
            Muon_dX.clear();
            Muon_dY.clear();
            Muon_pullX.clear();
            Muon_pullY.clear();
            Muon_pullDxDz.clear();
            Muon_pullDyDz.clear();
            numberOfSegments.clear();


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

            TriggerObject_pt.clear();
            TriggerObject_eta.clear();
            TriggerObject_phi.clear();
            TriggerObject_name.clear();

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
               MCSignalParticle_Vertex.clear();
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
               MCSignalParticle_Sourcepdgid.clear();
               MCSignalParticle_Sourcep4.clear();
               MCSignalParticle_SourceVertex.clear();

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

            Vertex_NMuonsAssocWithPV.clear();
            Vertex_signal_dca_reco.clear();
            Vertex_signal_KF_pos.clear();
            Vertex_signal_KF_cov.clear();
            Vertex_signal_KF_refittedTracksP4.clear();
            Vertex_signal_KF_Chi2.clear();
            Vertex_signal_AF_pos.clear();
            Vertex_signal_AF_Chi2.clear();
            Vertex_signal_AF_Ndf.clear();
            Vertex_signal_KF_BS_2Ddistance.clear();
            Vertex_signal_KF_BS_error.clear();
            Vertex_signal_KF_BS_significance.clear();

            Vertex_pair_quality.clear();
            Vertex_pairfit_status.clear();
            Vertex_Pair12_Pos.clear();
            Vertex_Pair23_Pos.clear();
            Vertex_Pair31_Pos.clear();

            Vertex_MatchedPrimaryVertex.clear();
            Vertex_SecondBestPrimaryVertex.clear();

            Vertex_RefitPVisValid.clear();
            Vertex_MatchedRefitPrimaryVertex.clear();
            Vertex_MatchedRefitPrimaryVertex_covariance.clear();
            Vertex_d0_reco.clear();
            Vertex_dz_reco.clear();
            Vertex_d0SV_reco.clear();
            Vertex_dzSV_reco.clear();
            Vertex_d0sig_reco.clear();
            Vertex_d0sigSV_reco.clear();
            Vertex_d0BeamSpot_reco.clear();
            Vertex_d0BeamSpot_reco_sig.clear();
            Vertex_2Ddisplacement.clear();
            Vertex_3Ddisplacement.clear();
            Vertex_Isolation1.clear();
            Vertex_Isolation2.clear();
            Vertex_Isolation3.clear();
            Vertex_Isolation4.clear();

            SV_Track_P4.clear();
            SV_pos.clear();
            SV_Mass.clear();
            SV_PosCovariance.clear();
            SV_TrackCharge.clear();

            IsolationBranch_Trackp4.clear();

            IsolationTrack_p4.clear();

	    IsolationTrack_VertexWithSignalMuonIsValid.clear();
	    IsolationTrack_VertexWithSignalMuonChi2.clear();
	    IsolationTrack_VertexWithSignalMuonPosition.clear();



            IsolationTrack_charge.clear();
	    IsolationTrack_isHighPurity.clear();
            IsolationTrack_quality.clear();

            IsolationTrack_dxySV.clear();
            IsolationTrack_dzSV.clear();

            IsolationTrack_dxyPV.clear();
            IsolationTrack_dzPV.clear();

            IsolationTrack_DocaMu1.clear();
            IsolationTrack_DocaMu2.clear();
            IsolationTrack_DocaMu3.clear();

            Trigger_l1name.clear();
            Trigger_l1decision.clear();
            Trigger_l1prescale.clear();
            Trigger_hltname.clear();
            Trigger_hltdecision.clear();
         }

//define this as a plug-in
DEFINE_FWK_MODULE(T3MNtuple);

