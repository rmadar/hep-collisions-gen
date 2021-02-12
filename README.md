# Simulation of high energy collisions

Generation of simulated high energy collisions data using
[MadGraph](https://launchpad.net/mg5amcnlo) (hard scattering),
[Pythia](http://home.thep.lu.se/~torbjorn/Pythia.html) (showering)
and [Delphes](https://github.com/delphes/delphes) (detector fast simulation).
This requires to have [ROOT](https://root.cern/) installed.


## Setup

> **Note:** in what follow, `version` should be replaced by the actual MG version.

1. Download MadGraph: https://launchpad.net/mg5amcnlo. Untar the archive as `MG5_aMC/` directory.

2. Install needed pythia and Delphes
```
cd MG5_aMC/
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

4. Data analysis and event display: link/load the required libraries
```
cd analysis
ln -s ../MG5_aMC/Delphes/libDelphesDisplay.so .
```


## Usage

The workflow consists in first generating the process setup (directory) with MadGraph
using a "process card". Then, the showering of events is performed with Pythia and
the detector simulation is ran with Delphes. The three steps can be done in one
step using a `run_madevent.config`. The following line details the procedure.

### Generate events for a process

Under dev

```
cd MG_aMC/
./bin/mg5_aMC ../generate/gen.sh
cd output/.
./bin/generate_events [enabling shower and detector]

```

> What we want:
>  + specification of intput state (particle, energy, polarization), output state, model parameter.
>  + specification of detector simulation (Delphes card)
>  + create a script, which run all MG steps and produce a ntuple ready to be analyzed.

### Inspect event displays

```
root -l rootlogon.C
root[0] .x EventDisplay.C("../MG5_aMC/Delphes/cards/delphes_card_ATLAS.tcl","delphes_events.root")
```

![Event Display example](analysis/evtDisplay.jpg)

### Analyze events