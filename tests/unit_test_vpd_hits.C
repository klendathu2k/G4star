#include "tests/unit_tests.h"
#include <assert.h>


//___________________________________________________________________
std::string check_vpd_hit( std::string message, const g2t_vpd_hit_st* hit, std::function<std::string(const g2t_vpd_hit_st*)> f) {
  std::string result = "\u001b[37m [" + message + "] " + (hit? f(hit):FAIL);
  LOG_TEST << result << std::endl;
  return result;
};


// TODO: Implement test of prompt-hits

#ifndef __CINT__
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/error_of.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>
using namespace boost::accumulators;

using Accumulator_t = accumulator_set<double, 
stats< tag::count, 
       tag::sum,
       tag::mean, 
       tag::median(with_p_square_quantile),
       tag::max, 
       tag::min, 
       tag::error_of<tag::mean>
>>;
#endif

#include <TVector3.h>



//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;

//______________________________________________________________________

struct Vec_t {
  double x, y, z;
};

void unit_test_vpd_hits( int longtest=0 ) {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of tracks and TPC hits on single muons"     << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  Accumulator_t edep; // Energy deposition
  Accumulator_t step; // Step size
  Accumulator_t time; // Time per throw

  Vec_t pos[] = {  
    {6.6656,  -7.6678,  564.1075},
    {9.6627,  -3.1396,  564.1075},
    {9.8996,  2.2855,  564.1075},
    {7.3085,  7.0577,  564.1075},
    {2.6296,  9.8138,  564.1075},
    {-2.8005,  9.7664,  564.1075},
    {-7.4306,  6.9291,  564.1075},
    {-9.9380,  2.1124,  564.1075},
    {-9.6065,  -3.3078,  564.1075},
    {-6.5307,  -7.7830,  564.1075},
    {11.9141,  -7.8858,  564.1075},
    {14.2739,  -0.6232,  564.1075},
    {12.5561,  6.8174,  564.1075},
    {7.2515,  12.3105,  564.1075},
    {-0.1247,  14.2870,  564.1075},
    {-7.4652,  12.1821,  564.1075},
    {-12.6732,  6.5972,  564.1075},
    {-14.2609,  -0.8722,  564.1075},
    {-11.7747,  -8.0925,  564.1075}
  };


  for ( double side = 1.0; side >= -1.0; side -= 2.0 ) {
    int detector=0; // detector copy number
  for ( auto point : pos ) {



    detector++;

    LOG_TEST << "======================================================================================" << std::endl;
    LOG_TEST << "Target VPD detector at " << point.x << " " << point.y << " " << point.z << std::endl;
    TVector3 p( point.x, point.y, point.z );

    double eta = _eta = side* p.Eta();
    double phi = p.Phi() * 180.0 / TMath::Pi();
    timer.Start();
    time( timer.CpuTime() );
    throw_muon( eta, phi, 0.35 );
    timer.Reset();

    auto* chain = StMaker::GetChain();
    vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("g2t_vertex")  );
    track_table  = dynamic_cast<TTable*>( chain->GetDataSet("g2t_track")   );
    hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("g2t_vpd_hit") ) ;

    check_track( "A muon must have been processed by geant",       [=](const g2t_track_st* t){
	std::string result = "";
	return result+PASS; 
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
    check_track( Form("The track should have the correct  eta"),     [=](const g2t_track_st* t){
	double delta = abs(t->eta-_eta);	
	return abs(t->eta-_eta)<1E-5 ?PASS:FAIL;      
    });
    check_track( "Expect 1 hit in the VPDD (dev2021 geometry)",   [=](const g2t_track_st* t){
      int n = t->n_vpd_hit;
      std::string  result = FAIL;
      if ( n==1 ) result = PASS;
      result = Form(" n=%i ",n) + result;
      return result;
    });    


    for ( int i=0;i<hit_table->GetNRows();i++ ) {

      auto hit = static_cast<const g2t_vpd_hit_st*>( hit_table->At(i) );
      if ( 0==hit ) continue;     // skip null entries
      if ( 1!=hit->track_p ) continue; // not interested in secondaries

      LOG_TEST << "------------------------------------------------------------------" << std::endl;
      LOG_TEST << GIVEN << "A hit on that track" << std::endl;


      check_vpd_hit( "The hit should have a nonzero volume_id",                    hit,[=](const g2t_vpd_hit_st* h) {
       	  std::string result = FAIL;
          if ( h->volume_id > 0 ) result = PASS;
     	  result = Form("id=%i vid=%i de=%f ds=%f ",h->id,h->volume_id,h->de,h->ds) + result;
     	  return result;
     	});

      check_vpd_hit( "The hit should have an energy deposit > 0",                  hit,[=](const g2t_vpd_hit_st* h) {
       	  std::string result = FAIL; // undetermined
	  if       ( h->de > 0 ) result = PASS;
	  return result;
	});
      check_vpd_hit( "The hit should have a step size > 0",                        hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->ds > 0 ) result = PASS;
	  return result;
	});
      check_vpd_hit( "The hit should have a path length > 0",                      hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->s_track > 0 ) result = PASS;
	  return result;
	});
      check_vpd_hit( "The hit should have time-of-flight > 0",                     hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->tof > 0 ) result = PASS;
	  result = Form(" tof=%f ",h->tof) + result;
	  return result;
	});
      check_vpd_hit( "The hit should have a nonzero momentum",                     hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->p[0] != 0 ) result = PASS;
	  if ( h->p[1] != 0 ) result = PASS;
	  if ( h->p[2] != 0 ) result = PASS;
	  return result;
	});
      check_vpd_hit( "The hit should have a length > 0",                           hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  if ( h->s_track > 0 ) result = PASS;
	  return result;
	});
      check_vpd_hit( "The track length and tof*c agree to w/in 0.15 mm ",          hit,[=](const g2t_vpd_hit_st* h) {
	  // There should be some tolerance on this, b/c of roundoff error at each tracking step
	  std::string result = FAIL;
	  double c_tof = 2.99792458E10 /* cm/s */ * h->tof;
	  double s_trk = h->s_track;
	  double diff = abs(c_tof-s_trk);
	  if ( diff < 0.015 ) result = PASS;
	  result = Form("c_tof=%f cm  strack=%f cm diff=%f cm ",c_tof,s_trk,diff) + result;
	  return result;
	});
      // check_vpd_hit( "The hit be at a radius > 207.8",                             hit,[=](const g2t_vpd_hit_st* h) {
      // 	  std::string result = FAIL;
      // 	  double R2 = h->x[0]*h->x[0] + h->x[1]*h->x[1];
      // 	  if ( R2 > 207.8*207.8 ) result = PASS;
      // 	  result = Form(" R=%f ",sqrt(R2)) + result;
      // 	  return result;
      // 	});
      // check_vpd_hit( "The hit be at a radius < 221.0",                             hit,[=](const g2t_vpd_hit_st* h) {
      // 	  std::string result = FAIL;
      // 	  double R2 = h->x[0]*h->x[0] + h->x[1]*h->x[1];
      // 	  if ( R2 < 221.0*221.0 ) result = PASS;
      // 	  result = Form(" R=%f ",sqrt(R2)) + result;
      // 	  return result;
      // 	});
      check_vpd_hit( "The track length at the hit should be >= radius at the hit", hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  double R2 = h->x[0]*h->x[0] + h->x[1]*h->x[1];
	  double L2 = h->s_track * h->s_track;
	  if ( R2 <= L2 ) result = PASS;
	  result = Form(" R=%f L=%f",sqrt(R2),sqrt(L2)) + result;
	  return result;
	});
      check_vpd_hit( "Track's momentum at hit should be < initial value",          hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  double px = h->p[0];
	  double py = h->p[1];
	  double pz = h->p[2];
	  double p2 = px*px + py*py + pz*pz;
	  if ( p2 < _pmom*_pmom ) result = PASS;
	  return result;
	});

      // volume_id  =  1000*rileft + 100*innout + sector

      int expected_rileft = (_eta>0)? 1 : 2;
      check_vpd_hit( Form("VPD hit should be on side %i",expected_rileft),    hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  int rileft = h->volume_id / 1000;
	  result = ( rileft==expected_rileft) ? PASS : FAIL;
	  return result;
	});

      check_vpd_hit( Form("VPD hit should be on expected detector"),    hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  int det = h->volume_id %100;
	  result = ( detector==det) ? PASS : FAIL;
	  return result;
	});

      // Following tests are only for "primary hits"

      if ( hit->s_track <= 500 ) continue;

      check_vpd_hit( Form("VPD hit should be on side %i {s_track>500}",expected_rileft),    hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  int rileft = h->volume_id / 1000;
	  result = ( rileft==expected_rileft) ? PASS : FAIL;
	  return result;
	});

      check_vpd_hit( Form("VPD hit should be on expected detector {s_track>500}"),    hit,[=](const g2t_vpd_hit_st* h) {
	  std::string result = FAIL;
	  int det = h->volume_id %100;
	  result = ( detector==det) ? PASS : FAIL;
	  return result;
	});



    }    

  };// loop over detectors
  }


};
//___________________________________________________________________
