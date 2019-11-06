#ifndef DataMCType_h
#define DataMCType_h

#include "TString.h"

class DataMCType{
 public:
  enum Type {Data=1,
	     Ds_PhiPi=30,
	     Ds_Tau=40,
	     B0_Tau=60,
	     Bp_Tau=90,
	     MinBias=120,
	     Tau_3Mu2Nu=121,
	     Ds_EtaMu=122,
	     unknown=999
	     // add here more MC types
  };

  DataMCType();
  ~DataMCType();

  unsigned int GetType(TString name);
  bool isSignalParticle(int pdg_id);
  void StoreType(TString t){type=t;}
  unsigned int GetType(){return GetType(type);}

 private:
  static TString type;

};
#endif
