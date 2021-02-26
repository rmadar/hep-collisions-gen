// C++ std libs
#include <iostream>

// Root libs
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

// Delphes & ExRootAnalysis libs
#include "DelphesClasses.h"
#include "ExRootAnalysis/ExRootTreeReader.h"

using namespace std;


// Recursively print the decay the particle 'part'
// using its particle ID and it's pythia status code
void PrintDecay(GenParticle* p, TClonesArray *ps) {

  // Two decays
  if (p->D1>=0 && p->D2>=0) {
      GenParticle *d1 = (GenParticle*) ps->At(p->D1);
      GenParticle *d2 = (GenParticle*) ps->At(p->D2);
      cout << "Decaying (" << p->PID << ")[" << p->Status << "]" << endl;
      cout << "  (" << p->PID << ")[" << p->Status << "]  ->  ("
	   << d1->PID  << ")[" << d1->Status <<"] (" <<  d2->PID << ")[" << d2->Status << "]" << endl;
      PrintDecay(d1, ps);
      PrintDecay(d2, ps);
      return;
  }
  
  // Only one daugh ter (D1)
  else if (p->D1>=0) {
    GenParticle *d1 = (GenParticle*) ps->At(p->D1);
    cout << "Decaying (" << d1->PID << ")" << endl;
    PrintDecay(d1, ps);
    return;
  }
  
  // Only one daughter (D2)
  else if (p->D2>=0) {
    GenParticle *d2 = (GenParticle*) ps->At(p->D2);
    cout << "Decaying (" << d2->PID << ")" << endl;
    PrintDecay(d2, ps);
    return;
  }

  // No decay at all: do nothing
  else {
    return;
  }
  
}

// Get the PID of daughter particles.
vector<int> GetDecayPID(GenParticle *p, TClonesArray *ps) {

  // Initialize PIDs
  int id0 = p->PID;
  int id1 = id0;
  int id2 = id0;

  // Initialize generated particles
  GenParticle *p0 = p;
  GenParticle *d1 = p;
  GenParticle *d2 = p;

  // Loop until the two daughters are different from p
  while (d1->PID==id0 || d2->PID==id0) {
    d1 = (GenParticle*) ps->At(p0->D1);
    d2 = (GenParticle*) ps->At(p0->D2);
    if (d1->PID == id0) p0 = d1;
    if (d2->PID == id0) p0 = d2;
    d1 = (GenParticle*) ps->At(p0->D1);
    d2 = (GenParticle*) ps->At(p0->D2);
  }
  
  // Return a vector of 2 elements
  return vector<int>{d1->PID, d2->PID};
}

