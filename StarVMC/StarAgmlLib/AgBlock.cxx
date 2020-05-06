#include "AgBlock.h"
ClassImp(AgBlock);

#include "AgShape.h"
#include <iostream>
#include "AgMaterial.h"
#include "AgShape.h"
//#include "AgMedium.h"
#include "AgModule.h"
#include "AgPlacement.h"
#include "AgMath.h" 

#include "StarAgmlStacker.h"
#include "RVersion.h" 
#if ROOT_VERSION_CODE < ROOT_VERSION(6,00,0)                                                                                              
  #include "StMessMgr.h"                                                                                                                  
#define ENDL endm;
#else                                                                                                                                     
#include <iostream>

#define LOG_PRINT  std::cout << " -Print- "                                                                                
#define LOG_INFO   std::cout << " -Info-  "                                                                                
#define LOG_DEBUG  std::cout << " -Debug- "                                                                                
#define LOG_WARN   std::cout << " -Warn-  "                                                                                
#define LOG_ERROR  std::cout << " -Warn-  "                                                                                
#define LOG_FATAL  std::cout << " -Warn-  "                                                                                
#define ENDL std::endl
#endif       

// Setup static members
AgBlock                      *AgBlock::mCurrent=0;
std::map<TString, AgBlock *>  AgBlock::mBlockTable;
std::vector< AgBlock *>       AgBlock::mStack;

StarAgmlStacker *AgBlock::_stacker = 0;
AgModule *AgBlock::_module = 0;

struct _AgBlockDummy {
  _AgBlockDummy(){
    AgBlock::mStack.push_back(NULL);
  };
} _agblockdumdum_;

// ..................................................................................................
// Block IO methods
void AgBlock::Info   ( const Char_t *name, const Char_t *msg, ... ) const {
  LOG_INFO << Form("%10s",name) << ": " << msg << ENDL;
}
void AgBlock::Warning( const Char_t *name, const Char_t *msg, ... ) const {
  LOG_WARN << Form("%10s",name) << ": " << msg << ENDL; 
}
void AgBlock::Error  ( const Char_t *name, const Char_t *msg, ... ) const {
  LOG_ERROR << Form("%10s",name) << ": " << msg << ENDL;
}
void AgBlock::Fatal  ( const Char_t *name, const Char_t *msg, ... ) const {
  LOG_FATAL << Form("%10s",name) << ": " << msg << ENDL;
}
// ..................................................................................................
  

AgBlock::AgBlock(const Char_t *name, const Char_t *title)
  : TNamed(name,title),
    mMother(0),
    mDaughters(),
    _volume(0),
    mNicknames(),
    mGroups(),
    mMakeAssembly(0),
    mNumberBranches(0)
{
  addNickname(name); // ensure that the vector is populated
  _valid = true;
  mMakeAssembly=false;
}


void AgBlock::Create( const Char_t *name )
{
  // Get pointer to the named block
  AgBlock *block = mBlockTable[ name ];
  if ( !block )
    {
      LOG_WARN << "Create(const Char_t *name)" << Form("The specified block %s is not in the block table.",name)<<ENDL;
      LOG_WARN << "Create(const Char_t *name)" << Form("Was the block added to the module with AddBlock?") << ENDL;
      return;	      
    }
  
  // Push the current block onto the stack
  mStack.push_back( this );

  // Execute the ** next ** block's code
  //$$$  mStack.back()->Block();
  block -> Block( _create );

  // And any cleanup code
  //$$$ mStack.back()->End();
  block -> End();

  // Pop the stack to restore parentage
  mStack.pop_back();

}


void AgBlock::AddGroup( const Char_t *name )
{
  //$$$  mGroups.push_back(name);
  _stacker -> AddGroup(name);
}


;//{ mGroups.push_back( name ); }


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> WHY IS THIS COMMENTED OUT???
void AgBlock::Inherit()
{
  // Nothing to do here because this is an orphan block
  if ( !mMother ) 
    return;
  //$$$  _shape.Inherit( mMother -> _shape );
}

void AgBlock::Inherit( const AgShape &shape )
{
}

void AgBlock::Inherit( const AgMaterial &material )
{
}

void AgBlock::Inherit( const AgMedium &medium )
{
}

void AgBlock::Inherit( const AgAttribute &attribute )
{
}


void AgBlock::InheritFrom( AgShape &shape, Bool_t zeros )
{
  
  // List of parameters defined on the shape
  std::vector< TString > pars = shape.parList();

  // Loop over the parameters
  for ( UInt_t i=0;i<pars.size();i++ )
    {

    }
  
}

AgBlock* AgBlock::previous(Int_t offset)
{ 
  UInt_t size = mStack.size();
  UInt_t off  = offset;
  UInt_t index = size - off - 1;

  // if (index>=0) should always be satisfied
    {
      return mStack[index];
    }

  return NULL;

}


AgBlock *
AgBlock::Find( const Char_t *name)
{
  return mBlockTable[ name ];
}

void 
AgBlock::Print(Option_t *opts)const
{
  LOG_INFO << "========================================================================" << ENDL;
  LOG_INFO << GetName() << " :: " << GetTitle() << ENDL;
  LOG_INFO << "Current instance:  " << mNicknames.back().Data() << ENDL;
  LOG_INFO << "Defined in module: " << _module -> GetName() << ENDL;
  LOG_INFO << "========================================================================" << ENDL;
  _material.Print(); LOG_INFO << ENDL;
  _medium.Print();   LOG_INFO << ENDL;
  _shape.Print();
}

void AgBlock::List(Option_t *opts)
{

  TString Opts=opts;

  LOG_INFO << "========================================================================================" << ENDL;
  LOG_INFO << "List of known blocks: " << ENDL;
  std::map< TString, AgBlock *>::iterator iter = mBlockTable.begin();
  while ( iter != mBlockTable.end() )
    {
      TString name = (*iter).first;
      if ( Opts!="ALL" ) if ( name!=opts ) 
	{
	  iter++;
	  continue;
	}     
      AgBlock *ptr = (*iter).second;
      TString cmnt = "";
      if ( ptr ) cmnt=ptr->GetTitle();

      LOG_INFO << Form("%s %80s ptr=%p",name.Data(),cmnt.Data(),ptr) << " ";
      std::vector<TString> nicks = _stacker -> nicknames(name);
      UInt_t nnicks = nicks.size();
      if ( nicks.size() ) 
	{
	  LOG_INFO << nicks[0].Data() << " ";
	  if ( nicks.size()>1 )
	    LOG_INFO << ".. " << nicks.back().Data()<< " ";
	  LOG_INFO << nnicks << ENDL;
	}
      LOG_INFO << ENDL;

      iter++;
    }
  LOG_INFO << ENDL;
  
}

// ---------------------------------------------------------------------------------------------------
// --
//Bool_t AgBlock::isSame( const AgAttribute &attr )
//{
//  Bool_t same = true;
//  same &= attr("serial") == _attribute("serial");
//  return same;
//
//}

