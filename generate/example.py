import generation_handler as gen

# Defning the two types of collision under study
lhc_ttbar = gen.collision('p p'    , 't t~', ebeam1='6500', ebeam2='6500', name='LHCTopProd')
muc_ttbar = gen.collision('mu+ mu-', 't t~', ebeam1='180' , ebeam2='180' , name='MuCTopProd')

# Get a MadGraph instance
mg = gen.madgraph('../MG5_aMC')

# Create the process directories
mg.run_proc_dir(lhc_ttbar.mg_proc(), 'lhc_ttbar')
mg.run_proc_dir(muc_ttbar.mg_proc(), 'muc_ttbar')

# Generate events for the LHC
params = {'nevents': 5000, **lhc_ttbar.params()}
mg.gen_evts('lhc_ttbar', 'run01', params, pythia=True, delphes=True)

# Generate events for the muon collider
params = {'nevents': 5000, **muc_ttbar.params()}
mg.gen_evts('muc_ttbar', 'run01', params, pythia=True, delphes=True)