void RunAnalysis(TString inputFile){
  

  //---------------------------
  // 1. Load input informations
  //---------------------------

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);
  
  // Create a tree reader object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  
  // Get pointers to branches used in this analysis
  TClonesArray *branchEvtInfo   = treeReader->UseBranch("Event");
  TClonesArray *branchTruthPart = treeReader->UseBranch("Particle");

  //-------------------------------
  // 2. Prepare output informations
  //-------------------------------
  
  // Create a TFile containing processed information
  TString outputFile = inputFile;
  outputFile.ReplaceAll(".root","_processed.root");
  TFile *fout = new TFile(outputFile,"RECREATE");
  
  // Book a tree for simple variables
  double w_xsec(-999.);
  double init1_pz(-999.), init2_pz(-999.);
  int    init1_id(-999.), init2_id(-999.);
  int    init1_he(-999.), init2_he(-999.);
  double t_pt(-999), t_eta(-999), t_phi(-999), t_m(-999);
  double tbar_pt(-999), tbar_eta(-999), tbar_phi(-999), tbar_m(-999);
  int    t_pid(-999), tbar_pid(-999);
  double b_pt(-999), b_eta(-999), b_phi(-999), b_m(-999);
  double bbar_pt(-999), bbar_eta(-999), bbar_phi(-999), bbar_m(-999);
  int    b_pid(-999), bbar_pid(-999);
  double w_pt(-999), w_eta(-999), w_phi(-999), w_m(-999);
  double wbar_pt(-999), wbar_eta(-999), wbar_phi(-999), wbar_m(-999);
  int    w_pid(-999), wbar_pid(-999);
  double l_pt(-999), l_eta(-999), l_phi(-999), l_m(-999);
  double lbar_pt(-999), lbar_eta(-999), lbar_phi(-999), lbar_m(-999);
  int    l_pid(-999), lbar_pid(-999);
  double v_pt(-999), v_eta(-999), v_phi(-999), v_m(-999);
  double vbar_pt(-999), vbar_eta(-999), vbar_phi(-999), vbar_m(-999);
  int    v_pid(-999), vbar_pid(-999);

  double q1_pt(-999), q1_eta(-999), q1_phi(-999), q1_m(-999);
  double q1bar_pt(-999), q1bar_eta(-999), q1bar_phi(-999), q1bar_m(-999);
  int    q1_pid(-999), q1bar_pid(-999);
  double q2_pt(-999), q2_eta(-999), q2_phi(-999), q2_m(-999);
  double q2bar_pt(-999), q2bar_eta(-999), q2bar_phi(-999), q2bar_m(-999);
  int    q2_pid(-999), q2bar_pid(-999);
  
  TTree *outTree = new TTree("truth","truth");
  outTree->Branch("w_xsec"   , &w_xsec   , "w_xsec/D"  );
  outTree->Branch("init1_pz" , &init1_pz , "init1_pz/D");
  outTree->Branch("init2_pz" , &init2_pz , "init2_pz/D");
  outTree->Branch("init1_id" , &init1_id , "init1_id/I");
  outTree->Branch("init2_id" , &init2_id , "init2_id/I");
  outTree->Branch("init1_he" , &init1_id , "init1_he/I");
  outTree->Branch("init2_he" , &init2_id , "init2_he/I");
  outTree->Branch("t_pt"     , &t_pt     , "t_pt/D"    );
  outTree->Branch("t_eta"    , &t_eta    , "t_phi/D"   );
  outTree->Branch("t_phi"    , &t_phi    , "t_eta/D"   );
  outTree->Branch("t_m"      , &t_m      , "t_m/D"     );
  outTree->Branch("t_pid"    , &t_pid    , "t_pid/I"   );
  outTree->Branch("tbar_pt"  , &tbar_pt  , "tbar_pt/D" );
  outTree->Branch("tbar_eta" , &tbar_eta , "tbar_phi/D");
  outTree->Branch("tbar_phi" , &tbar_phi , "tbar_eta/D");
  outTree->Branch("tbar_m"   , &tbar_m   , "tbar_m/D"  );
  outTree->Branch("tbar_pid" , &tbar_pid , "tbar_pid/I");
  outTree->Branch("b_pt"     , &b_pt     , "b_pt/D"    );
  outTree->Branch("b_eta"    , &b_eta    , "b_phi/D"   );
  outTree->Branch("b_phi"    , &b_phi    , "b_eta/D"   );
  outTree->Branch("b_m"      , &b_m      , "b_m/D"     );
  outTree->Branch("b_pid"    , &b_pid    , "b_pid/I"   );
  outTree->Branch("bbar_pt"  , &bbar_pt  , "bbar_pt/D" );
  outTree->Branch("bbar_eta" , &bbar_eta , "bbar_phi/D");
  outTree->Branch("bbar_phi" , &bbar_phi , "bbar_eta/D");
  outTree->Branch("bbar_m"   , &bbar_m   , "bbar_m/D"  );
  outTree->Branch("bbar_pid" , &bbar_pid , "bbar_pid/I");
  outTree->Branch("w_pt"     , &w_pt     , "w_pt/D"    );
  outTree->Branch("w_eta"    , &w_eta    , "w_phi/D"   );
  outTree->Branch("w_phi"    , &w_phi    , "w_eta/D"   );
  outTree->Branch("w_m"      , &w_m      , "w_m/D"     );
  outTree->Branch("w_pid"    , &w_pid    , "w_pid/I"   );
  outTree->Branch("wbar_pt"  , &wbar_pt  , "wbar_pt/D" );
  outTree->Branch("wbar_eta" , &wbar_eta , "wbar_phi/D");
  outTree->Branch("wbar_phi" , &wbar_phi , "wbar_eta/D");
  outTree->Branch("wbar_m"   , &wbar_m   , "wbar_m/D"  );
  outTree->Branch("wbar_pid" , &wbar_pid , "wbar_pid/I");  
  outTree->Branch("l_pt"     , &l_pt     , "l_pt/D"    );
  outTree->Branch("l_eta"    , &l_eta    , "l_phi/D"   );
  outTree->Branch("l_phi"    , &l_phi    , "l_eta/D"   );
  outTree->Branch("l_m"      , &l_m      , "l_m/D"     );
  outTree->Branch("l_pid"    , &l_pid    , "l_pid/I"   );
  outTree->Branch("lbar_pt"  , &lbar_pt  , "lbar_pt/D" );
  outTree->Branch("lbar_eta" , &lbar_eta , "lbar_phi/D");
  outTree->Branch("lbar_phi" , &lbar_phi , "lbar_eta/D");
  outTree->Branch("lbar_m"   , &lbar_m   , "lbar_m/D"  );
  outTree->Branch("lbar_pid" , &lbar_pid , "lbar_pid/I");
  outTree->Branch("v_pt"     , &v_pt     , "v_pt/D"    );
  outTree->Branch("v_eta"    , &v_eta    , "v_phi/D"   );
  outTree->Branch("v_phi"    , &v_phi    , "v_eta/D"   );
  outTree->Branch("v_m"      , &v_m      , "v_m/D"     );
  outTree->Branch("v_pid"    , &v_pid    , "v_pid/I"   );
  outTree->Branch("vbar_pt"  , &vbar_pt  , "vbar_pt/D" );
  outTree->Branch("vbar_eta" , &vbar_eta , "vbar_phi/D");
  outTree->Branch("vbar_phi" , &vbar_phi , "vbar_eta/D");
  outTree->Branch("vbar_m"   , &vbar_m   , "vbar_m/D"  );
  outTree->Branch("vbar_pid" , &vbar_pid , "vbar_pid/I");  
  outTree->Branch("q1_pt"    , &q1_pt    , "q1_pt/D"    );
  outTree->Branch("q1_eta"   , &q1_eta   , "q1_phi/D"   );
  outTree->Branch("q1_phi"   , &q1_phi   , "q1_eta/D"   );
  outTree->Branch("q1_m"     , &q1_m     , "q1_m/D"     );
  outTree->Branch("q1_pid"   , &q1_pid   , "q1_pid/I"   );
  outTree->Branch("q1bar_pt" , &q1bar_pt , "q1bar_pt/D" );
  outTree->Branch("q1bar_eta", &q1bar_eta, "q1bar_phi/D");
  outTree->Branch("q1bar_phi", &q1bar_phi, "q1bar_eta/D");
  outTree->Branch("q1bar_m"  , &q1bar_m  , "q1bar_m/D"  );
  outTree->Branch("q1bar_pid", &q1bar_pid, "q1bar_pid/I");
  outTree->Branch("q2_pt"    , &q2_pt    , "q2_pt/D"    );
  outTree->Branch("q2_eta"   , &q2_eta   , "q2_phi/D"   );
  outTree->Branch("q2_phi"   , &q2_phi   , "q2_eta/D"   );
  outTree->Branch("q2_m"     , &q2_m     , "q2_m/D"     );
  outTree->Branch("q2_pid"   , &q2_pid   , "q2_pid/I"   );
  outTree->Branch("q2bar_pt" , &q2bar_pt , "q2bar_pt/D" );
  outTree->Branch("q2bar_eta", &q2bar_eta, "q2bar_phi/D");
  outTree->Branch("q2bar_phi", &q2bar_phi, "q2bar_eta/D");
  outTree->Branch("q2bar_m"  , &q2bar_m  , "q2bar_m/D"  );
  outTree->Branch("q2bar_pid", &q2bar_pid, "q2bar_pid/I");
  
  
  //------------------------
  // 3. Loop over all events
  //------------------------

  for(Int_t entry = 0; entry < numberOfEntries; ++entry){
    
    // Initialize all the variables
    w_xsec   = -999;
    init1_pz = -999;
    init2_pz = -999;
    init1_id = -999;
    init2_id = -999;
    init1_id = -999;
    init2_id = -999;
    t_pt     = -999;
    t_eta    = -999;
    t_phi    = -999;
    t_m      = -999;
    t_pid    = -999;
    tbar_pt  = -999;
    tbar_eta = -999;
    tbar_phi = -999;
    tbar_m   = -999;
    tbar_pid = -999;
    b_pt     = -999;
    b_eta    = -999;
    b_phi    = -999;
    b_m      = -999; 
    b_pid    = -999; 
    bbar_pt  = -999;
    bbar_eta = -999;
    bbar_phi = -999;
    bbar_m   = -999;
    bbar_pid = -999;
    w_pt     = -999;
    w_eta    = -999;
    w_phi    = -999;
    w_m      = -999;
    w_pid    = -999;
    wbar_pt  = -999;
    wbar_eta = -999;
    wbar_phi = -999;
    wbar_m   = -999;
    wbar_pid = -999;
    l_pt     = -999;
    l_eta    = -999;
    l_phi    = -999;
    l_m      = -999;
    l_pid    = -999;
    lbar_pt  = -999;
    lbar_eta = -999;
    lbar_phi = -999;
    lbar_m   = -999;
    lbar_pid = -999;
    v_pt     = -999;
    v_eta    = -999;
    v_phi    = -999;
    v_m      = -999;
    v_pid    = -999;
    vbar_pt  = -999;
    vbar_eta = -999;
    vbar_phi = -999;
    vbar_m   = -999;
    vbar_pid = -999;    
    q1_pt     = -999;
    q1_eta    = -999;
    q1_phi    = -999;
    q1_m      = -999;
    q1_pid    = -999;
    q1bar_pt  = -999;
    q1bar_eta = -999;
    q1bar_phi = -999;
    q1bar_m   = -999;
    q1bar_pid = -999;
    q2_pt     = -999;
    q2_eta    = -999;
    q2_phi    = -999;
    q2_m      = -999;
    q2_pid    = -999;
    q2bar_pt  = -999;
    q2bar_eta = -999;
    q2bar_phi = -999;
    q2bar_m   = -999;
    q2bar_pid = -999;
    
    // Load selected branches with data from specified event
    if ( entry%500==0 ) cout << "Processing event " << entry << endl;
    treeReader->ReadEntry(entry);
    
    // Information related to the event
    // Get the proper expected number of collisions ( Nobs = sigma / N[gen] * IntegratedLumi )
    HepMCEvent*  evtInfo = (HepMCEvent*) branchEvtInfo->At(0);
    w_xsec = evtInfo->Weight; // = sigma / N[gen]
    
    // Get some truth information
    GenParticle *top(NULL), *antitop(NULL);
    GenParticle *bot(NULL), *antibot(NULL);
    GenParticle *W(NULL)  , *antiW(NULL);
    GenParticle *lep(NULL), *antilep(NULL);
    GenParticle *nu(NULL) , *antinu(NULL);
    GenParticle *q1(NULL), *antiq1(NULL);
    GenParticle *q2(NULL) , *antiq2(NULL);
    for (int ip=0 ; ip<branchTruthPart->GetEntries() ; ip++){
      
      GenParticle *part = (GenParticle*) branchTruthPart->At(ip);
      int status  = part->Status;
      int pdgId   = part->PID;
      
      // Incoming particles
      if (abs(status) == 21) {
	double pz = part->Pz;
	if (pz>0) {
	  init1_pz = pz;
	  init1_id = part->PID;
	}
	if (pz<0) {
	  init2_pz = pz;
	  init2_id = part->PID;
	}
      }
      
      // Top quarks
      if ( pdgId ==  6 && abs(status) == 22) top = part;
      if ( pdgId == -6 && abs(status) == 22) antitop = part;
      
      // Bottom quarks
      if ( pdgId ==  5 && abs(status) == 23) bot = part;
      if ( pdgId == -5 && abs(status) == 23) antibot = part;
      
      // W-bosons
      if ( pdgId ==  24 && abs(status) == 22) W = part;
      if ( pdgId == -24 && abs(status) == 22) antiW = part;
      
      // Leptons and neutrinos
      //  - if tops are decayed in MG (t > w b, w > l v): status=23
      //  - if tops are decayed in Pythia: status=1, so checking the mother particle
      bool isLep     = (pdgId== 11 || pdgId== 13 || pdgId== 15);
      bool isAntiLep = (pdgId==-11 || pdgId==-13 || pdgId==-15);
      bool isNu      = (pdgId== 12 || pdgId== 14 || pdgId== 16);
      bool isAntiNu  = (pdgId==-12 || pdgId==-14 || pdgId==-16);
      if (isLep || isAntiLep || isNu || isAntiNu) {
	GenParticle *moth = (GenParticle*) branchTruthPart->At(part->M1);
	bool fromWp = moth->PID ==  24;
	bool fromWm = moth->PID == -24;
	if ( isLep     && (abs(status) == 23 || (status==1 && fromWm)) ) lep     = part;
	if ( isAntiLep && (abs(status) == 23 || (status==1 && fromWp)) ) antilep = part;
	if ( isNu      && (abs(status) == 23 || (status==1 && fromWp)) ) nu      = part;
	if ( isAntiNu  && (abs(status) == 23 || (status==1 && fromWm)) ) antinu  = part;
      }

      // Quarks (coming from W decay)
      bool isQ     = ( 0 < pdgId && pdgId < 6);
      bool isAntiQ = (-6 < pdgId && pdgId < 0);
      if (isQ || isAntiQ) {
	GenParticle *moth = (GenParticle*) branchTruthPart->At(part->M1);
	bool fromWp = moth->PID ==  24;
	bool fromWm = moth->PID == -24;
	if ( isQ     && fromWp ) q1     = part;
	if ( isAntiQ && fromWp ) antiq1 = part;
	if ( isQ     && fromWm ) q2     = part;
	if ( isAntiQ && fromWm ) antiq2 = part;
      }
    }
    
    // Determine W- decay mode
    if (W) {
      vector<int> ids = GetDecayPID(W, branchTruthPart);
      //cout << ids[0] << "  " << ids[1] << endl;
    }

    // Determine W+ decay mode
    if (antiW) {
      vector<int> ids = GetDecayPID(antiW, branchTruthPart);
      //cout << ids[0] << "  " << ids[1] << endl;
    }
    
    // Fill variables when corresponding particle were found    
    if (top) {
      t_pt  = top->PT;
      t_eta = top->Eta;
      t_phi = top->Phi;
      t_m   = top->Mass;
      t_pid = top->PID;
    }

    if (antitop) {
      tbar_pt  = antitop->PT;
      tbar_eta = antitop->Eta;
      tbar_phi = antitop->Phi;
      tbar_m   = antitop->Mass;
      tbar_pid = antitop->PID;
    }

    if (bot) {
      b_pt  = bot->PT;
      b_eta = bot->Eta;
      b_phi = bot->Phi;
      b_m   = bot->Mass;
      b_pid = bot->PID;
    }

    if (antibot) {
	bbar_pt  = antibot->PT;
	bbar_eta = antibot->Eta;
	bbar_phi = antibot->Phi;
	bbar_m   = antibot->Mass;
	bbar_pid = antibot->PID;
    }
    
    if (W) {
      w_pt  = W->PT;
      w_eta = W->Eta;
      w_phi = W->Phi;
      w_m   = W->Mass;
      w_pid = W->PID;
    }

    if (antiW) {
	wbar_pt  = antiW->PT;
	wbar_eta = antiW->Eta;
	wbar_phi = antiW->Phi;
	wbar_m   = antiW->Mass;
	wbar_pid = antiW->PID;
    }

    if (lep) {
      l_pt  = lep->PT;
      l_eta = lep->Eta;
      l_phi = lep->Phi;
      l_m   = lep->Mass;
      l_pid = lep->PID;
    }

    if (antilep) {
      lbar_pt  = antilep->PT;
      lbar_eta = antilep->Eta;
      lbar_phi = antilep->Phi;
      lbar_m   = antilep->Mass;
      lbar_pid = antilep->PID;
    }

    if (nu) {
      v_pt  = nu->PT;
      v_eta = nu->Eta;
      v_phi = nu->Phi;
      v_m   = nu->Mass;
      v_pid = nu->PID;
    }

    if (antinu) {
      vbar_pt  = antinu->PT;
      vbar_eta = antinu->Eta;
      vbar_phi = antinu->Phi;
      vbar_m   = antinu->Mass;
      vbar_pid = antinu->PID;
    }

    if (q1) {
      q1_pt  = q1->PT;
      q1_eta = q1->Eta;
      q1_phi = q1->Phi;
      q1_m   = q1->Mass;
      q1_pid = q1->PID;
    }

    if (antiq1) {
      q1bar_pt  = antiq1->PT;
      q1bar_eta = antiq1->Eta;
      q1bar_phi = antiq1->Phi;
      q1bar_m   = antiq1->Mass;
      q1bar_pid = antiq1->PID;
    }

    if (q2) {
      q2_pt  = q2->PT;
      q2_eta = q2->Eta;
      q2_phi = q2->Phi;
      q2_m   = q2->Mass;
      q2_pid = q2->PID;
    }

    if (antiq2) {
      q2bar_pt  = antiq2->PT;
      q2bar_eta = antiq2->Eta;
      q2bar_phi = antiq2->Phi;
      q2bar_m   = antiq2->Mass;
      q2bar_pid = antiq2->PID;
    }
    
    // Fill the tree
    outTree->Fill();
  }
  
  
  //--------------------
  // 4. Final operations
  //--------------------
  fout->cd();
  outTree->Write();
  fout->Close();

  return;

}

