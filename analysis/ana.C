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
  TClonesArray *branchTruthJet  = treeReader->UseBranch("GenJet");
  TClonesArray *branchTruthMET  = treeReader->UseBranch("GenMissingET");
  TClonesArray *branchJet       = treeReader->UseBranch("Jet");
  TClonesArray *branchElectron  = treeReader->UseBranch("Electron");
  TClonesArray *branchMuon      = treeReader->UseBranch("Muon");
  TClonesArray *branchMET       = treeReader->UseBranch("MissingET");
  TClonesArray *branchHT        = treeReader->UseBranch("ScalarHT");


  //-------------------------------
  // 2. Prepare output informations
  //-------------------------------
  
  // Create a TFile containing processed information
  TString outputFile = inputFile;
  outputFile.ReplaceAll(".root","_processed.root");
  TFile *fout = new TFile(outputFile,"RECREATE");
  
  // Book a tree for simple variables
  int    Njets(-99.)    , Nbjets(-99.)    , Nel(-99.)       , Nmu(-99.);
  double leadJetPt(-99.), leadJetEta(-99.), leadJetPhi(-99.);
  double leadElPt(-99.) , leadElEta(-99.) , leadElPhi(-99.);
  double leadMuPt(-99.) , leadMuEta(-99.) , leadMuPhi(-99.);
  double subleadElPt(-99.), subleadElEta(-99.), subleadElPhi(-99.);
  double subleadMuPt(-99.), subleadMuEta(-99.), subleadMuPhi(-99.);
  int    leadElQ(-99.),leadMuQ(-99.),subleadElQ(-99.),subleadMuQ(-99.);
  double mET(-99), truth_mET(-99), HT(-99);
  int    partFlavor1(-99.), partFlavor2(-99.);
  double partPz1(-9e5), partPz2(-9e5);
  double w_xsec(-99.);
  int    NtruthPart(-99);
  TTree *outTree = new TTree("AnalyzedTree","AnalyzedTree");
  outTree->Branch("Njets"       ,&Njets       ,"Njets/I" );
  outTree->Branch("Nbjets"      ,&Nbjets      ,"Nbjets/I");
  outTree->Branch("Nel"         ,&Nel         ,"Nel/I");
  outTree->Branch("Nmu"         ,&Nmu         ,"Nmu/I");
  outTree->Branch("leadJetPt"   ,&leadJetPt   ,"leadJetPt/D");
  outTree->Branch("leadJetPt"   ,&leadJetEta  ,"leadJetEta/D");
  outTree->Branch("leadJetPhi"  ,&leadJetPhi  ,"leadJetPhi/D");
  outTree->Branch("leadElPt"    ,&leadElPt    ,"leadElPt/D");
  outTree->Branch("leadElEta"   ,&leadElEta   ,"leadElEta/D");
  outTree->Branch("leadElPhi"   ,&leadElPhi   ,"leadElPhi/D");
  outTree->Branch("leadElQ"     ,&leadElQ     ,"leadElQ/I");
  outTree->Branch("subleadElPt" ,&subleadElPt ,"subleadElPt/D");
  outTree->Branch("subleadElEta",&subleadElEta,"subleadElEta/D");
  outTree->Branch("subleadElPhi",&subleadElPhi,"subleadElPhi/D");
  outTree->Branch("subleadElQ"  ,&subleadElQ  ,"subleadElQ/I");
  outTree->Branch("leadMuPt"    ,&leadMuPt    ,"leadMuPt/D");
  outTree->Branch("leadMuEta"   ,&leadMuEta   ,"leadMuEta/D");
  outTree->Branch("leadMuPhi"   ,&leadMuPhi   ,"leadMuPhi/D");
  outTree->Branch("leadMuQ"     ,&leadMuQ     ,"leadMuQ/I");
  outTree->Branch("subleadMuPt" ,&subleadMuPt ,"subleadMuPt/D");
  outTree->Branch("subleadMuEta",&subleadMuEta,"subleadMuEta/D");
  outTree->Branch("subleadMuPhi",&subleadMuPhi,"subleadMuPhi/D");
  outTree->Branch("subleadMuQ"  ,&subleadMuQ  ,"subleadMuQ/I");
  outTree->Branch("mET"         ,&mET         ,"mET/D");
  outTree->Branch("truth_mET"   ,&truth_mET   ,"truth_mET/D");
  outTree->Branch("HT"          ,&HT          ,"HT/D");
  outTree->Branch("w_xsec"      ,&w_xsec      ,"w_xsec/D");
  outTree->Branch("partFlavor1" ,&partFlavor1 ,"partFlavor1/I");
  outTree->Branch("partFlavor2" ,&partFlavor2 ,"partFlavor2/I");
  outTree->Branch("partPz1"     ,&partPz1     ,"partPz1/D");
  outTree->Branch("partPz2"     ,&partPz2     ,"partPz2/D");
  outTree->Branch("NtruthPart"  ,&NtruthPart  ,"NtruthPart/I");

  
  // Book histograms
  TH1 *histTruthJetPT = new TH1F("truth_jet_pt", "Truth jet P_{T}", 50, 0.0, 300.0);
  TH1 *histJetPT      = new TH1F("jet_pt"      , "jet P_{T}"      , 50, 0.0, 300.0);
  TH1 *histMass       = new TH1F("mass", "M_{inv}(e_{1}, e_{2})", 100, 40.0, 140.0);

  // Object selection parameters
  double JetPtCut = 20.;
  double ElPtCut  = 15.;
  double ElIsoCut = 1.0;
  double MuPtCut  = 15.;
  double MuIsoCut = 1.0;


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
    NtruthPart = 0;
    vector<GenParticle*> IncParton, IncProton;
    for (int ip=0 ; ip<branchTruthPart->GetEntries() ; ip++){
      GenParticle *part = (GenParticle*) branchTruthPart->At(ip);
      int status = part->Status;
      int pdgId  = part->PID;
      if (status==1) NtruthPart++;
      if (status==21) IncParton.push_back( part );
    }
    if ( IncParton.size()!=2) cout << "Warning:: There are " << IncParton.size()  << " colliding partons !" << endl;
    partPz1 = IncParton[0]->Pz;
    partPz2 = IncParton[1]->Pz;
    partFlavor1 = IncParton[0]->PID;
    partFlavor2 = IncParton[1]->PID;


    // Before any selection - loop over jets and fill jet pT histogram
    for (int ijet=0 ; ijet<branchTruthJet->GetEntries() ; ijet++){
      Jet *jet = (Jet*) branchTruthJet->At(ijet);
      histTruthJetPT->Fill(jet->PT,w_xsec);
    }

    for (int ijet=0 ; ijet<branchJet->GetEntries() ; ijet++){
      Jet *jet = (Jet*) branchJet->At(ijet);
      histJetPT->Fill(jet->PT,w_xsec);
    }



    // Select objects
    //---------------
    vector<Jet*>       selected_Jets, selected_bJets;
    vector<Electron*>  selected_El;
    vector<Muon*>      selected_Mu;
    // Jets & b-jets
    for (int ijet=0 ; ijet<branchJet->GetEntries() ; ijet++){
      Jet *jet = (Jet*) branchJet->At(ijet);
      if (jet->PT > JetPtCut){
	selected_Jets.push_back( jet );
	if (jet->BTag == 1)
	  selected_bJets.push_back( jet );
      }
    }
    // electrons
    for (int iel=0 ; iel<branchElectron->GetEntries() ; iel++){
      Electron* el = (Electron*) branchElectron->At(iel);
      if (el->PT > ElPtCut && el->IsolationVar<ElIsoCut)
	selected_El.push_back( el );
    }
    // Muons
    for (int imu=0 ; imu<branchMuon->GetEntries() ; imu++){
      Muon* mu = (Muon*) branchMuon->At(imu);
      if (mu->PT > MuPtCut && mu->IsolationVar<MuIsoCut)
      	selected_Mu.push_back( mu );
    }
    // Missing ET
    if (     branchMET->GetEntries()>1) cout << "Warning:: more than one object in the MET container" << endl;
    if (branchTruthMET->GetEntries()>1) cout << "Warning:: more than one object in the MET container" << endl;
    MissingET* recoMET  = (MissingET*) branchMET->At(0);
    MissingET* truthMET = (MissingET*) branchTruthMET->At(0);



    // Select events if you want ot keep only a certain type of collisions
    //--------------------------------------------------------------------
    


    // compute event-level variable (ie including many object in the collision)
    //------------------------------------------------------------------------
    // Example 1: invariant mass
    Electron *elec1, *elec2;
    if(selected_El.size() >= 2){
      elec1 = (Electron *) selected_El[0];
      elec2 = (Electron *) selected_El[1];
      TLorentzVector pEl1 = elec1->P4();
      TLorentzVector pEl2 = elec2->P4();
      double         Mee  = (pEl1+pEl2).M();
      histMass->Fill( Mee );
    }

    // Example 2: probe of the full reconstruced energy (variable called HT)
    // HT = sum[ reco object ] pT
    double hT=recoMET->MET;
    for (unsigned ijet=0 ; ijet<selected_Jets.size() ; ijet++)
      hT = hT + selected_Jets[ijet]->PT;
    for (unsigned iel=0 ; iel<selected_El.size() ; iel++)
      hT = hT + selected_El[iel]->PT;
    for (unsigned imu=0 ; imu<selected_Mu.size() ; imu++)
      hT = hT + selected_Mu[imu]->PT;


    
    // Fill the variables for the tree (should be done after basic selection)
    //-----------------------------------------------------------------------
    Njets     = selected_Jets.size();
    Nbjets    = selected_bJets.size();
    
    Nel = selected_El.size();
    if (Nel>0){
      leadElPt  = selected_El[0]->PT;
      leadElEta = selected_El[0]->Eta;
      leadElPhi = selected_El[0]->Phi;
      leadElQ   = selected_El[0]->Charge;
    }
    if (Nel>1){
      subleadElPt  = selected_El[1]->PT;
      subleadElEta = selected_El[1]->Eta;
      subleadElPhi = selected_El[1]->Phi;
      subleadElQ   = selected_El[1]->Charge;
    }
    Nmu = selected_Mu.size();
    if (Nmu>0){
      leadMuPt  = selected_Mu[0]->PT;
      leadMuEta = selected_Mu[0]->Eta;
      leadMuPhi = selected_Mu[0]->Phi;
      leadMuQ   = selected_Mu[0]->Charge;
    }
    if (Nmu>1){
      subleadMuPt  = selected_Mu[1]->PT;
      subleadMuEta = selected_Mu[1]->Eta;
      subleadMuPhi = selected_Mu[1]->Phi;
      subleadMuQ   = selected_Mu[1]->Charge;
    }
    
    mET       = recoMET->MET;
    truth_mET = truthMET->MET;
    HT        = hT;
    outTree->Fill();
    
  }



  //--------------------
  // 4. Final operations
  //--------------------
  fout->cd();
  outTree->Write();
  histTruthJetPT->Write();
  histJetPT->Write();
  histMass->Write();
  fout->Close();


  return;

}

