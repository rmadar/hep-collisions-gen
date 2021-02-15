import generation_handler as gen

# Defning the two types of collision under study
pp = gen.collision('p p'    , 't t~', ebeam1='6500', ebeam2='6500', name='LHCTopProd')
mm = gen.collision('mu+ mu-', 't t~', ebeam1='180' , ebeam2='180' , name='MuCTopProd')

# Get a MadGraph instance
mg = gen.madgraph('../MG5_aMC')

# Create the process directories
mg.run_proc_dir(proc=pp.mg_proc(), directory='lhc_ttbar')
mg.run_proc_dir(proc=mm.mg_proc(), directory='muc_ttbar')

# Generate events for the LHC
params = {'nevents': 5000, **pp.params()}
mg.gen_evts('lhc_ttbar', 'run01', params, pythia=True, delphes=True)

# Generate events for the muon collider
params = {'nevents': 5000, **mm.params()}
mg.gen_evts('muc_ttbar', 'run01', params, pythia=True, delphes=True)
