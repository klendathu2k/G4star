#include "tests/unit_tests.h"
#include <assert.h>

//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;
//___________________________________________________________________





void unit_test_tracks() {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of tracks "     << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  throw_particle( "e+", 0.4251, 3.1415/4, 10.0 );
  //  throw_particle( "e+", 0.3125, 3.1415/4+0.125, 10.0 );

  vertex_table = dynamic_cast<TTable*>( chain->GetDataSet("bfc/.make/geant4star/.data/g2t_vertex")  );
  track_table  = dynamic_cast<TTable*>( chain->GetDataSet("bfc/.make/geant4star/.data/g2t_track")   );
  hit_table    = dynamic_cast<TTable*>( chain->GetDataSet("bfc/.make/geant4star/.data/g2t_tpc_hit") ) ;

  // check_track( "Print the track table",                                            [=](const g2t_track_st* t){
  //     track_table->Print(0,10);
  //     return PASS; 
  //   });
  // check_track( "Print the vertex table",                                           [=](const g2t_track_st* t){
  //     vertex_table->Print(0,10);
  //     return PASS; 
  //   });

  for ( int idx=0;idx<track_table->GetNRows();idx++ ) {

  check_track( "A particle must have been processed by geant",                     [=](const g2t_track_st* t){
      LOG_TEST << "-----------------------------------------------------------" << std::endl;
      std::string result = Form("particle id = %i",t->eg_pid);
      return result + PASS; 
    }, idx);
  check_track( "The track should have a start vertex",                             [=](const g2t_track_st* t){
      return (t->start_vertex_p>0)?PASS:FAIL;      
    }, idx);
  check_track( "The track should have a stop vertex",                              [=](const g2t_track_st* t){
      return (t->stop_vertex_p>0)?PASS:FAIL;      
    }, idx);
  check_track( "The start vertex should be in the vertex table",                   [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istart = t->start_vertex_p;
      const g2t_vertex_st* vertex = (istart>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) ) : 0;
      if ( vertex ) {
	result = PASS;
      }
      return result;
    }, idx);  
  check_track( "The stop vertex should be in the vertex table",                    [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istart = t->stop_vertex_p;
      const g2t_vertex_st* vertex = (istart>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) ) : 0;
      if ( vertex ) {
	result = PASS;
      }
      return result;
    }, idx);
  check_track( "The id of the START vertex is nonzero",                            [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istart = t->start_vertex_p;
      if ( istart > 0 ) result = PASS;      
      return result;
    }, idx);  
  check_track( "The START vertex records a valid medium",                           [=](const g2t_track_st* t){
      auto result = FAIL;
      int istart = t->start_vertex_p;
      const g2t_vertex_st* vertex = (istart>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) ) : 0;
      if ( vertex ) {
	if ( vertex->ge_medium>0 ) result = PASS;
      }
      return result;

    }, idx);
  check_track( "The START vertex records a valid process",                          [=](const g2t_track_st* t){
      auto result = FAIL;
      int istart = t->start_vertex_p;
      const g2t_vertex_st* vertex = (istart>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) ) : 0;
      if ( vertex ) {
	//	vertex_table->Print(istart-1,1);
	result = Form(" (ge_proc=%i %s)", vertex->ge_proc, TMCProcessName[vertex->ge_proc] );
	if ( vertex->ge_proc > 0 && vertex->ge_proc < 44 ) result += PASS;
	else                                               result += FAIL;
      }
      else 
	result = Form("No start vertex on track") + result;

      return result;
    }, idx);
  check_track( "The id of the START vertex is less than the id of the STOP vetex", [=](const g2t_track_st* t){
      std::string result = FAIL;
      int istart = t->start_vertex_p;
      int istop  = t->stop_vertex_p;
      if ( istart > 0 && istop > istart ) result = PASS;      
      return result;
    }, idx);

  check_track( "The STOP vertex records a valid medium",                           [=](const g2t_track_st* t){
      auto result = FAIL;
      int istop = t->stop_vertex_p;
      const g2t_vertex_st* vertex = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
      if ( vertex ) {
	if ( vertex->ge_medium>0 ) result = PASS;
      }
      return result;

    }, idx);
  check_track( "The STOP vertex records a valid process",                          [=](const g2t_track_st* t){
      auto result = FAIL;
      int istop = t->stop_vertex_p;
      const g2t_vertex_st* vertex = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
      if ( vertex ) {
	//	vertex_table->Print(istop-1,1);
	result = Form(" (ge_proc=%i %s)", vertex->ge_proc, TMCProcessName[vertex->ge_proc] );
	if ( vertex->ge_proc > 0 && vertex->ge_proc < 44 ) result += PASS;
	else                                               result += FAIL;
      }
      else 
	result = Form("No stop vertex on track") + result;

      return result;
    }, idx);

  if ( idx==0 ) continue; 

  // check_track( "Print the STOP vertex",                                    [=](const g2t_track_st* t){
  //     auto result = FAIL;
  //     int istop = t->stop_vertex_p;
  //     const g2t_vertex_st* vertex = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
  //     if ( vertex ) {
  // 	vertex_table->Print(istop-1,1);
  //     }
  //     else 
  // 	result = Form("No stop vertex on track") + result;

  //     return result;
  //   }, idx);
  
  
  
  }


  // check_track( "The STOP vertex has daughter tracks",                              [=](const g2t_track_st* t){
  //     auto result = FAIL;
  //     int istop = t->stop_vertex_p;
  //     const g2t_vertex_st* vertex = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
  //     if ( vertex ) {
  // 	if ( vertex->n_daughter>0 ) result = PASS;
  //     }
  //     return result;

  //   });
  // check_track( "The STOP vertex has pointer to first daughter",                    [=](const g2t_track_st* t){
  //     auto result = FAIL;
  //     int istop = t->stop_vertex_p;
  //     const g2t_vertex_st* vertex = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
  //     if ( vertex ) {
  // 	if ( vertex->daughter_p>0 ) result = PASS;
  //     }
  //     return result;

  //   });


  // check_track( "Print daughters associated with the STOP vertex",                   [=](const g2t_track_st* t){
  //     auto result = PASS;
  //     int istop = t->stop_vertex_p;
  //     const g2t_vertex_st* vertex = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
  //     if ( vertex ) {
  // 	vertex_table->Print(istop-1,1);
  // 	int ntrack = track_table->GetNRows();
  // 	for ( int itrack=0;itrack<ntrack;itrack++ ) {
  // 	  const g2t_track_st* track = static_cast<const g2t_track_st*>( track_table->At(itrack) );
  // 	  if ( track->start_vertex_p == istop ) {
  // 	    track_table->Print(itrack,1);
  // 	  }
  // 	}
	
  //     }

  //     return result;
  //   });

  // check_track( "All tracks have a start vertex",                      [=](const g2t_track_st* t){
  //     std::string result = FAIL;

  //     std::map<int, std::vector<int> > vertex2daughters; // v2d[ivertex]=vector of daughters
  //     int ntracks = track_table->GetNRows();
  //     for ( int itrack=0;itrack<ntracks;itrack++ ) {
  // 	const g2t_track_st* track = static_cast<const g2t_track_st*>(track_table->At(itrack));
  // 	if ( 0==track ) continue;
  // 	int idtruth = track->id;
  // 	int idstart = track->start_vertex_p;
  // 	vertex2daughters[idstart].push_back( idtruth );
  //     }

  //     for (auto const& vd : vertex2daughters ) {
  // 	std::cout << "vertex id=" << vd.first << " daughters=";
  // 	for ( auto const& d : vd.second ) {
  // 	  std::cout << d << " ";
  // 	}
  // 	std::cout << std::endl;
  //     }
     
  //     return result;

  //   });




  
  // for ( int sector=1; sector<=24; sector++ ) {

  //   throw_muon_in_tpc_sector( sector );

  //   LOG_TEST << "Checking muon track in sector " << sector << std::endl;
  //   check_track( "A muon must have been processed by geant",       [=](const g2t_track_st* t){
  // 	// Failure is tested by check_track when it tests for a valid track pointer
  // 	return PASS; 
  //     });
  //   check_track( "The track should have a start vertex",           [=](const g2t_track_st* t){
  //     return (t->start_vertex_p>0)?PASS:FAIL;      
  //   });
  //   check_track( "The start vertex should be in the vertex table", [=](const g2t_track_st* t){
  //     std::string result = FAIL;
  //     int istart = t->start_vertex_p;
  //     const g2t_vertex_st* vertex = (istart>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) ) : 0;
  //     if ( vertex ) {
  // 	result = PASS;
  //     }
  //     return result;
  //   });
  //   check_track( "There should not be a stop vertex in the TPC",   [=](const g2t_track_st* t){
  //     std::string result = FAIL;
  //     int istop = t->stop_vertex_p;
  //     const g2t_vertex_st* v = (istop>0) ? static_cast<const g2t_vertex_st*>( vertex_table->At(istop-1) ) : 0;
  //     if ( 0==v ) 
  // 	result = PASS;
  //     else {
  // 	double x1 = v->eg_x[0];
  // 	double y1 = v->eg_x[1];
  // 	double z1 = v->eg_x[2];
  // 	double x2 = v->ge_x[0];
  // 	double y2 = v->ge_x[1];
  // 	double z2 = v->ge_x[2];
  // 	double xx1 = sqrt(x1*x1+y1*y1); // event generator
  // 	double xx2 = sqrt(x2*x2+y2*y2); // geant vertex
  // 	if ( xx1 > 208.0 || xx2 > 208.0 ) 
  // 	  result = PASS;
  // 	result = Form(" r=%f ",TMath::Max(xx1,xx2)) + result;
  //     }
  //     return result;
  //   });
  //   check_track( "The start vertex should be on the z-axis",       [=](const g2t_track_st* t){
  //     std::string result = FAIL;
  //     int istart = t->start_vertex_p;

  //     const g2t_vertex_st* v = 0;
  //     if ( istart > 0 ) 
  // 	v = static_cast<const g2t_vertex_st*>( vertex_table->At(istart-1) );
  //     else 
  // 	result = " no vertex in table " + result;

  //     if ( v ) {
  // 	double x1 = v->eg_x[0];
  // 	double y1 = v->eg_x[1];
  // 	double z1 = v->eg_x[2];
  // 	double x2 = v->ge_x[0];
  // 	double y2 = v->ge_x[1];
  // 	double z2 = v->ge_x[2];
  // 	double xx1 = sqrt(x1*x1+y1*y1); // event generator
  // 	double xx2 = sqrt(x2*x2+y2*y2); // geant vertex
  // 	if ( xx1 > 0.0001 ) {
  // 	  result = Form(" EG: %f %f %f ",x1,y1,z1) + result;
  // 	} 
  // 	if ( xx2 > 0.0001 ) {
  // 	  result = Form(" GE: %f %f %f ",x2,y2,z2) + result;
  // 	}
  // 	if ( xx1 < 0.0001 && xx2 < 0.0001 ) {
  // 	  result = PASS;
  // 	}	
  //     }
       
  //     return result;
  //   });
  //   check_track( "The track should be primary",                    [=](const g2t_track_st* t){
  // 	std::string result          = PASS;
  // 	if ( t->eta ==-999 ) result = FAIL;
  // 	return result;
  //     });
  //   check_track( Form("The track should have an eta=%f",_eta),     [=](const g2t_track_st* t){
  // 	double delta = abs(t->eta-_eta);	
  // 	return abs(t->eta-_eta)<1E-5 ?PASS:FAIL;      
  //   });
  //   check_track( "Expect 76 hits in the dev2021 geometry",         [=](const g2t_track_st* t){
  //     int n = t->n_tpc_hit;
  //     std::string  result = FAIL;
  //     if ( n==76 ) result = PASS;
  //     result = Form(" n=%i ",n) + result;
  //     return result;
  //   });

  //   LOG_TEST << "Checking hits on track in sector " << sector << std::endl;
  //   for ( int i=0;i<hit_table->GetNRows();i++ ) {
  //     auto hit = static_cast<const g2t_tpc_hit_st*>( hit_table->At(i) );
  //     if ( 0==hit ) continue;     // skip null entries
  //     if ( 1!=hit->track_p ) continue; // not interested in secondaries
  //     check_tpc_hit( "Print the hit...", hit, [=](const g2t_tpc_hit_st* h) {
  // 	  LOG_TEST << "id=" << h->id 
  // 		   << " track_p=" << h->track_p 
  // 		   << " volume_id=" << h->volume_id 
  // 		   << " x="  << h->x[0] 
  // 		   << " y="  << h->x[1] 
  // 		   << " z="  << h->x[2] 	    
  // 		   << std::endl;
  // 	  return PASS;
  // 	});
  //     check_tpc_hit( "The hit should have a nonzero volume_id",hit,[=](const g2t_tpc_hit_st* h) {
  // 	  std::string result = FAIL;
  // 	  if ( h->volume_id > 0 ) result = PASS;
  // 	  return result;
  // 	});
  //     check_tpc_hit( "The hit should have an energy deposit > 0",hit,[=](const g2t_tpc_hit_st* h) {
  // 	  std::string result = FAIL;
  // 	  if ( h->de > 0 ) result = PASS;
  // 	  return result;
  // 	});
  //     check_tpc_hit( "The hit should have a path length > 0",hit,[=](const g2t_tpc_hit_st* h) {
  //      	  std::string result = FAIL;
  //      	  if ( h->ds > 0 ) result = PASS;
  //      	  return result;
  //      	});
  //     check_tpc_hit( "The hit should have a nonzero momentum",hit,[=](const g2t_tpc_hit_st* h) {
  //      	  std::string result = FAIL;
  //      	  if ( h->p[0] != 0 ) result = PASS;
  //      	  if ( h->p[1] != 0 ) result = PASS;
  //      	  if ( h->p[2] != 0 ) result = PASS;
  //      	  return result;
  //      	});
  //     check_tpc_hit( "The hit should have a nonzero log10(gamma)",hit,[=](const g2t_tpc_hit_st* h) {
  //      	  std::string result = FAIL;
  //      	  if ( h->lgam != 0 ) result = PASS;
  // 	  result = Form(" lgam=%f (needs to be filled) ",h->lgam ) + result;
  //      	  return result;
  //      	});
  //     check_tpc_hit( "The hit should have a length > 0",hit,[=](const g2t_tpc_hit_st* h) {
  //      	  std::string result = FAIL;
  //      	  if ( h->length > 0 ) result = PASS;
  //      	  return result;
  //      	});
  //     check_tpc_hit( "The hit should have adc, pad and timebucket set to zero",hit,[=](const g2t_tpc_hit_st* h) {
  //      	  std::string result = PASS;
  // 	  if ( h->adc > 0 ) result = FAIL;
  // 	  if ( h->pad > 0 ) result = FAIL;
  // 	  if ( h->timebucket > 0 ) result = FAIL;
  // 	  return result;
  // 	});
  //     check_tpc_hit( "Track's momentum at hit should be < initial value",hit,    [=](const g2t_tpc_hit_st* h){
  // 	  std::string result = FAIL;
  // 	  double px = h->p[0];
  // 	  double py = h->p[1];
  // 	  double pz = h->p[2];
  // 	  double p2 = px*px + py*py + pz*pz;
  // 	  if ( p2 < _pmom*_pmom ) result = PASS;
  // 	  return result;
  // 	});
  //     check_tpc_hit( "Hit position should be w/in the fiducial volume of the sector",hit,[=](const g2t_tpc_hit_st* h){
  // 	  // TODO
  // 	  return TODO;
  // 	});
  //     check_tpc_hit( "The padrow should be 1 <= pad <= 72",hit,[=](const g2t_tpc_hit_st* h) {
  // 	  std::string result = PASS;
  // 	  int padrow = h->volume_id % 100;
  // 	  if ( padrow<1 || padrow > 72 ) result=FAIL;
  // 	  return result;
  // 	});
  //     check_tpc_hit( "The sector should be 1 <= sector <= 24",hit,[=](const g2t_tpc_hit_st* h) {
  // 	  std::string result = PASS;
  // 	  int sector = ( h->volume_id / 100 ) % 1000;
  // 	  if ( sector<1 || sector>24 ) result=FAIL;
  // 	  return result;
  // 	});
  //     check_tpc_hit( "The detector state is in (0,1,2)",hit,[=](const g2t_tpc_hit_st* h) {
  // 	  std::string result = PASS;
  // 	  int det = h->volume_id/100000;
  // 	  if ( det<0||det>2 ) result = FAIL;
  // 	  return result;
  // 	});
  //     check_tpc_hit( Form("The decoded sector number should be %i",sector),hit,[=](const g2t_tpc_hit_st* h) {
  // 	std::string result = FAIL;
  // 	int _sector = ( h->volume_id / 100 ) % 1000;
  // 	if ( sector == _sector ) result = PASS;
  // 	result = Form(" sector=%i",_sector ) + result;
  // 	return result;
  //     });
  //   }


  // }
  


}
//___________________________________________________________________
