//class LambdaAnalyzer LambdaAnalyzer.cc Analysis/LambdaAnalyzer/src/LambdaAnalyzer.cc
// Original Author:  Andrzej Zuranski,Address unknown,NONE,
//         Created:  Fri Dec 11 13:59:16 EST 2009

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
//#include "DataFormats/Common/interface/EDProduct.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h" 
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h" 
#include "TrackingTools/TrackFitters/interface/TrajectoryFitterRecord.h" 
#include "TrackingTools/Records/interface/TransientRecHitRecord.h" 

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include <TString.h>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <iostream>
#include "TMath.h"
#include "TTree.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "RecoVertex/KalmanVertexFit/interface/SingleTrackVertexConstraint.h"

//GEN MC Matching
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/OneToManyWithQuality.h"
#include "DataFormats/Common/interface/OneToManyWithQualityGeneric.h"

#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "RecoTracker/TrackProducer/interface/KfTrackProducerBase.h"
#include "RecoTracker/TrackProducer/interface/TrackProducerAlgorithm.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
//Hit association
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
//Cluster Splitting
#include "RecoLocalTracker/SiPixelClusterizer/plugins/PixelClusterizerBase.h"


typedef std::pair<uint32_t, EncodedEventId> SimHitIdpr;

class LambdaAnalyzer : public edm::EDAnalyzer {
    using Base = AlgoProductTraits<reco::Track>;
    //using TrackCollection = typename Base::TrackCollection;
    using AlgoProductCollection = typename Base::AlgoProductCollection;
    using TrackView = typename Base::TrackView; 
    public:
    explicit LambdaAnalyzer(const edm::ParameterSet&);
    ~LambdaAnalyzer();

    private:
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;
    void printGenInfo(const edm::Event& iEvent);
    void loop(const edm::Event& iEvent, const edm::EventSetup&, const reco::Vertex& RecVtx);
    void assignStableDaughters(const reco::Candidate* p, std::vector<int> & pids);
    void initialize();
    int nSharedPixelLayerHits(reco::TransientTrack *track1, reco::TransientTrack *track2);
    void getSimTracksFromPixelCluster(std::vector<SiPixelCluster::Pixel> cluster, const TrackingRecHit* hit, std::vector<unsigned int> & uniqueSimTrackIds, std::vector<int> & simTrackPDGIds);
    float DeepClusterEvaluate(std::vector<SiPixelCluster::Pixel> pixels);
    float MakeClusterImage(std::vector<SiPixelCluster::Pixel> pixels,float (&image)[20][20]);

    //      void getFromEvt(edm::Event& theEvent,edm::Handle<Base::TrackCollection>& theTCollection); 
    // ----------member data ---------------------------
    bool doGen, doK3pi, doKpi;
    double m_pi, m_K, m_p;
    std::vector<int> LambdaDPicands;
    std::vector<int> dScandsK3pi;
    std::vector<reco::TransientTrack*> goodTracks;
    std::vector<reco::TransientTrack*> slowPiTracks;
    std::vector<reco::TransientTrack> t_tks;
    std::vector<reco::TransientTrack> t_tks_noPXB1;
    TTree *tree1;
    TrackProducerAlgorithm<reco::Track> theAlgo;      

    const std::vector<TrackingParticle> *tPC;
    const edm::DetSetVector<PixelDigiSimLink> *pixelLinks;

    //std::vector<TrackingParticle> trkParticle;

    //ntuple variables
    int NKpiCand,NK3piCand,trigflag[160],NKpiMC,NK3piMC;

    //run, event, lumi section
    int run_n,event_n,lumi;

    //Kpi & K3pi Lambda D* vector vars
    std::vector<double> LambdaMass,DSMassKpi,LambdaVtxProb,LambdaVtxLSig,LambdaVtxLSig3D,LambdaPtKpi,DSPtKpi,LambdaVtxPosx,LambdaVtxPosy,LambdaVtxPosz,LambdaVtxerrx,LambdaVtxerry,LambdaVtxcxy,LambdaVtxcxz,LambdaVtxcyz;
    std::vector<double> LambdaVtxerrz,Lambdaeta,Lambdaphi,DSetaKpi,DSphiKpi,LambdaMassK3pi,LambdaMassK3pi1,DSMassK3pi,DSMassK3pi1,LambdaVtxProb3,LambdaVtx3LSig,LambdaVtx3LSig3D,LambdaPtK3pi,DSPtK3pi,LamdaVertexCAx,LamdaVertexCAy,LamdaVertexCAz;
    std::vector<double> cosAlphaK3pi,cosAlpha,cosAlpha3D,cosAlpha3DK3pi,flightLengthK3pi,flightLength,flightLength_noPXB1,flightLength_noPXB1_error;
    std::vector<double> LambdaVtxPosx3,LambdaVtxPosy3,LambdaVtxPosz3,LambdaVtxerrx3,LambdaVtxerry3,LambdaVtxerrz3,LambdaVtx3cxy,LambdaVtx3cxz,LambdaVtx3cyz,LambdaetaK3pi,LambdaphiK3pi;
    std::vector<double> DSetaK3pi,DSphiK3pi,LambdaMassK3proton,DSMassK3proton;

    std::vector<int> LambdaSharedHitLayer,LambdaSharedHitLadder,LambdaSharedHitModule,LambdaSharedHitPixelHits_x,LambdaSharedHitPixelHits_y,LambdaSharedHitPixelHits_adc;
    std::vector<int> ProtonPixelHit_x,ProtonPixelHit_y,ProtonPixelHit_adc,PionPixelHit_x,PionPixelHit_y,PionPixelHit_adc;
    int PionPixelHitLayer,ProtonPixelHitLayer;

    //Gen quantities
    std::vector<double> GenLambdaVtxPosx, GenLambdaVtxPosy, GenLambdaVtxPosz, GenLambdaSourceVtxPosx, GenLambdaSourceVtxPosy, GenLambdaSourceVtxPosz,GenLambdaPt, GenLambdaP, GenLambdaPhi, GenLambdaEta, GenLambdaMass, GenLambdaMt, GenLambdaE, GenLambdaEt, GenLambdaPx, GenLambdaPy, GenLambdaPz, GenFlightLength, GenDeltaR, GenLambdaDeltaR;

    std::vector<bool> GenVertexMatch; 

    std::vector<double> GenProtonPt, GenProtonP, GenProtonPhi, GenProtonEta, GenProtonMass, GenProtonMt, GenProtonE, GenProtonEt, GenProtonPx, GenProtonPy, GenProtonPz, GenProtonDeltaR;
    std::vector<double> GenPionPt, GenPionP, GenPionPhi, GenPionEta, GenPionMass, GenPionMt, GenPionE, GenPionEt, GenPionPx, GenPionPy, GenPionPz, GenPionDeltaR;


    //Hit Truth matching quantities
    std::vector<int> nSimTracksshared, nSimTrackspion, nSimTracksproton, nUniqueSimTracksInSharedHit,nUniqueSimTracksInPionHit,nUniqueSimTracksInProtonHit;
    std::vector<bool> sharedHitContainsGenLambda, sharedHitContainsGenPion, sharedHitContainsGenProton; 
    std::vector<std::vector<unsigned int>> uniquePionSimTrackIds, uniqueProtonSimTrackIds, uniqueSharedSimTrackIds;
    std::vector<std::vector<signed int>> uniquePionSimTrackPDGIds, uniqueProtonSimTrackPDGIds, uniqueSharedSimTrackPDGIds;




    //primarty vtx vars
    double PVx,PVy,PVz,PVerrx,PVerry,PVerrz,PVcxy,PVcxz,PVcyz;
    double BSx,BSy,BSz,BSerrx,BSerry,BSerrz;
    int nPV, PVOrder;     

    //tracks
    int ntracks;

    //Kpi tracks vars
    std::vector<double> KpiTrkKnhits,KpiTrkpinhits,KpiTrkSnhits;
    std::vector<double> KpiTrkKchi2,KpiTrkpichi2,KpiTrkSchi2;
    std::vector<double> TrkPi1pt,TrkProtonpt,TrkPi1eta,TrkProtoneta,TrkPi1mass,TrkProtonmass,TrkPi1phi,TrkProtonphi,TrkPi1chi2,TrkPi1ndof,TrkProtonndof,TrkProtonchi2,KpiTrkpipt,KpiTrkSpt,TrkPi1dxy,TrkPi1dz,TrkProtondxy,TrkProtondz,PionProtondz;
    std::vector<double> KpiTrkKdxy,KpiTrkpidxy,KpiTrkSdxy,KpiTrkSdxyErr;
    std::vector<double> KpiTrkKdz,KpiTrkpidz,KpiTrkSdz,KpiTrkSdzErr;
    std::vector<double> KpiTrkKeta,KpiTrkpieta,KpiTrkSeta;
    std::vector<double> KpiTrkKphi,KpiTrkpiphi,KpiTrkSphi;
    std::vector<double> KpiDSDeltaR;
    std::vector<double> KpiTrkScharge;
    //MC
    std::vector<double> MCLambdaDeltaR;
    //K3pi tracks vars
    std::vector<double> K3piTrkKnhits,K3piTrk1pinhits,K3piTrk2pinhits,K3piTrk3pinhits,K3piTrkSnhits;
    std::vector<double> K3piTrkKchi2,K3piTrk1pichi2,K3piTrk2pichi2,K3piTrk3pichi2,K3piTrkSchi2;
    std::vector<double> K3piTrkKpt,K3piTrk1pipt,K3piTrk2pipt,K3piTrk3pipt,K3piTrkSpt;
    std::vector<double> K3piTrkKdxy,K3piTrk1pidxy,K3piTrk2pidxy,K3piTrk3pidxy,K3piTrkSdxy,K3piTrkSdxyErr;
    std::vector<double> K3piTrkKdz,K3piTrk1pidz,K3piTrk2pidz,K3piTrk3pidz,K3piTrkSdz,K3piTrkSdzErr;
    std::vector<double> K3piTrkKeta,K3piTrk1pieta,K3piTrk2pieta,K3piTrk3pieta,K3piTrkSeta;
    std::vector<double> K3piTrkKphi,K3piTrk1piphi,K3piTrk2piphi,K3piTrk3piphi,K3piTrkSphi;
    std::vector<double> K3piDSDeltaR;
    std::vector<double> K3piTrkScharge;
    //MC
    std::vector<double> MCLambdaDeltaR3;
    std::vector<int> K3pi_MC_mode;

    edm::EDGetTokenT<reco::TrackCollection> TrackCollT_;
    edm::EDGetTokenT<reco::TrackCollection> TrackCollT_noPXB1_;
    edm::EDGetTokenT<reco::VertexCollection> VtxCollT_;
    edm::EDGetTokenT<reco::GenParticleCollection> GenCollT_;
    edm::EDGetTokenT<reco::BeamSpot> BSCollT_;
    edm::EDGetTokenT<edm::View<reco::Track>> theTCCollection; 

    edm::Handle<reco::GenParticleCollection> genParticles;

    typedef edm::AssociationMap<edm::OneToManyWithQuality< reco::VertexCollection, reco::TrackCollection, int> > TrackVertexAssMap;
    edm::Handle<TrackVertexAssMap> assomap;
    edm::EDGetTokenT<TrackVertexAssMap> T2VCollT_;
    typedef std::vector<TrackingParticle> TrackingParticleCollection;
    edm::EDGetTokenT<TrackingParticleCollection> TrackingParticleCollT_;
    typedef std::vector<TrackingVertex> TrackingVertexCollection;
    edm::EDGetTokenT<TrackingVertexCollection> TrackingVertexCollT_;
    //Hit matching
    edm::EDGetTokenT<edm::DetSetVector<PixelDigiSimLink> > pdslCollT_;
};

