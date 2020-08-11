#include "tests/unit_tests.h"

#include <vector>
//___________________________________________________________________
const int neta = 40;
const int nphi = 120;
const double dphi = 3.0;
const double phi0 = 0.0; // not really...

const double phibins1[] = { 
  // west side
  73.4199 , 70.5801 , 67.4199 , 64.5801 , 61.4199 , 58.5801 , 55.4199 , 52.5801 , 49.4199 , 46.5801 , 43.4199
  , 40.5801 , 37.4199 , 34.5801 , 31.4199 , 28.5801 , 25.4199 , 22.5801 , 19.4199 , 16.5801 , 13.4199 , 10.5801
  , 7.41987 , 4.58013 , 1.41987 , -1.41987 , -4.58013 , -7.41987 , -10.5801 , -13.4199 , -16.5801 , -19.4199 , -22.5801
  , -25.4199 , -28.5801 , -31.4199 , -34.5801 , -37.4199 , -40.5801 , -43.4199 , -46.5801 , -49.4199 , -52.5801 , -55.4199
  , -58.5801 , -61.4199 , -64.5801 , -67.4199 , -70.5801 , -73.4199 , -76.5801 , -79.4199 , -82.5801 , -85.4199 , -88.5801
  , -91.4199 , -94.5801 , -97.4199 , -100.58 , -103.42 , -106.58 , -109.42 , -112.58 , -115.42 , -118.58 , -121.42
  , -124.58 , -127.42 , -130.58 , -133.42 , -136.58 , -139.42 , -142.58 , -145.42 , -148.58 , -151.42 , -154.58
  , -157.42 , -160.58 , -163.42 , -166.58 , -169.42 , -172.58 , -175.42 , -178.58 , 178.58 , 175.42 , 172.58
 , 169.42 , 166.58 , 163.42 , 160.58 , 157.42 , 154.58 , 151.42 , 148.58 , 145.42 , 142.58 , 139.42
  , 136.58 , 133.42 , 130.58 , 127.42 , 124.58 , 121.42 , 118.58 , 115.42 , 112.58 , 109.42 , 106.58
  , 103.42 , 100.58 , 97.4199 , 94.5801 , 91.4199 , 88.5801 , 85.4199 , 82.5801 , 79.4199 , 76.5801 
};
const double phibins2[] = {
  // east side
  106.58 , 109.42 , 112.58 , 115.42 , 118.58 , 121.42 , 124.58 , 127.42 , 130.58 , 133.42 , 136.58
  , 139.42 , 142.58 , 145.42 , 148.58 , 151.42 , 154.58 , 157.42 , 160.58 , 163.42 , 166.58 , 169.42
  , 172.58 , 175.42 , 178.58 , -178.58 , -175.42 , -172.58 , -169.42 , -166.58 , -163.42 , -160.58
  , -157.42 , -154.58 , -151.42 , -148.58 , -145.42 , -142.58 , -139.42 , -136.58 , -133.42 , -130.58
  , -127.42 , -124.58 , -121.42 , -118.58 , -115.42 , -112.58 , -109.42 , -106.58 , -103.42 , -100.58
 , -97.4199 , -94.5801 , -91.4199 , -88.5801 , -85.4199 , -82.5801 , -79.4199 , -76.5801 , -73.4199 , -70.5801
  , -67.4199 , -64.5801 , -61.4199 , -58.5801 , -55.4199 , -52.5801 , -49.4199 , -46.5801 , -43.4199 , -40.5801
  , -37.4199 , -34.5801 , -31.4199 , -28.5801 , -25.4199 , -22.5801 , -19.4199 , -16.5801 , -13.4199 , -10.5801
  , -7.41986 , -4.58012 , -1.41986 , 1.41988 , 4.58014 , 7.41988 , 10.5801 , 13.4199 , 16.5801 , 19.4199
  , 22.5801 , 25.4199 , 28.5801 , 31.4199 , 34.5801 , 37.4199 , 40.5801 , 43.4199 , 46.5801 , 49.4199
  , 52.5801 , 55.4199 , 58.5801 , 61.4199 , 64.5801 , 67.4199 , 70.5801 , 73.4199 , 76.5801 , 79.4199
  , 82.5801 , 85.4199 , 88.5801 , 91.4199 , 94.5801 , 97.4199 , 100.58 , 103.42
};

// nominal boundaries... first and last eta bins are actually narrower
const std::vector<double> etabounds = {
  -1.00,-0.95, -0.90, -0.85, -0.80, -0.75, -0.70, -0.65, -0.60, -0.55, -0.50, -0.45, -0.40, -0.35, -0.30, -0.25, -0.20, -0.15, -0.10, -0.05, 0.00, 
   0.05, 0.10,  0.15,  0.20,  0.25,  0.30,  0.35,  0.40,  0.45,  0.50,  0.55,  0.60,  0.65,  0.70,  0.75,  0.80,  0.85,  0.90,  0.95,  1.00 
};

//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;
//___________________________________________________________________
void throw_muon_in_bemc_tower( double eta, double phid, int charge = 1 ) {

  throw_muon( eta, phid, 500.0, charge ); // energetic
  _eta = eta;
  _phid = phid;

  auto* chain = StMaker::GetChain();
  vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("g2t_vertex")  );
  track_table  = dynamic_cast<TTable*>( chain->GetDataSet("g2t_track")   );
  hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("g2t_emc_hit") );

}
//___________________________________________________________________
void unit_test_emc_hits() {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of tracks and BEMC hits on single muons"     << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  //  double eta  = 0.5 * ( etabounds[3]+etabounds[4] );
  //  double phid = 6.0 * 5;

  for ( int i=0;i<40;i++ ) { 
    double eta=0.5*(etabounds[i]+etabounds[i+1]);
    for ( int j=0;j<120;j++ ) {

      double phid;
      if ( eta > 0 ) phid = phibins1[j];
      else           phid = phibins2[j];

      LOG_TEST << "------------------------------------------------------------------" << std::endl;
      LOG_TEST << "Throwing muon at tower center eta=" << eta << " phi=" << phid << std::endl;
      LOG_TEST << "------------------------------------------------------------------" << std::endl;
		      
      throw_muon_in_bemc_tower( eta, phid );
  
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
      check_track( "There should not be a stop vertex in the EEMC",   [=](const g2t_track_st* t){
	  std::string result = TODO;
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
      check_track( "Expect 24 hits in the dev2021 geometry",          [=](const g2t_track_st* t){
	  int n = t->n_emc_hit;
	  std::string  result = FAIL;
	  if ( n==4 ) result = PASS;
	  result = Form(" n=%i ",n) + result;
	  return result;
	}); 

    }
  }

}
