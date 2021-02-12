{
  cout << "Add needed include path ..." << endl;
  gROOT->ProcessLine(".include ../MG5_aMC/Delphes/external");
  gROOT->ProcessLine(".include ../MG5_aMC/Delphes/classes");
  gROOT->ProcessLine(".include ../MG5_aMC/Delphes/display");

  cout << "Add Delphes library ..." << endl;
  gSystem->Load("libDelphesDisplay.so"); // For Delphes objects and display
  //gSystem->Load("libDelphes.so");  // Just for Delphes objects


}
