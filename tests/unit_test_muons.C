#include "tests/unit_tests.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
using namespace boost::accumulators;

#include <TTable.h>

StGeant4Maker* _g4mk = 0;

//accumulator_set<double, stats<tag::mean, tag::moment<2> > > acc

using Accumulator_t = accumulator_set<double, stats<tag::mean, tag::moment<2> > >;


const double GeV = 1;
const double MeV = 1E3;
const double keV = 1E6;
const double  eV = 1E9;
const std::map<double,std::string> scale2string = {
  {GeV,"GeV"},
  {MeV,"MeV"},
  {keV,"keV"},
  {eV,"eV"}
};

template<typename T> double energy_deposit(const T* h) { return h->de; } 
template<typename T> double path_length   (const T* h) { return h->ds; } 

//___________________________________________________________________
double _eta  = 0; 
double _phid = 0;
//___________________________________________________________________
const int ntracks=70;
//___________________________________________________________________

struct tpcTag  { 
  static double energy_deposit(const g2t_tpc_hit_st* h){ return h->de; }
  static double path_length(const g2t_tpc_hit_st* h){ return h->ds; }
} tpc; // TPC hits
struct bemcTag { 
  static double energy_deposit(const g2t_emc_hit_st* h){ return h->de; }
} bemc; // BEMC hits
struct bsmdTag { 
  static double energy_deposit(const g2t_emc_hit_st* h){ return h->de; }
} bsmd; // BSMD hits
struct eemcTag { 
  static double energy_deposit(const g2t_emc_hit_st* h){ return h->de; }
} eemc; // EEMC hits
struct esmdTag { 
  static double energy_deposit(const g2t_emc_hit_st* h){ return h->de; }
} esmd; // ESMD hits

template<typename T> struct HitTraits {
  /* empty, this will not go well */
};
template<> struct HitTraits<tpcTag>  {
  const std::string tableName = "g2t_tpc_hit";
  typedef g2t_tpc_hit_st hit_type;
};
template<> struct HitTraits<bemcTag> {
  const std::string tableName = "g2t_emc_hit";
  typedef g2t_emc_hit_st hit_type;
};
template<> struct HitTraits<bsmdTag> {
  const std::string tableName = "g2t_smd_hit";
  typedef g2t_emc_hit_st hit_type;
};
template<> struct HitTraits<eemcTag> {
  const std::string tableName = "g2t_eem_hit";
  typedef g2t_emc_hit_st hit_type;
};
template<> struct HitTraits<esmdTag> {
  const std::string tableName = "g2t_esm_hit";
  typedef g2t_emc_hit_st hit_type;
};

template<typename Tag> 
void check_hit_distribution( Tag, 
			     std::function<double(const typename HitTraits<Tag>::hit_type *)> stat, 
			     std::function<std::string(const Accumulator_t& acc)>             eval,
			     double scale=1.0) {
  Accumulator_t acc;
  HitTraits<Tag> traits;
  TTable* table = static_cast<TTable*>( _g4mk->GetDataSet(traits.tableName.c_str()) );
  for ( int irow=0; irow<table->GetNRows();irow++ ) {
    auto* hit = static_cast<const typename HitTraits<Tag>::hit_type *>( table->At(irow) );
    acc( stat(hit)*scale ); 
  }
  LOG_TEST <<  eval(acc) << std::endl;
}



// void check_tpc_de( std::string tableName, 
// 		   std::string message, 
// 		   std::function<double(const HitType&)> s,		   
// 		   std::function<std::string(Accumulator_t& acc)> f 
// 		   ) {
//   TTable* table = static_cast<TTable*>( _g4mk->GetDataSet(tableName.c_str()) );
//   int     nrows = table->GetNRows();
//   Accumulator_t A;
//   for ( int irow=0;irow<nrows;irow++ ) {
//     //    const g2t_tpc_hit_st* hit = static_cast<const g2t_tpc_hit_st*>( table->At(irow) );
//     const HitType& hit = *static_cast<const HitType*>( table->At(irow) );
//     if ( 0==hit ) continue;
//     A( s(hit) * keV );
//   };
//   LOG_TEST << "-require- [" << tableName << "][" << message << "] " << f(A) << std::endl;
// };

