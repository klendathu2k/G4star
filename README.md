# G4star  

geant4star VMC application



## HOWTO run geant4star against precompiled library in EVAL

`setup 64b
starver eval
setup root 6.16.00
source /star/simu/simu/geant4.10.3.3/bin/geant4.csh
setenv ROOT_INCLUDE_PATH $STAR/.$STAR_HOST_SYS/include/:$STAR/StRoot/StStarLogger/

cp -R $STAR/macros $STAR/tests .

geant4star -q -b --seed=1234 --output=geant4.geant.root tests/unit_test_output.C`

### Run pythia 8 example

Pythia 8 configuration is performed in macros/pythia8.C.  The code which runs the simulation
is under tests/unit_test_pythia8.C.  The following command will run 100 events through pythia8
and starsim, creating a pythia8.geant.root output file.

`geant4star -q -b --seed=1234 --output=pythia8.geant.root macros/pythia8.C tests/unit_test_pythia8.C\(100\)`

### Event reconstruction

The code in eval is a subset of the STAR software stack which may not be sufficient
to run reconstruction chains.  To run reconstruction I recommend switching back
to the DEV environment, and then running your analysis chain.

`stardev`


## HOWTO clone the repository and build the code

`setup 64b`
`starver eval`
`setup root 6.16.00`
`source /star/simu/simu/geant4.10.3.3/bin/geant4.csh`
`setenv ROOT_INCLUDE_PATH $STAR/.$STAR_HOST_SYS/include/:$STAR/StRoot/StStarLogger/`

`git clone https://github.com/klendathu2k/G4star`
`cd G4star`
`git checkout eval`
`cons +StarVMC/Geometry`
`cons`

Go drink some coffee, build a cabinet, smoke a salmon… anything that will take your mind off the brain numbing build process that is to occur…

 geant4star -q -b --seed=1234 --output=geant4.geant.root tests/unit_test_output.C


