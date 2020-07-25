#ifndef __unit_tests__
#define __unit_tests__
#include <StGeant4Maker/StGeant4Maker.h>
#include <StarGenerator/EVENT/StarGenParticle.h>
#include <StarGenerator/BASE/StarPrimaryMaker.h>
#include <StarGenerator/Kinematics/StarKinematics.h>
#include <StChain.h>
#include <iostream>
#include <TVector3.h>
#include <TMath.h>
#include <g2t_track.h>
#include <g2t_vertex.h>
#include <g2t_tpc_hit.h>
#include <g2t_fts_hit.h>
#include <g2t_emc_hit.h>
#include <TTable.h>
#include <TROOT.h>
#include <string>
#define __COLOR__
#ifdef __COLOR__
const std::string FAIL = "\u001b[31m -failed- \u001b[0m";
const std::string PASS = "\u001b[32m -passed- \u001b[0m";
const std::string UNKN = "\u001b[33m -unknown- \u001b[0m";
const std::string TODO = "\u001b[36m -todo- \u001b[0m";
#else
const std::string FAIL = " -failed- ";
const std::string PASS = " -passed- ";
const std::string UNKN = " -unknown- ";
const std::string TODO = " -todo- ";
#endif
using namespace std;
//___________________________________________________________________
#define LOG_TEST std::cout << "\u001b[35m -require- \u001b[0m"
//___________________________________________________________________
TTable* hit_table    = 0;
TTable* track_table  = 0;
TTable* vertex_table = 0;
static TVector3 _vector3;
//___________________________________________________________________
double _pmom = 0;
void throw_muon( double eta, double phid, double pT = 25.0, int q=1 ) {
  // eta  = pseudorapidity
  // phid = azimuthal angle in degrees
  double phi = TMath::Pi() * phid / 180.0;
  TVector3 momentum;
  momentum.SetPtEtaPhi(pT,eta,phi);
  _pmom = momentum.Mag();
  auto* chain = StMaker::GetChain();
  auto* _kine = dynamic_cast<StarKinematics*>( chain->GetMaker("StarKine") );
  auto* particle = _kine->AddParticle( (q==1)?"mu+":"mu-" );
  particle->SetPx(momentum[0]);
  particle->SetPy(momentum[1]);
  particle->SetPz(momentum[2]);
  double mass = particle->GetMass();
  double ener = sqrt( momentum.Mag2() + mass*mass );
  particle->SetEnergy(ener);
  chain->Clear();
  chain->Make();
}
void throw_particle( const char* part, double eta, double phid, double pT = 25.0, int q=1 ) {
  // eta  = pseudorapidity
  // phid = azimuthal angle in degrees
  double phi = TMath::Pi() * phid / 180.0;
  TVector3 momentum;
  momentum.SetPtEtaPhi(pT,eta,phi);
  _pmom = momentum.Mag();
  auto* chain = StMaker::GetChain();
  auto* _kine = dynamic_cast<StarKinematics*>( chain->GetMaker("StarKine") );
  auto* particle = _kine->AddParticle( part );
  particle->SetPx(momentum[0]);
  particle->SetPy(momentum[1]);
  particle->SetPz(momentum[2]);
  double mass = particle->GetMass();
  double ener = sqrt( momentum.Mag2() + mass*mass );
  particle->SetEnergy(ener);
  chain->Clear();
  chain->Make();
}
//___________________________________________________________________
std::string check_track( std::string message, std::function<std::string(const g2t_track_st*)> f, int idx=0) {
  const g2t_track_st* track = static_cast<const g2t_track_st*>( track_table->At(idx) );  
  std::string result = "\u001b[37m [" + message + "] " + (track? f(track):FAIL );
  LOG_TEST << result << std::endl;
  return result;
};
//___________________________________________________________________
std::string check_tpc_hit( std::string message, const g2t_tpc_hit_st* hit, std::function<std::string(const g2t_tpc_hit_st*)> f) {
  std::string result = "\u001b[37m [" + message + "] " + (hit? f(hit):FAIL);
  LOG_TEST << result << std::endl;
  return result;
};
//___________________________________________________________________
std::string check_stg_hit( std::string message, const g2t_fts_hit_st* hit, std::function<std::string(const g2t_fts_hit_st*)> f) {
  std::string result = "\u001b[37m [" + message + "] " + (hit? f(hit):FAIL);
  LOG_TEST << result << std::endl;
  return result;
};
//___________________________________________________________________
std::string check_emc_hit( std::string message, const g2t_emc_hit_st* hit, std::function<std::string(const g2t_emc_hit_st*)> f) {
  std::string result = "\u001b[37m [" + message + "] " + (hit? f(hit):FAIL);
  LOG_TEST << result << std::endl;
  return result;
};
//___________________________________________________________________

#endif
