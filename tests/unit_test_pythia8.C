#include "tests/unit_tests.h"
#include "StarGenerator/Pythia8_1_86/StarPythia8.h"
#include <cassert>

#include <TTable.h>


void unit_test_pythia8() {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of pythia 8 generator" << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  auto* chain = StMaker::GetChain();

  chain->ls(5);
  chain->Clear();
  chain->Make();
  chain->Finish();


}
//___________________________________________________________________
