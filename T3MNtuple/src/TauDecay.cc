#include "DsTau23Mu/T3MNtuple/interface/TauDecay.h"
#include "DsTau23Mu/T3MNtuple/interface/PDGInfo.h"


#include <iomanip>
#include <cstdlib> 
#include <iostream>
 
TauDecay::TauDecay(){
  Reset();
}

TauDecay::~TauDecay(){

}

void TauDecay::Reset(){
  n_pi=0;
  n_pi0=0;
  n_K=0;
  n_K0L=0;
  n_K0S=0;
  n_gamma=0;
  n_nu=0;
  n_e=0;
  n_mu=0;
  n_a1=0;
  n_a10=0;
  n_rho=0;
  n_rho0=0;
  n_eta=0;
  n_omega=0;
  n_Kstar0=0;
  n_Kstar=0;
  unknown=0;
}

bool TauDecay::isTauFinalStateParticle(int pdgid){
  int id=abs(pdgid);
  if(id==PDGInfo::e_minus)   return true;  // e+-
  if(id==PDGInfo::nu_e)      return true;  // nu_e
  if(id==PDGInfo::mu_minus)  return true;  // mu+-
  if(id==PDGInfo::nu_mu)     return true;  // nu_mu
  if(id==PDGInfo::nu_tau)    return true;  // nu_tau
  if(id==PDGInfo::gamma)     return true;  // gamma happends in generator
  if(id==PDGInfo::pi0)       return true;  // pi0
  if(id==PDGInfo::pi_plus)   return true;  // pi+-
  if(id==PDGInfo::K_L0)      return true;  // K0L
  if(id==PDGInfo::K_S0)      return true;  // KS
  if(id==PDGInfo::K_plus)    return true;  // K+-
  return false;
}

bool TauDecay::isTauParticleCounter(int pdgid){
  int id=abs(pdgid);
  //count particles
  if(id==PDGInfo::pi_plus) { n_pi++;       return true;}
  if(id==PDGInfo::pi0)     { n_pi0++;      return true;}
  if(id==PDGInfo::K_plus)  { n_K++;        return true;}
  if(id==PDGInfo::K_L0)    { n_K0L++;      return true;}
  if(id==PDGInfo::K_S0)    { n_K0S++;      return true;}
  if(id==PDGInfo::gamma)   { n_gamma++;    return true;}
  if(id==PDGInfo::nu_tau ||
     id==PDGInfo::nu_e   ||
     id==PDGInfo::nu_mu)    { n_nu++;      return true;}
  if(id==PDGInfo::e_minus)  { n_e++;       return true;}
  if(id==PDGInfo::mu_minus) { n_mu++;      return true;}
  return false;
}

bool TauDecay::isTauResonanceCounter(int pdgid){
  int id=abs(pdgid);
  //count resonances
  if(id==PDGInfo::a_1_plus)   { n_a1++;      return true;}
  if(id==PDGInfo::a_10)       { n_a10++;     return true;}
  if(id==PDGInfo::rho_plus)   { n_rho++;     return true;}
  if(id==PDGInfo::rho0)       { n_rho0++;    return true;}
  if(id==PDGInfo::eta)        { n_eta++;     return true;}
  if(id==PDGInfo::omega)      { n_omega++;   return true;}
  //if(id==PDGInfo::K_S0)       { n_K0S++;     return true;}
  if(id==PDGInfo::K_star0)    { n_Kstar0++;  return true;}
  if(id==PDGInfo::K_star_plus){ n_Kstar++;   return true;}
  if(id==PDGInfo::W_plus)     { return true;}
  unknown++;
  return false;
}