LambdaAnalyzer::LambdaAnalyzer(const edm::ParameterSet& iConfig):
    doGen(iConfig.getParameter<bool>("doGen")),
    doK3pi(iConfig.getParameter<bool>("doK3pi")),
    doKpi(iConfig.getParameter<bool>("doKpi")),
    theAlgo(iConfig)
{
    //now do what ever initialization is needed
    edm::Service<TFileService> fs;
    tree1 = fs->make<TTree>("tree1","tree1");
    //tree2 = fs->make<TTree>("tree2","tree2");

    TrackCollT_ = consumes<reco::TrackCollection>(iConfig.getUntrackedParameter<edm::InputTag>("tracks"));
    TrackCollT_noPXB1_ = consumes<reco::TrackCollection>(iConfig.getUntrackedParameter<edm::InputTag>("tracks_noPXB1"));
    VtxCollT_ = consumes<reco::VertexCollection>(iConfig.getUntrackedParameter<edm::InputTag>("vertices"));
    GenCollT_ = consumes<reco::GenParticleCollection>(iConfig.getUntrackedParameter<edm::InputTag>("genParticles"));
    T2VCollT_ = consumes<TrackVertexAssMap>(iConfig.getUntrackedParameter<edm::InputTag>("T2V"));
    BSCollT_ = consumes<reco::BeamSpot>(iConfig.getUntrackedParameter<edm::InputTag>("BeamSpot"));
    theTCCollection = consumes<edm::View<reco::Track>>(iConfig.getUntrackedParameter<edm::InputTag>("trackCandidates"));
    //MixCollT_ = consumes<edm::TrackingParticle>(iConfig.getUntrackedParameter<edm::InputTag>("mix"));

    TrackingParticleCollT_ = consumes<TrackingParticleCollection>(iConfig.getParameter<edm::InputTag>("trackingParticles") );
    TrackingVertexCollT_ = consumes<TrackingVertexCollection>(iConfig.getParameter<edm::InputTag>("trackingParticles") );
    //trackerHitAssociatorConfig_(iConfig, consumesCollector());
    pdslCollT_ = consumes<edm::DetSetVector<PixelDigiSimLink> >(iConfig.getParameter<edm::InputTag>("PixelDigiSimLinkVector"));
}

LambdaAnalyzer::~LambdaAnalyzer(){}

void LambdaAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
    using namespace edm;
    using namespace std;
    using namespace reco;

    m_pi=0.13957018;
    m_K=0.493677;
    m_p = 0.9382720813;


    //clean vectors and vars
    initialize();

    //run, event, lumi section
    run_n = iEvent.id().run();
    event_n = iEvent.id().event();
    lumi = iEvent.luminosityBlock();

    //if (run_n != 254790 || lumi != 292) {
    //    return;
    //}

    //if (event_n != 387623140)  {
    //    //std::cout<<"Found the right lumi and run number, but not the correct event number"<<std::endl;
    //    //std::cout<<"event_n = "<<event_n<<std::endl;      
    //    return; 
    // }

    //std::cout<<"We found the event!!!!"<<std::endl;

    //HLT trigger
    //edm::Handle<TriggerResults> HLTR;
    //iEvent.getByLabel("TriggerResults",HLTR);
    //
    //for(size_t itrig=0;itrig != HLTR->size();++itrig){
    // if(HLTR->accept(itrig)) 
    //   trigflag[itrig]=1;
    // else 
    //   trigflag[itrig]=0;
    // }

    /*
       Handle<TriggerResults>  hltresults;
       InputTag tag("TriggerResults");
       iEvent.getByLabel(tag,hltresults);

       TriggerNames triggerNames_;
       triggerNames_.init(* hltresults);

       int ntrigs = hltresults->size();
       for (int itrig = 0; itrig != ntrigs; ++itrig){
       TString trigName=triggerNames_.triggerName(itrig);
       bool accept = hltresults->accept(itrig);
       if (accept){trigflag[itrig] = 1;}
       else {trigflag[itrig] = 0;}
       }
       */

    // wasn't working on MC, I don't think we need it anyway
    /*//BeamSpot
      reco::BeamSpot vertexBeamSpot;
      edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
    //iEvent.getByType(recoBeamSpotHandle);
    iEvent.getByLabel("offlineBeamSpot", recoBeamSpotHandle); 
    vertexBeamSpot = *recoBeamSpotHandle;

    BSx=vertexBeamSpot.x0();
    BSy=vertexBeamSpot.y0();
    BSz=vertexBeamSpot.z0();
    BSerrx=vertexBeamSpot.x0Error();
    BSerry=vertexBeamSpot.y0Error();
    BSerrz=vertexBeamSpot.z0Error();*/


    Handle<reco::TrackCollection> generalTracks;
    iEvent.getByToken(TrackCollT_, generalTracks);
    Handle<reco::TrackCollection> generalTracks_noPXB1;
    iEvent.getByToken(TrackCollT_noPXB1_, generalTracks_noPXB1);
    //iEvent.getByLabel("generalTracks",generalTracks);


    if(doGen){
        edm::Handle<TrackingParticleCollection> TruthTrackContainer ;
        iEvent.getByToken( TrackingParticleCollT_, TruthTrackContainer );
        tPC = TruthTrackContainer.product();

        edm::Handle<TrackingVertexCollection> TruthVertexContainer ;
        iEvent.getByToken( TrackingVertexCollT_, TruthVertexContainer );
        const TrackingVertexCollection *tVC = TruthVertexContainer.product();

        edm::Handle< edm::DetSetVector<PixelDigiSimLink> > PDSLContainer;
        iEvent.getByToken(pdslCollT_, PDSLContainer);
        pixelLinks = PDSLContainer.product();
    }
    edm::ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
    ///////t_tks = (*theB).build(generalTracks_noPXB1);
    t_tks = (*theB).build(generalTracks);
    t_tks_noPXB1 = (*theB).build(generalTracks_noPXB1);

    // Primary Vtx with most tracks
    Handle<reco::VertexCollection> recVtxs;
    iEvent.getByToken(VtxCollT_, recVtxs);
    //iEvent.getByLabel("offlinePrimaryVertices", recVtxs); //has a BeamSpot if no vtx found;

    std::vector<reco::Track> slowPitrks;
    std::vector<reco::Track> goodtrks;

    // Get Matched Vertices
    //typedef AssociationMap<OneToManyWithQuality< VertexCollection, TrackCollection, int> > TrackVertexAssMap;
    //Handle<TrackVertexAssMap> assomap;
    //iEvent.getByLabel("Tracks2Vertex",assomap); 
    /////iEvent.getByToken(T2VCollT_, assomap);

    //////nPV = assomap->size();
    nPV = recVtxs->size();
    int itnum = 0;
    for(size_t i = 0; i < recVtxs->size(); ++ i) {
        //for(TrackVertexAssMap::const_iterator iAM = assomap->begin(); iAM != assomap->end(); iAM++) 
        itnum++;
        //if (itnum==1) continue; // ignore first PV, as per Vincenzo's suggestion
        //if (itnum!=1) continue; // only consider first vertex, to match with Valentina
        const Vertex &RecVtx = (*recVtxs)[i];
        //const Vertex &RecVtx = *(iAM->key);
        //std::cout<<"processing a reco vtx: "<<i<<std::endl;
        //std::cout<<"ndof = "<<RecVtx.ndof()<<std::endl;
        //std::cout<<"tracksSize = "<<RecVtx.tracksSize()<<std::endl;
        //std::cout<<"isFake = "<<RecVtx.isFake()<<std::endl;
        if(RecVtx.ndof()<4 || RecVtx.tracksSize()<3 || RecVtx.isFake()) continue;  
        //std::cout << "vertex: " << itnum << " ntracks: " << RecVtx.tracksSize() << std::endl;

        PVOrder = itnum;
        PVx = RecVtx.x();
        PVy = RecVtx.y();
        PVz = RecVtx.z();
        PVerrx = sqrt(RecVtx.covariance(0,0));
        PVerry = sqrt(RecVtx.covariance(1,1));
        PVerrz = sqrt(RecVtx.covariance(2,2));
        PVcxy = RecVtx.covariance(0,1);
        PVcxz = RecVtx.covariance(0,2);
        PVcyz = RecVtx.covariance(1,2);

        // track selector

        // first build transient tracks 
        std::vector<TransientTrack> vtx_trks;
        vtx_trks.reserve(RecVtx.tracksSize());

        for(reco::Vertex::trackRef_iterator trk_it = RecVtx.tracks_begin(); trk_it != RecVtx.tracks_end(); ++trk_it){

            const Track& track = *trk_it->get();
            vtx_trks.push_back( (*theB).build(track) );    
        }
        //for(size_t j=0;j<vtx_trks.size();j++)
        for (size_t j=0;j<t_tks.size();j++) {     

            //TransientTrack t_trk = vtx_trks.at(j);
            TransientTrack t_trk = t_tks.at(j);

            if( fabs(t_trk.track().eta())<2.4 && 
                    //   t_trk.track().normalizedChi2() < 10.0 &&
                    t_trk.track().pt() > 0.25){
                //if ( fabs(t_trk.track().dxy(RecVtx.position()) / t_trk.track().d0Error()) < 3.0
                //  && fabs(t_trk.track().dz(RecVtx.position()) / t_trk.track().dzError()) < 3.0) {
                //    slowPiTracks.push_back( &t_tks.at(j));
                //}
                if( (t_trk.track().numberOfValidHits() >= 7) && (t_trk.track().pt() > 0.35) &&
                        fabs(t_trk.track().dz(RecVtx.position()))<2.0 &&
                        fabs(t_trk.track().dxy(RecVtx.position()) / t_trk.track().d0Error()) > 2.0 ) {
                    //fabs(t_trk.track().dxy(RecVtx.position()))<0.1 && 
                    goodTracks.push_back( &t_tks.at(j) );
                }
            }
            //slowPiTracks.push_back( &t_tks.at(j));
            //goodTracks.push_back( &t_tks.at(j) );
            //slowPiTracks.push_back( &vtx_trks.at(j));
            //goodTracks.push_back( &vtx_trks.at(j) );
            //cout<<"track pt, eta, phi, dxy, dz, nhits, chi2: "<<t_trk.track().pt()<<", "<<t_trk.track().eta()<<", "<<t_trk.track().phi()<<", "<<t_trk.track().dxy(RecVtx.position())<<", "<<t_trk.track().dz(RecVtx.position())<<", "<<t_trk.track().numberOfValidHits()<<", "<<t_trk.track().normalizedChi2()<<endl;
        }

        ntracks = slowPiTracks.size();

        //cout << t_tks.size() << "  " << slowPiTracks.size() << " " << goodTracks.size() << endl;


        if(doGen){
            printGenInfo(iEvent);
        }
        loop(iEvent,iSetup,RecVtx);

        //cout << "loop done" << endl;
        goodTracks.clear();
        initialize();
    }


}

