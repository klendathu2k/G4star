#include <StSensitiveDetector.h>

#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TVirtualMC.h>

#include <StMCParticleStack.h>
#include <StMessMgr.h>

TVirtualMC*    mc        = 0;
TGeoNavigator* navigator = 0;

#include <StMessMgr.h>
#include <TString.h>

#include <TMath.h> 
//____________________________________________________________________________________________
ostream&  operator<<(ostream& os,  const TrackerHit& hit) {
   // Printout hit information
   os << Form( "Tracker Hit [%i %i]\n\t[%s]\n\tpos:(%f,%f,%f)(%f,%f,%f)\n\tmom:(%f,%f,%f),(%f,%f,%f) \n\tde=%f nstep=%i",
               hit.id, hit.idtruth,
               hit.path.Data(),
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
DetectorHit::DetectorHit() : id(0), idtruth(0), volu{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, copy{0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, path(), nsteps(0)  
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
StSensitiveDetector::StSensitiveDetector( const char* name, const char* title ) : TVirtualMCSensitiveDetector(name,title) {
  LOG_INFO << "SD created for " << name << " " << title << endm;
} 
//____________________________________________________________________________________________
void StSensitiveDetector::Initialize(){ 
  navigator = gGeoManager->GetCurrentNavigator();
  mc = TVirtualMC::GetMC();
}
//____________________________________________________________________________________________
void StSensitiveDetector::ProcessHits(){ 

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

  //  LOG_INFO << GetName() << " " << *truth << " Edep=" << Edep << endm;

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

     // Get the volume and copy numbers for this sensitive detector
     //navigator->GetBranchNumbers( hit->copy, hit->volu ); 
#if 0
     std::vector<int> volu;
     std::vector<int> copy;
     int vnum, cnum;
     int off = 0;
     vnum = mc->CurrentVolOffID( off, cnum );
     while ( vnum > 0 ) {
         volu.push_back(vnum);
         copy.push_back(cnum);
         vnum = mc->CurrentVolOffID( ++off, cnum );
     } 
     for ( unsigned int i=volu.size()-1; i>=0; i-- ) {
        hit->volu[i] = volu[i];
        hit->copy[i] = copy[i]; 
     }
#endif 

     hit->path = mc->CurrentVolPath(); 

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
//double Ekin = Etot - mass; 
//double gamma = Ekin / mass;

//  hit -> lgam   = TMath::Log10( Etot/mass - 1.0 );

  return; 
}
//____________________________________________________________________________________________
void StSensitiveDetector::EndOfEvent(){ 

  for ( auto hit : mHits ) {
     LOG_INFO << *hit << endm; 
  }

  mHits.clear(); 

}
