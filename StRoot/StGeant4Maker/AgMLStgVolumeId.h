#ifndef __AgMLStgVolumeId_h__
#define __AgMLStgVolumeId_h__

#include <StarVMC/StarAgmlLib/AgMLExtension.h>
#include <StMessMgr.h>

class AgMLStgVolumeId : public AgMLVolumeId {
public:
  
  AgMLStgVolumeId(){}

  virtual int id( int* numbv ) const { 

    int station = numbv[0];

    return station;

  };
};


#endif