// void check_tpc_de( std::string tableName, 
// 		   std::string message, 
// 		   std::function<std::string(Accumulator_t& acc)> f 
// 		   ) {
//   TTable* table = static_cast<TTable*>( _g4mk->GetDataSet(tableName.c_str()) );
//   int     nrows = table->GetNRows();
//   Accumulator_t A;
//   for ( int irow=0;irow<nrows;irow++ ) {
//     const g2t_tpc_hit_st* hit = static_cast<const g2t_tpc_hit_st*>( table->At(irow) );
//     if ( 0==hit ) continue;
//     A( energy_deposit(hit) * keV );
//   };
//   LOG_TEST << "-require- [" << tableName << "][" << message << "] " << f(A) << std::endl;
// };


void unit_test_muons() {

  gROOT->ProcessLine("initChain();");

  auto* pm = dynamic_cast<StarPrimaryMaker*>( StMaker::GetChain()->GetMaker("PrimaryMaker") );
  pm->SetVertex(0.,0.,0.);
  pm->SetSigma(0.0,0.,0.);

  LOG_TEST << "=======================================================" << std::endl;
  LOG_TEST << "Unit testing of muons" << std::endl;
  LOG_TEST << "=======================================================" << std::endl;

  auto* chain = StMaker::GetChain();
  auto* _kine = dynamic_cast<StarKinematics*> ( chain->GetMaker("StarKine") );
  _g4mk = dynamic_cast<StGeant4Maker*>  ( chain->GetMaker("geant4star") );

  double minPt =  0.100;
  double maxPt = 10.000;
  int    nbinPt = 100;
  _kine->Kine(ntracks,"mu+,mu-",0.100,10.00,-2.0,5.0);

  chain->Clear();
  chain->Make();

  check_hit_distribution( tpc, tpc.energy_deposit, [=](const Accumulator_t& acc){
      std::string result = PASS;
      double _mean = mean(acc);
      double _2mom = moment<2>(acc);
      result = Form("energy deposition: mean=%f 2nd-moment=%f ",_mean,_2mom) + result;
      return result;      
    }, keV);
  check_hit_distribution( tpc, tpc.path_length,    [=](const Accumulator_t& acc){
      std::string result = PASS;
      double _mean = mean(acc);
      double _2mom = moment<2>(acc);
      result = Form("path length: mean=%f 2nd-moment=%f ",_mean,_2mom) + result;
      return result;      
    });


  // check_hit_distribution( tpc, [=](const g2t_tpc_hit_st* h){ return h->ds; }, [=](const Accumulator_t& acc){
  //     std::string result = PASS;
  //     double _mean = mean(acc);
  //     double _2mom = moment<2>(acc);
  //     result = Form("path length: mean=%f 2nd-moment=%f ",_mean,_2mom) + result;
  //     return result;      
  //   });

  // check_hit_distribution( tpc, [=](const g2t_tpc_hit_st* h){ return h->de; }, [=](const Accumulator_t& acc){
  //     std::string result = PASS;
  //     double _mean = mean(acc);
  //     double _2mom = moment<2>(acc);
  //     result = Form("energy deposition: mean=%f 2nd-moment=%f ",_mean,_2mom) + result;
  //     return result;      
  //   }, keV);
  // check_hit_distribution( tpc, [=](const g2t_tpc_hit_st* h){ return h->ds; }, [=](const Accumulator_t& acc){
  //     std::string result = PASS;
  //     double _mean = mean(acc);
  //     double _2mom = moment<2>(acc);
  //     result = Form("path length: mean=%f 2nd-moment=%f ",_mean,_2mom) + result;
  //     return result;      
  //   });




  // check_tpc_de( "g2t_tpc_hit", "Check TPC energy loss and variance", 
  // 		energy_deposit<g2t_tpc_hit_st>, 
  // 		[=](Accumulator_t& acc){
  //     double _mean = mean(acc);
  //     double _mom2 = moment<2>(acc);
  //     std::string result = Form(" mean=%f 2ndmoment=%f ",_mean,_mom2) + PASS;
  //     return result;
  //   });




    


}
//___________________________________________________________________
