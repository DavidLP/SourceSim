A program to simulate ionizing particles from a radioactive source (e.g. SR-90, Am-241, electron beam) interacting with pixel sensors. Shielding and triggering can be simulated and the setup (e.g. geometry, materials) can be easily changed.

The program uses the GEANT4 framework. 
The corresponding GEANT4 license can be found here:
http://geant4.web.cern.ch/geant4/license/LICENSE.html

This software is under development and not released yet. Nevertheless only compiling revisions are comitted to the master branch and this code is a better example how to use GEANT4 then most of the GEANT4 examples.

Usage:

1. Make sure that GEANT4 is installed and the GEANT4 environment variables are set (`source geant4-install/bin/geant4.sh`)
2. Create a directory for SourceSim (e.g. SourceSim-build) and  compile the code:
3. 
   ```
   mkdir SourceSim-build
   cd SourceSim-build
   cmake path_to_SourceSim_sources
   make
   ```
   
3. Run SourceSim: ``` . SourceSim-build\SourceSim ```
