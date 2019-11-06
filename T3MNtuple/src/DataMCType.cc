#include "DsTau23Mu/T3MNtuple/interface/DataMCType.h"
#include "DsTau23Mu/T3MNtuple/interface/PDGInfo.h"
#include <iostream>
#include <cstdlib>

TString DataMCType::type="unknown";


DataMCType::DataMCType(){
}

DataMCType::~DataMCType(){
}

unsigned int DataMCType::GetType(TString name){
  name.ToLower();
  StoreType(name);
  if(name=="data")          return Data;
  if(name=="ds_tau")        return Ds_Tau;
  if(name=="ds_phipi")      return Ds_PhiPi;
  if(name=="b0_tau")        return B0_Tau;
  if(name=="bp_tau")        return Bp_Tau;
  if(name=="tau_3mu2nu")    return Tau_3Mu2Nu;
  if(name=="ds_etamu")      return Ds_EtaMu;
  if(name=="minbias")       return MinBias;


  std::cout << "ERROR: Data/MC Type " << name << " UNKNOWN!!!! " << std::endl;
  return unknown;
}


bool DataMCType::isSignalParticle(int pdg_id){
  unsigned int pdgid=abs(pdg_id);
  if(pdgid==PDGInfo::Ds_plus || pdgid==PDGInfo::B_plus || pdgid==PDGInfo::B_0 ){
    return true;
  }
  return false; 
}