void LambdaAnalyzer::loop(const edm::Event& iEvent, const edm::EventSetup& iSetup, const reco::Vertex& RecVtx){

    using namespace std;
    using namespace reco;
    using namespace edm;

    //cout << RecVtx.position() << endl;
    //cout<<"Starting loop"<<endl;

    for(size_t i=0;i<goodTracks.size();i++){  

        TransientTrack* trk1 = goodTracks.at(i);

        for(size_t j=i+1;j<goodTracks.size();j++){

            TransientTrack* trk2 = goodTracks.at(j);

            if(trk1->charge() == trk2->charge()) continue;
            //cout<<"pass1"<<std::endl;

            ////math::XYZVector Lambda_p = trk1->track().momentum() + trk2->track().momentum();

            //////if(sqrt(Lambda_p.perp2()) < 3.) continue;
            //cout<<"pass2"<<std::endl;

            //for(size_t k=0;k<slowPiTracks.size();k++){

            //TransientTrack* trkS = slowPiTracks.at(k);

            //if(*trkS == *trk1 || *trkS == *trk2) continue;
            //cout<<"pass3"<<std::endl;

            //math::XYZVector DS_p = Lambda_p + trkS->track().momentum();
            //if(sqrt(DS_p.perp2())<4.) continue;
            //cout<<"pass4"<<std::endl;

            TransientTrack *pi1=0,*proton=0;


            //if (abs(pi1->track().pt() - 1.1041716) < 0.01) {
            //    cout<<"Found the pions!!"<<endl;
            //    cout<<"Pion1 pt: "<<pi1->track().pt()<<endl;
            //    cout<<"Pion2 pt: "<<proton->track().pt()<<endl;
            //}
            //before vertexing do loose preselection
            //math::XYZTLorentzVector ip4_pi1(pi1->track().px(),pi1->track().py(),pi1->track().pz(),sqrt(pow(pi1->track().p(),2)+pow(m_pi,2)));
            //math::XYZTLorentzVector ip4_proton(proton->track().px(),proton->track().py(),proton->track().pz(),sqrt(pow(proton->track().p(),2)+pow(m_pi,2)));

            //math::XYZTLorentzVector ip4_Lambda = ip4_pi1 + ip4_proton;

            //cout<<"pass5"<<std::endl;

            //math::XYZTLorentzVector p4_S(trkS->track().px(),trkS->track().py(),trkS->track().pz(),sqrt(pow(trkS->track().p(),2)+pow(m_pi,2)));
            //math::XYZTLorentzVector ip4_DS = ip4_Lambda + p4_S;
            //if((ip4_DS.M() - ip4_Lambda.M()) > 0.3) continue;
            //cout<<"pass6"<<std::endl;

            //now the time consuming vertexing

            vector<TransientTrack> tks;
            tks.push_back(*trk1);
            tks.push_back(*trk2);

            KalmanVertexFitter kalman(true);
            TransientVertex v = kalman.vertex(tks);
            if(!v.isValid() || !v.hasRefittedTracks()) continue;
            //cout<<"pass7"<<std::endl;
            double vtxProb =TMath::Prob( (Double_t) v.totalChiSquared(), (Int_t) v.degreesOfFreedom());
            if (vtxProb < 0.05) continue;
            //cout<<"pass8"<<std::endl;
            TransientTrack trk1_f = v.refittedTrack(*trk1);
            TransientTrack trk2_f = v.refittedTrack(*trk2);        

            //TransientTrack pi1_f = *pi1;
            //TransientTrack proton_f = *proton;

            GlobalPoint vert(v.position().x(), v.position().y(), v.position().z());
            TrajectoryStateClosestToPoint  traj1 = trk1->trajectoryStateClosestToPoint(vert );
            TrajectoryStateClosestToPoint  traj2 = trk2->trajectoryStateClosestToPoint(vert );

            double d0_1 = traj1.perigeeParameters().transverseImpactParameter();
            double d0_1_error = traj1.perigeeError().transverseImpactParameterError();
            double d0_2 = traj2.perigeeParameters().transverseImpactParameter();
            double d0_2_error = traj2.perigeeError().transverseImpactParameterError();
            //if ( (d0_1/d0_1_error) > 4.0 || (d0_2/d0_2_error) > 4.0) continue;


            TransientTrack proton_f;
            TransientTrack pi1_f;
            //assign proton/pion to the tracks based on momentum
            if( trk1_f.track().p() > trk2_f.track().p()){
                proton = trk1;
                pi1 = trk2;
                proton_f = trk1_f;
                pi1_f = trk2_f;
            }else{
                proton = trk2;
                pi1 = trk1;
                proton_f = trk2_f;
                pi1_f = trk1_f;
            }


            math::XYZTLorentzVector ip4_pi1(pi1_f.track().px(),pi1_f.track().py(),pi1_f.track().pz(),sqrt(pow(pi1_f.track().p(),2)+pow(m_pi,2)));
            math::XYZTLorentzVector ip4_proton(proton_f.track().px(),proton_f.track().py(),proton_f.track().pz(),sqrt(pow(proton_f.track().p(),2)+pow(m_p,2)));

            math::XYZTLorentzVector ip4_Lambda = ip4_pi1 + ip4_proton;
            //double d0mass = d0_p4.M();
            //if(fabs(d0mass - 1.86484)>0.030) continue;
            //cout<<"d0mass = "<<d0mass<<endl;
            //cout<<"pass9"<<std::endl;
            if( fabs(ip4_Lambda.M()-1.1157)  > 0.05) continue;

            // math::XYZTLorentzVector dS_p4 = d0_p4 + p4_S;
            // double dsmass = dS_p4.M();
            // if( (dsmass - d0mass) > 0.16) continue;
            //cout<<"pass10"<<std::endl;

            //reco::vertex RecVtx (PV), TransientVertex v (Lambda vertex)  
            math::XYZVector PV_position = math::XYZVector(RecVtx.position().x(), RecVtx.position().y(), RecVtx.position().z() );
            math::XYZVector Lambda_position = math::XYZVector(v.position().x(), v.position().y(), v.position().z() );

            math::XYZVector displacement = Lambda_position - PV_position;
            math::XYZVector Lambda_3vector = math::XYZVector(ip4_Lambda.Px(), ip4_Lambda.Py(), ip4_Lambda.Pz());
            double cosalpha = (displacement.Dot(Lambda_3vector) - displacement.Z()*Lambda_3vector.Z() ) / ( sqrt(displacement.perp2()) * sqrt(Lambda_3vector.perp2()) );
            //double flightlength = sqrt(displacement.perp2()) * (cosalpha/fabs(cosalpha)); // flight length in the transverse plane
            //double flightlength = sqrt(displacement.perp2()) * cosalpha; // flight length in the transverse plane in the direction of momentum (what we call lifetime in AN)
            double flightlength = sqrt( displacement.perp2());

            // calculate lifetime significance
            double deriv_x = displacement.X() / fabs(flightlength);
            double deriv_y = displacement.Y() / fabs(flightlength);

            double sigma_L = pow(deriv_x,2) * (pow(PVerrx,2) + v.positionError().cxx()) + pow(deriv_y,2) * (pow(PVerry,2) + v.positionError().cyy()) + 2*deriv_x*deriv_y * (PVcxy + v.positionError().cyx());
            sigma_L = sqrt(sigma_L);
            double LSig = flightlength / sigma_L;

            if (LSig < 10.) continue;

            //double flightlength3D = (displacement.Dot(Lambda_3vector) ) / ( sqrt(Lambda_3vector.Mag2()) );
            double flightlength3D = sqrt(displacement.Mag2());
            double cosalpha3D = (displacement.Dot(Lambda_3vector) ) / ( sqrt(Lambda_3vector.Mag2()) * sqrt(displacement.Mag2()) );
            double deriv3D_x = displacement.X() / fabs(flightlength3D);
            double deriv3D_y = displacement.Y() / fabs(flightlength3D);
            double deriv3D_z = displacement.Z() / fabs(flightlength3D);

            double sigma_L3D = pow(deriv3D_x,2) * (pow(PVerrx,2) + v.positionError().cxx()) + pow(deriv3D_y,2) * (pow(PVerry,2) + v.positionError().cyy()) + pow(deriv3D_z,2) * (pow(PVerrz,2) + v.positionError().czz()) + 2*deriv3D_x*deriv3D_y * (PVcxy + v.positionError().cyx()) + 2*deriv3D_x*deriv3D_z * (PVcxz + v.positionError().czx()) + 2*deriv3D_y*deriv3D_z * (PVcyz + v.positionError().czy());

            sigma_L3D = sqrt(sigma_L3D);
            double LSig3D = flightlength3D / sigma_L3D;

            if (cosalpha < 0.9998) continue;
            //if (cosalpha < 0) continue;
            //cout<<"passALL"<<std::endl;
            //cout<<"pion has pt: "<<pi1->track().pt()<<std::endl;
            //cout<<"pion has unfitted pt: "<<pi1->track().pt()<<std::endl;

            if(doGen){

                //Handle<GenParticleCollection> genParticles;
                //iEvent.getByLabel("genParticles",genParticles);
                iEvent.getByToken(GenCollT_, genParticles);

                double dR = 99.;

                for(size_t i=0; i<LambdaDPicands.size();i++){
                    const reco::GenParticle & la = genParticles->at(LambdaDPicands.at(i));
                    double delta_R = deltaR(ip4_Lambda.eta(),ip4_Lambda.phi(),la.eta(),la.phi());
                    //double delta_R = 99.;
                    //if(delta_R < 0.15 && delta_R < dR)
                    if(delta_R < dR)
                        dR = delta_R;
                }

                MCLambdaDeltaR.push_back(dR);
            }

            // little test
            //std::cout<<"testing..."<<std::endl;
            //std::cout<<pi1_f.numberOfValidHits()<<std::endl;
            //std::cout<<pi1->hitPattern().getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,0)<<std::endl;
            //for (int i=0; i<pi1->numberOfValidHits(); i++) {
            //    std::cout<<"i = "<<i<<std::endl;
            //    std::cout<<pi1->hitPattern().getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,i)<<std::endl;

            //}
            //for (int i=0; i<proton->numberOfValidHits(); i++) {
            //    std::cout<<"i = "<<i<<std::endl;
            //    std::cout<<proton->hitPattern().getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,i)<<std::endl;

            //}
            //pixelBarrelHitFilter
            //        int nSharedPBHits = 0;
            //        std::vector<uint16_t> pion_pxb_hits;
            //        reco::HitPattern hp_pion = pi1->hitPattern();
            //        reco::HitPattern hp_proton = proton->hitPattern();
            //        for (int i=0; i<pi1->numberOfValidHits(); i++) {
            //            uint16_t hit = hp_pion.getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,i);
            //            if (!hp_pion.validHitFilter(hit) || !hp_pion.pixelBarrelHitFilter(hit)) { continue;}
            //            pion_pxb_hits.push_back(hit);
            //        }
            //        for (int i=0; i<proton->numberOfValidHits(); i++) {
            //            uint16_t hit2 = hp_proton.getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,i);
            //            if (!hp_proton.validHitFilter(hit2) || !hp_proton.pixelBarrelHitFilter(hit2)) { continue;}
            //            if(std::find(pion_pxb_hits.begin(), pion_pxb_hits.end(), hit2) != pion_pxb_hits.end()) {
            //                nSharedPBHits += 1;
            //                std::cout<<"found shared pixel barrel hit! : "<<hit2<<" layer : "<<hp_proton.getLayer(hit2)<<std::endl;
            //            }
            //        }
            //        std::cout<<"nSharedPBHits = "<<nSharedPBHits<<std::endl;

            trackingRecHit_iterator hb_pi1 = pi1->recHitsBegin();
            trackingRecHit_iterator hb_proton = proton->recHitsBegin();
            TrackingRecHit const * h1[4] = { (*hb_pi1), (*(hb_pi1+1)), (*(hb_pi1+2)), (*(hb_pi1+3))  };
            TrackingRecHit const * h2[4] = { (*hb_proton), (*(hb_proton+1)), (*(hb_proton+2)), (*(hb_proton+3)) };
            const SiPixelRecHit* pixelhit_p = dynamic_cast<const SiPixelRecHit*>(h2[0]);
            std::vector<SiPixelCluster::Pixel> protonHitPixels;
            if(pixelhit_p!=nullptr && h2[0]->isValid() && LambdaMass.size()==0) {
                // only fill for the first row in the event since we can't have a vector of vectors
                std::vector<SiPixelCluster::Pixel> pixels_p(pixelhit_p->cluster()->pixels());
                protonHitPixels = pixels_p;

                //    std::cout<<"pixels.size() = "<<pixels.size()<<std::endl;
                //    std::cout<<"pixelhit->clusterProbability(0) = "<<pixelhit->clusterProbability(0)<<std::endl;
                //    std::cout<<"pixelhit->clusterProbability(1) = "<<pixelhit->clusterProbability(1)<<std::endl;
                //    std::cout<<"pixelhit->hasFilledProb() = "<<std::endl;
                for (unsigned int k=0; k<pixels_p.size(); k++) {
                    SiPixelCluster::Pixel pixel_p = pixels_p[k];
                    ProtonPixelHit_x.push_back(pixel_p.x);
                    ProtonPixelHit_y.push_back(pixel_p.y);
                    ProtonPixelHit_adc.push_back(pixel_p.adc);
                }
                PXBDetId pxb_id_p = h2[0]->geographicalId();
                ProtonPixelHitLayer = pxb_id_p.layer();
            }
            else {
                ProtonPixelHit_x.push_back(-99);
                ProtonPixelHit_y.push_back(-99);
                ProtonPixelHit_adc.push_back(-99);
                ProtonPixelHitLayer = -1;
            }
            const SiPixelRecHit* pixelhit_pi = dynamic_cast<const SiPixelRecHit*>(h1[0]);
            std::vector<SiPixelCluster::Pixel> pionHitPixels;
            if(pixelhit_pi!=nullptr && h1[0]->isValid() && LambdaMass.size()==0) {
                // only fill for the first row in the event since we can't have a vector of vectors
                std::vector<SiPixelCluster::Pixel> pixels_pi(pixelhit_pi->cluster()->pixels());

                std::cout << "pixels_pi" << std::endl;
                for (unsigned int k=0; k<pixels_pi.size(); k++) {
                    SiPixelCluster::Pixel pixel = pixels_pi[k];
                    std::cout << pixel.x << " " << pixel.y << " " << pixel.adc << std::endl;
                }
                pionHitPixels = pixels_pi;

                std::cout << "DeepCluster Evaluate Pion: " << LambdaAnalyzer::DeepClusterEvaluate(pixels_pi) << std::endl;
                //    std::cout<<"pixels.size() = "<<pixels.size()<<std::endl;
                //    std::cout<<"pixelhit->clusterProbability(0) = "<<pixelhit->clusterProbability(0)<<std::endl;
                //    std::cout<<"pixelhit->clusterProbability(1) = "<<pixelhit->clusterProbability(1)<<std::endl;
                //    std::cout<<"pixelhit->hasFilledProb() = "<<std::endl;
                for (unsigned int k=0; k<pixels_pi.size(); k++) {
                    SiPixelCluster::Pixel pixel_pi = pixels_pi[k];
                    PionPixelHit_x.push_back(pixel_pi.x);
                    PionPixelHit_y.push_back(pixel_pi.y);
                    PionPixelHit_adc.push_back(pixel_pi.adc);
                }
                PXBDetId pxb_id_pi = h1[0]->geographicalId();
                PionPixelHitLayer = pxb_id_pi.layer();

            }
            else {
                PionPixelHit_x.push_back(-99);
                PionPixelHit_y.push_back(-99);
                PionPixelHit_adc.push_back(-99);
                PionPixelHitLayer = -1;
            }
            //std::cout<<"new lambda pairing"<<std::endl;

            //const SiPixelRecHit* sharedHit;
            const TrackingRecHit* sharedHit;
            bool foundSharedHit = false;
            //bool foundSharedHitLayer0 = false;
            std::vector<SiPixelCluster::Pixel> sharedHitPixels;
            for (int k1=0; k1<4; k1++) {
                if (!h1[k1]->isValid()) continue;
                for (int k2=0; k2<4; k2++) {
                    if (!h2[k2]->isValid()) continue;
                    bool shared = h1[k1]->sharesInput(h2[k2],TrackingRecHit::some);
                    if (shared) {
                        const SiPixelRecHit* pixelhit = dynamic_cast<const SiPixelRecHit*>(h1[k1]);
                        if(pixelhit!=nullptr) {
                            std::vector<SiPixelCluster::Pixel> pixels(pixelhit->cluster()->pixels());
                            sharedHitPixels =  pixels;
                            if (LambdaSharedHitLayer.size() == 0) {
                                // can't have a vector of vectors so I don't have a better way to do this now then only deal with the first
                                // Lambda if there are more than one for this vertex
                                //std::cout<<"pixels.size() = "<<pixels.size()<<std::endl;
                                for (unsigned int k=0; k<pixels.size(); k++) {
                                    SiPixelCluster::Pixel pixel = pixels[k];
                                    LambdaSharedHitPixelHits_x.push_back(pixel.x);
                                    LambdaSharedHitPixelHits_y.push_back(pixel.y);
                                    LambdaSharedHitPixelHits_adc.push_back(pixel.adc);
                                }
                            }
                        }
                        //std::cout<<"k1, k2 = "<<k1<<" : "<<k2<<std::endl;
                        //std::cout<<"h1[k1], h2[k2] = "<<h1[k1]<<" : "<<h2[k2]<<std::endl;
                        //std::cout<<h1[k1]->geographicalId()<<std::endl;
                        PXBDetId pxb_id = h1[k1]->geographicalId();
                        //std::cout<<pxb_id.layer()<<std::endl;
                        //std::cout<<pxb_id.ladder()<<std::endl;
                        //std::cout<<pxb_id.module()<<std::endl;
                        LambdaSharedHitLayer.push_back(pxb_id.layer());
                        LambdaSharedHitLadder.push_back(pxb_id.ladder());
                        LambdaSharedHitModule.push_back(pxb_id.module());
                        foundSharedHit = true;
                        sharedHit = h1[k1];
                        //if (pxb_id.layer() == 0) {
                        //    foundSharedHitLayer0 = true;
                        //}
                        //if (pxb_id.layer() == 0 && flightlength > 6.) {
                        //    // shared hit at layer 0 but clearly coming from layer 2
                        //    std::cout<<"found outlier shared hit at layer 0!"<<std::endl;
                        //} 
                        break; // only consider the innermost shared hit
                        //std::cout<<int(h1[k1]->geographicalId())<<std::endl;
                        //std::cout<<h1[k1]->det().subDetector().<<std::endl;
                        //std::cout<<h1[k1]->globalPosition().perp()<<std::endl;
                        //std::cout<<"position = "<<h1[k1]->globalPosition().x()<<" : "<<h1[k1]->globalPosition().y()<<" : "<<h1[k1]->globalPosition().z()<<" : "<<std::endl;
                    }
                }
            }

            if (!foundSharedHit) {
                LambdaSharedHitLayer.push_back(-99);
                LambdaSharedHitLadder.push_back(-99);
                LambdaSharedHitModule.push_back(-99);
            } 

            double flightlength_noPXB1 = -99.;
            double flightlength_noPXB1_error = -99.;
            // try to rebuild Lambda candidate using the corresponding proton/pion tracks with hits at layer 0 removed
            //if (foundSharedHitLayer0) {
            double minDR_pi1 = 999.;
            double minDR_proton = 999.;
            TransientTrack pi1_noPXB1; 
            TransientTrack proton_noPXB1; 
            //std::cout<<"starting new loop"<<std::endl;
            for (size_t j=0;j<t_tks_noPXB1.size();j++) {
                TransientTrack t_trk = t_tks_noPXB1.at(j);

                if( fabs(t_trk.track().eta())<2.4 && 
                        t_trk.track().pt() > 0.25){
                    if( (t_trk.track().numberOfValidHits() >= 6) && (t_trk.track().pt() > 0.35) &&
                            fabs(t_trk.track().dz(RecVtx.position()))<2.0 &&
                            fabs(t_trk.track().dxy(RecVtx.position()) / t_trk.track().d0Error()) > 2.0 ) {
                        double deta_pi1 = fabs(pi1->track().eta() - t_trk.track().eta()); 
                        double dphi_pi1 = fabs(pi1->track().phi() - t_trk.track().phi());
                        double dr_pi1 = TMath::Sqrt(TMath::Power(deta_pi1,2) + TMath::Power(dphi_pi1,2));
                        if (dr_pi1 < minDR_pi1) {
                            minDR_pi1 = dr_pi1;
                            pi1_noPXB1 = t_trk;
                        }
                        double deta_proton = fabs(proton->track().eta() - t_trk.track().eta()); 
                        double dphi_proton = fabs(proton->track().phi() - t_trk.track().phi());
                        double dr_proton = TMath::Sqrt(TMath::Power(deta_proton,2) + TMath::Power(dphi_proton,2));
                        if (dr_proton < minDR_proton && minDR_pi1 != dr_pi1) {
                            minDR_proton = dr_proton;
                            proton_noPXB1 = t_trk;
                        }
                        int nSharedHits = nSharedPixelLayerHits(pi1,&t_trk);
                        //std::cout<<"going to run a sanity check on the no-PXB1 track"<<std::endl;
                        const reco::HitPattern &p = t_trk.hitPattern();
                        for (int i = 0; i < p.numberOfAllHits(HitPattern::TRACK_HITS); i++) {
                            uint32_t hit = p.getHitPattern(HitPattern::TRACK_HITS, i);
                            // if the hit is valid and in pixel barrel, print out the layer
                            if (p.validHitFilter(hit) && p.pixelBarrelHitFilter(hit)){
                                //std::cout << "valid hit found in pixel barrel layer " 
                                //     << p.getLayer(hit) 
                                //    << std::endl;
                            }
                        }
                        //std::cout<<"and now for the normal track..."<<std::endl;
                        const reco::HitPattern &p2 = pi1->hitPattern();
                        for (int i = 0; i < p2.numberOfAllHits(HitPattern::TRACK_HITS); i++) {
                            uint32_t hit = p2.getHitPattern(HitPattern::TRACK_HITS, i);
                            // if the hit is valid and in pixel barrel, print out the layer
                            if (p2.validHitFilter(hit) && p2.pixelBarrelHitFilter(hit)){
                                //std::cout << "valid hit found in pixel barrel layer " 
                                //     << p2.getLayer(hit) 
                                //     << std::endl;
                            }
                        }
                        //std::cout<<"nSharedHits = "<<nSharedHits<<std::endl;
                        //std::cout<<"dr_pi1 = "<<dr_pi1<<std::endl;
                        //if (nSharedHits > 0) {
                        //    std::cout<<"PVOrder = "<<PVOrder<<std::endl;
                        //    std::cout<<"flight length = "<<flightlength<<std::endl;
                        //    std::cout<<"nSharedHits = "<<nSharedHits<<std::endl;
                        //    std::cout<<"dr_pi1 = "<<dr_pi1<<std::endl;
                        //    std::cout<<pi1->track().pt()<<" : "<<pi1->track().eta()<<" : "<<pi1->track().phi()<<std::endl;
                        //    std::cout<<t_trk.track().pt()<<" : "<<t_trk.track().eta()<<" : "<<t_trk.track().phi()<<std::endl;
                        // }
                    }
                }
            }

            if (minDR_pi1 < 0.01 && minDR_proton < 0.01) {
                //std::cout<<"min DR's: "<<minDR_pi1<<" : "<<minDR_proton<<std::endl;
                vector<TransientTrack> tks_noPXB1;
                tks_noPXB1.push_back(pi1_noPXB1);
                tks_noPXB1.push_back(proton_noPXB1);
                //KalmanVertexFitter kalman(true);
                TransientVertex v_noPXB1 = kalman.vertex(tks_noPXB1);
                if(v_noPXB1.isValid() && v_noPXB1.hasRefittedTracks()) {
                    //double vtxProb =TMath::Prob( (Double_t) v.totalChiSquared(), (Int_t) v.degreesOfFreedom());
                    //if (vtxProb < 0.05) continue;
                    //TransientTrack pi1_f_noPXB1 = v_noPXB1.refittedTrack(*pi1_noPXB1);
                    //TransientTrack proton_f_noPXB1 = v_noPXB1.refittedTrack(*proton_noPXB1);        
                    math::XYZVector Lambda_position_noPXB1 = math::XYZVector(v_noPXB1.position().x(), v_noPXB1.position().y(), v_noPXB1.position().z() );

                    math::XYZVector displacement_noPXB1 = Lambda_position_noPXB1 - PV_position;
                    flightlength_noPXB1 = sqrt( displacement_noPXB1.perp2());
                    //std::cout<<"flight length with PXB1: "<<flightlength<<" : and without PXB1: "<<flightlength_noPXB1<<std::endl;
                    double deriv_x_noPXB1 = displacement_noPXB1.X() / fabs(flightlength_noPXB1);
                    double deriv_y_noPXB1 = displacement_noPXB1.Y() / fabs(flightlength_noPXB1);
                    flightlength_noPXB1_error = pow(deriv_x_noPXB1,2) * (pow(PVerrx,2) + v_noPXB1.positionError().cxx()) + pow(deriv_y_noPXB1,2) * (pow(PVerry,2) + v_noPXB1.positionError().cyy()) + 2*deriv_x_noPXB1*deriv_y_noPXB1 * (PVcxy + v_noPXB1.positionError().cyx());
                    flightlength_noPXB1_error = sqrt(flightlength_noPXB1_error);
                    //std::cout<<"change in relative flight length error: "<<sigma_L<< " : "<<sigma_L_noPXB1<<std::endl;
                } 
            }
            //}


            //}
            //std::cout<<pi1_f.hitPattern().getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,0)<<std::endl;
            //std::cout<<proton_f.hitPattern().getHitPattern(reco::HitPattern::HitCategory::TRACK_HITS,0)<<std::endl;

            // find point of closest approach between lambda momentum vector and vertex
            double x_p = ip4_Lambda.X(); 
            double y_p = ip4_Lambda.Y(); 
            double z_p = ip4_Lambda.Z();
            double scale_ca = (x_p*PVx + y_p*PVy + z_p*PVz )/(x_p*x_p + y_p*y_p + z_p*z_p);

            // an attempt to refit tracks by hand
            //edm::ESHandle<TrackerGeometry> theG;
            //edm::ESHandle<MagneticField> theMF;
            //edm::ESHandle<TrajectoryFitter> theFitter;
            //edm::ESHandle<Propagator> thePropagator;
            //edm::ESHandle<MeasurementTracker>  theMeasTk;
            //edm::ESHandle<TransientTrackingRecHitBuilder> theBuilder;
            //iSetup.get<TrackerDigiGeometryRecord>().get(theG);
            ////getFromES(iSetup,theG,theMF,theFitter,thePropagator,theMeasTk,theBuilder);
            //iSetup.get<IdealMagneticFieldRecord>().get(theMF); 
            //iSetup.get<TrajectoryFitter::Record>().get("KFFittingSmootherWithOutliersRejectionAndRK",theFitter);
            //iSetup.get<TrackingComponentsRecord>().get("RungeKuttaTrackerPropagator",thePropagator);
            //iSetup.get<CkfComponentsRecord>().get("",theMeasTk);
            //iSetup.get<TransientRecHitRecord>().get("WithAngleAndTemplate",theBuilder);

            //edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
            //iEvent.getByToken(BSCollT_,recoBeamSpotHandle);
            //reco::BeamSpot bs = *recoBeamSpotHandle;

            //AlgoProductCollection algoResults;
            //edm::Handle<edm::View<reco::Track>> theTCollection;
            //AlgoProductTraits<reco::Track>::AlgoProductCollection algoResults;
            //AlgoProductTraits<reco::Track>::AlgoProduct algoResults;
            //edm::View<reco::Track> theTCollection;
            //reco::Track t_test;
            // Track(double chi2, double ndof, const Point & referencePoint,
            //   const Vector & momentum, int charge, const CovarianceMatrix &,
            //   TrackAlgorithm = undefAlgorithm, TrackQuality quality = undefQuality,
            //      float t0 = 0, float beta = 0, 
            //      float covt0t0 = -1., float covbetabeta = -1.);
            //Handle<TrackCollection> test_generalTracks;
            //iEvent.getByToken(TrackCollT_, test_generalTracks);

            //edm::Handle<edm::View<reco::Track>>  theTCollection;
            //edm::Handle<TrackCollection> theTCollection;
            //edm::View<reco::Track> theTCollection;
            //edm::Handle<TrackCandidateCollection> theTCollection;
            //iEvent.getByToken(theTCCollection,theTCollection);
            //ckfTrackCandidates
            //iEvent.getByLabel("generalTracks",theTCollection );

            ///edm::Handle<edm::View<reco::Track>> theTCollection;
            //getFromEvt(iEvent,theTCollection);

            //edm::Handle<edm::View<reco::Track>>  theTCollection_skimmed;
            //std::cout<<"theTCollection->size() = "<<theTCollection->size()<<std::endl;
            //std::cout<<"theTCollection->at(0) = "<<theTCollection->at(0)<<std::endl;
            //reco::Track test_trk = theTCollection->at(0);
            //std::cout<<test_trk.pt()<<std::endl; 

            // how to build a custom edm::Handle<edm::View<reco::Track>> to refit, i.e. just the
            // p,pi tracks with layer 1 hits removed?

            //theAlgo.runWithTrack(theG.product(), theMF.product(), *theTCollection, 
            //             theFitter.product(), thePropagator.product(), 
            //             theBuilder.product(), bs, algoResults);

            //std::cout<<"algoResults.size() = "<<algoResults.size()<<std::endl;
            //std::cout<<"t_tks.size() = "<<t_tks.size()<<std::endl;

            //TRUTH SECTION
            //Iterate over lambdas to find the best match to pion/proton tracks
            double min_lambda_dR = 99;
            double min_pion_dR = 99;
            double min_p_dR = 99;
            bool genLambdaFound = false;
            double gen_dR = 99;
            TrackingParticle genLambda;
            TrackingVertexRef genLambdaVtx;
            TrackingParticle genLambda2;


            TrackingParticle genPion;
            TrackingParticle genProton;
            TrackingParticleRef genPionRef;
            TrackingParticleRef genProtonRef;

            TrackingParticleRefVector decayTracks;

            int nSimTracksShared = 0;
            int nSimTracksPion = 0;
            int nSimTracksProton = 0;
            //std::cout << "tPC->size(): " << tPC->size() << std::endl; 

            if(doGen){
                //Simtrack Matching
                if(foundSharedHit) std::cout << "Shared Hit!" <<std::endl;
                else std::cout << "Not Shared Hit!" << std::endl;

                //Match gen-level lambdas to reco lambdas 
                if(tPC->size() > 0){
                    //std::cout << "tPC loop" << std::endl;
                    for(TrackingParticleCollection::const_iterator iter = tPC->begin(); iter != tPC->end(); ++iter){
                        //is lambda?
                        if(abs(iter->pdgId())==3122){
                            //Match lambda to reco lambda via dR
                            double lambda_dR = deltaR(ip4_Lambda.Eta(),ip4_Lambda.Phi(),iter->eta(),iter->phi());
                            if(lambda_dR < min_lambda_dR){
                                min_lambda_dR = lambda_dR;
                                genLambda2 = tPC->at(std::distance(tPC->begin(),iter));
                            }

                            //Match lambda via decay products

                            TrackingVertexRef decayVtx = *(iter->decayVertices().end()-1);//some lambdas have multiple decay vertices, looking at just the last one seems to work
                            decayTracks = decayVtx->daughterTracks();

                            TrackingParticleRef pionCand;
                            TrackingParticleRef protonCand; 
                            //check lambda decays to p,pi?
                            if(decayTracks.size() >= 2){
                                if(abs(decayTracks.at(0)->pdgId())==211 && abs(decayTracks.at(1)->pdgId())==2212){
                                    pionCand = decayTracks.at(0);
                                    protonCand = decayTracks.at(1);
                                }
                                else if(abs(decayTracks.at(0)->pdgId())==2212 && abs(decayTracks.at(1)->pdgId())==211){

                                    pionCand = decayTracks.at(1);
                                    protonCand = decayTracks.at(0);
                                }
                                else continue;
                                //Now we have the pion/proton tracks
                                double pion_dR = deltaR(ip4_pi1.Eta(),ip4_pi1.Phi(),pionCand->eta(),pionCand->phi());  
                                double p_dR = deltaR(ip4_proton.Eta(),ip4_proton.Phi(),protonCand->eta(),protonCand->phi());   
                                if(pion_dR + p_dR < gen_dR){
                                    gen_dR = pion_dR + p_dR;
                                    min_pion_dR = pion_dR;
                                    min_p_dR = p_dR;

                                    genLambda = tPC->at(std::distance(tPC->begin(), iter));
                                    genPion = *pionCand;
                                    genProton = *protonCand;
                                    genPionRef = pionCand;
                                    genProtonRef = protonCand;
                                    genLambdaVtx = decayVtx;
                                    genLambdaFound = true;

                                }
                            }
                        }
                    } 

                    if(genLambdaFound){
                        std::cout << "Matched Gen Lambda, dR = " << gen_dR << std::endl; 
                        std::cout << "dPhi gen lamdas:" << genLambda.phi() - genLambda2.phi() << std::endl;
                    }
                }
            }


            //Splitting Cluster Section
            SiPixelCluster::PixelPos newpix(1,1);
            PixelClusterizerBase::AccretionCluster acluster;
            acluster.add(newpix, 10);


            //Merged Truth Section
            //if it's a shared hit, loop through all the pixels (check hit validity!)
            //loop through links associated with that hit, and see if any links have the right pixel
            //output the pdgid and if it's the genpion/proton/lambda
            int nUniqueSimTracksShared = -99;
            int nUniqueSimTracksPion = -99;
            int nUniqueSimTracksProton = -99;
            bool genLambdaInSharedHit = false;
            bool genPionInSharedHit = false;
            bool genProtonInSharedHit = false;
            if(doGen){
                //TODO May need to add ->isValid() check on all hits here
                if(foundSharedHit){
                    std::vector<unsigned int> uniqueSimTracksInSharedHit;
                    std::vector<int> uniqueSimTrackSharedPDGIds;
                    LambdaAnalyzer::getSimTracksFromPixelCluster(sharedHitPixels, sharedHit, uniqueSimTracksInSharedHit, uniqueSimTrackSharedPDGIds);

                    uniqueSharedSimTrackPDGIds.push_back(uniqueSimTrackSharedPDGIds);
                    uniqueSharedSimTrackIds.push_back(uniqueSimTracksInSharedHit);

                    std::cout << "Found this many unique simtracks in shared hit: " << uniqueSimTracksInSharedHit.size() << std::endl;
                    nUniqueSimTracksShared = uniqueSimTracksInSharedHit.size();
                    //Flag if the track IDs match gen lambda/proton/pion
                    if(genLambdaFound){
                        for(auto trackId : uniqueSimTracksInSharedHit){
                            if(trackId == (genLambda.g4Tracks()).front().trackId() ){    std::cout << "It's the lambda!" << std::endl; genLambdaInSharedHit = true;}
                            if(trackId == (genProtonRef->g4Tracks()).front().trackId() ){    std::cout << "It's the proton!" << std::endl; genProtonInSharedHit = true;}
                            if(trackId == (genPionRef->g4Tracks()).front().trackId() ){    std::cout << "It's the pion!" << std::endl; genPionInSharedHit = true;}
                        }
                    }

                    std::cout << "shared hit simTrack PDGIds" << std::endl;
                    for(auto pdgId : uniqueSimTrackSharedPDGIds) std::cout << pdgId << std::endl;
                }
                else{
                    //calculate sim tracks in proton/pion clusters
                    std::vector<unsigned int> uniqueSimTracksInPionHit;
                    std::vector<int> uniqueSimTrackPionPDGIds;
                    LambdaAnalyzer::getSimTracksFromPixelCluster(pionHitPixels, pixelhit_pi, uniqueSimTracksInPionHit, uniqueSimTrackPionPDGIds);
                    nUniqueSimTracksPion = uniqueSimTracksInPionHit.size(); 
                    uniquePionSimTrackPDGIds.push_back(uniqueSimTrackPionPDGIds);
                    uniquePionSimTrackIds.push_back(uniqueSimTracksInPionHit);


                    std::vector<unsigned int> uniqueSimTracksInProtonHit;
                    std::vector<int> uniqueSimTrackProtonPDGIds;
                    LambdaAnalyzer::getSimTracksFromPixelCluster(protonHitPixels, pixelhit_p, uniqueSimTracksInProtonHit, uniqueSimTrackProtonPDGIds);
                    nUniqueSimTracksProton = uniqueSimTracksInProtonHit.size(); 
                    uniqueProtonSimTrackPDGIds.push_back(uniqueSimTrackProtonPDGIds);
                    uniqueProtonSimTrackIds.push_back(uniqueSimTracksInProtonHit);

                    //std::cout << "DeepCluster Evaluate Pion" << LambdaAnalyzer::DeepClusterEvaluate(pionHitPixels) << std::endl;
                }

            }



            LambdaVtxProb.push_back(vtxProb);
            LambdaVtxLSig.push_back(LSig);
            LambdaVtxLSig3D.push_back(LSig3D);
            LambdaMass.push_back(ip4_Lambda.M());
            LambdaPtKpi.push_back(ip4_Lambda.Pt());
            Lambdaeta.push_back(ip4_Lambda.eta());
            Lambdaphi.push_back(ip4_Lambda.phi());
            LamdaVertexCAx.push_back(scale_ca*x_p);
            LamdaVertexCAy.push_back(scale_ca*y_p);
            LamdaVertexCAz.push_back(scale_ca*z_p);

            cosAlpha3D.push_back(cosalpha3D);
            cosAlpha.push_back(cosalpha);
            flightLength.push_back(flightlength);
            flightLength_noPXB1.push_back(flightlength_noPXB1);
            flightLength_noPXB1_error.push_back(flightlength_noPXB1_error);

            LambdaVtxPosx.push_back(v.position().x());
            LambdaVtxPosy.push_back(v.position().y());
            LambdaVtxPosz.push_back(v.position().z());
            LambdaVtxerrx.push_back(sqrt(v.positionError().cxx()));
            LambdaVtxerry.push_back(sqrt(v.positionError().cyy()));
            LambdaVtxerrz.push_back(sqrt(v.positionError().czz()));
            LambdaVtxcxy.push_back(v.positionError().cyx());
            LambdaVtxcxz.push_back(v.positionError().czx());
            LambdaVtxcyz.push_back(v.positionError().czy());

            //        KpiTrkKdxy.push_back(K_f.track().dxy(RecVtx.position()));
            //        KpiTrkpidxy.push_back(pi_f.track().dxy(RecVtx.position()));
            //        KpiTrkSdxy.push_back(trkS->track().dxy(RecVtx.position()));
            //        KpiTrkSdxyErr.push_back(trkS->track().d0Error());

            //        KpiTrkKdz.push_back(K_f.track().dz(RecVtx.position()));
            //        KpiTrkpidz.push_back(pi_f.track().dz(RecVtx.position()));
            //        KpiTrkSdz.push_back(trkS->track().dz(RecVtx.position()));
            //        KpiTrkSdzErr.push_back(trkS->track().dzError());

            //        KpiTrkKnhits.push_back(K->track().numberOfValidHits());
            //        KpiTrkpinhits.push_back(pi->track().numberOfValidHits());
            //        KpiTrkSnhits.push_back(trkS->track().numberOfValidHits());

            //        KpiTrkKchi2.push_back(K->track().normalizedChi2());
            //        KpiTrkpichi2.push_back(pi->track().normalizedChi2());
            //        KpiTrkSchi2.push_back(trkS->track().normalizedChi2());

            //        KpiDSDeltaR.push_back(deltaR(d0_p4.eta(),d0_p4.phi(),trkS->track().eta(),trkS->track().phi()));

            TrkPi1pt.push_back(ip4_pi1.Pt());
            TrkProtonpt.push_back(ip4_proton.Pt());
            TrkPi1eta.push_back(ip4_pi1.Eta());
            TrkProtoneta.push_back(ip4_proton.Eta());
            TrkPi1mass.push_back(ip4_pi1.M());
            TrkProtonmass.push_back(ip4_proton.M());
            TrkPi1phi.push_back(ip4_pi1.Phi());
            TrkProtonphi.push_back(ip4_proton.Phi());
            TrkPi1chi2.push_back(pi1->track().normalizedChi2());
            TrkProtonchi2.push_back(proton->track().normalizedChi2());
            TrkPi1ndof.push_back(pi1->track().ndof());
            TrkProtonndof.push_back(proton->track().ndof());
            TrkPi1dxy.push_back(pi1->track().dxy(RecVtx.position()));
            TrkPi1dz.push_back(pi1->track().dz(RecVtx.position()));
            TrkProtondxy.push_back(pi1->track().dxy(RecVtx.position()));
            TrkProtondz.push_back(pi1->track().dz(RecVtx.position()));
            PionProtondz.push_back(fabs(traj1.position().z()-traj2.position().z()));
            //TrkPi1pt.push_back(pi1->track().pt());
            //TrkProtonpt.push_back(proton->track().pt());
            //TrkPi1eta.push_back(pi1->track().eta());
            //TrkProtoneta.push_back(proton->track().eta());
            //TrkPi1mass.push_back(ip4_pi1.M());
            //TrkProtonmass.push_back(ip4_proton.M());
            //TrkPi1phi.push_back(pi1->track().phi());
            //TrkProtonphi.push_back(proton->track().phi());
            //TrkPi1chi2.push_back(pi1->track().normalizedChi2());
            //TrkProtonchi2.push_back(proton->track().normalizedChi2());
            //TrkPi1ndof.push_back(pi1->track().ndof());
            //TrkProtonndof.push_back(proton->track().ndof());

            //        KpiTrkpipt.push_back(pi_f.track().pt());
            //        KpiTrkSpt.push_back(trkS->track().pt());

            //        KpiTrkKeta.push_back(K_f.track().eta());
            //        KpiTrkpieta.push_back(pi_f.track().eta());
            //        KpiTrkSeta.push_back(trkS->track().eta());

            //        KpiTrkKphi.push_back(K_f.track().phi());
            //        KpiTrkpiphi.push_back(pi_f.track().phi());
            //        KpiTrkSphi.push_back(trkS->track().phi());

            //        KpiTrkScharge.push_back(trkS->charge());
            if(genLambdaFound){
                //std::cout << "Adding Gen Lambda quantities" << std::endl;
                GenLambdaVtxPosx.push_back(genLambdaVtx->position().x());
                GenLambdaVtxPosy.push_back(genLambdaVtx->position().y());
                GenLambdaVtxPosz.push_back(genLambdaVtx->position().z());
                GenLambdaSourceVtxPosx.push_back(genLambda.vx());
                GenLambdaSourceVtxPosy.push_back(genLambda.vy());
                GenLambdaSourceVtxPosz.push_back(genLambda.vz());
                GenLambdaPt.push_back(genLambda.pt());
                GenLambdaP.push_back(genLambda.p());
                GenLambdaPhi.push_back(genLambda.phi());
                GenLambdaEta.push_back(genLambda.eta());
                GenLambdaMass.push_back(genLambda.mass());
                GenLambdaMt.push_back(genLambda.mt());
                GenLambdaE.push_back(genLambda.energy());
                GenLambdaEt.push_back(genLambda.et());
                GenLambdaPx.push_back(genLambda.px());
                GenLambdaPy.push_back(genLambda.py());
                GenLambdaPz.push_back(genLambda.pz());
                GenDeltaR.push_back(gen_dR);
                GenLambdaDeltaR.push_back(min_lambda_dR);

                //genLambdaVtx->position is the lambda->pion,proton vertex.  genLambda.v() is the vertex the lambda is created at
                //std::cout << "Calculating Gen Lambda Flight Length" << std::endl;
                GenFlightLength.push_back(sqrt(pow(genLambdaVtx->position().x()-genLambda.vx(),2) + pow(genLambdaVtx->position().y() - genLambda.vy(),2) + pow(genLambdaVtx->position().z() - genLambda.vz(),2)));

                // Gen Protons and Pions:
                GenProtonPt.push_back(genProton.pt());
                GenProtonP.push_back(genProton.p());
                GenProtonPhi.push_back(genProton.phi());
                GenProtonEta.push_back(genProton.eta());
                GenProtonMass.push_back(genProton.mass());
                GenProtonMt.push_back(genProton.mt());
                GenProtonE.push_back(genProton.energy());
                GenProtonEt.push_back(genProton.et());
                GenProtonPx.push_back(genProton.px());
                GenProtonPy.push_back(genProton.py());
                GenProtonPz.push_back(genProton.pz());
                GenProtonDeltaR.push_back(min_p_dR);


                GenPionPt.push_back(genPion.pt());
                GenPionP.push_back(genPion.p());
                GenPionPhi.push_back(genPion.phi());
                GenPionEta.push_back(genPion.eta());
                GenPionMass.push_back(genPion.mass());
                GenPionMt.push_back(genPion.mt());
                GenPionE.push_back(genPion.energy());
                GenPionEt.push_back(genPion.et());
                GenPionPx.push_back(genPion.px());
                GenPionPy.push_back(genPion.py());
                GenPionPz.push_back(genPion.pz());
                GenPionDeltaR.push_back(min_pion_dR);


                //Hit matching
                nSimTracksshared.push_back(nSimTracksShared);
                nSimTrackspion.push_back(nSimTracksPion);
                nSimTracksproton.push_back(nSimTracksProton);
                nUniqueSimTracksInSharedHit.push_back(nUniqueSimTracksShared);
                nUniqueSimTracksInPionHit.push_back(nUniqueSimTracksPion);
                nUniqueSimTracksInProtonHit.push_back(nUniqueSimTracksProton);

                sharedHitContainsGenLambda.push_back(genLambdaInSharedHit);
                sharedHitContainsGenPion.push_back(genPionInSharedHit);
                sharedHitContainsGenProton.push_back(genProtonInSharedHit);

            }        
            NKpiCand++;

            if(NKpiCand>999) break;
            //
            if(NKpiCand>999) break;
        } 
        if(NKpiCand>999) break;
    }

    if (NKpiCand > 0) {
        tree1->Fill();
    }
}


