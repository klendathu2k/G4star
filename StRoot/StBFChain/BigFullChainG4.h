  {"g4clhep",     "","", "",                      "", "$G4PATH/lib64/libG4clhep.so","Load G4 libs",false},
  {"g4global",    "","", "g4clhep",               "", "$G4PATH/lib64/libG4global.so","Load G4 libs",false},
  {"g4intercoms", "","", "g4global",              "", "$G4PATH/lib64/libG4intercoms.so","Load G4 libs",false},
  {"g4materials", "","", "g4intercoms",           "", "$G4PATH/lib64/libG4materials.so","Load G4 libs",false},
  {"g4interfaces","","", "g4intercoms",           "", "$G4PATH/lib64/libG4interfaces.so","Load G4 libs",false},
  {"g4graphics",  "","", "g4intercoms",           "", "$G4PATH/lib64/libG4graphics_reps.so","Load G4 libs",false},
  {"g4geometry",  "","", "g4graphics,g4materials","", "$G4PATH/lib64/libG4geometry.so","Load G4 libs",false},
  {"g4particles", "","", "g4geometry",            "", "$G4PATH/lib64/libG4particles.so", "Load G4",false},
  {"g4track"    , "","", "g4particles",           "", "$G4PATH/lib64/libG4track.so",     "Load G4",false},
  {"g4digits",    "","", "g4track",               "", "$G4PATH/lib64/libG4digits_hits.so", "Load G4",false},
  {"g4processes", "","", "g4digits",              "", "$G4PATH/lib64/libG4processes.so", "Load G4",false},

  {"g4tracking" , "","", "g4processes",           "", "$G4PATH/lib64/libG4tracking.so",  "Load G4",false},
  {"g4event",     "","", "g4tracking",            "", "$G4PATH/lib64/libG4event.so",       "Load G4",false},
  {"g4run",       "", "","g4event",               "", "$G4PATH/lib64/libG4run.so",         "Load G4",false},
  {"g4physics",   "", "", "g4run",                "", "$G4PATH/lib64/libG4physicslists.so", "Load G4",false},

  {"g4modeling",  "", "", "g4physics",            "", "$G4PATH/lib64/libG4modeling.so", "Load G4", false},

  // visualization drivers
  {"g4visual",    "", "", "g4modeling",           "", "$G4PATH/lib64/libG4vis_management.so", "Load G4", false},  
  {"g4ascii",     "", "", "g4visual",             "", "$G4PATH/lib64/libG4Tree.so", "G4 ASCII Tree", false},
  {"g4vrml",      "", "", "g4visual",             "", "$G4PATH/lib64/libG4VRML.so", "G4 ASCII Tree", false},
  {"g4dawn",      "", "", "g4visual",             "", "$G4PATH/lib64/libG4FR.so",   "G4 DAWN", false},
  {"g4heprep",    "", "", "g4visual",             "", "$G4PATH/lib64/libG4zlib.so,$G4PATH/lib64/libG4visHepRep.so",   "G4 DAWN", false},
  {"g4raytracer", "", "", "g4visual",             "", "$G4PATH/lib64/libG4RayTracer.so",   "G4 DAWN", false},
  {"g4gmocren",   "", "", "g4visual",             "", "$G4PATH/lib64/libG4GMocren.so", "G4 ASCII Tree", false},
  {"g4opengl",    "", "", "g4visual",             "", "$G4PATH/lib64/libG4gl2ps.so,$G4PATH/lib64/libG4OpenGL.so", "G4 ASCII Tree", false},
  {"g4geant3",    "", "", "",                     "", "$G4PATH/lib64/libG3toG4.so", "Load g3 to g4 support", false },
  {"geant4",      "","", "g4physics,g4interfaces,g4visual","","","Load G4 libs", false},

  // vmc
  {"g4root",      "","", "",       "", "/star/simu/simu/geant4vmc/lib64/libg4root.so", "Load g4root support", false },
  {"geant4vmc",   "","", "geant4,g4geant3,g4root,g4ascii,g4dawn,g4heprep,g4raytracer,g4vrml,g4gmocren,g4opengl", "", 
                         "libVMC.so,/star/simu/simu/geant4vmc/lib64/libgeant4vmc.so", "Load G4 VMC libs", false},
  // agml etc...
  {"ag4ml",       "","", "g4geometry, agml"            ,"","", "AgML + g4",false},
  //  {"stargen",     "","", "tables",                      "", "libStarGeneratorUtil.so,libStarGeneratorEvent.so,libStarGeneratorBase.so,libStarGeneratorFilt.so,libMathMore.so","STAR Generator BASE",false},
  {"stargen",     "","", "gen_T,sim_T"/*+++*/,                     "", "libStarGeneratorUtil.so,libStarGeneratorEvent.so,libStarGeneratorBase.so,libStarGeneratorFilt.so,libMathMore.so","STAR Generator BASE",false},
  {"geant4mk",    "","", "stargen,geant4vmc", "", "StGeant4Maker.so,StarMagField.so", "Load G4 VMC libs", false},

  // event generators
  {"pythia8.1.86","","","stargen", "", "Pythia8_1_86.so", "Load Pythia 8.1.86 generator", false },
  {"pythia8.2.35","","","stargen", "", "Pythia8_2_35.so", "Load Pythia 8.1.86 generator", false },
  {"hijing1.383" ,"","","stargen", "", "Hijing1_383.so",  "Load Hijing  1.383 generator", false },
  {"kinematics"  ,"","","stargen", "", "Kinematics.so",   "Load STAR Particle Gun", false },

  // support IO
  {"Geant4Out"    ,""  ,"","Tree"                                ,"","","Write g2t tables to StTree",kFALSE},

 
