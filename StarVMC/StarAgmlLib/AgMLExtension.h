#ifndef __AgmlExtension_h__
#define __AgmlExtension_h__

#include <TGeoExtension.h>
#include <TString.h>

class AgMLExtension : public TGeoRCExtension {

public:

  AgMLExtension();
  virtual ~AgMLExtension(){ /* nada */ };

  void SetModuleName( const char* name ){ mModuleName = name; }
  void SetFamilyName( const char* name ){ mFamilyName = name; }
  void SetVolumeName( const char* name ){ mVolumeName = name; }
  void SetSensitive( bool flag ) { mSensitive = flag; }
  void SetTracking( short track ) { 
    if ( mVolumeName == "CAVE" ) {
      mTracking = 2;
    }
    else { 
      mTracking = track; 
    };
  }

  void SetBranchings( int b ) { mBranchings=b; }  


  TString GetModuleName(){ return mModuleName; }
  TString GetFamilyName(){ return mFamilyName; }
  TString GetVolumeName(){ return mVolumeName; }

  bool GetSensitive() { return mSensitive; }
  short GetTracking() { return mTracking; }
  int GetBranchings(){ return mBranchings; }
 
private:
protected:

  TString mModuleName; // name of the module
  TString mFamilyName; // name of the family of volumes
  TString mVolumeName; // name of the volume

  bool    mSensitive;  // volume sensitivity
  short   mTracking;   // 0=blackhole, 1=calorimeter, 2=tracking region
  int     mBranchings; // number of branchings (placed family members)

  ClassDef(AgMLExtension,1);

};

#endif
