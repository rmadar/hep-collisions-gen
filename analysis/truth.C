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
  double l_pt(-999), l_eta(-999), l_phi(-999), l_m(-999);
  double lbar_pt(-999), lbar_eta(-999), lbar_phi(-999), lbar_m(-999);
  int    l_pid(-999), lbar_pid(-999);
  double v_pt(-999), v_eta(-999), v_phi(-999), v_m(-999);
  double vbar_pt(-999), vbar_eta(-999), vbar_phi(-999), vbar_m(-999);
  int    v_pid(-999), vbar_pid(-999);
  TTree *outTree = new TTree("truth","truth");
  outTree->Branch("w_xsec"  , &w_xsec  , "w_xsec/D"  );
  outTree->Branch("init1_pz", &init1_pz, "init1_pz/D");
  outTree->Branch("init2_pz", &init2_pz, "init2_pz/D");
  outTree->Branch("init1_id", &init1_id, "init1_id/I");
  outTree->Branch("init2_id", &init2_id, "init2_id/I");
  outTree->Branch("init1_he", &init1_id, "init1_he/I");
  outTree->Branch("init2_he", &init2_id, "init2_he/I");
  outTree->Branch("t_pt"    , &t_pt    , "t_pt/D"    );
  outTree->Branch("t_eta"   , &t_eta   , "t_phi/D"   );
  outTree->Branch("t_phi"   , &t_phi   , "t_eta/D"   );
  outTree->Branch("t_m"     , &t_m     , "t_m/D"     );
  outTree->Branch("t_pid"   , &t_pid   , "t_pid/I"   );
  outTree->Branch("tbar_pt" , &tbar_pt , "tbar_pt/D" );
  outTree->Branch("tbar_eta", &tbar_eta, "tbar_phi/D");
  outTree->Branch("tbar_phi", &tbar_phi, "tbar_eta/D");
  outTree->Branch("tbar_m"  , &tbar_m  , "tbar_m/D"  );
  outTree->Branch("tbar_pid", &tbar_pid, "tbar_pid/I");
  outTree->Branch("b_pt"    , &b_pt    , "b_pt/D"    );
  outTree->Branch("b_eta"   , &b_eta   , "b_phi/D"   );
  outTree->Branch("b_phi"   , &b_phi   , "b_eta/D"   );
  outTree->Branch("b_m"     , &b_m     , "b_m/D"     );
  outTree->Branch("b_pid"   , &b_pid   , "b_pid/I"   );
  outTree->Branch("bbar_pt" , &bbar_pt , "bbar_pt/D" );
  outTree->Branch("bbar_eta", &bbar_eta, "bbar_phi/D");
  outTree->Branch("bbar_phi", &bbar_phi, "bbar_eta/D");
  outTree->Branch("bbar_m"  , &bbar_m  , "bbar_m/D"  );
  outTree->Branch("bbar_pid", &bbar_pid, "bbar_pid/I");
  outTree->Branch("l_pt"    , &l_pt    , "l_pt/D"    );
  outTree->Branch("l_eta"   , &l_eta   , "l_phi/D"   );
  outTree->Branch("l_phi"   , &l_phi   , "l_eta/D"   );
  outTree->Branch("l_m"     , &l_m     , "l_m/D"     );
  outTree->Branch("l_pid"   , &l_pid   , "l_pid/I"   );
  outTree->Branch("lbar_pt" , &lbar_pt , "lbar_pt/D" );
  outTree->Branch("lbar_eta", &lbar_eta, "lbar_phi/D");
  outTree->Branch("lbar_phi", &lbar_phi, "lbar_eta/D");
  outTree->Branch("lbar_m"  , &lbar_m  , "lbar_m/D"  );
  outTree->Branch("lbar_pid", &lbar_pid, "lbar_pid/I");
  outTree->Branch("v_pt"    , &v_pt    , "v_pt/D"    );
  outTree->Branch("v_eta"   , &v_eta   , "v_phi/D"   );
  outTree->Branch("v_phi"   , &v_phi   , "v_eta/D"   );
  outTree->Branch("v_m"     , &v_m     , "v_m/D"     );
  outTree->Branch("v_pid"   , &v_pid   , "v_pid/I"   );
  outTree->Branch("vbar_pt" , &vbar_pt , "vbar_pt/D" );
  outTree->Branch("vbar_eta", &vbar_eta, "vbar_phi/D");
  outTree->Branch("vbar_phi", &vbar_phi, "vbar_eta/D");
  outTree->Branch("vbar_m"  , &vbar_m  , "vbar_m/D"  );
  outTree->Branch("vbar_pid", &vbar_pid, "vbar_pid/I");
  
  
  //------------------------
  // 3. Loop over all events
  //------------------------

  for(Int_t entry = 0; entry < numberOfEntries; ++entry){

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
    for (int ip=0 ; ip<branchTruthPart->GetEntries() ; ip++){
      
      GenParticle *part = (GenParticle*) branchTruthPart->At(ip);
      int status = part->Status;
      int pdgId  = part->PID;
      
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
      
      // top quarks
      if ( pdgId ==  6 && abs(status) == 22) top = part;
      if ( pdgId == -6 && abs(status) == 22) antitop = part;
      
      // Bottom quarks
      if ( pdgId ==  5 && abs(status) == 23) bot = part;
      if ( pdgId == -5 && abs(status) == 23) antibot = part;
      
      // W-bosons
      if ( pdgId ==  24 && abs(status) == 22) W = part;
      if ( pdgId == -24 && abs(status) == 22) antiW = part;
      
      // leptons
      bool isLep     = (pdgId== 11 || pdgId== 13 || pdgId== 15);
      bool isAntiLep = (pdgId==-11 || pdgId==-13 || pdgId==-15);
      if ( isLep     && abs(status) == 23) lep = part;
      if ( isAntiLep && abs(status) == 23) antilep = part;
      
      // Neutrino
      bool isNu     = (pdgId== 12 || pdgId== 14 || pdgId== 16);
      bool isAntiNu = (pdgId==-12 || pdgId==-14 || pdgId==-16);
      if ( isNu     && abs(status) == 23) nu = part;
      if ( isAntiNu && abs(status) == 23) antinu = part;
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

