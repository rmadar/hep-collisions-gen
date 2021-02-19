import os

class collision:

    '''
    This class is meant to ease the definition of a given collision in MG,
    for which both initial state and final state can be changed.
    Initial state, here, is defined by the incoming particle, their
    energy as well as their polarization.

    NOTE: sub-processes is currently not supported.
    '''
    
    pdf_name = {
        '0': 'no PDF',
        '1': 'proton',
        '-1': 'anti-proton',
    }
    
    def __init__(self, initial_state, final_state,
                 ebeam1='6500', ebeam2='6500',
                 polbeam1='0', polbeam2='0', name=''):
        '''
        Initialize all the needed information to generate 
        a given type of collisions, using the MG syntax.
         - initial_state (string): list of initial particles, eg 'p p'
         - final_state   (string): list of final particles, eg 't t~, (t > W- b), (t~> W+ b~)'
         - ebeam1        (string): energy in GeV of the first beam, default: '7500'
         - ebeam2        (string): energy in GeV of the second beam, default: '7500'
         - polbeam1      (string): polarization of the first beam, default '0'
         - polbeam2      (string): polarization of the second beam, default '0'
         - name          (string): collision name, optional
        '''

        # Initial/final state
        self.inital_state = initial_state
        self.final_sate   = final_state
        self.name         = name
        
        # Beam caracteristics
        self.beam1_energy       = ebeam1
        self.beam2_energy       = ebeam2
        self.beam1_polarization = polbeam1
        self.beam2_polarization = polbeam2

        return

    def mg_proc(self):
        '''
        Return the MG process to be pu in the proc card
        '''
        return '{} > {}'.format(self.inital_state, self.final_sate)

    def beam_params(self):
        '''
        Return a dictionnary of beam parameters as to be specified
        in un_card.dat, used by MG.
        '''
        return {
            'ebeam1'  : self.beam1_energy,
            'ebeam2'  : self.beam2_energy,
            'polbeam1': self.beam1_polarization,
            'polbeam2': self.beam2_polarization
        }
    
    def __str__(self):
        i, f   = self.inital_state, self.final_sate
        e1, e2 = self.beam1_energy, self.beam2_energy
        p1, p2 = self.beam1_polarization, self.beam2_polarization
        d1, d2 = self.beam1_pdf, self.beam2_pdf
        n1, n2 = self.pdf_name[self.beam1_pdf], self.pdf_name[self.beam2_pdf]
        s  = ''
        if self.name != '':
            s += '** ' + self.name + ' **\n'
        s += 'Process           : {} > {}\n'
        s += 'Beam energies     : {} + {} GeV\n'
        s += 'Beam polarizations: {}, {} \n'
        s += 'Beam PDFs         : {} ({}), {} ({})'
        return s.format(i, f, e1, e2, p1, p2, d1, d2, n1, n2)

    
class madgraph:

    def __init__(self, path):
        '''
        Creates a madgraph_handler object
        '''
        self.path = path
    

    def run_proc_dir(self, proc, directory, eeISR=False):
        '''
        This function creates the process directory
        later used to compute cross-section and 
        generate events.
        '''

        # Checking that eeISR is called for e+ e- collisions only
        istate = proc.split('>')[0].strip()
        if eeISR:
            if istate != 'e+ e-' and istate != 'e- e+':
                war  = '\n\n [generation_handler.madgraph WARNING] eeISR is enabled '
                war += 'for \'{}\' initial state, while only \'e+ e-\' is supported. '
                war += 'Will do nothing.\n\n'
                print(war.format(istate))
                eeISR = False
        
        # Create the proc card on the fly
        f = open('{}.txt'.format(directory), 'w')
        f.write('import model sm\n\n')
        f.write('define p  = g u c d s u~ c~ d~ s~\n')
        f.write('define p~ = g u c d s u~ c~ d~ s~\n')
        f.write('define j = g u c d s u~ c~ d~ s~\n')
        f.write('define l+ = e+ mu+\n')
        f.write('define l- = e- mu-\n')
        f.write('define l- = e- mu-\n')
        f.write('define vl = ve vm vt\n')
        f.write('define vl~ = ve~ vm~ vt~\n\n')
        f.write('generate {}\n\n'.format(proc))
        print(eeISR)
        if eeISR:
            f.write('output EE_ISR {}'.format(directory))
        else:
            f.write('output {}'.format(directory))
        f.close()

        # Create the directory
        os.system('{}/bin/mg5_aMC {}.txt'.format(self.path, directory))

        # Copy the created proc card into the newly created directory
        os.system('cp {}.txt {}'.format(directory, directory))

        
    def gen_evts(self, directory, run, params={}, beam_params={}, pythia=False, delphes=False):
        '''
        This function generates events of a given process, 
        based on its directory.
        
         - directory [string]: name of the process directory, it must be created
                               before using run_proc_dir()
         - run [string]: name of the run
         - params [dict]: parameters to be changed, given as {param_name: param_value}
         - beam_params [dict]: beam parameters to be changed, can be directly collision.beam_params()
         - pythia [bool]: enabling showering with pythia
         - delphes [bool]: enabling detector simulation
        '''

        # Check the directory exists
        if not os.path.exists(directory):
            err  = '\n\n [generation_handler.madgraph ERROR] Directory {} doesn\'t exist,'
            err += 'please make sure you called mg.run_proc_dir(proc, \'{}\') first'
            raise NameError(err.format(directory, directory))
        
        # Create proc_card on the fly
        f = open('{}_gen.txt'.format(directory), 'w')
        f.write('launch {} -n {} \n'.format(directory, run))

        # Disable analysis by default
        f.write('analysis=OFF\n')

        # Adding showering and detector simulation
        if pythia:
            f.write('shower=Pythia8\n')
        if delphes:
            f.write('detector=Delphes\n')
        
        # Update parameters
        for p, v in params.items():
            f.write('set {} {}\n'.format(p, v))

        f.close()

        # Source the environement, if a setup if found
        if os.path.isfile('../setup.sh'):
            os.system('source ../setup.sh')
        
        # Create the directory
        os.system('{}/bin/mg5_aMC {}_gen.txt'.format(self.path, directory))

        # Copy the created proc card into the newly created directory
        os.system('cp {}_gen.txt {}'.format(directory, directory))
        
    
