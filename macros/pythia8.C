#include "StarGenerator/BASE/StarPrimaryMaker.h"
#include "StarGenerator/Pythia8_1_86/StarPythia8.h"
void pythia8( const std::string config="pp:minbias", const double energy=510.0 ) {

  LOG_INFO << "Adding Pythia8 maker " << config.c_str() << endm;

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  auto* py = new StarPythia8();
  pm->AddGenerator(py);

  py->SetFrame("CMS",energy);
  py->SetBlue("proton");
  py->SetYell("proton");

  py -> Set("SoftQCD:all = off");
  //  py->Set("SoftQCD:minBias = on"); // but they changed the name to...
  py -> Set("SoftQCD:nonDiffractive = on");
}
