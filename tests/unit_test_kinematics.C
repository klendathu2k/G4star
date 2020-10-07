#include "tests/unit_tests.h"

#include <TTable.h>

template<typename Add, typename Test>
std::string check_kine( std::string message, Add addfunc, Test testfunc ) {
  std::string af = addfunc();
  std::string tf = testfunc();
  std::string result = "\u001b[37m [" + message + "] " + "(" + af +") " + tf;
  gMessMgr->Info() << result << endm;
  return result;
};

void unit_test_kinematics() {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of paticle gun" << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  auto* chain = StMaker::GetChain();

  auto add_deuteron = [=]() {
    std::string result = "Deuteron";
    const double ptmn = 0.1;
    const double ptmx = 10.;
    const double etamn= -1.; 
    const double etamx= +4.;
    auto* _kine = dynamic_cast<StarKinematics*>( chain->GetMaker("StarKine") );
    LOG_INFO << "Add function called" << endm;
    _kine->Clear();
    _kine->Kine(1,"D",ptmn,ptmx,etamn,etamx);
    pm->Make();
    pm->event()->Print();
    return result;
  };
  
  check_kine( "After adding a particle to the generator, a single particle appears in the event", add_deuteron, [=]() {
      std::string result = FAIL;
      int np = _kine->GetNumberOfParticles();
      if ( 1 == np ) {
	result = PASS;
      }
      result = Form(" /number of particles = %i/ ", np) + result;
      return result;
    });






}
//___________________________________________________________________
