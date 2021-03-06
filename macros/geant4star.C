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

std::map<TString,TString> _generatorMap = {
  { "genreader", "StarGenEventReader" }
};

// Hack to make sure finish is called on the chain
struct __Fini {
  ~__Fini() {
    gROOT->ProcessLine("chain->Finish();");
  }
} __fini__;

bool hasRuntimeArg( const char* arg_ ) {
  bool result = false;
  for ( int i=0; i<gApplication->Argc();i++ ) {
    TString arg = gApplication->Argv(i);
    arg.ReplaceAll("--","");
    if ( arg.Contains(arg_) ) {
      result = true;
      break;
    }
  }
  return result;
}

void loadStar(const Char_t *mytag="dev2021", Bool_t agml = true  )
{

  gROOT->ProcessLine("chain = new StBFChain();");
  gROOT->ProcessLine("chain->cd();");
  gROOT->ProcessLine("chain->SetDebug(1);");  

  TString chainOpts = "agml geant4 geant4vmc stargen geant4mk pythia8.1.86 kinematics -emc_t -ftpcT nodefault ";

  // pickup command line options ala "--" and add them as a chain option

  for ( int i=0; i<gApplication->Argc();i++ ) {
    TString arg = gApplication->Argv(i);
    if ( arg.Contains("--web") || arg.Contains("notebook") ) continue;
    // Parse "--" style options for ourselves
    if ( arg.Contains("--") ) {
      arg.ReplaceAll("--"," "); // n.b. the space pads out the chain options
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

  // Set the chain options
  gROOT->ProcessLine(Form("chain->SetFlags(\"%s\");",chainOpts.Data()));

  // Find the output filename, if given, and set as the output
  TString output = "";
  for ( int i=0; i<gApplication->Argc();i++ ) {
    TString arg = gApplication->Argv(i);  
    if ( arg.Contains("--") ) {
      gMessMgr->Info() << arg.Data() << endm;
      arg.ReplaceAll("--","");
      // If the option matches key=value, treat this as an attribute to be
      // set on the G4 maker...
      if ( arg.Contains("=") ) {
	TString key = arg.Tokenize("=")->At(0)->GetName();
	TString val = arg.Tokenize("=")->At(1)->GetName();
	std::cout << " key = [" << key.Data() << "] value = " << val.Data() << std::endl;
	if ( key=="output" ){
	  output = val;
	  gROOT->ProcessLine(Form("chain->Set_IO_Files(\"\",\"%s\");",output.Data()));
	  break;
	}
      }
    }
  }


  // Load shared libraries
  gROOT->ProcessLine("chain->Load();");

  // Add in star mag field
  Load("libStarMagFieldNoDict.so");

  gROOT->ProcessLine( "int __result = chain->Instantiate();" );

  // Now add makers...
  addMaker( "primary", "StarPrimaryMaker()" );
  addMaker( "geant4",  "StGeant4Maker()" );

  //  addMaker( "pythia8", "StarPythia8()" );
  //  gROOT->ProcessLine("_primary->AddGenerator( _pythia8 );");

  // Always add the kinematic generator
  addMaker( "kine",        "StarKinematics()" );
  gROOT->ProcessLine("_primary->AddGenerator( _kine );");


  // Loop on the chain options and add in other generators which have been called for
  for ( auto _s : *chainOpts.Tokenize(" ") ) {
    auto s = ( dynamic_cast<TObjString*>( _s ) ) -> String() ; // annoying
    if ( _generatorMap[s] != "" ) {
      addGenerator( s, _generatorMap[s] );
    }
    
  }
  



  // Move outputStream after the geant maker
  gROOT->ProcessLine("StMaker* __outputStream = chain->GetMaker(\"outputStream\");");
  // gROOT->ProcessLine("LOG_INFO << "
  // 		     "\"outputStream = \" << __outputStream << endm;")

  gROOT->ProcessLine("chain->AddAfter( _geant4->GetName(), __outputStream ); ");




  //  gROOT->ProcessLine("auto* __outputStream = chain->GetMaker(\"outputStream\"\);


  gROOT->ProcessLine("StMaker::lsMakers(chain);");


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
	  gMessMgr->Info() << "--seed=" << __rngSeed << " detected" << endm;
	  continue;
	}

	// Process output file
	if ( arg.Contains("output") ) {
	  /* nada */ 
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

bool initChain( std::vector<std::string> _cmds={ "std::cout << \"Chain has been initialized.\" << std::endl;" } ) {
  if ( !__initialized ) { 
    //    gROOT->ProcessLine(Form("chain->SetAttr(\"Random:G4\",%i)",__rngSeed));
    // Setup RNG seed and map all ROOT TRandom here
    // gROOT->ProcessLine(Form("StarRandom::seed( %i );",__rngSeed));
    // gROOT->ProcessLine("StarRandom::capture();"); 
    // gMessMgr->Info() << "RNG seed set to " << __rngSeed << endm;
    gROOT->ProcessLine("chain->Init();"); 
    for ( auto cmd : _cmds ) {
      gROOT->ProcessLine( cmd.c_str() );
    }
    __initialized = true; 
  }
  return true;
}

void particleGun( const int ntrack=1, const char* particles="pi+,pi-", double ptmn=1.0,double ptmx=10.0, double etamn=-1, double etamx=2 ) {

  initChain();

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


void particleGun( const char* particle="mu+", double px=1.0/sqrt(2), double py=1.0/sqrt(2), double pz=0.0, double vx=0., double vy=0., double vz=0. ) {

  initChain();

  const char* _cmds[] = {
    "chain->Clear();",
    "{",
    Form("double _px=%f",px),
    Form("double _py=%f",py),
    Form("double _pz=%f",pz),
    Form("auto   _part=_kine->AddParticle(\"%s\");",particle),
    "double _mass = _part->GetMass();",
    "double _energy = sqrt( _px*_px+_py*_py+_pz*_pz+_mass*_mass );",
    "_part->SetPx(_px);",
    "_part->SetPy(_py);",
    "_part->SetPz(_pz);",
    "_part->SetEnergy(_energy);",
    "_part->SetVx(_vx);",
    "_part->SetVy(_vy);",
    "_part->SetVz(_vz);",
    "_part->SetTof(0);",
    "chain->Make();",    
    "}"
  };

  for ( auto cmd : _cmds ) {
    gROOT->ProcessLine( cmd );
  }

};




void geant4star(){ 
  loadStar(); 
}
