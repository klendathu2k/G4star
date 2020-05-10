#include <StHitCollection.h>

#include <TVirtualMC.h>
#include <TGeoManager.h>
#include <TGeoNavigator.h>

#include <StSensitiveDetector.h>
#include <StMCParticleStack.h>
#include <StMessMgr.h>

//____________________________________________________________________________________________
ostream&  operator<<(ostream& os,  const TrackerHit& hit) {

  TString mypath("");

  int i=0;
  int volu=hit.volu[i];
  int copy=hit.copy[i];
  while ( volu>0 ) {

    mypath += "/"; mypath += gGeoManager->GetVolume(volu)->GetName(); mypath += "_"; mypath += copy;

    i++;

    volu=hit.volu[i];
    copy=hit.copy[i];

  }

   // Printout hit information
   os << Form( "Tracker Hit [%i %i]\n\t[%s]\n\t[%s]\n\tpos:(%f,%f,%f)(%f,%f,%f)\n\tmom:(%f,%f,%f),(%f,%f,%f) \n\tde=%f nstep=%i",
               hit.id, hit.idtruth,
               hit.path.Data(),
	       mypath.Data(),
	       hit.position_in[0],  
               hit.position_in[1],  
               hit.position_in[2],  
               hit.position_out[0],  
               hit.position_out[1],  
               hit.position_out[2],  
	       hit.momentum_in[0],  
               hit.momentum_in[1],  
               hit.momentum_in[2],  
               hit.momentum_out[0],  
               hit.momentum_out[1],  
               hit.momentum_out[2],  
               hit.de,
               hit.nsteps );
   return os; 
   
}
//____________________________________________________________________________________________
ostream&  operator<<(ostream& os,  const CalorimeterHit& hit) {
   // Printout hit information

  TString mypath("");
  
  int i=0;
  int volu=hit.volu[i];
  int copy=hit.copy[i];

  while ( volu>0 ) {
    
    mypath += "/"; mypath += gGeoManager->GetVolume(volu)->GetName(); mypath += "_"; mypath += copy;

    i++;

    volu=hit.volu[i];
    copy=hit.copy[i];

  }

   os << Form( "Calorimeter Hit [%i %i]\n\t[%s]\n\t[%s]\n\tpos:(%f,%f,%f)\n\tde=%f nstep=%i",
               hit.id, hit.idtruth,
               hit.path.Data(),
	       mypath.Data(),
	       hit.position_in[0],  
               hit.position_in[1],  
               hit.position_in[2],  
	       hit.de,
               hit.nsteps );
   return os; 
   
}



//_____________________________________________________________________________________________
StHitCollection::StHitCollection( const char* name, const char* title ) : TNamed(name,title){ }
//_____________________________________________________________________________________________
StTrackerHitCollection::StTrackerHitCollection( const char* name, const char* title ) : StHitCollection(name,title), mHits() { }
//_____________________________________________________________________________________________
StCalorimeterHitCollection::StCalorimeterHitCollection( const char* name, const char* title ) : StHitCollection(name,title), mHits(),mBirk{1.0,0.0130,9.6E-6},mEsum(0) { }
//_____________________________________________________________________________________________


