#include "tests/unit_tests.h"
const int kEEmcNumDepths     =  5;
const int kEEmcNumSectors    = 12; 
const int kEEmcNumSubSectors =  5; 
const int kEEmcNumEtas       = 12; 
const int kEEmcNumSmdPlanes  =  3; 
const int kEEmcNumStrips     =288;  
const int kEEmcNumEdgeStrips =283;
const int kEEmcNumSmdUVs  =  2; 

const double etabounds[] = {  2.0000 ,
			      1.9008 , 1.8065 , 1.7168 , 1.6317 , 1.5507 , 1.4738 ,
			      1.4007 , 1.3312 , 1.2651 , 1.2023 , 1.1427 , 1.086                             
};
//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;
//___________________________________________________________________
void throw_muon_in_eemc_tower( double eta, double phid, int charge = 1 ) {

  throw_muon( eta, phid, 500.0, charge ); // energetic
  _eta = eta;
  _phid = phid;

  StMaker::GetChain()->ls(10);

  assert( vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("g2t_vertex")  ) );
  assert( track_table  = dynamic_cast<TTable*>( chain->GetDataSet("g2t_track")   ) );
  assert( hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("g2t_eem_hit") ) );

}
//___________________________________________________________________
void unit_test_eem_hits() {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of tracks and EEMC hits on single muons"     << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  //  double eta  = 0.5 * ( etabounds[3]+etabounds[4] );
  //  double phid = 6.0 * 5;

  for ( int i=0;i<12;i++ ) { 
    for ( int j=0;j<60;j++ ) {

      double eta=0.5*(etabounds[i]+etabounds[i+1]);
      double phid = 6.0*j;

      LOG_TEST << "------------------------------------------------------------------" << std::endl;
      LOG_TEST << "Throwing muon at tower center eta=" << eta << " phi=" << phid << std::endl;
      LOG_TEST << "------------------------------------------------------------------" << std::endl;
		      
      throw_muon_in_eemc_tower( eta, phid );
  
      check_track( "Print the track table", [=]( const g2t_track_st* ){
      int nrows = track_table->GetNRows();
      std::string result = FAIL;
      if ( nrows > 0 ) {
	track_table->Print(0,5);
	result = PASS;
      }
      return result;
    });
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
      int n = t->n_eem_hit;
      std::string  result = FAIL;
      if ( n==4 ) result = PASS;
      result = Form(" n=%i ",n) + result;
      return result;
    }); 
      //  LOG_TEST << "Checking hits on track in sector " << sector << std::endl;
      for ( int i=0;i<hit_table->GetNRows();i++ ) {
    auto hit = static_cast<const g2t_emc_hit_st*>( hit_table->At(i) );
    if ( 0==hit ) continue;     // skip null entries
    if ( 1!=hit->track_p ) continue; // not interested in secondaries
    check_emc_hit( "Print the hit...", hit, [=](const g2t_emc_hit_st* h) {
	LOG_TEST << "id=" << h->id 
		 << " track_p=" << h->track_p 
		 << " volume_id=" << h->volume_id 
		 << " de="  << h->de 
		 << std::endl;
	return PASS;
      });
    check_emc_hit( "The hit should have a nonzero volume_id",hit,[=](const g2t_emc_hit_st* h) {
	std::string result = FAIL;
	if ( h->volume_id > 0 ) result = PASS;
	return result;
      });
    check_emc_hit( "The hit should have an energy deposit > 0",hit,[=](const g2t_emc_hit_st* h) {
	std::string result = FAIL;
	if ( h->de > 0 ) result = PASS;
	return result;
      });
      
  }

    }
  }

}
