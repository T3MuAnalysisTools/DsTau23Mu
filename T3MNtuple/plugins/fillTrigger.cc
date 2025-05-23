#include "DsTau23Mu/T3MNtuple/interface/T3MNtuple.h"

void T3MNtuple::fillTrigger(const edm::Event& iEvent,
                            const edm::EventSetup& iSetup,
                            const Handle<TriggerResults>& triggerBitsH,
                            const Handle<trigger::TriggerEvent>& triggerSummary,
                            const Handle<vector<pat::TriggerObjectStandAlone> >& triggerObjects,
                            const TriggerNames& triggerNames)
{
  //  std::cout<<"do we Fill trigger ???  "<< std::endl;

   gtUtil_->retrieveL1(iEvent, iSetup, algToken_);
   const vector<pair<string, bool> > initialDecisions = gtUtil_->decisionsInitial();

   
   if (!iEvent.isRealData())
   {
      for (size_t i_l1t = 0; i_l1t < initialDecisions.size(); i_l1t++){

         string l1tName = (initialDecisions.at(i_l1t)).first;

         if(l1tName.find("DoubleMu") != string::npos || l1tName.find("TripleMu") != string::npos || l1tName.find("SingleMu"))
         {
            Trigger_l1name.push_back( l1tName );

            Trigger_l1decision.push_back( initialDecisions.at(i_l1t).second );
            Trigger_l1prescale.push_back( 1 );
	    //	    std::cout<<"l1 trigger  "<< l1tName <<  "  =     "<< initialDecisions.at(i_l1t).second <<std::endl;
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
	 
	 //         if(l1tName.find("DoubleMu") != string::npos || l1tName.find("TripleMu") != string::npos || l1tName.find("SingleMu"))  
	 // lets keep all L1's
         {
            Trigger_l1name.push_back( l1tName );
            Trigger_l1decision.push_back( initialDecisions.at(i_l1t).second );
            Trigger_l1prescale.push_back( (psAndVetos->prescale_table_)[columnN][i_l1t]);
         }
      }
   } 

   //std::cout<<"   new event  "<< std::endl;
   for (size_t i_hlt = 0; i_hlt != triggerBitsH->size(); ++i_hlt)
   {
      string hltName = triggerNames.triggerName(i_hlt);
      
      //std::cout<<"Test: "<< triggerBitsH->accept(triggerNames.triggerIndex("Flag_goodVertices")) << std::endl;
      
      //if(triggerBitsH->accept(i_hlt ) && ((hltName.find("IsoMu") != std::string::npos) || (hltName.find("IsoMu") != std::string::npos)) ){
      //      std::cout<<"   hltName  "<< hltName << "     status= "<<triggerBitsH->accept(i_hlt ) << std::endl;
      //}
      if(hltName.find("HLT_DoubleMu") != string::npos  or hltName.find("HLT_Mu")      != string::npos or 
	 hltName.find("HLT_Dimuon0")  != string::npos  or hltName.find("HLT_Tau3Mu")  != string::npos or
	 hltName.find("HLT_IsoMu27")  != string::npos  or hltName.find("HLT_IsoMu24") != string::npos or hltName.find("HLT_IsoMu20") != string::npos or                   //  Single Mu Triggers
	 hltName.find("HLT_Ele32_WPTight_Gsf") !=string::npos or hltName.find("HLT_Ele35_WPTight_Gsf") != string::npos or hltName.find("HLT_Ele15_WPLoose_Gsf") != string::npos or    //  Single E  Triggers
	 hltName.find("HLT_PFJet15") !=string::npos or hltName.find("HLT_PFJet25") != string::npos or hltName.find("HLT_AK8PFJet25") != string::npos or hltName.find("HLT_PFJet40") != string::npos)        //  Iso Jet Triggers
	 
      {
	//	std::cout<<"hltName "<< hltName << "   =   " << triggerBitsH->accept(i_hlt ) <<std::endl;
         Trigger_hltname.push_back(hltName);
         Trigger_hltdecision.push_back(triggerBitsH->accept(i_hlt ));
      }
   }

   // Fill Trigger object info
   if (miniAODRun_){
     //     std::cout<<"----------- " << std::endl;
      for (pat::TriggerObjectStandAlone to: *triggerObjects){

         to.unpackPathNames(triggerNames);
         trigger::size_type nFilters = to.filterLabels().size();
         for (trigger::size_type iFilter=0; iFilter<nFilters; iFilter++){
            std::string filterName = to.filterLabels()[iFilter];
	    //	    std::cout<<"  filter Name   "<< filterName  <<std::endl;
            
            //if( filterName.find("SingleMu") != std::string::npos || filterName.find("SingleMu") != std::string::npos)          std::cout<<"  filter Name   "<< filterName  <<std::endl;


	    
            if (filterName.compare("hltTau3muTkVertexFilter")!=0 &&
                filterName.compare("hltdstau3muDisplaced3muFltr")!=0 &&
                filterName.compare("hltTau3MuIsoFilterCharge1")!=0 &&
                filterName.compare("hltTau3MuTriMuon1filter")!=0 &&
                filterName.compare("hltMu1Mu1TrkPairPt2DR0p5MassMax1p9")!=0 &&

                filterName.compare("hltL1sSingleMu22or25")!=0 &&
                filterName.compare("hltPreIsoMu27")!=0 &&
                filterName.compare("hltL1fL1sMu22or25L1Filtered0")!=0 &&
                filterName.compare("hltL2fL1sMu22or25L1f0L2Filtered10Q")!=0 &&
                filterName.compare("hltL1fForIterL3L1fL1sMu22or25L1Filtered0")!=0 &&
                filterName.compare("hltL3fL1sMu22Or25L1f0L2f10QL3Filtered27Q")!=0 &&
                filterName.compare("hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p07")!=0 &&
                filterName.compare("hltL1sSingleMu22")!=0 &&
                filterName.compare("hltPreIsoMu24")!=0 &&
                filterName.compare("hltL1fL1sMu22L1Filtered0")!=0 &&
                filterName.compare("hltL2fL1sSingleMu22L1f0L2Filtered10Q")!=0 &&
                filterName.compare("hltL1fForIterL3L1fL1sMu22L1Filtered0")!=0 &&
                filterName.compare("hltL3crIsoL1sSingleMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p07")!=0 &&
                filterName.compare("hltPreEle32WPTightGsf")!=0 &&
                filterName.compare("hltPreEle35WPTightGsf")!=0 &&
                
                filterName.compare("hltL1sSingleMu18")!=0 &&
                filterName.compare("hltL1sSingleMu22")!=0 &&
                filterName.compare("hltL1sSingleMu7")!=0 &&
                filterName.compare("hltL3fL1sMu5L1L2L3SingleMu")!=0 &&
                filterName.compare("hltL1sSingleMu3")!=0 &&
                
                filterName.compare("hltEle15WPLoose1EcalIsoFilter")!=0 &&
                filterName.compare("hltEle5WPTightEcalIsoFilter")!=0 &&
                filterName.compare("hltL1sSingleEGor")!=0 &&
                filterName.compare("hltEle8EtFilter")!=0 &&
                
                filterName.compare("hltSinglePFJet15")!=0 &&
                filterName.compare("hltSinglePFJet25")!=0 &&
                filterName.compare("hltSinglePFJet25AK8")!=0 &&
                filterName.compare("hltSinglePFJet40")!=0 &&
                filterName.compare("hltAK4PFJetCollection20Filter")!=0 &&
                filterName.compare("hltAK8TrimPFJetCollection20Filter")!=0 &&
                filterName.compare("hltPFTauTrack")!=0 &&
                filterName.compare("hltSingleCaloJet5")!=0 &&

                //filterName.compare("hltL3crIsoL1sMu18L1f0L2f10QL3f20QL3trkIsoFiltered0p07")!=0 &&
		filterName.compare("hltDisplacedmumuFilterDimuon0LowMass")) continue;
	    

	    /*	    if (filterName.compare("hltTau3muTkVertexFilter") or
                filterName.compare("hltdstau3muDisplaced3muFltr") or
                filterName.compare("hltTau3MuIsoFilterCharge1") or
                filterName.compare("hltTau3MuTriMuon1filter") or
                filterName.compare("hltMu1Mu1TrkPairPt2DR0p5MassMax1p9") or
		filterName.compare("hltDisplacedmumuFilterDimuon0LowMass")) 
	    */
		TriggerObject_name.push_back(filterName);
		TriggerObject_pt.push_back(to.pt());
		TriggerObject_eta.push_back(to.eta());
		TriggerObject_phi.push_back(to.phi());

	 }
      }

   } else {

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
