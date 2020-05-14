#include "StGeant4Maker.h"
//________________________________________________________________________________________________
#include "StMessMgr.h"
//________________________________________________________________________________________________
#include "StarMagField.h"
#include "StMCParticleStack.h"
#include "TSystem.h"
#include "StBFChain.h"
#include "TInterpreter.h"
#include "TGeoManager.h"
//#include "StarVMC/StarAgmlLib/AgModule.h"
#include "StarVMC/StarAgmlLib/StarAgmlStacker.h"
//#include "StDetectorDbMaker/St_MagFactorC.h"  
#include "StarGenerator/BASE/StarPrimaryMaker.h"
#include "StarGenerator/BASE/StarParticleStack.h"
#include "StarGenerator/UTIL/StarParticleData.h"
#include "TString.h"

#include "StMCTruthTable.h"
#include "StSensitiveDetector.h"

#include "StarVMC/StarAgmlLib/AgMLExtension.h"

#include "TString.h"


//_______________________________________________________________________________________________
#include <AgMLVolumeIdFactory.h>
//_______________________________________________________________________________________________



//#include "StVMCStepManager.h"
//________________________________________________________________________________________________
//#include <G4SDManager.hh>
//#include <G4RunManager.hh>
//#include <G4LogicalVolume.hh>
//#include <G4LogicalVolumeStore.hh>
//#include <SystemOfUnits.h>
//#include <StGeant4Maker/SCORING/GenTrackerSensitiveDetector.h>
//#include <StGeant4Maker/StSensitiveDetectorFactory.h>
//________________________________________________________________________________________________
#include "TGeant4.h"
#include "TG4RunManager.h"
#include "TG4RunConfiguration.h"
//#include "TG4TrackManager.h"
//#include "TG4SDManager.h"
//#include "TG4SDConstruction.h"
//#include "TG4StepStatus.h"
//#include "G4Step.hh"
//#include "G4Track.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4StepPoint.hh"
//________________________________________________________________________________________________
//#include <StGeant4Maker/SCORING/StTrackerSD.h>
//________________________________________________________________________________________________
//#include "StMcHitFiller.h"
//________________________________________________________________________________________________
#include "tables/St_g2t_vertex_Table.h"
#include "tables/St_g2t_track_Table.h"
//________________________________________________________________________________________________
#include "g2t/St_g2t_tpc_Module.h"
#include "g2t/St_g2t_hca_Module.h"
#include "g2t/St_g2t_wca_Module.h"
#include "g2t/St_g2t_pre_Module.h"
#include "g2t/St_g2t_fts_Module.h"
#include "g2t/St_g2t_stg_Module.h"
#include "g2t/St_g2t_epd_Module.h"
//________________________________________________________________________________________________
#include <StHitCollection.h> 
//________________________________________________________________________________________________

// Functors used to copy the hits from the sensitive detector hit collections into the g2t tables.
// There's an explitive-load of boilerplate in these things

struct SD2Table_TPC {
  void operator()( StSensitiveDetector* sd, St_g2t_tpc_hit* table, St_g2t_track* track ) {
    // Retrieve the hit collection 
    StTrackerHitCollection* collection = (StTrackerHitCollection *)sd->hits();
    // Iterate over all hits
    for ( auto hit : collection->hits() ) {
      
      g2t_tpc_hit_st g2t_hit; memset(&g2t_hit,0,sizeof(g2t_tpc_hit_st)); 
      
      g2t_hit.id        = hit->id;
      // TODO: add pointer to next hit on the track 
      g2t_hit.track_p   = hit->idtruth;
      g2t_hit.volume_id = hit->volId;
      g2t_hit.de        = hit->de;
      g2t_hit.ds        = hit->ds;
      for ( int i=0; i<3; i++ ) {
	g2t_hit.p[i]  = 0.5 * ( hit->momentum_in[i] + hit->momentum_out[i] );
	g2t_hit.x[i]  = 0.5 * ( hit->position_in[i] + hit->position_out[i] );
      }
      g2t_hit.tof       = 0.5 * ( hit->position_in[3] + hit->position_out[3] ); 
      g2t_hit.length    = hit->length;
      /*
      g2t_hit.lgam = ...;

      // these are used downstream by the slow simulator
      g2t_hit.adc = ...;
      g2t_hit.pad = ...;
      g2t_hit.timebucket = ...;
      g2t_hit.np = ...; // number of primary electrons

      */
      
      table -> AddAt( &g2t_hit );     

      int idtruth = hit->idtruth;
      g2t_track_st* trk = (g2t_track_st*)track->At(idtruth-1);
      trk->n_tpc_hit++;
      
    }
    // TODO: increment hit count on track 
  } 
} sd2table_tpc; 

