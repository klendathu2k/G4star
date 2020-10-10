#ifndef __AgMLVolumeIdFactory_h__
#define __AgMLVolumeIdFactory_h__

#include "TString.h"
#include <map>

#include <StarVMC/StarAgmlLib/AgMLExtension.h>
#include <AgMLTpcVolumeId.h>
#include <AgMLFstVolumeId.h>
#include <AgMLStgVolumeId.h>
#include <AgMLWcaVolumeId.h>
#include <AgMLHcaVolumeId.h>
#include <AgMLPreVolumeId.h>
#include <AgMLEpdVolumeId.h>
#include <AgMLEmcVolumeId.h>

class AgMLVolumeIdFactory {
public:
 
  static AgMLVolumeId* Create( TString name, bool test=false ) { 

    static std::map<TString,AgMLVolumeId*> VolumeId;

    AgMLVolumeId* id = VolumeId[name];
    if ( 0==id ) {
      if      ( name == "TPAD" && test==false )  
	id = new AgMLTpcVolumeId;
      else if ( name == "TPAD" && test==true )
	id = new AgMLTpcVolumeIdTest__;
      else if ( name == "FTUS" )
	id = new AgMLFstVolumeId;
      else if ( name == "TGCG" )
	id = new AgMLStgVolumeId;
      else if ( name == "WSCI" )
	id = new AgMLWcaVolumeId;
      else if ( name == "HSCI" )
	id = new AgMLHcaVolumeId;
      else if ( name == "PSCI" )
	id = new AgMLPreVolumeId;
      else if ( name == "EPDT" )
	id = new AgMLEpdVolumeId;
      else if ( name == "CSCI" )
	id = new AgMLEmcVolumeId;
      VolumeId[name] = id;
    }
    
    return id;

  };
  
};

#endif
