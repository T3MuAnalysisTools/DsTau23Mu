#include "DsTau23Mu/T3MNtuple/interface/TauDecay_CMSSW.h"
#include "DsTau23Mu/T3MNtuple/interface/PDGInfo.h"
#include "DsTau23Mu/T3MNtuple/interface/DataMCType.h"
 

#include <iomanip>
#include <cstdlib> 
using namespace edm;
using namespace std;
using namespace reco;
TauDecay_CMSSW::TauDecay_CMSSW():
  TauDecay()
{

}

TauDecay_CMSSW::~TauDecay_CMSSW(){

} 
                            
bool TauDecay_CMSSW::AnalyzeTau(const reco::GenParticle *Tau,unsigned int &JAK_ID,unsigned int &TauBitMask){
  Reset();
  MotherIdx.clear();
  TauDecayProducts.clear();
  if(abs(Tau->pdgId())==PDGInfo::tau_minus){ // check that it is a tau
    unsigned int Tauidx=TauDecayProducts.size();
    TauDecayProducts.push_back(Tau);
    MotherIdx.push_back(Tauidx);
    for (unsigned int i=0; i< Tau->numberOfDaughters(); i++){
      const reco::Candidate *dau=Tau->daughter(i);
      Analyze(static_cast<const reco::GenParticle*>(dau),Tauidx);
    }
    ClassifyDecayMode(JAK_ID,TauBitMask);
    return true;
  }
  return false;
}



  
void TauDecay_CMSSW::Analyze(const reco::GenParticle *Particle,unsigned int midx){
  unsigned int pdgid=abs(Particle->pdgId());
  if(isTauFinalStateParticle(pdgid)){
    if(!isTauParticleCounter(pdgid)) std::cout << "TauDecay_CMSSW::Analyze WARNING: Unknow Final State Particle in Tau Decay... " << std::endl;
    TauDecayProducts.push_back(Particle);
    MotherIdx.push_back(midx);
    if(pdgid==PDGInfo::pi0){// store information on pi0 decay products even though a pi0 is a finsal state particle (for 3PiPi0 studies)
      midx=MotherIdx.size()-1;
      for (unsigned int i=0; i< Particle->numberOfDaughters(); i++){
	const reco::Candidate *dau=Particle->daughter(i);
	AddPi0Info(static_cast<const reco::GenParticle*>(dau),midx);
      }
    }
    return;
  }
  if(Particle->status()==1 || isTauResonanceCounter(pdgid)){
    TauDecayProducts.push_back(Particle);
    MotherIdx.push_back(midx);
    midx=MotherIdx.size()-1;
  }
  for (unsigned int i=0; i< Particle->numberOfDaughters(); i++){
    const reco::Candidate *dau=Particle->daughter(i);
    Analyze(static_cast<const reco::GenParticle*>(dau),midx);
  }
}


void TauDecay_CMSSW::AddPi0Info(const reco::GenParticle *Particle,unsigned int midx){
  if(Particle->status()==1){
    TauDecayProducts.push_back(Particle);
    MotherIdx.push_back(midx);
    return;
  }
  for (unsigned int i=0; i< Particle->numberOfDaughters(); i++){
    const reco::Candidate *dau=Particle->daughter(i);
    AddPi0Info(static_cast<const reco::GenParticle*>(dau),midx);
  }
}


void TauDecay_CMSSW::CheckForSignal(unsigned int &type,const Handle<GenParticleCollection>& genHandle){
  //void TauDecay_CMSSW::CheckForSignal(unsigned int &type,edm::Handle <edm::View<reco::GenParticle> > genHandle){
  DataMCType DMT;
  //  for(reco::GenParticleCollection::const_iterator itr = genParticles->begin(); itr!= genParticles->end(); ++itr){
  for (unsigned int iGen = 0; iGen < genHandle->size(); iGen++){
    unsigned int JAK_ID1(0), nprong1(0), JAK_ID2(0), nprong2(0);
    const GenParticle& genP = (*genHandle)[iGen];
    if(DMT.DecodeDecayForSignalParticle((*genHandle)[iGen].pdgId()) && genP.numberOfDaughters() > 1){

      for(unsigned int i = 0; i <(*genHandle)[iGen].numberOfDaughters(); i++){
	const reco::Candidate *dau=(*genHandle)[iGen].daughter(i);
	if(abs(dau->pdgId())==PDGInfo::tau_minus){

	  if(type==DataMCType::DY_ll) type=DataMCType::DY_tautau;
	  //	  if(type==DataMCType::W_lnu) type=DataMCType::W_taunu;
	  unsigned int JAK_ID,TauBitMask;
	  AnalyzeTau(static_cast<const reco::GenParticle*>(dau),JAK_ID,TauBitMask);
	  if(JAK_ID1==0){
	    JAK_ID1=JAK_ID;
	    nprong1=nProng(TauBitMask);
	  }
	  else{
	    JAK_ID2=JAK_ID;
	    nprong2=nProng(TauBitMask);
	  }
	}
      }
      type=DMT.SignalCode(type,JAK_ID1,nprong1,JAK_ID2,nprong2);
      break;
    }
  }
}