struct SD2Table_EPD {
  void operator()( StSensitiveDetector* sd, St_g2t_epd_hit* table, St_g2t_track* track ) {
    // Retrieve the hit collection 
    StTrackerHitCollection* collection = (StTrackerHitCollection *)sd->hits();
    // Iterate over all hits
    for ( auto hit : collection->hits() ) {
      
      g2t_epd_hit_st g2t_hit; memset(&g2t_hit,0,sizeof(g2t_epd_hit_st)); 
      
      g2t_hit.id        = hit->id;
      // TODO: add pointer to next hit on the track 
      g2t_hit.track_p   = hit->idtruth;
      g2t_hit.volume_id = hit->volId;
      g2t_hit.de        = hit->de;
      g2t_hit.ds        = hit->ds;
      for ( int i=0; i<3; i++ ) {
	g2t_hit.p[i]  = 0.5 * ( hit->momentum_in[i] + hit->momentum_out[i] );
	g2t_hit.x[i]  = 0.5 * ( hit->position_in[i] + hit->position_out[i] );
      }
      g2t_hit.tof       = 0.5 * ( hit->position_in[3] + hit->position_out[3] ); 
      
      table -> AddAt( &g2t_hit );     

      int idtruth = hit->idtruth;
      g2t_track_st* trk = (g2t_track_st*)track->At(idtruth-1);
      //      trk->n_epd_hit++;
      
    }

  } 
} sd2table_epd; 

// Copy to sTGC and FST structures
struct SD2Table_STGC {
  void operator()( StSensitiveDetector* sd, St_g2t_fts_hit* table, St_g2t_track* track ) {
    // Retrieve the hit collection 
    StTrackerHitCollection* collection = (StTrackerHitCollection *)sd->hits();
    // Iterate over all hits
    for ( auto hit : collection->hits() ) {
      
      g2t_fts_hit_st g2t_hit; memset(&g2t_hit,0,sizeof(g2t_fts_hit_st)); 
      
      g2t_hit.id        = hit->id;
      // TODO: add pointer to next hit on the track 
      g2t_hit.track_p   = hit->idtruth;
      g2t_hit.volume_id = hit->volId;
      g2t_hit.de        = hit->de;
      g2t_hit.ds        = hit->ds;
      for ( int i=0; i<3; i++ ) {
	g2t_hit.p[i]  = 0.5 * ( hit->momentum_in[i] + hit->momentum_out[i] );
	g2t_hit.x[i]  = 0.5 * ( hit->position_in[i] + hit->position_out[i] );
      }
      g2t_hit.tof       = 0.5 * ( hit->position_in[3] + hit->position_out[3] ); 
      
      table -> AddAt( &g2t_hit );     

      int idtruth = hit->idtruth;
      g2t_track_st* trk = (g2t_track_st*)track->At(idtruth-1);
      trk->n_stg_hit++;
      
    }
  } 
} sd2table_stgc; 

struct SD2Table_FST {
  void operator()( StSensitiveDetector* sd, St_g2t_fts_hit* table, St_g2t_track* track ) {
    // Retrieve the hit collection 
    StTrackerHitCollection* collection = (StTrackerHitCollection *)sd->hits();
    // Iterate over all hits
    for ( auto hit : collection->hits() ) {
      
      g2t_fts_hit_st g2t_hit; memset(&g2t_hit,0,sizeof(g2t_fts_hit_st)); 
      
      g2t_hit.id        = hit->id;
      // TODO: add pointer to next hit on the track 
      g2t_hit.track_p   = hit->idtruth;
      g2t_hit.volume_id = hit->volId;
      g2t_hit.de        = hit->de;
      g2t_hit.ds        = hit->ds;
      for ( int i=0; i<3; i++ ) {
	g2t_hit.p[i]  = 0.5 * ( hit->momentum_in[i] + hit->momentum_out[i] );
	g2t_hit.x[i]  = 0.5 * ( hit->position_in[i] + hit->position_out[i] );
      }
      g2t_hit.tof       = 0.5 * ( hit->position_in[3] + hit->position_out[3] ); 
      
      table -> AddAt( &g2t_hit );     

      int idtruth = hit->idtruth;
      g2t_track_st* trk = (g2t_track_st*)track->At(idtruth-1);
      trk->n_fts_hit++;

    }
  } 
} sd2table_fst; 


