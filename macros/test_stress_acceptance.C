#include <StBFChain.h>
#include <StChain.h>
#include <StGeant4Maker/StMCParticleStack.h>
#include <TVirtualMC.h>
#include <StMessMgr.h>

void test_stress_acceptance(int nevents=10, int ntracks=10,const char* particles="pi+,pi-,K+,K-,mu+,mu-,e+,e-,proton,antiproton,pi0,gamma") {

  double ptmn =  0.100; 
  double ptmx = 10.000;
  double etamn= -2.0; 
  double etamx= +4.0;

  // Configure G4 maker 
  StMaker* _geant4 = chain->GetMaker("geant4");

  // Run particle gun simulation
  for ( int i=0;i<nevents;i++ ) {
    particleGun(ntracks,particles,ptmn,ptmx,etamn,etamx);
  }


};
