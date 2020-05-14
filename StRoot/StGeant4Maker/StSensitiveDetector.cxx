#include <StSensitiveDetector.h>

#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TVirtualMC.h>

#include <StMCParticleStack.h>
#include <StMessMgr.h>
#include <StHitCollection.h>

#include <StarVMC/StarAgmlLib/AgMLExtension.h>

TVirtualMC*    mc        = 0;
TGeoNavigator* navigator = 0;

#include <StMessMgr.h>
#include <TString.h>

#include <TMath.h> 
// //____________________________________________________________________________________________
// ostream&  operator<<(ostream& os,  const TrackerHit& hit) {
//    // Printout hit information
//    os << Form( "Tracker Hit [%i %i]\n\t[%s]\n\tpos:(%f,%f,%f)(%f,%f,%f)\n\tmom:(%f,%f,%f),(%f,%f,%f) \n\tde=%f nstep=%i",
//                hit.id, hit.idtruth,
//                hit.path.Data(),
// 	       hit.position_in[0],  
//                hit.position_in[1],  
//                hit.position_in[2],  
//                hit.position_out[0],  
//                hit.position_out[1],  
//                hit.position_out[2],  
// 	       hit.momentum_in[0],  
//                hit.momentum_in[1],  
//                hit.momentum_in[2],  
//                hit.momentum_out[0],  
//                hit.momentum_out[1],  
//                hit.momentum_out[2],  
//                hit.de,
//                hit.nsteps );
//    return os; 
   
// }

