#include "tests/unit_tests.h"

#include "StEpdUtil/StEpdGeom.h"

//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;
//___________________________________________________________________
void throw_muon_in_epd_tile( double eta, double phid, int charge = 1 ) {

  throw_muon( eta, phid, 10.0, charge ); // energetic
  _eta = eta;
  _phid = phid;

  auto* chain = StMaker::GetChain();
  vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("g2t_vertex")  );
  track_table  = dynamic_cast<TTable*>( chain->GetDataSet("g2t_track")   );
  hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("g2t_epd_hit") );

}

//___________________________________________________________________
void unit_test_epd_hits() {

  gROOT->ProcessLine("initChain();");

  StEpdGeom epd;

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of tracks and EPD hits on single muons"     << std::endl;
  LOG_TEST << "=======================================================" << std::endl;


  TVector3 direction;

  for ( int eastwest=-1; eastwest<=+1; eastwest+=2 ) {
    for ( int supersector=1; supersector<=12; supersector++ ) {
      for ( int tilenumber=1; tilenumber<=31; tilenumber++ ) {

      LOG_TEST << "------------------------------------------------------------------" << std::endl;
      LOG_TEST << "eastwest    = " << eastwest << std::endl;
      LOG_TEST << "supersector = " << supersector << std::endl;
      LOG_TEST << "tilenumber  = " << tilenumber << std::endl;
    
      direction = epd.TileCenter( supersector, tilenumber, eastwest );
     		      
      throw_muon_in_epd_tile( direction.Eta(), direction.Phi()*180.0/TMath::Pi() );
  
      check_track( "A muon must have been processed by geant",       [=](const g2t_track_st* t){
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
      check_track( "There should not be a stop vertex in the EPD",  [=](const g2t_track_st* t){
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
      check_track( "Expect 1 hit in the dev2021 geometry",          [=](const g2t_track_st* t){
	  int n = t->n_epd_hit;
	  std::string  result = FAIL;
	  if ( n==1 )  result = PASS;
	  result = Form(" n=%i ",n) + result;
	  return result;
	}); 


      LOG_TEST << "GetNRows = " << hit_table->GetNRows() << std::endl;

      for ( int i=0;i<hit_table->GetNRows();i++ ) {

       	auto hit = static_cast<const g2t_epd_hit_st*>( hit_table->At(i) );
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
       	    result = Form(" volumeId=%i ", h->volume_id ) + result;
       	    return result;
       	  });

	// 	check_emc_hit( "The hit's volume_id should be < 2*100000",hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    if ( h->volume_id < 200000 ) result = PASS;
      // 	    result = Form(" volumeId=%i ", h->volume_id ) + result;
      // 	    return result;
      // 	  });	
      // 	check_emc_hit( "The hit should be in half 1 or half 2",hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    int half = id.half;
      // 	    if ( half>=1 && half <=2 ) result = PASS;
      // 	    result = Form(" ... half=%i ", half ) + result;
      // 	    return result;
      // 	  });
      // 	check_emc_hit( "The sector of the hit should be 1-12",hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    int sector    = id.sector;
      // 	    if ( sector >= 1 && sector <= 12 ) result = PASS;
      // 	    result = Form(" ... sector=%i ", sector ) + result;
      // 	    return result;
      // 	  });
      // 	check_emc_hit( Form("The sector of the hit should be %i",sector),hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    if ( sector ==id.sector ) result = PASS;
      // 	    result = Form(" ... sector=%i ", id.sector ) + result;
      // 	    return result;
      // 	  });
      // 	check_emc_hit( Form("The sub-sector of the hit should be %i",subsector),hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    if ( id.subsector == subsector ) result = PASS;
      // 	    result = Form(" ... subsector=%i ", id.subsector ) + result;
      // 	    return result;
      // 	  });
      // 	check_emc_hit( Form("The phibin of the hit should be %i",phibin),hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    if ( id.phibin == phibin ) result = PASS;
      // 	    result = Form(" ... phibin=%i ", id.phibin ) + result;
      // 	    return result;
      // 	  });
      // 	check_emc_hit( Form("The etabin of the hit should be %i",etabin),hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    if ( etabin == id.etabin ) result = PASS;
      // 	    result = Form(" ... etabin=%i ", id.etabin ) + result;
      // 	    return result;
      // 	  });
      // 	check_emc_hit( "The depth of the hit should be 1-5",hit,[=](const g2t_emc_hit_st* h) {
      // 	    std::string result = FAIL;
      // 	    int volumeId = h->volume_id;
      // 	    EEmcVolumeId id = decode_eemc_volume_id( volumeId );
      // 	    int depth    = id.depth;

      // 	    if ( depth >= 1 && depth <= 5 ) result = PASS;
      // 	    result = Form(" ... depth=%i ", depth ) + result;
      // 	    return result;
      // 	  });
       	check_emc_hit( "The hit should have an energy deposit > 0",hit,[=](const g2t_emc_hit_st* h) {
       	    std::string result = FAIL;
	    if ( h->de > 0 ) result = PASS;
	    return result;
	  });
	
      }
      
      }

    }

  }



}
