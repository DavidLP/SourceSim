//Based on Geant4 TestEm11 example
{
	gROOT->Reset();
	//gStyle->SetTitleFontSize(0.3);
	// Draw histograms fill by  simulation
	TFile f1("unit_test_1_0.root");
	TFile f2("unit_test_1_1.root");
	TH1D* h1 = (TH1D*) f1.Get("10");
	TH1D* h2 = (TH1D*) f2.Get("10");
	h1->SetTitle("#splitline{Angular distribution of 15.7 MeV e- after 19.3/9.6 um gold foil}{Hanson et al. Phys.Rev.84p.634(1951)}");
	h1->SetTitleOffset(3);
	h1 = (TH1D*) h1->Rebin(4);
	h2 = (TH1D*) h2->Rebin(4);
	h1->Scale(1. / h1->GetMaximum());
	h2->Scale(1. / h2->GetMaximum());
	TGraph* g1 = new TGraph(h1);
	g1->SetLineWidth(2);
	//g1->RemovePoint(0);
	TGraph* g2 = new TGraph(h2);
	g2->SetLineWidth(2);
	//g2->RemovePoint(0);

	g1->GetXaxis()->SetTitle("Theta [deg]");
	g1->GetYaxis()->SetTitle("dN/dOmega");
	g1->GetYaxis()->SetTitleOffset(1.3);
	g1->Draw("AL");
	g2->Draw("L");

	ifstream in1, in2;
	in1.open("19um.ascii");
	in2.open("9um.ascii");

	Double_t x, y;
	// First indicate number of data
	int nbdata = 0;
	in1 >> nbdata;
	TGraph* t = new TGraph(16);
	t->SetMarkerStyle(22);
	for ( int i = 0; i < nbdata; i++ ) {
		in1 >> x >> y;
		if (!in1.good()) break;
		t->SetPoint(i, x, y / 1.98e-2);
		cout<<x<<" "<<y<<endl;
	}
	in1.close();

	nbdata = 0;
	in2 >> nbdata;
	TGraph* t2 = new TGraph(16);
	t2->SetMarkerStyle(20);
	for ( int i = 0; i < nbdata; i++ ) {
		in2 >> x >> y;
		if (!in2.good()) break;
		t2->SetPoint(i, x, y/4.40e-2);
		cout<<x<<" "<<y<<endl;
	}
	in2.close();

	t->Draw("P");
	t2->Draw("P");

	// Print the histograms legend
	TLegend* legend = new TLegend(0.6,0.5,0.8,0.68);
	legend->AddEntry(h1,"simulation ","l");
	legend->AddEntry(t,"Hanson data","P");
	legend->Draw();
}
