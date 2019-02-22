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
  if(name=="data")      return Data;
  if(name=="ds_tau")    return Ds_Tau;
  std::cout << "ERROR: Data/MC Type " << name << " UNKNOWN!!!! " << std::endl;
  return unknown;
}


bool DataMCType::isSignalParticle(int pdg_id){
  unsigned int pdgid=abs(pdg_id);
  if(pdgid==PDGInfo::Z0 || pdgid==PDGInfo::W_plus || pdgid==PDGInfo::Higgs0 || pdgid==PDGInfo::Higgs_plus){
    return true;
  }
  return false; 
}