//_____________________________________________________________________________________________
void StTrackerHitCollection::Initialize() {

}
//_____________________________________________________________________________________________
void StTrackerHitCollection::ProcessHits() {

  TGeoNavigator* navigator = gGeoManager->GetCurrentNavigator();
  TVirtualMC*    mc = TVirtualMC::GetMC();

  // Is this a charged particle?  If not, skip it...
  if ( 0 == mc->TrackCharge() ) return;


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

  bool isNewTrack      = mc->IsNewTrack();
  bool isTrackEntering = mc->IsTrackEntering();
  bool isTrackExiting  = mc->IsTrackExiting();
  bool isTrackInside   = mc->IsTrackInside();
  bool isTrackOut      = mc->IsTrackOut();
  bool isTrackStop     = mc->IsTrackStop();

  TrackerHit* hit = 0;
  
  // Track has entered this volume, create a new hit
  if ( isTrackEntering || isNewTrack ) {

    // Make sure that the level is not too deep
    if ( navigator->GetLevel() > int(DetectorHit::maxdepth) ) {
      LOG_INFO << "Cannot score hits with depth " <<   navigator->GetLevel() << endm;
      return; 
    }
    mHits.push_back( hit = new TrackerHit );

    // Get the current path to the sensitive volume
    hit->path = mc->CurrentVolPath(); 

    // Get the current volume / copy numbers to the sensitive volume.  n.b. GetBranchNumbers 
    // only writes to the current level, so if hit is not new or cleared, need to clear by hand.
    gGeoManager->GetBranchNumbers( hit->copy, hit->volu );

    // Except... 


    // for ( int i=0;i<navigator->GetLevel(); i++ ) {
    //   hit->copy[i] = copy[i];
    //   hit->volu[i] = volu[i];
    // }
    //    delete copy;
    //    delete volu;

    // But for some reason the copy numbers are not right, so...
    //    TGeoNodeCache* cache = navigator->GetCache();
    

    
    // Assign the hit a unqiue ID (index + 1)
    hit->id = mHits.size();

    // Assign the hit the ID truth of the current track (index + 1)
    hit->idtruth = truthTable.size();

    // Score entrance momentum and 
    mc->TrackMomentum( hit->momentum_in[0], hit->momentum_in[1],  hit->momentum_in[2],  hit->momentum_in[3] ); 
    mc->TrackPosition( hit->position_in[0], hit->position_in[1],  hit->position_in[2] );
    hit->position_in[3] = mc->TrackTime();
 
  } 
  
  if ( mHits.size() == 0 ) {
    LOG_INFO << "No available hits" << endm;
    return; 
  }

  // Update the final step in this hit
  hit = mHits.back();
  
  hit -> nsteps += 1; 

  // For all other tracking states, update the exit momentum and position 
  mc->TrackMomentum( hit->momentum_out[0], hit->momentum_out[1],  hit->momentum_out[2],  hit->momentum_out[3] ); 
  mc->TrackPosition( hit->position_out[0], hit->position_out[1],  hit->position_out[2] );
  hit->position_out[3] = mc->TrackTime();

  // Increment the energy loss and step sums
  hit -> de += mc->Edep();
  hit -> ds += mc->TrackStep();

  // Total length to this point
  hit -> length = mc->TrackLength();

  double mass = mc->TrackMass();
  double Etot = mc->Etot(); // total energy

}
//_____________________________________________________________________________________________
void StTrackerHitCollection::EndOfEvent() {
  for ( auto hit : mHits ) {
    LOG_INFO << *hit << endm;
  }
  mHits.clear();
}
//_____________________________________________________________________________________________


//_____________________________________________________________________________________________
void StCalorimeterHitCollection::Initialize() {

}
//_____________________________________________________________________________________________
void StCalorimeterHitCollection::ProcessHits() {

  TGeoNavigator* navigator = gGeoManager->GetCurrentNavigator();
  TVirtualMC*    mc = TVirtualMC::GetMC();

  // Is this a charged particle?  If not, skip it...
  if ( 0 == mc->TrackCharge() ) return;

  // Energy deposited in this tracking step
  double Edep = mc->Edep();

  // Correct for Birk's law
  //  Edep = Edep * mBirk[0] / ( 1.0 + mBirk[1]*Edep + mBirk[2]*Edep*Edep );

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

  bool isNewTrack      = mc->IsNewTrack();
  bool isTrackEntering = mc->IsTrackEntering();
  bool isTrackExiting  = mc->IsTrackExiting();
  bool isTrackInside   = mc->IsTrackInside();
  bool isTrackOut      = mc->IsTrackOut();
  bool isTrackStop     = mc->IsTrackStop();

  CalorimeterHit* hit = 0;
  
  // Track has entered this volume, create a new hit
  if ( isTrackEntering || isNewTrack ) {

    // Zero out the energy sum
    mEsum = 0;

    // Make sure that the level is not too deep
    if ( navigator->GetLevel() > int(DetectorHit::maxdepth) ) {
      LOG_INFO << "Cannot score hits with depth " <<   navigator->GetLevel() << endm;
      return; 
    }

    mHits.push_back( hit = new CalorimeterHit );

    // Get the current path to the sensitive volume
    hit->path = mc->CurrentVolPath(); 

    // Get the current volume / copy numbers to the sensitive volume.  n.b. GetBranchNumbers 
    // only writes to the current level, so if hit is not new or cleared, need to clear by hand.
    gGeoManager->GetBranchNumbers( hit->copy, hit->volu );

    // Assign the hit a unqiue ID (index + 1)
    hit->id = mHits.size();

    // Assign the hit the ID truth of the current track (index + 1)
    hit->idtruth = truthTable.size();

    // Score entrance
    mc->TrackPosition( hit->position_in[0], hit->position_in[1],  hit->position_in[2] );
    hit->position_in[3] = mc->TrackTime();
 
  } 
  
  if ( mHits.size() == 0 ) {
    LOG_INFO << "No available hits" << endm;
    return; 
  }

  // Update the final step in this hit
  hit = mHits.back();
  
  hit -> nsteps += 1; 

  // Add current energy deposit to energy sum
  mEsum += Edep;

  // Hit energy will be the total energy deposition corrected by Birk's law
  hit -> de =   mEsum * mBirk[0] / ( 1.0 + mBirk[1]*mEsum + mBirk[2]*mEsum*mEsum );

}
//_____________________________________________________________________________________________
void StCalorimeterHitCollection::EndOfEvent() {
  for ( auto hit : mHits ) {
    LOG_INFO << *hit << endm;
  }
  mHits.clear();
}
//_____________________________________________________________________________________________
