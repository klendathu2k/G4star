#include <StMCTruthTable.h>

#include <StMessMgr.h>
#include <TVirtualMC.h>
#include <TVirtualMCStack.h>
#include <TParticle.h>

#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TGeoVolume.h>
#include <TGeoNode.h>

#include <StarVMC/StarAgmlLib/AgMLExtension.h>

#include <StMCParticleStack.h>

#include <TString.h>
#include <string>

int level = 0;

std::string pad( int level ) {
  std::string result("");
  for ( int i=0;i<level;i++ ) result += "...";
  return result;
};

//_________________________________________________________________________________________
StMCTruthTable::StMCTruthTable() : 
  TNamed("TruthTable","You can't handle it"),
  mMCStack(0),
  mNavigator(0),
  mCurrentPrimary(0),
  mCurrentTrack(0),
  mStickyTrack(0)
{            }
//_________________________________________________________________________________________
void StMCTruthTable::BeginEvent(){ 
  // Obtain stack, navigator, etc.. at event start
  mMCStack = (StMCParticleStack *)TVirtualMC::GetMC()->GetStack();
  mNavigator = gGeoManager->GetCurrentNavigator();
#ifdef __DEBUG__
  LOG_INFO << pad(level++) << "Begin Event" << endm; 
#endif
}
//_________________________________________________________________________________________
void StMCTruthTable::BeginPrimary(){ 
  mCurrentPrimary = mMCStack->GetCurrentTrack();
#ifdef __DEBUG__
  LOG_INFO << pad(level++) << "Begin Primary" << endm; 
#endif
}
//_________________________________________________________________________________________
void StMCTruthTable::PreTrack(){ 
  mCurrentTrack = mMCStack->GetCurrentTrack();
#ifdef __DEBUG__
  LOG_INFO << pad(level++) << "Pre Track" << endm;
  if ( mCurrentTrack ) {
    mCurrentTrack->Print();
  }
#endif
  TGeoVolume* volume=gGeoManager->GetCurrentVolume();
  short tracking = 0;
  if ( volume ) {
    AgMLExtension* ae = dynamic_cast<AgMLExtension*>( volume->GetUserExtension() );
    if ( 0==ae ) {
      TGeoNode* mother = gGeoManager->GetMother(); assert(mother);
      ae = dynamic_cast<AgMLExtension*>(mother->GetVolume()->GetUserExtension());
    }
    if ( ae ) {      
      tracking = ae->GetTracking();
#ifdef __DEBUG__
      LOG_INFO << pad(level) << "tracking region = " << tracking << endm;
#endif
    }
    else {
#ifdef __DEBUG__
      LOG_WARN << pad(level) << "no agml extension [" << volume->GetName() << "]" << endm;
#endif
    }
  }
  else { 
    LOG_WARN << pad(level) << "no voulume (is ROOT navigating?)" << endm;
  }
}

//_________________________________________________________________________________________
void StMCTruthTable::Stepping(){ 

}
//_________________________________________________________________________________________
void StMCTruthTable::PostTrack(){ 
#ifdef __DEBUG__
  LOG_INFO << pad(--level) << "Post Track" << endm;
  if ( mCurrentTrack ) {
    mCurrentTrack->Print();
  }
#endif
}
//_________________________________________________________________________________________
void StMCTruthTable::FinishPrimary(){ 
#ifdef __DEBUG__
  LOG_INFO << pad(--level) << "Finish Primary" << endm;
#endif
}
//_________________________________________________________________________________________
void StMCTruthTable::FinishEvent(){ 
#ifdef __DEBUG__
  LOG_INFO << pad(--level) << "Finish Event" << endm;
#endif
}
//_________________________________________________________________________________________
