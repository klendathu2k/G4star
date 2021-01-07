# G4star  

geant4star VMC application  

##HOWTO run geant4star against precompiled library in EVAL

> setup 64b
> starver eval
> setup root 6.16.00
> source /star/simu/simu/geant4.10.3.3/bin/geant4.csh
> setenv ROOT_INCLUDE_PATH $STAR/.$STAR_HOST_SYS/include/:$STAR/StRoot/StStarLogger/
>
> cp -R $STAR/macros $STAR/tests .
>
> geant4star -q -b --seed=1234 --output=geant4.geant.root tests/unit_test_output.C


##HOWTO clone the repository and build the code

> setup 64b
> starver eval
> setup root 6.16.00
> source /start/simu/simu/geant4.10.3.3/bin/geant4.csh
> setenv ROOT_INCLUDE_PATH $STAR/.$STAR_HOST_SYS/include/:$STAR/StRoot/StStarLogger/
>
> git clone https://github.com/klendathu2k/G4star
> cd G4star
> git checkout eval
> cons +StarVMC/Geometry
> cons

Go drink some coffee, build a cabinet, smoke a salmon… anything that will take your mind off the brain numbing build process that is to occur…

> geant4star -q -b --seed=1234 --output=geant4.geant.root tests/unit_test_output.C


