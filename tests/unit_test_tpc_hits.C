#include <StarGenerator/EVENT/StarGenParticle.h>
#include <StarGenerator/Kinematics/StarKinematics.h>
#include <TVector3.h>
#include <TMath.h>
#include <g2t_track.h>
#include <g2t_vertex.h>
#include <g2t_tpc_hit.h>
#include <string>
#include <StMessMgr.h>
const std::string FAIL = "\u001b[31m -failed- \u001b[0m";
const std::string PASS = "\u001b[32m -passed- \u001b[0m";
const std::string UNKN = "\u001b[33m -unknown- \u001b[0m";
using namespace std;
//___________________________________________________________________
TTable* hit_table    = 0;
TTable* track_table  = 0;
TTable* vertex_table = 0;
//___________________________________________________________________
void throw_muon( double eta, double phid, double pT = 25.0, int q=1 ) {
  // eta  = pseudorapidity
  // phid = azimuthal angle in degrees
  double phi = TMath::Pi() * phid / 180.0;
  TVector3 momentum;
  momentum.SetPtEtaPhi(pT,eta,phi);
  auto* particle = _kine->AddParticle( (q==1)?"mu+":"mu-" );
  particle->SetPx(momentum[0]);
  particle->SetPy(momentum[1]);
  particle->SetPy(momentum[2]);
  double mass = particle->GetMass();
  double ener = sqrt( momentum.Mag2() + mass*mass );
  particle->SetEnergy(ener);
  chain->Clear();
  chain->Make();
  vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("bfc/.make/geant4star/.data/g2t_vertex")  );
  track_table  = dynamic_cast<TTable*>( chain->GetDataSet("bfc/.make/geant4star/.data/g2t_track")   );
  hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("bfc/.make/geant4star/.data/g2t_tpc_hit") );
}
//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;
void throw_muon_in_sector( int sectorid ) {
  assert(sectorid>0 && sectorid <= 24);
  const double sectors[] = { 15.0, 45.0, 75.0, 105.0, 135.0, 165.0, 195.0, 225.0, 255.0, 285.0, 315.0, 345.0 };
  double eta = (sectorid<=12) ? 0.5 : -0.5;
  _eta = eta;
  double phid = sectors[sectorid-1];
  _phid = phid; 
  throw_muon( eta, phid, 50.0 );
}
//___________________________________________________________________
std::string check_track( std::string message, std::function<std::string(const g2t_track_st*)> f) {

  const g2t_track_st* track = static_cast<const g2t_track_st*>( track_table->At(0) );  
  std::string result = "[" + message + "] " + (track? f(track):FAIL );

  LOG_INFO << result << endm;

  return result;

};
//___________________________________________________________________
void unit_test_tpc_hits() {
  initChain(); 
  _primary->SetVertex(0.,0.,0.);
  _primary->SetSigma(0.0,0.,0.);

  LOG_INFO << "=======================================================" << endm;
  LOG_INFO << "Unit testing of TPC hits on single muons"                << endm;
  LOG_INFO << "=======================================================" << endm;
  
  for ( int sector=1; sector<=1; sector++ ) {

    LOG_INFO << "Checking tracks in sector " << sector << endm;
    throw_muon_in_sector( sector );
    check_track( "Print the track table", [=]( const g2t_track_st* ){
	int nrows = track_table->GetNRows();
	std::string result = FAIL;
	if ( nrows > 0 ) {
	  track_table->Print(0,5);
	  result = PASS;
	}
	return result;
      });
    check_track( "A muon must have been processed by geant",       [=](const g2t_track_st* t){ return PASS; } );
    check_track( "The track should have a start vertex",           [=](const g2t_track_st* t){
      return (t->start_vertex_p>0)?PASS:FAIL;      
    });
    check_track( "The start vertex should be in the vertex table", [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istart = t->start_vertex_p;
      const g2t_vertex_st* vertex = (istart>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) ) : 0;
      if ( vertex ) {
	result = PASS;
      }
      return result;
    });
    check_track( "There should not be a stop vertex in the TPC",   [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istop = t->stop_vertex_p;
      const g2t_vertex_st* v = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
      if ( 0==v ) 
	result = PASS;
      else {
	double x1 = v->eg_x[0];
	double y1 = v->eg_x[1];
	double z1 = v->eg_x[2];
	double x2 = v->ge_x[0];
	double y2 = v->ge_x[1];
	double z2 = v->ge_x[2];
	double xx1 = sqrt(x1*x1+y1*y1); // event generator
	double xx2 = sqrt(x2*x2+y2*y2); // geant vertex
	if ( xx1 > 208.0 || xx2 > 208.0 ) 
	  result = PASS;
	result = Form(" r=%f ",TMath::Max(xx1,xx2)) + result;
      }
      return result;
    });
    check_track( "The start vertex should be on the z-axis",       [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istart = t->start_vertex_p;

      const g2t_vertex_st* v = 0;
      if ( istart > 0 ) 
	v = static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) );
      else 
	result = " no vertex in table " + result;

      if ( v ) {
	double x1 = v->eg_x[0];
	double y1 = v->eg_x[1];
	double z1 = v->eg_x[2];
	double x2 = v->ge_x[0];
	double y2 = v->ge_x[1];
	double z2 = v->ge_x[2];
	double xx1 = sqrt(x1*x1+y1*y1); // event generator
	double xx2 = sqrt(x2*x2+y2*y2); // geant vertex
	if ( xx1 > 0.0001 ) {
	  result = Form(" EG: %f %f %f ",x1,y1,z1) + result;
	} 
	if ( xx2 > 0.0001 ) {
	  result = Form(" GE: %f %f %f ",x2,y2,z2) + result;
	}
	if ( xx1 < 0.0001 && xx2 < 0.0001 ) {
	  result = PASS;
	}	
      }
       
      return result;
    });
    check_track( "The track should be primary",                    [=](const g2t_track_st* t){
	std::string result          = UNKN;
	if ( t->eta ==-999 ) result = FAIL;
	if ( t->eta >=   0 ) result = PASS;
	return result;
      });
    check_track( Form("The track should have a eta=%f",_eta),      [=](const g2t_track_st* t){
	double delta = abs(t->eta-_eta);	
	return abs(t->eta-_eta)<1E-5 ?PASS:FAIL;      
    });
    check_track( "Expect 76 hits in the dev2021 geometry",         [=](const g2t_track_st* t){
      int n = t->n_tpc_hit;
      std::string  result = FAIL;
      if ( n==76 ) result = PASS;
      if ( n>=70 ) result = UNKN;
      result = Form(" n=%i ",n) + result;
      return result;
    });
  }
  


}
//___________________________________________________________________
