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
#define LOG_TEST LOG_QA << " -test- "
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
  particle->SetPz(momentum[2]);
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
void throw_muon_in_sector( int sectorid, int charge = 1 ) {
  assert(sectorid>0 && sectorid <= 24);
  //  const double sectors[] = { 15.0, 45.0, 75.0, 105.0, 135.0, 165.0, 195.0, 225.0, 255.0, 285.0, 315.0, 345.0 };
  const double sectors[] = { 
    60.0, 30.0, 0.0, 330.0, 300.0, 270., 240.0, 210.0, 180.0, 150.0, 120.0, 90.0,
    60.0, 30.0, 0.0, 330.0, 300.0, 270., 240.0, 210.0, 180.0, 150.0, 120.0, 90.0,
  };
  double eta = (sectorid<=12) ? 0.5 : -0.5;
  _eta = eta;
  double phid = sectors[sectorid-1];
  _phid = phid; 
  throw_muon( eta, phid, 500.0, charge ); // energetic
}
//___________________________________________________________________
std::string check_track( std::string message, std::function<std::string(const g2t_track_st*)> f) {

  const g2t_track_st* track = static_cast<const g2t_track_st*>( track_table->At(0) );  
  std::string result = "[" + message + "] " + (track? f(track):FAIL );

  LOG_TEST << result << endm;

  return result;

};
//___________________________________________________________________
std::string check_hit( std::string message, const g2t_tpc_hit_st* hit, std::function<std::string(const g2t_tpc_hit_st*)> f) {
  std::string result = "[" + message + "] " + (hit? f(hit):FAIL);
  LOG_TEST << result << endm;
  return result;
};


//___________________________________________________________________
void unit_test_tpc_hits() {
  initChain(); 
  _primary->SetVertex(0.,0.,0.);
  _primary->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << endm;
  LOG_TEST << "Unit testing of tracks and TPC hits on single muons"     << endm;
  LOG_TEST << "=======================================================" << endm;
  
  for ( int sector=1; sector<=1; sector++ ) {

    throw_muon_in_sector( sector );

    LOG_TEST << "Checking track in sector " << sector << endm;
    // check_track( "Print the track table", [=]( const g2t_track_st* ){
    // 	int nrows = track_table->GetNRows();
    // 	std::string result = FAIL;
    // 	if ( nrows > 0 ) {
    // 	  track_table->Print(0,5);
    // 	  result = PASS;
    // 	}
    // 	return result;
    //   });
    check_track( "A muon must have been processed by geant",       [=](const g2t_track_st* t){
	// Failure is tested by check_track when it tests for a valid track pointer
	return PASS; 
      });
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
	std::string result          = PASS;
	if ( t->eta ==-999 ) result = FAIL;
	return result;
      });
    check_track( Form("The track should have an eta=%f",_eta),     [=](const g2t_track_st* t){
	double delta = abs(t->eta-_eta);	
	return abs(t->eta-_eta)<1E-5 ?PASS:FAIL;      
    });
    check_track( "Expect 76 hits in the dev2021 geometry",         [=](const g2t_track_st* t){
      int n = t->n_tpc_hit;
      std::string  result = FAIL;
      // if ( n>=70 ) result = UNKN;
      if ( n==76 ) result = PASS;
      result = Form(" n=%i ",n) + result;
      return result;
    });

    LOG_TEST << "Checking hits on track in sector " << sector << endm;
    for ( int i=0;i<hit_table->GetNRows();i++ ) {
      auto hit = static_cast<const g2t_tpc_hit_st*>( hit_table->At(i) );
      if ( 0==hit ) continue;     // skip null entries
      if ( 1!=hit->track_p ) continue; // not interested in secondaries
      check_hit( "Print the hit...", hit, [=](const g2t_tpc_hit_st* h) {
	  LOG_TEST << "id=" << h->id 
		   << " track_p=" << h->track_p 
		   << " volume_id=" << h->volume_id 
		   << " x="  << h->x[0] 
		   << " y="  << h->x[1] 
		   << " z="  << h->x[2] 	    
		   << endm;
	  return PASS;
	});
      check_hit( "The hit should have a nonzero volume_id",hit,[=](const g2t_tpc_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->volume_id > 0 ) result = PASS;
	  return result;
	});
      check_hit( "The hit should have an energy deposit > 0",hit,[=](const g2t_tpc_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->de > 0 ) result = PASS;
	  return result;
	});
      // check_hit( "The hit should have a path length > 0",hit,[=](const g2t_tpc_hit_st* h) {
      // 	  std::string result = FAIL;
      // 	  if ( h->ds > 0 ) result = PASS;
      // 	  return result;
      // 	});
      // check_hit( "The hit should have a nonzero momentum",hit,[=](const g2t_tpc_hit_st* h) {
      // 	  std::string result = FAIL;
      // 	  if ( h->p[0] != 0 ) result = PASS;
      // 	  if ( h->p[1] != 0 ) result = PASS;
      // 	  if ( h->p[2] != 0 ) result = PASS;
      // 	  return result;
      // 	});
      // check_hit( "The hit should have a nonzero log10(gamma)",hit,[=](const g2t_tpc_hit_st* h) {
      // 	  std::string result = FAIL;
      // 	  if ( h->lgam != 0 ) result = PASS;
      // 	  return result;
      // 	});

      // check_hit( "The hit should have a length > 0",hit,[=](const g2t_tpc_hit_st* h) {
      // 	  std::string result = FAIL;
      // 	  if ( h->length > 0 ) result = PASS;
      // 	  return result;
      // 	});

      // check_hit( "The hit should have adc, pad and timebucket set to zero",hit,[=](const g2t_tpc_hit_st* h) {
      // 	  std::string result = PASS;
      // 	  if ( h->adc > 0 ) result = FAIL;
      // 	  if ( h->pad > 0 ) result = FAIL;
      // 	  if ( h->timebucket > 0 ) result = FAIL;
      // 	  return result;
      // 	});

      //  g2t_tpc_volume_id = 100000*det + 100*sector + pad

      check_hit( "The padrow should be 1 <= pad <= 72",hit,[=](const g2t_tpc_hit_st* h) {
	  std::string result = PASS;
	  int padrow = h->volume_id % 100;
	  if ( padrow<1 || padrow > 72 ) result=FAIL;
	  return result;
	});
      check_hit( "The sector should be 1 <= sector <= 24",hit,[=](const g2t_tpc_hit_st* h) {
	  std::string result = PASS;
	  int sector = ( h->volume_id / 100 ) % 1000;
	  if ( sector<1 || sector>24 ) result=FAIL;
	  return result;
	});
      check_hit( "The detector state is in (0,1,2)",hit,[=](const g2t_tpc_hit_st* h) {
	  std::string result = PASS;
	  int det = h->volume_id/100000;
	  if ( det<0||det>2 ) result = FAIL;
	  return result;
	});
      check_hit( Form("The decoded sector number should be %i",sector),hit,[=](const g2t_tpc_hit_st* h) {
	std::string result = FAIL;
	int _sector = ( h->volume_id / 100 ) % 1000;
	if ( sector == _sector ) result = PASS;
	result = Form(" sector=%i",_sector ) + result;
	return result;
      });


    }


  }
  


}
//___________________________________________________________________
