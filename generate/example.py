
import generation_handler as gen

# Defning the two types of collision under study
pp = gen.collision('p p'    , 't t~', ebeam1='50000', ebeam2='50000', name='FCCTopProd')
mm = gen.collision('mu+ mu-', 't t~', ebeam1='180'  , ebeam2='180'  , name='MuCTopProd')
ee = gen.collision('e+ e-'  , 't t~', ebeam1='180'  , ebeam2='180'  , name='eeTopProd')

# Get a MadGraph instance
mg = gen.madgraph('../MG5_aMC')

# Run pp collisions
mg.run_proc_dir(proc=pp.mg_proc(), directory='test_pp_ttbar')
mg.gen_evts('test_pp_ttbar', 'run01', pp.beam_params(), pythia=True, delphes=True)

# Run mm collisions
mg.run_proc_dir(proc=mm.mg_proc(), directory='test_mm_ttbar')
mg.gen_evts('test_mm_ttbar', 'run01', ee.beam_params(), pythia=True, delphes=True)

# Run ee process with ISRs
mg.run_proc_dir(proc=ee.mg_proc(), directory='test_ee_ttbar_ISR', eeISR=True)
mg.gen_evts('test_ee_ttbar_ISR', 'run01', ee.beam_params(), pythia=True, delphes=True)

# Run ee process without ISRs
mg.run_proc_dir(proc=ee.mg_proc(), directory='test_ee_ttbar_noISR', eeISR=False)
mg.gen_evts('test_ee_ttbar_noISR', 'run01', ee.beam_params(), pythia=True, delphes=True)