void LambdaAnalyzer::assignStableDaughters(const reco::Candidate* p, std::vector<int> & pids){

    std::cout<<"called assignStableDaughters for: "<<p->pdgId()<<std::endl;
    for(size_t i=0;i<p->numberOfDaughters();i++){
        if(p->daughter(i)->status()==1) {
            std::cout<<"adding stable daughter: "<<p->daughter(i)->pdgId()<<std::endl;
            pids.push_back(abs(p->daughter(i)->pdgId()));
        }
        else {
            std::cout<<"recursive call"<<std::endl;
            assignStableDaughters(p->daughter(i),pids);
        }
    }
    return;
}


void LambdaAnalyzer::printGenInfo(const edm::Event& iEvent){

    using namespace std;
    using namespace reco;
    using namespace edm;

    //Handle<GenParticleCollection> genParticles;
    //iEvent.getByLabel("genParticles",genParticles);
    iEvent.getByToken(GenCollT_, genParticles);

    for(size_t i=0;i<genParticles->size();i++){

        const reco::GenParticle & p = (*genParticles)[i];

        if(p.pdgId()==3122){ //Lambda

            LambdaDPicands.push_back(i); 
        }

    }
}

void LambdaAnalyzer::initialize(){
    //clearing the vectors
    //analysis
    //ntracks.clear(); PVx.clear(); PVy.clear(); PVz.clear(); PVerrx.clear(); PVerry.clear(); PVerrz.clear();
    LambdaDPicands.clear();  dScandsK3pi.clear();  goodTracks.clear(); slowPiTracks.clear();
    //Kpi D* Lambda
    LambdaMass.clear();  DSMassKpi.clear();  LambdaVtxProb.clear(); LambdaVtxLSig.clear(); LambdaVtxLSig3D.clear();  LambdaPtKpi.clear();  DSPtKpi.clear();  LambdaVtxPosx.clear();
    LambdaVtxPosy.clear();  LambdaVtxPosz.clear();  Lambdaeta.clear();  Lambdaphi.clear();  LambdaVtxerrx.clear();  LambdaVtxerry.clear(); LamdaVertexCAx.clear(); LamdaVertexCAy.clear(); LamdaVertexCAz.clear();
    LambdaVtxerrz.clear();  LambdaVtxcxy.clear();   LambdaVtxcxz.clear();  LambdaVtxcyz.clear();  DSetaKpi.clear(); DSphiKpi.clear();
    cosAlphaK3pi.clear(); cosAlpha.clear(); cosAlpha3D.clear(); cosAlpha3DK3pi.clear(); flightLengthK3pi.clear(); flightLength.clear(); flightLength_noPXB1.clear(); flightLength_noPXB1_error.clear();
    LambdaSharedHitLayer.clear(); LambdaSharedHitLadder.clear(); LambdaSharedHitModule.clear(); LambdaSharedHitPixelHits_x.clear(); LambdaSharedHitPixelHits_y.clear(); LambdaSharedHitPixelHits_adc.clear();
    ProtonPixelHit_x.clear(); ProtonPixelHit_y.clear(); ProtonPixelHit_adc.clear(); PionPixelHit_x.clear(); PionPixelHit_y.clear(); PionPixelHit_adc.clear(); 
    //K3pi D* Lambda
    LambdaMassK3pi.clear(); DSMassK3pi.clear(); LambdaMassK3pi1.clear();  DSMassK3pi1.clear();  LambdaVtxProb3.clear(); LambdaVtx3LSig.clear(); LambdaVtx3LSig3D.clear(); LambdaPtK3pi.clear();  DSPtK3pi.clear();  LambdaVtxPosx3.clear();
    LambdaVtxPosy3.clear();  LambdaVtxPosz3.clear();  LambdaetaK3pi.clear();  LambdaphiK3pi.clear();  LambdaVtxerrx3.clear();  LambdaVtxerry3.clear(); 
    LambdaVtxerrz3.clear(); LambdaVtx3cxy.clear();  LambdaVtx3cxz.clear();  LambdaVtx3cyz.clear(); DSetaK3pi.clear(); DSphiK3pi.clear();
    LambdaMassK3proton.clear(); DSMassK3proton.clear();
    //Kpi tracks
    KpiTrkKdxy.clear();  KpiTrkpidxy.clear();  KpiTrkSdxy.clear(); KpiTrkSdxyErr.clear();
    KpiTrkKdz.clear();  KpiTrkpidz.clear();  KpiTrkSdz.clear(); KpiTrkSdzErr.clear();
    TrkPi1pt.clear(); TrkProtonpt.clear(); TrkPi1eta.clear(); TrkProtoneta.clear(); TrkPi1phi.clear(); TrkProtonphi.clear(); TrkPi1mass.clear(); TrkProtonmass.clear(); TrkPi1chi2.clear(); TrkProtonchi2.clear(); TrkPi1ndof.clear(); TrkProtonndof.clear(); TrkPi1dxy.clear(); TrkPi1dz.clear(); TrkProtondxy.clear(); TrkProtondz.clear(); PionProtondz.clear();

    KpiTrkpipt.clear();  KpiTrkSpt.clear();
    KpiTrkKchi2.clear();  KpiTrkpichi2.clear();  KpiTrkSchi2.clear();
    KpiTrkKnhits.clear();  KpiTrkpinhits.clear();  KpiTrkSnhits.clear();
    KpiTrkKeta.clear();  KpiTrkpieta.clear();  KpiTrkSeta.clear();
    KpiTrkKphi.clear();  KpiTrkpiphi.clear();  KpiTrkSphi.clear();
    KpiDSDeltaR.clear(); KpiTrkScharge.clear();
    //MC
    MCLambdaDeltaR.clear();MCLambdaDeltaR3.clear(),K3pi_MC_mode.clear();

    //K3pi tracks
    K3piTrkKdxy.clear();  K3piTrk1pidxy.clear();  K3piTrk2pidxy.clear();  K3piTrk3pidxy.clear();  K3piTrkSdxy.clear(); K3piTrkSdxyErr.clear();
    K3piTrkKdz.clear();  K3piTrk1pidz.clear();  K3piTrk2pidz.clear();  K3piTrk3pidz.clear();  K3piTrkSdz.clear(); K3piTrkSdzErr.clear();
    K3piTrkKpt.clear();  K3piTrk1pipt.clear();  K3piTrk2pipt.clear();  K3piTrk3pipt.clear();  K3piTrkSpt.clear();
    K3piTrkKchi2.clear();  K3piTrk1pichi2.clear();  K3piTrk2pichi2.clear();  K3piTrk3pichi2.clear();  K3piTrkSchi2.clear();
    K3piTrkKnhits.clear();  K3piTrk1pinhits.clear();  K3piTrk2pinhits.clear();  K3piTrk3pinhits.clear();
    K3piTrkSnhits.clear();
    K3piTrkKeta.clear();  K3piTrk1pieta.clear();  K3piTrk2pieta.clear();  K3piTrk3pieta.clear();  K3piTrkSeta.clear();
    K3piTrkKphi.clear();  K3piTrk1piphi.clear();  K3piTrk2piphi.clear();  K3piTrk3piphi.clear();  K3piTrkSphi.clear();
    K3piDSDeltaR.clear(); K3piTrkScharge.clear();

    //Gen quantities
    GenLambdaVtxPosx.clear(); GenLambdaVtxPosy.clear(); GenLambdaVtxPosz.clear();
    GenLambdaSourceVtxPosx.clear(); GenLambdaSourceVtxPosy.clear(); GenLambdaSourceVtxPosz.clear();
    GenLambdaMass.clear(); GenLambdaPt.clear(); GenLambdaPhi.clear(); GenLambdaEta.clear(); GenLambdaMt.clear(); GenLambdaE.clear(); GenLambdaEt.clear(); GenLambdaPx.clear(); GenLambdaPy.clear(); GenLambdaPz.clear(); GenFlightLength.clear(); GenDeltaR.clear(); GenVertexMatch.clear(); GenLambdaDeltaR.clear();


    // Gen Proton and Pion
    GenProtonPt.clear(); GenProtonP.clear(); GenProtonPhi.clear(); GenProtonEta.clear(); GenProtonMass.clear(); GenProtonMt.clear(); GenProtonE.clear(); GenProtonEt.clear(); GenProtonPx.clear(); GenProtonPy.clear(); GenProtonPz.clear(); GenProtonDeltaR.clear();
    GenPionPt.clear(); GenPionP.clear(); GenPionPhi.clear(); GenPionEta.clear(); GenPionMass.clear(); GenPionMt.clear(); GenPionE.clear(); GenPionEt.clear(); GenPionPx.clear(); GenPionPy.clear(); GenPionPz.clear(); GenPionDeltaR.clear();

    //Hit Matching
    nSimTracksshared.clear(); nSimTrackspion.clear(); nSimTracksproton.clear(); nUniqueSimTracksInSharedHit.clear(); nUniqueSimTracksInPionHit.clear(); nUniqueSimTracksInProtonHit.clear(); sharedHitContainsGenLambda.clear(); sharedHitContainsGenPion.clear(); sharedHitContainsGenProton.clear(); uniqueSharedSimTrackIds.clear(); uniquePionSimTrackIds.clear(); uniqueProtonSimTrackIds.clear(); uniqueSharedSimTrackPDGIds.clear(); uniquePionSimTrackPDGIds.clear(); uniqueProtonSimTrackPDGIds.clear();



    //MC ids
    //static variables
    for(int i=0;i<160;i++)
        trigflag[i]=0;
    BSx = BSy = BSz = BSerrx = BSerry = BSerrz = -99.;
    NKpiCand=0,NK3piCand=0,NKpiMC=0,NK3piMC=0;//,run_n=0,event_n=0,lumi=0;
}