// Generic EMC copy (no increment on track hits)
struct SD2Table_EMC {
  void operator()( StSensitiveDetector* sd, St_g2t_emc_hit* table, St_g2t_track* track ) {
    // Retrieve the hit collection 
    StCalorimeterHitCollection* collection = (StCalorimeterHitCollection *)sd->hits();
    // Iterate over all hits
    for ( auto hit : collection->hits() ) {
      
      g2t_emc_hit_st g2t_hit; memset(&g2t_hit,0,sizeof(g2t_emc_hit_st)); 
      
      g2t_hit.id        = hit->id;
      // TODO: add pointer to next hit on the track 
      g2t_hit.track_p   = hit->idtruth;
      g2t_hit.volume_id = hit->volId;
      g2t_hit.de        = hit->de;
      g2t_hit.x         = hit->position_in[0];
      g2t_hit.y         = hit->position_in[1];
      g2t_hit.z         = hit->position_in[2];
      
      table -> AddAt( &g2t_hit );     

      int idtruth = hit->idtruth;
      g2t_track_st* trk = (g2t_track_st*)track->At(idtruth-1);
      //      trk->n_fts_hit++;

    }
  } 
} sd2table_emc; 

//________________________________________________________________________________________________
TGeant4* gG4 = 0;
//________________________________________________________________________________________________
// Pointer to the maker so we can forward VMC calls there
static StGeant4Maker* _g4maker = 0;
//________________________________________________________________________________________________
StarParticleData &particleData = StarParticleData::instance();
//________________________________________________________________________________________________
StarVMCApplication::StarVMCApplication( const Char_t *name, const Char_t *title ) : TVirtualMCApplication(name,title) {

}
//________________________________________________________________________________________________
StGeant4Maker::StGeant4Maker( const char* nm ) : 
  StMaker(nm),
  mVmcApplication ( NULL ),
  mGeomPath       ( "./StarDb/AgMLGeometry:$STAR/StarDb/AgMLGeometry" ),
  mStarField      ( NULL ),
  mMCStack        ( new StMCParticleStack( "MCstack" ) ),
  mMagfield       ( NULL ),
  mRunConfig      ( NULL ),
  mTruthTable     ( new StMCTruthTable() ),
  mCurrentNode    (0),
  mPreviousNode   (0),
  mCurrentVolume  (0),
  mPreviousVolume (0),
  mCurrentTrackingRegion(2),
  mPreviousTrackingRegion(2),
  acurr(0),aprev(0)
{ 

  // Setup default attributes
  //  SetAttr( "G4Opt:Nav",   "geomRoot" );// Default uses only possibility for us... ROOT geom with ROOT nav.  No VGM to convert to G4 geometry.
  //  Possibilities geomVMCtoGeant4 geomVMCtoRoot geomRoot geomRootToGeant4 geomGeant4
  SetAttr( "G4VmcOpt:Nav",   "geomVMCtoRoot" );
  SetAttr( "G4VmcOpt:Name",  "Geant4"  );
  SetAttr( "G4VmcOpt:Title", "The Geant4 Monte Carlo" );
  SetAttr( "G4VmcOpt:Phys",  "FTFP_BERT" ); // default physics list
  SetAttr( "G4VmcOpt:Process", "stepLimit+specialCuts" ); // special process

  SetAttr( "AgMLOpt:TopVolume", "HALL" );

  SetAttr( "Stepping:Punchout:Stop", 1 ); // 0=no action, 1=track stopped, 2=track stopped and re-injected            

  SetAttr( "Random:G4", 0); // 1=allows G4 to use its own RNG 


  SetAttr( "field", -5.0 );


  // Setup default cuts
  SetAttr("CUTGAM", 0.001);
  SetAttr("CUTELE", 0.001);
  SetAttr("CUTHAD", 0.01);
  SetAttr("CUTNEU", 0.01);
  SetAttr("CUTMUO", 0.01);
  SetAttr("BCUTE",  /*     R 'Cut for electron brems.'     D=*/     -1.);
  SetAttr("BCUTM",  /*     R 'Cut for muon brems.'         D=-1.*/  -1.);
  SetAttr("DCUTE",  /*     R 'Cut for electron delta-rays' D=*/     -1.);
  SetAttr("DCUTM",  /*     R 'Cut for muon delta-rays'     D=-1.*/  -1.);
  SetAttr("PPCUTM", /*     R 'Cut for e+e- pairs by muons' D=0.01*/ 0.01);
  SetAttr("TOFMAX", /*     R 'Time of flight cut'          D=*/     1.E+10);

  
  // TODO-- 
  //  SetAttr( "AgMLOpt:Hits:Deactivate", "ECAL:*,TPCE:*,*" );
  //  SetAttr( "AgMLOpt:Hits:Activate", "TPAD,CSUP,ESCI" );

  _g4maker = this; // Provide a global pointer to the G4 maker  

}
//________________________________________________________________________________________________
int StGeant4Maker::Init() {

  LOG_INFO << "Initialize geometry" << endm;
  InitGeom();

  LOG_INFO << "Create VMC application" << endm;
  mVmcApplication = new StarVMCApplication();

  LOG_INFO << "Create VMC run configuration" << endm;
  mRunConfig = new TG4RunConfiguration( SAttr("G4VmcOpt:Nav"), SAttr("G4VmcOpt:Phys" ), SAttr("G4VmcOpt:Process") );

  AddObj( mVmcApplication, ".const", 0 ); // Register VMC application  

  LOG_INFO << "Create GEANT4 instance" << endm;
  AddObj( gG4 = new TGeant4(SAttr("G4VmcOpt:Name"), SAttr("G4VmcOpt:Title") ,mRunConfig) , ".const", 0 );
  //gMC = gG4;

  //  for ( TString cut : { "CUTGAM", "CUTELE", "CUTHAD", "CUTNEU", "CUTMUO", "BCUTE", "DCUTE", "BCUTM","DCUTM" } ) {
  //    gG4->SetCut( cut, DAttr(cut) );
    //  }
    gG4->SetCut( "CUTGAM", DAttr("cutgam") );
    gG4->SetCut( "CUTELE", DAttr("cutele") );
    gG4->SetCut( "CUTHAD", DAttr("cuthad") );
    gG4->SetCut( "CUTNEU", DAttr("cutneu") );
    gG4->SetCut( "CUTMUO", DAttr("cutmuo") );
    gG4->SetCut( "BCUTE" , DAttr("bcute") );
    gG4->SetCut( "DCUTE" , DAttr("dcute") );
    gG4->SetCut( "BCUTM" , DAttr("bcutm") );
    gG4->SetCut( "DCUTM" , DAttr("dcutm") );


  LOG_INFO << "Create StarMagFieldAdaprtor" << endm;
  mMagfield = new StarMagFieldAdaptor(/*nada*/);

  LOG_INFO << "Pass stack and magnetic field to G4, flag ROOT geometry" << endm;
  if ( gG4 ) {
    gG4->SetStack( mMCStack );  
    gG4->SetMagField( mMagfield );
    gG4 -> SetRootGeometry();
  } else {
    LOG_FATAL << "Could not instantiate concrete MC.  WTF?" << endm;
    return kStFATAL;
  };

  // Obtain the G4 run manager
  TG4RunManager* runManager = TG4RunManager::Instance();
  if ( 0==IAttr("Random:G4") ) {
    LOG_INFO << "Map G4 random number generator to ROOT" << endm;
    runManager->UseRootRandom(true);
  }



  LOG_INFO << "Initialize GEANT4" << endm;
  gG4 -> Init(); // FinishGeometry is called here...

  // VMC SD manager appears to be the last thing initialized when gMC->Init()
  // is called... so we should initialize our hits here...

  LOG_INFO << "Initialize GEANT4 Physics" << endm;
  gG4 -> BuildPhysics();

  return StMaker::Init();
}
//________________________________________________________________________________________________
int StGeant4Maker::InitRun( int /* run */ ){

  // Get magnetic field scale
  double field = DAttr("field"); /* kG */ 

  if ( 0 == StarMagField::Instance() ) new StarMagField( StarMagField::kMapped, field / 5.0 );

  if ( 0.0 == field ) { 
    // field = St_MagFactorC::instance()->ScaleFactor();
    // if ( TMath::Abs(field)<1E-3 ) field = 1E-3;  
    StarMagField::Instance()->SetFactor(field);
  }

  return kStOK;
}
//________________________________________________________________________________________________
void StarVMCApplication::ConstructGeometry(){ _g4maker -> InitGeom(); }
int  StGeant4Maker::InitGeom() {
  if ( gGeoManager ) {
    LOG_INFO << "Running with existing geometry manager" << endm;
    return kStOK;
  }

  const DbAlias_t *DbAlias = GetDbAliases();
  for (int i = 0; DbAlias[i].tag; i++) // iterate over DB aliases
    {
      StBFChain *bfc = dynamic_cast<StBFChain *>(GetTopChain());
      if ( 0==bfc ) break; // nothing to do in this case...

      //
      // Look for BFC option of form y2019x or ry2019x
      //
      TString  rtag = "r"; rtag += DbAlias[i].tag;
      TString   tag =              DbAlias[i].tag;
      if ( 0==bfc->GetOption(rtag.Data()) && 0==bfc->GetOption(tag.Data()) ) continue;
      //
      // Get the geometry generation macro
      //
      TString   geo = "Geometry."; geo += DbAlias[i].tag; geo += ".C";
      Char_t   *mac = gSystem->Which( mGeomPath.Data(), geo, kReadPermission );
      if ( 0==mac ) continue;
      //
      // Load and execute
      //
      LOG_INFO << "Load geometry file " << mac << endm;
      gInterpreter->ProcessLine( Form( ".L %s", mac ) );
      gInterpreter->ProcessLine( "CreateTable();" );
      gInterpreter->ProcessLine( Form( ".U %s", mac ) );
      //
      // AgML should have initalized the geometry data structure.  Add it as a const.
      //
      //      AddConst( AgModule::Geom() );
      //
      // Cleanup file
      // 
      if (mac) delete [] mac;
      goto GEOMETRY;
    }

  LOG_WARN << "Geometry no initialzied.  This should be quick." << endm;
  assert(0);
  return kStWarn; // we have no geometry here

 GEOMETRY:

  LOG_INFO << "Geometry constructed." << endm;
  TGeoVolume* top = gGeoManager->FindVolumeFast( SAttr("AgMLOpt:TopVolume") );
  gGeoManager->SetTopVolume( top );

  return kStOK;
}
//________________________________________________________________________________________________
int StGeant4Maker::InitHits() {
  return kStOK;
}
//________________________________________________________________________________________________
struct A { };
struct B { };
int StGeant4Maker::Make() {

  // Process one single event.  Control handed off to VMC application.
  gG4 -> ProcessRun( 1 );


  return kStOK; 
}
//________________________________________________________________________________________________
void StGeant4Maker::Clear( const Option_t* opts ){
  // Clear the MC stack
  LOG_INFO << "Clear" << endm;
  mMCStack -> Clear();
  StMaker::Clear();
}
//________________________________________________________________________________________________
void StarVMCApplication::InitGeometry(){ 

  _g4maker -> ConfigureGeometry(); 
  //  _g4maker -> InitHits();

}
//________________________________________________________________________________________________
void StarVMCApplication::ConstructSensitiveDetectors() {

  assert(gGeoManager);
  LOG_INFO << "Create and register sensitive detectors" << endm;

  // First collect all AgML extensions with sensitive volumes
  // by the family name of the volume
  std::map<TString, StSensitiveDetector*> sdmap;

  // Get list of volumes
  TObjArray *volumes = gGeoManager->GetListOfVolumes();

  // Get list of extensions mapped to volume 
  for ( int i=0; i<volumes->GetEntries(); i++ ) {

    TGeoVolume* volume = (TGeoVolume *)volumes->At(i);
    AgMLExtension* ae = (AgMLExtension *)volume->GetUserExtension();
    if ( 0==ae ) {
      LOG_DEBUG << "No agml extension on volume = " << volume->GetName() << endm;
      continue; // shouldn't happen
    }
    if ( 0==ae->GetSensitive() ) {
      LOG_DEBUG << "Not sensitive = " << volume->GetName() << endm;
      continue; 
    }

    // Name of the volume
    TString vname=volume->GetName();
    TString fname=ae->GetFamilyName();
    TString mname=ae->GetModuleName();

    
    AgMLVolumeId* identifier = AgMLVolumeIdFactory::Create( fname );
    if ( identifier ) {
      LOG_INFO << "Setting volume identifier for " << fname.Data() << " " << vname.Data() << endm;
      ae -> SetVolumeIdentifier( identifier );
    }


    //
    // Get the sensitive detector.  If we don't have one registered
    // to this family, create one and register it
    //
    StSensitiveDetector* sd = sdmap[fname];
    if ( 0==sd ) {
      // add sensitive detector to local map
      sd = sdmap[fname] = new StSensitiveDetector( fname, mname );
    }

    // Register this volume to the sensitive detector
    LOG_INFO << vname.Data() << "/" << fname.Data() << " --> " << sd->GetName() << endm;
    TVirtualMC::GetMC()->SetSensitiveDetector( vname, sd );
    
    // Register this volume with the sensitive detector
    sd->addVolume( volume );

  }




}
//________________________________________________________________________________________________
int  StGeant4Maker::ConfigureGeometry() {
  return kStOK;
}
//________________________________________________________________________________________________
void StarVMCApplication::BeginEvent(){ _g4maker->BeginEvent(); }
void StGeant4Maker::BeginEvent(){

  mTruthTable->BeginEvent();

}
//________________________________________________________________________________________________
void StarVMCApplication::FinishEvent(){ _g4maker -> FinishEvent(); }
void StGeant4Maker::FinishEvent(){

  mTruthTable->FinishEvent();

  LOG_INFO << "End of Event" << endm;

  StMCParticleStack* stack    = (StMCParticleStack *)TVirtualMC::GetMC()->GetStack();
  auto&              vertex   = stack->GetVertexTable();
  auto&              particle = stack->GetParticleTable();
  unsigned int nvertex = vertex.size();
  unsigned int ntrack  = particle.size();

  LOG_INFO << " nvertex=" << nvertex << endm;
  LOG_INFO << " ntrack =" << ntrack << endm;

  St_g2t_vertex* g2t_vertex = new St_g2t_vertex("g2t_vertex",nvertex);  AddData(g2t_vertex);
  St_g2t_track*  g2t_track  = new St_g2t_track ("g2t_track", ntrack);   AddData(g2t_track);

  // Add tracks and vertices to the data structures...

  std::map<const StarMCParticle*,int> truthTrack;
  std::map<const StarMCVertex*,int>  truthVertex;

  // Map vertex and track ID truth to their pointers

  int ivertex = 1; // vertex numbering starts from 1
  for ( auto v : vertex ) {
    truthVertex[v] = ivertex;
    ivertex++;
  }

  int itrack = 1; // track numbering starts from 1
  for ( auto t : particle ) {
    truthTrack[t] = itrack;
    itrack++;
  }

  ivertex = 1;
  for ( auto v : vertex ) {

    // partial fill of vertex table ________________________
    g2t_vertex_st myvertex;   memset(&myvertex, 0, sizeof(g2t_vertex_st));    
    myvertex.id = ivertex;
    myvertex.eg_x[0] = myvertex.ge_x[0] = v->vx();
    myvertex.eg_x[1] = myvertex.ge_x[1] = v->vy();
    myvertex.eg_x[2] = myvertex.ge_x[2] = v->vz();
    myvertex.eg_tof  = myvertex.ge_tof  = v->tof();
    myvertex.n_daughter = v->daughters().size();
    if ( v->daughters().size() ) {
      myvertex.daughter_p = truthTrack[ v->daughters()[0] ];
    }
    if ( v->parent() ) {
      myvertex.n_parent = 1; // almost by definition
      myvertex.parent_p = truthTrack[ v->parent() ];
    }

    // TODO: map ROOT mechanism to G3 names
    
    //__________________________________________ next vertex
    g2t_vertex->AddAt( &myvertex );
    ivertex++;

  }

  itrack = 1; // track numbering starts from 1
  for ( auto t : particle ) {

    // partial fill of track table _______________________
    g2t_track_st mytrack;   memset(&mytrack, 0, sizeof(g2t_track_st));    
    mytrack.id       = itrack;
    mytrack.eg_pid   = t->GetPdg();
    mytrack.p[0]     = t->px();
    mytrack.p[1]     = t->py();
    mytrack.p[2]     = t->pz();
    mytrack.e        = t->E();
    mytrack.pt       = t->pt(); // NOTE: starsim secondaries have pt = -999
    mytrack.eta      = t->particle()->Eta();
    mytrack.rapidity = t->particle()->Y();
    // index of the start and stop vertices.
    // TODO: particle stop vertices need to be scored
    mytrack.start_vertex_p = truthVertex[ t->start() ];
    mytrack.stop_vertex_p  = truthVertex[ t->stop()  ];
    //__________________________________________ next track
    g2t_track->AddAt(&mytrack);
    itrack++;
  }
  
  // Copy hits to tables
  AddHits<St_g2t_tpc_hit>( "TPCH", {"TPAD"}, "g2t_tpc_hit", sd2table_tpc  );
  AddHits<St_g2t_epd_hit>( "EPDH", {"EPDT"}, "g2t_epd_hit", sd2table_epd  );
  AddHits<St_g2t_fts_hit>( "FSTH", {"FTUS"}, "g2t_fsi_hit", sd2table_fst  );
  AddHits<St_g2t_fts_hit>( "STGH", {"TGCG"}, "g2t_stg_hit", sd2table_stgc );
  AddHits<St_g2t_emc_hit>( "PREH", {"PSCI"}, "g2t_pre_hit", sd2table_emc  );
  AddHits<St_g2t_emc_hit>( "WCAH", {"WSCI"}, "g2t_wca_hit", sd2table_emc  );
  AddHits<St_g2t_emc_hit>( "HCAH", {"HSCI"}, "g2t_hca_hit", sd2table_emc  ); 

}
//________________________________________________________________________________________________
void StarVMCApplication::BeginPrimary(){ _g4maker -> BeginPrimary(); }
void StGeant4Maker::BeginPrimary()
{


}
//________________________________________________________________________________________________
void StarVMCApplication::FinishPrimary(){ _g4maker->FinishPrimary(); }
void StGeant4Maker::FinishPrimary()
{

}
//________________________________________________________________________________________________
void StarVMCApplication::PreTrack(){ _g4maker->PreTrack(); }
void StGeant4Maker::PreTrack()
{
  // Reset the history (tracks always born in full tracking region)
  mPreviousNode = mCurrentNode = 0;
  mPreviousVolume = mCurrentVolume = 0;
  mCurrentTrackingRegion=2;
  mPreviousTrackingRegion=2;
}
//________________________________________________________________________________________________
void StarVMCApplication::PostTrack(){ _g4maker->PostTrack(); }
void StGeant4Maker::PostTrack()
{

}
//________________________________________________________________________________________________
void StGeant4Maker::UpdateHistory() {

  static auto* navigator    = gGeoManager->GetCurrentNavigator();

  mPreviousNode   = mCurrentNode;
  mPreviousVolume = mCurrentVolume;
  mCurrentNode    = navigator->GetCurrentNode();
  mCurrentVolume  = navigator->GetCurrentVolume();

  // Obtain the agml extensions, giving priority to anything attached to
  // a node.
  /*AgMLExtension*/ aprev = (mPreviousNode ) ? dynamic_cast<AgMLExtension*>( mPreviousNode->GetUserExtension() ) : 0;
  /*AgMLExtension*/ acurr = (mCurrentNode  ) ? dynamic_cast<AgMLExtension*>( mCurrentNode->GetUserExtension() )  : 0;
  if ( 0==aprev ) {
    aprev = (mPreviousVolume) ? dynamic_cast<AgMLExtension*>( mPreviousVolume->GetUserExtension() ) : 0;
  }
  if ( 0==acurr ) {
    acurr = (mCurrentVolume) ? dynamic_cast<AgMLExtension*>( mCurrentVolume->GetUserExtension() ) : 0;
  }

  // If the previous or current extension is null, there is no change in the tracking state.

  if ( aprev ) {     
    mPreviousTrackingRegion = aprev->GetTracking(); 
    // HACK override for CAVE
    if ( aprev->GetVolumeName() == "CAVE") mPreviousTrackingRegion = 2;
  }
  if ( acurr ) { 
    mCurrentTrackingRegion  = acurr->GetTracking(); 
    // HACK override for CAVE
    if ( acurr->GetVolumeName() == "CAVE") mCurrentTrackingRegion = 2;
  }

}
//________________________________________________________________________________________________
int regionTransition( int curr, int prev ) {
  return curr - prev;
  //     2      2       0     no transition
  //     2      1       1     into tracking from calorimeter
  //     1      2      -1     into calorimeter from tracking
  //     1      1       0     no transition
}
//________________________________________________________________________________________________
void StarVMCApplication::Stepping(){ _g4maker -> Stepping(); }
void StGeant4Maker::Stepping(){                                           

  //  static auto* navigator    = gGeoManager->GetCurrentNavigator();
  //  static auto* trackManager = TG4TrackManager::Instance();
  static auto* mc = TVirtualMC::GetMC(); 
  static auto* stack = (StMCParticleStack* )mc->GetStack();

  // Get access to the current track
  TParticle* current = stack->GetCurrentTrack(); 

  // Get access to the current MC truth
  StarMCParticle* truth = stack->GetParticleTable().back(); 

  // Update the immediate track history
  UpdateHistory();

  // Check for region transitions
  int transit = regionTransition( mCurrentTrackingRegion, mPreviousTrackingRegion );

  // Check if option to stop punchout tracks is enabled
  if ( IAttr("Stepping:Punchout:Stop") && 1==transit) {
    
    mc->StopTrack();

    if ( 2==IAttr("Stepping:Punchout:Stop") ) {

        // Inject the stopped particle at the current vertex 
        //
        // Parent track is the ID known to the particle stack
        int parent = truth->idStack(); 
        // PDG of the track has not changed
        int pdg = current->GetPdgCode();
        // We will use the current momentum of the particle
        double px, py, pz, e;
        mc->TrackMomentum( px, py, pz, e );
        // ... and its current vertex and TOF from the point where it
        // emerges from the calorimeter
        double vx, vy, vz, tof;
        mc->TrackPosition( vx,vy,vz );
        tof = mc->TrackTime(); // because consistent interface ala TrackMomentum is hard...
        // this is a user process (and I would dearly love to be able to extend the definitions here...)
        TMCProcess mech = kPUserDefined;
        int ntr;
        
        LOG_INFO << "Punchout track reinjected, parent = " << parent << endm; 

        stack->PushTrack( 1, parent, pdg, px, py, pz, e, vx, vy, vz, tof, 0., 0., 0., mech, ntr, 1.0, 1 ); 

    } 
    
  }


}
//________________________________________________________________________________________________
void StarVMCApplication::GeneratePrimaries() { _g4maker -> PushPrimaries(); }
void StGeant4Maker::PushPrimaries() {

  StMaker           *mymaker   = GetMaker("PrimaryMaker");
  StarPrimaryMaker  *myprimary = (StarPrimaryMaker *)mymaker;
  StarParticleStack *mystack   = myprimary->stack();
  
  int ntrack = mystack -> GetNtrack();

  int ntr = 0; // set by reference below...
  for ( int itrack = 0; itrack < ntrack; itrack++ )
    {

      TParticle *track = mystack->GetParticle(itrack);
      int pdg = track -> GetPdgCode();
      TParticlePDG *pdgPart = particleData(pdg); 

      if ( 0 == pdgPart )
	{ // Protect against unknown particle codes
	  continue;
	}

      int parent = track->GetFirstMother();
      int kid1   = track->GetFirstDaughter();
      int kid2   = track->GetLastDaughter();
      double px = track->Px();
      double py = track->Py();
      double pz = track->Pz();

      double M = pdgPart->Mass();

      double E  = TMath::Sqrt( px * px + py * py + pz * pz + M * M );
      double vx = track->Vx();
      double vy = track->Vy();
      double vz = track->Vz();
      double tof= track->T();
      double weight = 1.0;
      TMCProcess proc = kPPrimary;
      int stat = track->GetStatusCode();
      
      if ( 1 == stat ) 
	{
	  
	  // Push all tracks with parent = -1 to flag as primary
	  mMCStack->PushTrack( 1, parent=-1, pdg, px, py, pz, E, vx, vy, vz, tof, 0, 0, 0, proc, ntr, weight, stat );

	}
            

    }

  LOG_INFO << "Pushed " << ntr << " tracks from primary event generator" << endm;


}
//________________________________________________________________________________________________