//____________________________________________________________________________________________
DetectorHit::DetectorHit() : id(0), idtruth(0), volu{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, copy{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, numbv{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, path(), nsteps(0), user()
{ 
  /* nada */ 
}
TrackerHit::TrackerHit() : DetectorHit(), 
                         position_in{0,0,0,0}, position_out{0,0,0,0}, 
                         momentum_in{0,0,0,0}, momentum_out{0,0,0,0}, 
                         de(0), ds(0), length(0), lgam(0) 
{
   /*nada*/
} 
CalorimeterHit::CalorimeterHit() : DetectorHit(), position_in{0,0,0,0}, de(0) { /* nada */ } 

//____________________________________________________________________________________________
StSensitiveDetector::StSensitiveDetector( const char* name, const char* title ) : TVirtualMCSensitiveDetector(name,title), mVolumes(), mHits(), mAgMLInfo(0), mCollection(0) {
  LOG_INFO << "SD created for " << name << " " << title << endm;
} 
//____________________________________________________________________________________________
void StSensitiveDetector::Initialize(){ 
  navigator = gGeoManager->GetCurrentNavigator();
  mc = TVirtualMC::GetMC();
}
//____________________________________________________________________________________________
void StSensitiveDetector::addVolume( TGeoVolume* volume ) {
  if ( 0 == mVolumes.size() ) {
    mAgMLInfo = dynamic_cast<AgMLExtension*>( volume->GetUserExtension() );   

    TString cname  = mAgMLInfo->GetFamilyName(); cname += "_hits";
    TString ctitle = mAgMLInfo->GetModuleName(); ctitle += " "; ctitle += cname;

    if ( DetectorType::kTracker == detectorType() ) {
      mCollection = new StTrackerHitCollection(cname.Data(),ctitle.Data());
      LOG_INFO << "Setting tracker collection" << endm;
    }
    if ( DetectorType::kCalorimeter == detectorType() ) {
      mCollection = new StCalorimeterHitCollection(cname.Data(),ctitle.Data());
      LOG_INFO << "Setting calorimeter collection" << endm;
    }

  }

  mVolumes.push_back( volume );  

};
//____________________________________________________________________________________________
void StSensitiveDetector::ProcessHits(){ 

  // Is this a charged particle?  If not, skip it...
  if ( 0 == mc->TrackCharge() ) return;

  // The actual hit processing occurs in the collection. 
  mCollection->ProcessHits();


  return;


  // Energy deposited in this tracking step
  double Edep = mc->Edep();

  // Position and momentum of the hit 
  double x, y, z, px, py, pz, etot;
  mc->TrackPosition( x, y, z );
  mc->TrackMomentum( px, py, pz, etot ); 

  StMCParticleStack* stack = (StMCParticleStack *)mc->GetStack();
  
  // Get list of tracks from particle stack
  std::vector<StarMCParticle*>& truthTable = stack->GetParticleTable();

  // This should be the current particle truth 
  StarMCParticle* truth = truthTable.back();
  truth->addHit(); 

  //  LOG_INFO << GetName() << " " << *truth << " Edep=" << Edep << endm;

  bool isNewTrack      = mc->IsNewTrack();
  bool isTrackEntering = mc->IsTrackEntering();
  bool isTrackExiting  = mc->IsTrackExiting();
  bool isTrackInside   = mc->IsTrackInside();
  bool isTrackOut      = mc->IsTrackOut();
  bool isTrackStop     = mc->IsTrackStop();

  DetectorHit* hit = 0;

  // Track has entered this volume, create a new hit
  if ( isTrackEntering || isNewTrack ) {

     // Make sure that the level is not too deep
     if ( navigator->GetLevel() > int(DetectorHit::maxdepth) ) {
        LOG_INFO << "Cannot score hits with depth " <<   navigator->GetLevel() << endm;
        return; 
     }
     mHits.push_back( hit = new TrackerHit );

     hit->path = mc->CurrentVolPath(); 

     // Assign the hit a unqiue ID (index + 1)
     hit->id = mHits.size();

     // Assign the hit the ID truth of the current track (index + 1)
     hit->idtruth = truthTable.size();
 
  } 

  if ( mHits.size() == 0 ) {
     LOG_INFO << "No available hits" << endm;
     return; 
  }

  hit = mHits.back();
  hit -> nsteps += 1; 

  // // For all other tracking states, update the exit momentum and position 
  // mc->TrackMomentum( hit->momentum_out[0], hit->momentum_out[1],  hit->momentum_out[2],  hit->momentum_out[3] ); 
  // mc->TrackPosition( hit->position_out[0], hit->position_out[1],  hit->position_out[2] );
  // hit->position_out[3] = mc->TrackTime();

  // // Increment the energy loss and step sums
  // hit -> de += mc->Edep();
  // hit -> ds += mc->TrackStep();

  // // Total length to this point
  // hit -> length = mc->TrackLength();

  // double mass = mc->TrackMass();
  // double Etot = mc->Etot(); // total energy

  // LOG_INFO << *hit 
  // 	   << " aginfo=" << mAgMLInfo->GetFamilyName().Data() << " nvb=" << mAgMLInfo->GetBranchings() << endm;
  // LOG_INFO << " isNew:    " << isNewTrack      << endm;
  // LOG_INFO << " isEnter:  " << isTrackEntering << endm;
  // LOG_INFO << " isExit:   " << isTrackExiting  << endm;
  // LOG_INFO << " isInside: " << isTrackInside   << endm;
  // LOG_INFO << " isOut:    " << isTrackOut      << endm;
  // LOG_INFO << " isStop:   " << isTrackStop     << endm;

  return; 
}
//____________________________________________________________________________________________
void StSensitiveDetector::EndOfEvent(){ 

  // for ( auto hit : mHits ) {
  //    LOG_INFO << *hit << endm; 
  // }
  mCollection->EndOfEvent();
  mHits.clear(); 

}
//____________________________________________________________________________________________
StSensitiveDetector::DetectorType StSensitiveDetector::detectorType() {

  // For the time being, just flag on the name of the module to decide tracker vs calorimeter  
  if ( 0 == mAgMLInfo ) {
    return DetectorType::kUninitialized;
  }

  TString module = mAgMLInfo->GetModuleName();
  module.ToLower();

  if ( module.Contains("cal") ) {
    return DetectorType::kCalorimeter;
  }
  else {
    return DetectorType::kTracker;    
  }

}
//____________________________________________________________________________________________
int StSensitiveDetector::numberOfHits(){ return int( mCollection->numberOfHits() ); }
//____________________________________________________________________________________________