void LambdaAnalyzer::beginJob(){
    tree1->Branch("trigflag",&trigflag,"trigflag[160]/I");
    tree1->Branch("NKpiCand",&NKpiCand,"NKpiCand/I");
    tree1->Branch("NKpiMC",&NKpiMC,"NKpiMC/I");

    tree1->Branch("run_n",&run_n,"run_n/I");
    tree1->Branch("event_n",&event_n,"event_n/I");
    tree1->Branch("lumi",&lumi,"lumi/I");


    tree1->Branch("LambdaMass",&LambdaMass);
    tree1->Branch("LambdaVtxProb",&LambdaVtxProb);
    tree1->Branch("LambdaVtxLSig",&LambdaVtxLSig);
    tree1->Branch("LambdaVtxLSig3D",&LambdaVtxLSig3D);
    tree1->Branch("LambdaVtxPosx",&LambdaVtxPosx);
    tree1->Branch("LambdaVtxPosy",&LambdaVtxPosy);
    tree1->Branch("LambdaVtxPosz",&LambdaVtxPosz);
    tree1->Branch("LambdaVtxerrx",&LambdaVtxerrx);
    tree1->Branch("LambdaVtxerry",&LambdaVtxerry);
    tree1->Branch("LambdaVtxerrz",&LambdaVtxerrz);
    tree1->Branch("LambdaVtxcxy", &LambdaVtxcxy);
    tree1->Branch("LambdaVtxcxz", &LambdaVtxcxz);
    tree1->Branch("LambdaVtxcyz", &LambdaVtxcyz);
    tree1->Branch("Lambdaeta",&Lambdaeta);
    tree1->Branch("Lambdaphi",&Lambdaphi);
    tree1->Branch("LamdaVertexCAx",&LamdaVertexCAx);
    tree1->Branch("LamdaVertexCAy",&LamdaVertexCAy);
    tree1->Branch("LamdaVertexCAz",&LamdaVertexCAz);
    tree1->Branch("cosAlpha",&cosAlpha);
    tree1->Branch("cosAlpha3D",&cosAlpha3D);
    tree1->Branch("flightLength", &flightLength);
    tree1->Branch("flightLength_noPXB1", &flightLength_noPXB1);
    tree1->Branch("flightLength_noPXB1_error", &flightLength_noPXB1_error);
    tree1->Branch("LambdaSharedHitLayer",&LambdaSharedHitLayer);
    tree1->Branch("LambdaSharedHitLadder",&LambdaSharedHitLadder);
    tree1->Branch("LambdaSharedHitModule",&LambdaSharedHitModule);
    tree1->Branch("LambdaSharedHitPixelHits_x",&LambdaSharedHitPixelHits_x);
    tree1->Branch("LambdaSharedHitPixelHits_y",&LambdaSharedHitPixelHits_y);
    tree1->Branch("LambdaSharedHitPixelHits_adc",&LambdaSharedHitPixelHits_adc);
    tree1->Branch("PionPixelHit_x",&PionPixelHit_x);
    tree1->Branch("PionPixelHit_y",&PionPixelHit_y);
    tree1->Branch("PionPixelHit_adc",&PionPixelHit_adc);
    tree1->Branch("ProtonPixelHit_x",&ProtonPixelHit_x);
    tree1->Branch("ProtonPixelHit_y",&ProtonPixelHit_y);
    tree1->Branch("ProtonPixelHit_adc",&ProtonPixelHit_adc);
    tree1->Branch("PionPixelHitLayer",&PionPixelHitLayer);
    tree1->Branch("ProtonPixelHitLayer",&ProtonPixelHitLayer);

    //tracks
    tree1->Branch("ntracks",&ntracks,"ntracks/I");
    //primary vertex
    tree1->Branch("PVOrder",&PVOrder,"PVOrder/I");
    tree1->Branch("nPV",&nPV,"nPV/I");
    tree1->Branch("PVx",&PVx,"PVx/D");
    tree1->Branch("PVy",&PVy,"PVy/D");
    tree1->Branch("PVz",&PVz,"PVz/D");
    tree1->Branch("PVerrx",&PVerrx,"PVerrx/D");
    tree1->Branch("PVerry",&PVerry,"PVerry/D");
    tree1->Branch("PVerrz",&PVerrz,"PVerrz/D");
    tree1->Branch("PVcxy", &PVcxy, "PVcxy/D");
    tree1->Branch("PVcxz", &PVcxz, "PVcxz/D");
    tree1->Branch("PVcyz", &PVcyz, "PVcyz/D");
    tree1->Branch("BSx",&BSx,"BSx/D");
    tree1->Branch("BSy",&BSy,"BSy/D");
    tree1->Branch("BSz",&BSz,"BSz/D");
    tree1->Branch("BSerrx",&BSerrx,"BSerrx/D");
    tree1->Branch("BSerry",&BSerry,"BSerry/D");
    tree1->Branch("BSerrz",&BSerrz,"BSerrz/D");

    //pion tracks vars
    tree1->Branch("TrkPi1pt",&TrkPi1pt);
    tree1->Branch("TrkProtonpt",&TrkProtonpt);
    tree1->Branch("TrkPi1eta",&TrkPi1eta);
    tree1->Branch("TrkProtoneta",&TrkProtoneta);
    tree1->Branch("TrkPi1mass",&TrkPi1mass);
    tree1->Branch("TrkProtonmass",&TrkProtonmass);
    tree1->Branch("TrkPi1phi",&TrkPi1phi);
    tree1->Branch("TrkProtonphi",&TrkProtonphi);
    tree1->Branch("TrkPi1chi2",&TrkPi1chi2);
    tree1->Branch("TrkProtonchi2",&TrkProtonchi2);
    tree1->Branch("TrkPi1ndof",&TrkPi1ndof);
    tree1->Branch("TrkProtonndof",&TrkProtonndof);
    tree1->Branch("TrkPi1dxy",&TrkPi1dxy);
    tree1->Branch("TrkProtondxy",&TrkProtondxy);
    tree1->Branch("TrkPi1dz",&TrkPi1dz);
    tree1->Branch("TrkProtondz",&TrkProtondz);
    tree1->Branch("PionProtondz",&PionProtondz);
    //tree1->Branch("KpiTrkpipt",&KpiTrkpipt);
    //tree1->Branch("KpiTrkSpt",&KpiTrkSpt);
    tree1->Branch("LambdaPt",&LambdaPtKpi);
    tree1->Branch("MCLambdaDeltaR",&MCLambdaDeltaR);
    //tree1->Branch("DSPtKpi",&DSPtKpi);
    //tree1->Branch("KpiDSDeltaR",&KpiDSDeltaR);
    //tree1->Branch("KpiTrkKnhits",&KpiTrkKnhits);
    //tree1->Branch("KpiTrkpinhits",&KpiTrkpinhits);
    //tree1->Branch("KpiTrkSnhits",&KpiTrkSnhits);
    //tree1->Branch("KpiTrkKchi2",&KpiTrkKchi2);
    //tree1->Branch("KpiTrkpichi2",&KpiTrkpichi2);
    //tree1->Branch("KpiTrkSchi2",&KpiTrkSchi2);
    //tree1->Branch("KpiTrkKdxy",&KpiTrkKdxy);
    //tree1->Branch("KpiTrkpidxy",&KpiTrkpidxy);
    //tree1->Branch("KpiTrkSdxy",&KpiTrkSdxy);
    //tree1->Branch("KpiTrkSdxyErr",&KpiTrkSdxyErr);
    //tree1->Branch("KpiTrkKdz",&KpiTrkKdz);
    //tree1->Branch("KpiTrkpidz",&KpiTrkpidz);
    //tree1->Branch("KpiTrkSdz",&KpiTrkSdz);
    //tree1->Branch("KpiTrkSdzErr",&KpiTrkSdzErr);
    //tree1->Branch("KpiTrkKeta",&KpiTrkKeta);
    //tree1->Branch("KpiTrkpieta",&KpiTrkpieta);
    //tree1->Branch("KpiTrkSeta",&KpiTrkSeta);
    //tree1->Branch("KpiTrkKphi",&KpiTrkKphi);
    //tree1->Branch("KpiTrkpiphi",&KpiTrkpiphi);
    //tree1->Branch("KpiTrkSphi",&KpiTrkSphi);
    //tree1->Branch("KpiTrkScharge",&KpiTrkScharge);
    //MC
    //tree1->Branch("MCLambdaDeltaR",&MCLambdaDeltaR);

    //Gen quantities
    tree1->Branch("GenLambdaVtxPosx",&GenLambdaVtxPosx);
    tree1->Branch("GenLambdaVtxPosy",&GenLambdaVtxPosy);
    tree1->Branch("GenLambdaVtxPosz",&GenLambdaVtxPosz);
    tree1->Branch("GenLambdaSourceVtxPosx",&GenLambdaSourceVtxPosx);
    tree1->Branch("GenLambdaSourceVtxPosy",&GenLambdaSourceVtxPosy);
    tree1->Branch("GenLambdaSourceVtxPosz",&GenLambdaSourceVtxPosz);
    tree1->Branch("GenLambdaPt",&GenLambdaPt);
    tree1->Branch("GenLambdaP",&GenLambdaP);
    tree1->Branch("GenLambdaPhi",&GenLambdaPhi);
    tree1->Branch("GenLambdaEta",&GenLambdaEta);
    tree1->Branch("GenLambdaMass",&GenLambdaMass);
    tree1->Branch("GenLambdaMt",&GenLambdaMt);
    tree1->Branch("GenLambdaE",&GenLambdaE);
    tree1->Branch("GenLambdaPx",&GenLambdaPx);
    tree1->Branch("GenLambdaPy",&GenLambdaPy);
    tree1->Branch("GenLambdaPz",&GenLambdaPz);
    tree1->Branch("GenFlightLength",&GenFlightLength);
    tree1->Branch("GenDeltaR",&GenDeltaR);
    tree1->Branch("GenLambdaDeltaR",&GenLambdaDeltaR);

    tree1->Branch("GenVertexMatch",&GenVertexMatch);

    //Gen Protons and Pions
    tree1->Branch("GenProtonPt",&GenProtonPt);
    tree1->Branch("GenProtonP",&GenProtonP);
    tree1->Branch("GenProtonPhi",&GenProtonPhi);
    tree1->Branch("GenProtonEta",&GenProtonEta);
    tree1->Branch("GenProtonMass",&GenProtonMass);
    tree1->Branch("GenProtonMt",&GenProtonMt);
    tree1->Branch("GenProtonE",&GenProtonE);
    tree1->Branch("GenProtonPx",&GenProtonPx);
    tree1->Branch("GenProtonPy",&GenProtonPy);
    tree1->Branch("GenProtonPz",&GenProtonPz);
    tree1->Branch("GenProtonDeltaR",&GenProtonDeltaR);

    tree1->Branch("GenPionPt",&GenPionPt);
    tree1->Branch("GenPionP",&GenPionP);
    tree1->Branch("GenPionPhi",&GenPionPhi);
    tree1->Branch("GenPionEta",&GenPionEta);
    tree1->Branch("GenPionMass",&GenPionMass);
    tree1->Branch("GenPionMt",&GenPionMt);
    tree1->Branch("GenPionE",&GenPionE);
    tree1->Branch("GenPionPx",&GenPionPx);
    tree1->Branch("GenPionPy",&GenPionPy);
    tree1->Branch("GenPionPz",&GenPionPz);
    tree1->Branch("GenPionDeltaR",&GenPionDeltaR);

    //Hit Matching
    tree1->Branch("nSimTracksshared",&nSimTracksshared);
    tree1->Branch("nSimTrackspion",&nSimTrackspion);
    tree1->Branch("nSimTracksproton",&nSimTracksproton);
    tree1->Branch("nUniqueSimTracksInSharedHit",&nUniqueSimTracksInSharedHit);
    tree1->Branch("nUniqueSimTracksInPionHit",&nUniqueSimTracksInPionHit);
    tree1->Branch("nUniqueSimTracksInProtonHit",&nUniqueSimTracksInProtonHit);
    tree1->Branch("sharedHitContainsGenLambda",&sharedHitContainsGenLambda);
    tree1->Branch("sharedHitContainsGenPion",&sharedHitContainsGenPion);
    tree1->Branch("sharedHitContainsGenProton",&sharedHitContainsGenProton);
    tree1->Branch("uniqueSharedSimTrackPDGIds",&uniqueSharedSimTrackPDGIds);
    tree1->Branch("uniquePionSimTrackPDGIds",&uniquePionSimTrackPDGIds);
    tree1->Branch("uniqueProtonSimTrackPDGIds",&uniqueProtonSimTrackPDGIds);
    tree1->Branch("uniqueSharedSimTrackIds",&uniqueSharedSimTrackIds);
    tree1->Branch("uniquePionSimTrackIds",&uniquePionSimTrackIds);
    tree1->Branch("uniqueProtonSimTrackIds",&uniqueProtonSimTrackIds);

}

