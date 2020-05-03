class StBFChain;
StBFChain* chain = 0;

#include <TString.h>
#include <iostream>
#include <StMessMgr.h>

// Random number generator seed
int __rngSeed = 12345;

// Add a new maker to the chain
void addMaker( const char* name, const char* maker ) {
  LOG_INFO << "auto* _" << name << " = new " << maker << endm;
  gROOT->ProcessLine( Form( "auto* _%s = new %s;", name, maker ) );
}

// Add a new event generator to the primary event generator
void addGenerator( const char* name, const char* maker ) {
  LOG_INFO << "auto* _" << name << " = new " << maker << endm;
  gROOT->ProcessLine( Form( "auto* _%s = new %s;", name, maker ) );
  gROOT->ProcessLine( Form( "_primary->AddGenerator( _%s );", name ) );
}

void loadStar(const Char_t *mytag="dev2021", Bool_t agml = true  )
{

  gROOT->ProcessLine("StLoggerManager::StarLoggerInit();");

  Load("StChain.so"); 
  Load("StBFChain.so"); 

  gROOT->ProcessLine("chain = new StBFChain();");

  TString chainOpts = "agml geant4 geant4vmc stargen geant4mk pythia8.1.86 kinematics nodefault ";

  // pickup command line options ala "--" and add them as a chain option
  for ( int i=0; i<gApplication->Argc();i++ ) {
    TString arg = gApplication->Argv(i);
    if ( arg.Contains("--web") || arg.Contains("notebook") ) continue;
    // Parse "--" style options for ourselves
    if ( arg.Contains("--") ) {
      arg.ReplaceAll("--"," ");
      // If the option matches key=value, treat this as an attribute to be
      // set on the G4 maker...
      if ( arg.Contains("=") ) {
	// Skip for now
      }
      else {
	chainOpts += arg;
      }
    }
  }



  gROOT->ProcessLine(Form("chain->SetFlags(\"%s\");",chainOpts.Data()));

  // Load shared libraries
  gROOT->ProcessLine("chain->Load();");

  // Add in star mag field
  Load("libStarMagFieldNoDict.so");

  addMaker( "primary", "StarPrimaryMaker()" );
  addMaker( "geant4",  "StGeant4Maker()" );
  //  addMaker( "pythia8", "StarPythia8()" );
  //  gROOT->ProcessLine("_primary->AddGenerator( _pythia8 );");
  addMaker( "kine",        "StarKinematics()" );
  gROOT->ProcessLine("_primary->AddGenerator( _kine );");



  // set attributes for arguements matching --x=y
  for ( int i=0; i<gApplication->Argc();i++ ) {                                                                                           

    TString arg = gApplication->Argv(i);                                                                                                  
    if ( arg.Contains("--web") || arg.Contains("notebook") ) continue;                                                                    
    // Parse "--" style options for ourselves                                                                                             

    if ( arg.Contains("--") ) {                                                                                                           

      arg.ReplaceAll("--"," ");                                                                                                           

      // If the option matches key=value, treat this as an attribute to be                                                                
      // set on the G4 maker...                                                                                                           
      if ( arg.Contains("=") ) {                                                                                                          

	TString key = arg.Tokenize("=")->At(0)->GetName();
	TString val = arg.Tokenize("=")->At(0)->GetName();

	// Process RNG seed
	if ( key=="seed" ) {
	  __rngSeed = val.Atoi();
	  continue;
	}

	// All other variables pass through to G4 maker
	
	gROOT->ProcessLine(Form("_geant4->SetAttr(\"%s\",%s);",
				arg.Tokenize("=")->At(0)->GetName(),
				arg.Tokenize("=")->At(1)->GetName()));
      }                                                                                                                                   
    }                                                                                                                                     
  } 


  //
  // ROOT6 command line processing
  //
  const char* cmds[] = { 

    // // Pythia configuration
    // "_pythia8->SetFrame(\"CMS\", 510.0);"
    // "_pythia8->SetBlue(\"proton\");"
    // "_pythia8->SetYell(\"proton\");"
    // "_pythia8->Set(\"HardQCD:all= on\");"

    // Geometry instantiation
    "TString __geometry_tag = \"dev2021\";"
    "AgModule::SetStacker( new StarTGeoStacker() );"
    "StarGeometry::Construct(__geometry_tag);"
    "gGeoManager->Export(\"y2014x.root\");"
    "gGeoManager->Export(\"y2014x.C\");"

  };

  for ( auto cmd : cmds ) {
    gROOT->ProcessLine( cmd );
  }

}

bool __initialized = false;

void particleGun( const int ntrack=1, const char* particles="pi+,pi-", double ptmn=1.0,double ptmx=10.0, double etamn=-1, double etamx=2 ) {

  if ( !__initialized ) { 
    gROOT->ProcessLine("chain->Init();"); 
    // Setup RNG seed and map all ROOT TRandom here
    gROOT->ProcessLine(Form("StarRandom::seed( %i );",__rngSeed));
    gROOT->ProcessLine("StarRandom::capture();"); 
    __initialized = true; 
  }

  const char* _cmds[] = {

    // Clear the chain from the previous event
    "chain->Clear();",
    Form("_kine->Kine(%i,\"%s\",%f,%f,%f,%f);",ntrack,particles,ptmn,ptmx,etamn,etamx),
    "chain->Make();"
  };

  for ( auto cmd : _cmds ) {
    gROOT->ProcessLine( cmd );
  }

};


void geant4star(){ 
  loadStar(); 
}
