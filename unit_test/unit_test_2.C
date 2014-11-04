{
   gROOT->Reset();

   // Draw histograms fill by Geant4 TestEm5 simulation
   TFile f1("./unit_test_2.root");
   TH1D* h1 = (TH1D*) f1.Get("7");
   h1->SetTitle("Energy deposited in 530 um silicon by 1 MeV e-");
   h1->GetXaxis()->SetTitle("Ekine (keV)");
   h1->GetYaxis()->SetTitle("nb / MeV");
   h1->Scale(7.6 / h1->GetMaximum());
   h1->SetStats(kFALSE);  // Eliminate statistics box
   h1->SetLineColor(1);   // black
   h1->Draw("HIST");

   ifstream in;
   in.open("530um.ascii");
   
   // First indicate number of data
   int nbdata = 0;
   in >> nbdata;
   
   // Create a new histogram with data.acsii values
   float x_min = 110;
   float x_max = 1030;
   TH1F* h3 = new TH1F("h1f","",nbdata,x_min,x_max);

   Float_t x, y;
   while (1) {
      in >> x >> y ;
      if (!in.good()) break;
      h3->Fill(x * 1000.,y);
      cout<<x<<"\t"<<y<<"\n";
   }
   in.close();

   // Draw histogram fill by data.acsii values
   h3->SetLineColor(2);   // red
   h3->Draw("SAME");

   // Print the histograms legend
   TLegend *legend = new TLegend(0.6,0.6,0.8,0.8);
   legend->AddEntry(h1,"Simulation","l");  
   legend->AddEntry(h3,"Data","L");
   legend->Draw();
}