// ------------ method called once each job just after ending the event loop  ------------
void LambdaAnalyzer::endJob() {
}


int LambdaAnalyzer::nSharedPixelLayerHits(reco::TransientTrack *track1, reco::TransientTrack *track2) {
    //std::cout<<"called nSharedPixelLayerHits"<<std::endl;
    int nShared = 0;
    trackingRecHit_iterator hb_t1 = track1->recHitsBegin();
    trackingRecHit_iterator hb_t2 = track2->recHitsBegin();
    TrackingRecHit const * h1[7] = { (*hb_t1), (*(hb_t1+1)), (*(hb_t1+2)), (*(hb_t1+3)),  (*(hb_t1+4)),  (*(hb_t1+5)),  (*(hb_t1+6)),   };
    TrackingRecHit const * h2[7] = { (*hb_t2), (*(hb_t2+1)), (*(hb_t2+2)), (*(hb_t2+3)),  (*(hb_t2+4)),  (*(hb_t2+5)),  (*(hb_t2+6)),   };
    for (int k1=0; k1<7; k1++) {
        if (!h1[k1]->isValid()) continue;
        //std::cout<<"track2 # recHits = "<<track2->recHitsSize()<<std::endl;
        for (int k2=0; k2<7; k2++) {
            if (!h2[k2]->isValid()) continue;
            //if (k1==0) {
            //PXBDetId tmp = h2[k2]->geographicalId();
            //if (tmp.subdetId()== 1) {
            //std::cout<<"k2 = "<<k2<<std::endl;
            //std::cout<<tmp.det()<<std::endl;
            //std::cout<<tmp.subdetId()<<std::endl;
            //std::cout<<"track 2 valid hit at layer: "<<tmp.layer()<< " : "<<tmp.ladder()<<" : "<<tmp.module()<<std::endl;}
            //if (k2==6) {std::cout<<"done"<<std::endl;}
            //}
            bool shared = h1[k1]->sharesInput(h2[k2],TrackingRecHit::some);
            if (shared) {
                nShared += 1;
            }
        }
    }
    //if (nShared>=3) {
    //    std::cout<<"nShared >= 3"<<std::endl;
    //    if (h2[0]->isValid()) {
    //        PXBDetId pxb_id = h2[0]->geographicalId();
    //        std::cout<<"layer = "<<pxb_id.layer()<<std::endl;
    //   }
    //}
    return nShared;   
}
void LambdaAnalyzer::getSimTracksFromPixelCluster(std::vector<SiPixelCluster::Pixel> cluster, const TrackingRecHit* hit, std::vector<unsigned int> & uniqueSimTrackIds, std::vector<int> & simTrackPDGIds){
    for(auto pixel : cluster){
        auto firstLink = pixelLinks->find(hit->rawId());
        //std::cout << "pixelLink matches for shared hit: " << std::count(pixelLinks->begin(),pixelLinks->end(),cluster->rawId()) << std::endl;
        if(firstLink != pixelLinks->end()){
            auto link_detset = (*firstLink);
            for(auto linkiter : link_detset.data){
                std::pair<int,int> pos = PixelDigi::channelToPixel(linkiter.channel());
                if(pos.first == pixel.x && pos.second == pixel.y){
                    for(TrackingParticleCollection::const_iterator iter = tPC->begin(); iter != tPC->end(); ++iter){
                        if( (iter->g4Tracks()).front().trackId() == linkiter.SimTrackId()){
                            bool isNew = true;
                            for(auto iD : uniqueSimTrackIds){
                                if(iD==(unsigned int)(iter->g4Tracks()).front().trackId()) isNew=false;
                            }
                            if(isNew){
                                uniqueSimTrackIds.push_back((unsigned int)(iter->g4Tracks()).front().trackId());
                                std::cout << "SimTrack pdgId: " << iter->pdgId() << std::endl;
                                simTrackPDGIds.push_back((signed int) iter->pdgId());
                            }
                        }
                    }
                }
            }
        } 

    }
}