void TauDecay::ClassifyDecayMode(unsigned int &JAK_ID,unsigned int &TauBitMask){
  //Reset Bits
  JAK_ID=0;
  TauBitMask=0;
  // Classify according to JAK and TauDecayStructure
  ///////////////////////////////////////////////////////
  //
  // Exlusive modes remove first
  //
  if(n_pi>=1 && n_pi0>=1 && n_nu==1 && n_eta==1){ // eta modes
    JAK_ID=JAK_ETAPIPI0;
    TauBitMask=OneProng;
    if(n_pi0==1)TauBitMask+=OnePi0;
    if(n_pi0==2)TauBitMask+=TwoPi0;
    if(n_pi0==3)TauBitMask+=ThreePi0;
    ClassifyDecayResonance(TauBitMask);
    return;
  }
  //JAK_K0BK0PI
  if ((n_K0S+n_K0L==2 && n_pi==1 && n_pi0==0 && n_K==0 && n_nu==1) || (n_pi>=1 && n_pi0==0 && n_K0L+n_K0S==2 && n_K==0 && n_nu==1) ){
    JAK_ID=JAK_K0BK0PI;
    TauBitMask=OneProng;
    if(n_pi>1)TauBitMask+=KS0_to_pipi;
    ClassifyDecayResonance(TauBitMask);
    return;
  }
  //if(n_Kstar==1){ //JAK_KSTAR K0SPi && KPi0 
  if(n_e==0 && n_mu==0 && n_pi>=1 && n_pi0==0 && n_K==0 && n_K0L+n_K0S==1 && n_nu==1){
    JAK_ID=JAK_KSTAR;
    TauBitMask=OneProng;
    if(n_pi==3)TauBitMask+=KS0_to_pipi;
    ClassifyDecayResonance(TauBitMask);
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==0 && n_pi0==1 && n_K==1 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_KSTAR;
    TauBitMask=OneProng;
    ClassifyDecayResonance(TauBitMask);
    return;
  }
  //}
  //JAK_PIK0PI0
  if(n_e==0 && n_mu==0 && n_pi>=1 && n_pi0==1 && n_K==0 && n_K0L+n_K0S==1  && n_nu==1){
    JAK_ID=JAK_PIK0PI0;
    TauBitMask=OneProng;
    TauBitMask+=OnePi0;
    if(n_pi==3)TauBitMask+=KS0_to_pipi;
    ClassifyDecayResonance(TauBitMask);
    return;
  }
  //JAK_KK0B
  if(n_e==0 && n_mu==0 && n_pi0==0 && n_K==1 && n_K0L+n_K0S==1 && n_nu==1){
    JAK_ID=JAK_KK0B;
    TauBitMask=OneProng;
    if(n_pi==2)TauBitMask+=KS0_to_pipi;
    return;
  }
  //JAK_ID=JAK_KK0BPI0
  if(n_e==0 && n_mu==0 && n_pi0==1 && n_K==1 && n_K0L+n_K0S==1  && n_nu==1){
    JAK_ID=JAK_KK0BPI0;
    TauBitMask=OneProng;
    TauBitMask+=OnePi0;
    if(n_pi==2)TauBitMask+=KS0_to_pipi;
    ClassifyDecayResonance(TauBitMask);
    return;
  }
  
  
  //Safty handelling for exlusive modes
  if (n_K0L!=0){
    std::cout << "Unknown mode with KL0: n_e " <<  n_e << " n_mu " << n_mu << " n_pi " << n_pi << " n_pi0 " << n_pi0 << " n_K " << n_K << "  n_K0L " << n_K0L << "  n_K0S " << n_K0S << " n_nu  " << n_nu << " n_gamma " << n_gamma 
	      << std::endl;
    return;
  }
  if (n_K0S!=0){
    std::cout << "Unknown mode with KS0: n_e " <<  n_e << " n_mu " << n_mu << " n_pi " << n_pi << " n_pi0 " << n_pi0 << " n_K " << n_K << "  n_K0L " << n_K0L << "  n_K0S " << n_K0S << " n_nu  " << n_nu << " n_gamma " << n_gamma 
	      << std::endl;
    return;
  }
  if (n_eta!=0){
    std::cout << "Unknown mode with eta: n_e " <<  n_e << " n_mu " << n_mu << " n_pi " << n_pi << " n_pi0 " << n_pi0 << " n_K " << n_K << "  n_K0L " << n_K0L << "  n_K0S " << n_K0S << " n_nu  " << n_nu << " n_gamma " << n_gamma 
	      << std::endl;
    return;
  }



  if(n_pi+n_K+n_e+n_mu==1)TauBitMask=OneProng;
  if(n_mu==3)TauBitMask=ThreeProng;
  if(n_pi+n_K==3)TauBitMask=ThreeProng;
  if(n_pi+n_K==5)TauBitMask=FiveProng;
  if(n_pi0==1)TauBitMask+=OnePi0;
  if(n_pi0==2)TauBitMask+=TwoPi0;
  if(n_pi0==3)TauBitMask+=ThreePi0;
  ClassifyDecayResonance(TauBitMask);
  ///////////////////////////////////////////////////
  //
  // Standard modes
  //
  if(n_e==1 && n_mu==0 && n_pi==0 && n_pi0==0 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==2){
    JAK_ID=JAK_ELECTRON;
    return;
  }
  if(n_e==0 && n_mu==1 && n_pi==0 && n_pi0==0 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==2){
    JAK_ID=JAK_MUON;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==1 && n_pi0==0 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_PION;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==1 && n_pi0==1 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1 ){
    JAK_ID=JAK_RHO_PIPI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==1 && n_pi0==2 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_A1_3PI;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==3 && n_pi0==0 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_A1_3PI;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==0 && n_pi0==0 && n_K==1 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_KAON;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==3 && n_pi0==1 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_3PIPI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==1 && n_pi0==3 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_PI3PI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==3 && n_pi0==2 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_3PI2PI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==5 && n_pi0==0 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_5PI;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==5 && n_pi0==1 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_5PIPI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==3 && n_pi0==3 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_3PI3PI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==1 && n_pi0==0 && n_K==2 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_KPIK;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==0 && n_pi0==2 && n_K==1 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_K2PI0;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==2 && n_pi0==0 && n_K==1 && n_K0L==0 && n_K0S==0 && n_nu==1){
    JAK_ID=JAK_KPIPI;
    return;
  }
  if(n_e==0 && n_mu==0 && n_pi==1 && n_pi0==1 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==1 && n_gamma>=1 && n_rho==0){
    JAK_ID=JAK_PIPI0GAM;
    return;
  }
  ///////////////////////////////////////////////////
  //
  // Exotic modes
  //

  if(n_e==0 && n_mu==3 && n_pi==0 && n_pi0==0 && n_K==0 && n_K0L==0 && n_K0S==0 && n_nu==0){
    JAK_ID=JAK_3MUON;
    return;
  }
  // std::cout << "Tau Mode not found: n_e " <<  n_e << " n_mu " << n_mu << " n_pi " << n_pi << " n_pi0 " << n_pi0 << " n_K " << n_K << "  n_K0L " << n_K0L << "  n_K0S " << n_K0S << " n_nu  " << n_nu << " n_gamma " << n_gamma << std::endl;
  JAK_ID=JAK_UNKNOWN;
}

void TauDecay::ClassifyDecayResonance(unsigned int &TauBitMask){
  // Add Resonance info to TauBitMask
  if(n_a1>0)     TauBitMask+=Res_a1_pm;
  if(n_a10>0)    TauBitMask+=Res_a1_0;
  if(n_rho>0)    TauBitMask+=Res_rho_pm;
  if(n_rho0>0)   TauBitMask+=Res_rho_0;
  if(n_eta>0)    TauBitMask+=Res_eta;
  if(n_omega>0)  TauBitMask+=Res_omega;
  if(n_Kstar>0)  TauBitMask+=Res_Kstar_pm;
  if(n_Kstar0>0) TauBitMask+=Res_Kstar_0;
}
