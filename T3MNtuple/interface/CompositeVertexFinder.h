#ifdef DsTau23Mu_T3MNtuple_CompositeVertexFinder_H
#define DsTau23Mu_T3MNtuple_CompositeVertexFinder_H

/* \class CompositeVertexFinder
 *
 * \author Bhargav Joshi, UF
 *
 */

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "DataFormats/Candidate/interface/Candidate.h"

class CompositeVertexFinder : public edm::EDProducer {

   public:
      
      explicit CompositeVertexFinder(const edm::ParameterSet &);
      ~CompositeVertexFinder();

   private:

      virtual void produce(edm::Event&, const edm::EventSetup&) override;

      unsigned int findPhiToKKVertices(const edm::Handle<std::vector<reco::Track> >& ,
                                       const edm::Handle<std::vector<recco::Vertex> >&);

      unsigned int findKsToPiPiVertices(const edm::Handle<std::vector<reco::Track> >& ,
                                       const edm::Handle<std::vector<recco::Vertex> >&);

      bool DEBUG = false;

}

#endif
