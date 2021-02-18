import generation_handler as gen

# Defning the two types of collision under study
fs = 't t~, (t > b W+, W+ > l+ vl), (t~ > b~ W-, W- > l- vl~)'
pp13TeV  = gen.collision('p p'    , fs, ebeam1='6500', ebeam2='6500')
pp100TeV = gen.collision('p p'    , fs, ebeam1='50000', ebeam2='50000')
ee       = gen.collision('mu+ mu-', fs, ebeam1='180' , ebeam2='180')
mm       = gen.collision('e+ e-'  , fs, ebeam1='180' , ebeam2='180')
tt       = gen.collision('ta+ ta-', fs, ebeam1='180' , ebeam2='180')

# Get a MadGraph instance
mg = gen.madgraph('../MG5_aMC')

# Create the process directories
mg.run_proc_dir(proc=pp.mg_proc(), directory='pp_ttbar')
mg.run_proc_dir(proc=ee.mg_proc(), directory='ee_ttbar')
mg.run_proc_dir(proc=mm.mg_proc(), directory='mm_ttbar')
mg.run_proc_dir(proc=tt.mg_proc(), directory='tt_ttbar')

# Generate events for the LHC
mg.gen_evts('pp_ttbar', 'run01', pp.beam_params())
mg.gen_evts('ee_ttbar', 'run01', ee.beam_params())
mg.gen_evts('mm_ttbar', 'run01', mm.beam_params())
mg.gen_evts('tt_ttbar', 'run01', tt.beam_params())

