#ifndef __StMCTruthTable_h__
#define __StMCTruthTable_h__

#include <TNamed.h>

//class TVirtualMCStack;
class TParticle;

class TGeoNavigator;

class StMCParticleStack;

class StMCTruthTable : public TNamed {
public:

  StMCTruthTable();
  virtual ~StMCTruthTable(){ /* nada */ };

  virtual void BeginEvent();
  virtual void BeginPrimary();
  virtual void PreTrack();
  virtual void Stepping();
  virtual void PostTrack();
  virtual void FinishPrimary();
  virtual void FinishEvent();

private:
protected:
  StMCParticleStack* mMCStack;
  TGeoNavigator    * mNavigator;
  TParticle        * mCurrentPrimary;
  TParticle        * mCurrentTrack;
  TParticle        * mStickyTrack;
  int                mStickyStack;

};

#endif
