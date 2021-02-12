# Simulation of high energy collisions

Generation of simulated high energy collisions data using
[MadGraph](https://launchpad.net/mg5amcnlo) (hard scattering),
[Pythia](http://home.thep.lu.se/~torbjorn/Pythia.html) (showering)
and [Delphes](https://github.com/delphes/delphes) (detector fast simulation).
This requires to have [ROOT](https://root.cern/) installed.


## Setup

1. Download MadGraph: https://launchpad.net/mg5amcnlo

2. Install needed pythia and Delphes
```
cd MG5_aMC_version/
./bin/mg5_aMC
install pythia8
install Delphes
```

> **Note:** the Delphes compilation might fail if the installed ROOT version is not compatible.
> In that case, a compatible version of ROOT must be installed and Delphes can be compiled by hand,
> doing:
> ```
> cd Delphes
> ./configure
> make
> ```

3. Compile the display library of Delphes and produce `libDelphesDisplay.so`, to enable event display.
```
cd Delphes
./configure
make display
```

## Usage

The workflow consists in first generating the hard-scattering setup (directory) with MadGraph
using a "process card". Then, the showering of events is performed with Pythia and
the detector simulation is ran with Delphes. The three steps can be done in one
step using a `run_madevent.config`. The following line details the procedure.
