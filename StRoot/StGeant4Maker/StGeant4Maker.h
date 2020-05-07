#ifndef __StGeant4Maker_h__
#define __StGeant4Maker_h__
#include <StMaker.h>

#include "StChain.h"

#include "TVirtualMC.h"
#include "TVirtualMCApplication.h"
#include "TVirtualMagField.h"
#include "StarMagField.h"



class StGeant4Maker;
class StMCParticleStack;
class StMCTruthTable;

class AgMLExtension;

class TGeoNode;
class TGeoVolume;

class TG4RunConfiguration;
//class StVMCStepManager;

//______________________________________________________________________________________
class StarMagFieldAdaptor : public TVirtualMagField {
public:
  void Field( const Double_t *x, Double_t *B )
  { 
    StarMagField::Instance()->BField(x,B); 
  }
};
//______________________________________________________________________________________
class StarVMCApplication : public TVirtualMCApplication {
public:
  StarVMCApplication( const Char_t *name = "starsim", const Char_t *title="STAR VMC simulation" );
 ~StarVMCApplication(){ /* nada */ };

  /// Geometry construction is the responsability of the STAR chain
  virtual void ConstructGeometry();
  virtual void ConstructSensitiveDetectors();

  /// Misalignment of geometry.  Default false.
  virtual Bool_t MisalignGeometry(){ return false; }

  /// Define parameters for optical processes (optional)
  virtual void ConstructOpGeometry() { LOG_INFO << "VMC APP ConstructOpGeometry()" << endm; }
  
  /// TODO: Initialize geometry. (Usually used to define sensitive volumes IDs)
  virtual void InitGeometry();

  /// Add user defined particles (optional)
  virtual void AddParticles() {LOG_INFO << "VMC APP AddParticles()" << endm; };

  /// Add user defined ions (optional)
  virtual void AddIons() {}

  /// Primary generation is responsability of the STAR framework
  virtual void GeneratePrimaries();

  /// Define actions at the beginning of the event
  virtual void BeginEvent();

  /// Define actions at the beginning of the primary track
  virtual void BeginPrimary();

  /// Define actions at the beginning of each track
  virtual void PreTrack();

   /// Define action at each step
  virtual void Stepping();

  /// Define actions at the end of each track
  virtual void PostTrack();

  /// Define actions at the end of the primary track
  virtual void FinishPrimary();

  /// Define actions at the end of the event
  virtual void FinishEvent();

  /// Define maximum radius for tracking (optional)
  virtual Double_t TrackingRmax() const { return DBL_MAX; }

  /// Define maximum z for tracking (optional)
  virtual Double_t TrackingZmax() const { return DBL_MAX; }

  /// Calculate user field \a b at point \a x
  virtual void Field(const Double_t* x, Double_t* b) const { StarMagField::Instance()->BField(x,b); }

  /// Define action at each step for Geane
  virtual void GeaneStepping() {;}    

private:
protected:

  ClassDef(StarVMCApplication, 1);

};
//______________________________________________________________________________________
class StGeant4Maker : public StMaker {
public:
  StGeant4Maker( const char* nm="geant4" );
  ~StGeant4Maker(){ /* nada */ };

  /// Initialize maker
  int Init();
  int  InitRun( int run );
  int  InitGeom();
  int  InitHits();

  /// Perform runtime initialization of per-medium track propagation cuts, processes, etc...
  int  ConfigureGeometry();

  /// Process one event
  int Make();  

  void BeginEvent();
  void BeginPrimary();
  void PreTrack();
  void PostTrack();
  void FinishPrimary();
  void FinishEvent();
  
  // Executed on each tracking step
  void Stepping();

  /// Push primary event out for tracking
  void PushPrimaries();

  /// Clear for the next event
  void Clear( const Option_t* opts="" );

private:
protected:

  StarVMCApplication *mVmcApplication;
  TString mGeomPath; // Search path for geometry construction
  StarMagField *mStarField;
  StMCParticleStack *mMCStack;      // Monte Carlo stack
  StarMagFieldAdaptor *mMagfield;
  TG4RunConfiguration* mRunConfig;
  //  StVMCStepManager* mStepping;
  StMCTruthTable* mTruthTable;

  void UpdateHistory();

  TGeoNode*   mCurrentNode;
  TGeoNode*   mPreviousNode;
  TGeoVolume* mCurrentVolume;
  TGeoVolume* mPreviousVolume;

  int         mCurrentTrackingRegion;
  int         mPreviousTrackingRegion;

  AgMLExtension* acurr;
  AgMLExtension* aprev;

  ClassDef(StGeant4Maker,1);

};
//______________________________________________________________________________________
#endif
