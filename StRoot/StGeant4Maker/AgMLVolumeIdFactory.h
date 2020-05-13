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

class AgMLVolumeIdFactory {
public:

  static AgMLVolumeId* Create( TString name ) { 

    static std::map<TString,AgMLVolumeId*> VolumeId;

    AgMLVolumeId* id = VolumeId[name];
    
    if ( name == "TPAD" ) { return (id)? id : id=new AgMLTpcVolumeId; }
    if ( name == "FTUS" ) { return (id)? id : id=new AgMLFstVolumeId; }
    if ( name == "TGCG" ) { return (id)? id : id=new AgMLStgVolumeId; }
    if ( name == "WSCI" ) { return (id)? id : id=new AgMLWcaVolumeId; }
    if ( name == "HSCI" ) { return (id)? id : id=new AgMLHcaVolumeId; }
    if ( name == "PSCI" ) { return (id)? id : id=new AgMLPreVolumeId; }
    if ( name == "EPDT" ) { return (id)? id : id=new AgMLEpdVolumeId; }
    
    assert(id==0);
    return 0;
    
  };
  
};

#endif