float LambdaAnalyzer::DeepClusterEvaluate(std::vector<SiPixelCluster::Pixel> pixels){
    float pixelImage[20][20];
    float reduced_adc_tot = LambdaAnalyzer::MakeClusterImage(pixels,pixelImage); 
    tensorflow::GraphDef* graphDef = tensorflow::loadGraphDef("../ml/DeepCluster.pb");
    tensorflow::Session* session = tensorflow::createSession(graphDef);

    tensorflow::Tensor input_1(tensorflow::DT_FLOAT, {1,20,20,1});
    input_1.flat<float>().setZero();
    //input_1.tensor<float, 4>()(0,10,10,0) = 1.0;
    for(int x=0;x<20;x++){
        for(int y=0;y<20;y++){
            input_1.tensor<float, 4>()(0,x,y,0) = pixelImage[x][y];
        }
    }

    tensorflow::Tensor input_2(tensorflow::DT_FLOAT, {1,1});
    input_2.matrix<float>()(0,0)=reduced_adc_tot;

    const std::vector<std::string> outputNames = {"Output/Softmax:0"};
    std::vector<tensorflow::Tensor> outputs;
    
    tensorflow::run(session, { {"input_1:0",input_1},{"input_2:0",input_2} },outputNames, &outputs);
    return outputs[0].matrix<float>()(0,0);
}
float LambdaAnalyzer::MakeClusterImage(std::vector<SiPixelCluster::Pixel> pixels,float (& image)[20][20]){
    for(int x=0;x<20;x++){
        for(int y=0;y<20;y++){
            image[x][y]=0.0;
        }
    }

    int adc_tot = 0;
    int x_centroid = 0;
    int y_centroid = 0;

    for (unsigned int k=0; k<pixels.size(); k++) {
        SiPixelCluster::Pixel pixel = pixels[k];
        x_centroid += pixel.adc*pixel.x;
        y_centroid += pixel.adc*pixel.y;
        adc_tot += pixel.adc;
    }
    x_centroid = (int) std::round((float) x_centroid/adc_tot );
    y_centroid = (int) std::round((float) y_centroid/adc_tot );

    //Centroid is at (10,10), fill in pixels relative to this with normalized charge
    //std::cout << "pixels:" << std::endl;
    for (unsigned int k=0; k<pixels.size(); k++) {
        SiPixelCluster::Pixel pixel = pixels[k];

        int x_rel = pixel.x-x_centroid+10;
        int y_rel = pixel.y-y_centroid+10;
        std::cout << x_rel << " " << y_rel << " " << pixel.adc << std::endl;
        if( x_rel>=0 && x_rel<20 && y_rel>=0 && y_rel< 20 && pixel.adc>1e-8){
            //std::cout << "FILLING PIXEL " << x_rel << "," << y_rel << std::endl;
            image[y_rel][x_rel] = (float) pixel.adc/adc_tot;
        }
    }
    return (float) adc_tot/1.0e4;
}
//void LambdaAnalyzer::getFromEvt(edm::Event& theEvent,edm::Handle<TrackCollection>& theTCollection) {
//    theEvent.getByLabel("generalTracks",theTCollection ); 
//}

//define this as a plug-in
DEFINE_FWK_MODULE(LambdaAnalyzer);
