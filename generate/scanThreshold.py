import generation_handler as gen
import numpy as np

# Get a MadGraph instance
mg = gen.madgraph('../MG5_aMC')

# Define/initialize processes to compute
ee = gen.collision('e+ e-'  , 't t~', name='ee_xsec')
mg.run_proc_dir(ee.mg_proc(), ee.name)
mg.run_proc_dir(ee.mg_proc(), ee.name+'ISR', eeISR=True)

mm = gen.collision('mu+ mu-', 't t~', name='mm_xsec')
mg.run_proc_dir(mm.mg_proc(), mm.name)
    
tt = gen.collision('ta+ ta-', 't t~', name='tt_xsec')
mg.run_proc_dir(tt.mg_proc(), tt.name)

# Define center-of-mass energies to loop over
mT   = 172.5
CMEs = np.linspace(2*mT+0.1, 2*mT+10, 20)

# Loop over center-of-mass energies
for s in CMEs :

    params = {
        'ebeam1': '{:2.1f}'.format(s/2.0),
        'ebeam2': '{:2.1f}'.format(s/2.0),
        'mt'    : mT,
    }

    for name in [ee.name, ee.name+'ISR', mm.name, tt.name]:
        mg.gen_evts(name, 'run_CME{:2.1f}'.format(s), params)
    
