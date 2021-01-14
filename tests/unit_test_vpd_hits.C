#include "tests/unit_tests.h"
#include <assert.h>

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



  for ( double side = 1.0; side >= -1.0; side -= 2.0 ) 
  for ( auto point : pos ) {



    LOG_TEST << "======================================================================================" << std::endl;
    LOG_TEST << "Target VPD detector at " << point.x << " " << point.y << " " << point.z << std::endl;
    TVector3 p( point.x, point.y, point.z );

    double eta = _eta = side* p.Eta();
    double phi = p.Phi() * 180.0 / TMath::Pi();
    timer.Start();
    time( timer.CpuTime() );
    throw_muon( eta, phi, 5.0 );
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
    check_track( Form("The track should have an eta=%f",_eta),     [=](const g2t_track_st* t){
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


  };


};
//___________________________________________________________________
