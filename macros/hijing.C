#include "StarGenerator/BASE/StarPrimaryMaker.h"
#include "StarGenerator/Hijing1_383/StarHijing.h"
void hijing( const std::string config="AuAu", const double energy=200.0 ) {

  LOG_INFO << "Adding hijing maker " << config.c_str() << endm;

  auto pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );

  auto* hijing = new StarHijing();
 
  hijing->SetFrame("CMS",200.0);
  hijing->SetBlue("Au");
  hijing->SetYell("Au");  
  hijing->SetImpact(0.0, 30.0);       // Impact parameter min/max (fm)    0.   30.

  pm -> AddGenerator(hijing);
  pm -> SetCuts( 1.0E-6 , -1., -2.5, +2.5 );
  
}
