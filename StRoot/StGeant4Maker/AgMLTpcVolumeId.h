#ifndef __AgMLTpcVolumeId_h__
#define __AgMLTpcVolumeId_h__

#include <StarVMC/StarAgmlLib/AgMLExtension.h>
#include <StMessMgr.h>

class AgMLTpcVolumeId : public AgMLVolumeId {
public:
  
  AgMLTpcVolumeId();

  int tpads[76];
  int isdet[76];

  virtual int id( int* numbv ) const { 

    int tpgv = numbv[0]; // tpc gas volume 1, 2
    int tpss = numbv[1]; // tpc super sector 1-12
    int tpad = numbv[2]; // tpc padrow

    int sector = tpss + 12 * (tpgv-1); // sector 1-24    

    int det = isdet[tpad-1];
    int pad = tpads[tpad-1];

    int volumeid = 100000*det + 100*sector + pad;                

    return volumeid;

  };
};


#endif
