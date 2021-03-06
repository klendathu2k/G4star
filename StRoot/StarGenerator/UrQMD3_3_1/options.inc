c $Id: options.inc,v 1.1 2012/11/29 20:57:03 jwebb Exp $
c... law: include file (only) for global parameters & options
      integer numcto,numctp,maxstables
      parameter(numcto=400) ! maximum number of options
      parameter(numctp=400) ! maximum number of parameters
      parameter(maxstables=20) ! maximum number of stable particles
c...
      integer   CTOption(numcto)
      character ctodc(numcto)*2
c...
      real*8    CTParam(numctp)
      character ctpdc(numctp)*2

      integer nstable
      integer stabvec(maxstables)

      logical bf13,bf14,bf15,bf16,bf17,bf18,bf19,bf20,fixedseed
      common /options/CTOption,CTParam
      common /optstrings/ctodc,ctpdc
      common /loptions/fixedseed,bf13,bf14,bf15,bf16,bf17,bf18,
     .     bf19,bf20
      common /stables/nstable,stabvec
