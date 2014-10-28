
{
   gROOT->Reset();

   // Draw histograms fill by Geant4 TestEm11 simulation
   TFile f1("unit_test_1_0.root");
   TH1D* h1 = (TH1D*) f1.Get("5");
   h1->SetTitle("Angular distribution of 15.7 MeV e- after 19um Au foil, Hanson et al. Phys.Rev.84p.634(1951)");
   h1->Scale(0.0198 / h1->GetMaximum());

TH1D* h2 = (TH1D*) f1.Get("4");

  TGraph* g1 = new TGraph(h1);
g1->SetLineWidth(2);
   
	g1->GetXaxis()->SetTitle("Theta [deg]");
   g1->GetYaxis()->SetTitle("dN/dOmega");
g1->GetYaxis()->SetTitleOffset(1.3);
	g1->Draw("AL");
  
   
   /*TFile f2("./19um.opt3.root");
   TH1D* h2 = (TH1D*) f2.Get("12");
   h2->SetStats(kFALSE);  // Eliminate statistics box
   h2->SetLineColor(kBlue);
   h2->Draw("SAME HIST");
   
   TFile f3("./19um.local.root");
   TH1D* h3 = (TH1D*) f3.Get("12");
   h3->SetStats(kFALSE);  // Eliminate statistics box
   h3->SetLineColor(kGreen);
   h3->Draw("SAME HIST"); 
  */   
/* data
* angle distribution of  15.7 MeV electrons
* transmitted through thin gold foils.
* A.O.Hanson et al. Phys.Rev.84 (1951) page 634.
*/

   ifstream in;
   in.open("19um.ascii");
   //in.open("./9um.ascii");
   
   TMarker *pt;
   Double_t x, y;
   // First indicate number of data
   int nbdata = 0;
   in >> nbdata;
   TGraph* t = new TGraph(16);
   for ( int i = 0 ; i < nbdata ; i++ ) {
      in >> x >> y ;
      if (!in.good()) break;
	t->SetPoint(i, x, y);
	cout<<x<<" "<<y<<endl;
   }
   in.close();
   t->Draw("P*");

   // Print the histograms legend
   TLegend* legend = new TLegend(0.6,0.5,0.8,0.68);
   legend->AddEntry(h1,"simulation ","l");
   //legend->AddEntry(h2,"opt3 ","l");
   //legend->AddEntry(h3,"local","l");      
   legend->AddEntry(t,"Hanson data","P");
   legend->Draw();
}
