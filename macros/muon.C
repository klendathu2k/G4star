void muon() {
  particleGun(1,"mu+,mu-",9.9,10.1,3.25,3.26);
  gROOT->ProcessLine("chain->Clear();");
};
