#ifndef __StSensitiveDetector_h__
#define __StSensitiveDetector_h__

#include <TVirtualMCSensitiveDetector.h>
#include <TGeoVolume.h>
#include <vector>
#include <TString.h>

class AgMLExtension;
class StHitCollection;

class DetectorHit {
public:

  DetectorHit();

  static const unsigned int maxdepth = 15;

  unsigned int id;              /// unique ID assigned to hit
  unsigned int idtruth;         /// unique ID of the track
           int volu[maxdepth];  /// volume numbers specifying path to hit
           int copy[maxdepth];  /// copy numbers specifying path to the hit
           int numbv[maxdepth]; /// "Reduced" numbering

  TString path;

  unsigned int nsteps; /// number of steps inside volume 

};

class TrackerHit : public DetectorHit {
public:

  TrackerHit();

  double position_in[4];        /// x,y,z,t position where track entered volume
  double position_out[4];       /// x,y,z,t position where track exited volume

  double momentum_in[4];
  double momentum_out[4];

  double de;
  double ds;

  double length; /// total track length to this point
  double lgam;   /// log10( Ekin / mass )

};

class CalorimeterHit : public DetectorHit {
public:

  CalorimeterHit(); 

  double position_in[4];        /// x,y,z,t position where track entered volume
  double de;

};

class StSensitiveDetector : public TVirtualMCSensitiveDetector {
public:
  StSensitiveDetector( const char* sdname, const char* title );
  virtual ~StSensitiveDetector(){ /* nada */ }

  void Initialize();
  void ProcessHits();
  void EndOfEvent();

  void addVolume(TGeoVolume *v);
  int  numberOfVolumes(){ return mVolumes.size(); }

  int numberOfHits(){ return int( mHits.size() ); }

  enum class DetectorType { kUninitialized, kCalorimeter, kTracker };

  DetectorType detectorType();

private:
protected:

  std::vector<TGeoVolume*>   mVolumes;
  std::vector<DetectorHit*>  mHits;
  AgMLExtension* mAgMLInfo;

  StHitCollection* mCollection;

};

#endif
