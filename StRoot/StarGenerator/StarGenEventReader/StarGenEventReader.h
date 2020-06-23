#ifndef __StarGenEventReader_h__
#define __StarGenEventReader_h__

#include "StarGenerator/BASE/StarGenerator.h"
#include "StarGenerator/EVENT/StarGenEvent.h"

class StarGenEventReader : public StarGenerator
{
public:
  StarGenEventReader( const Char_t *name="genreader" ) 
    : StarGenerator(name), 
      mEntry(0)      
  { 
    mEvent = new StarGenEvent("primaryEvent");
  };
 ~StarGenEventReader(){ /* nada */ };

  int Init();
  int Generate();

private:
protected:
  int mEntry;
  ClassDef( StarGenEventReader, 1 );
};

#endif
