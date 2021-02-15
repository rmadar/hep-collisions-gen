import generation_handler as gen

# Defning the collision under study
c = gen.collision('p p', 't t~', name='TopProduction')

# Get a MadGraph instance
mg = gen.madgraph('/home/rmadar/cernbox/FutureColliders/hep-collisions-gen/MG5_aMC')

# Create the process directory
mg.run_proc_dir(c.process(), 'proc_test')

# Generate events
params = {'nevents': 5000, 'run_tag': 'example', **c.params()}
mg.gen_evts('proc_test', 'runShowerDet', params, pythia=True, delphes=True)
