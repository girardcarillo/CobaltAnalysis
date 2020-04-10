// Author Cloé Girard-Carillo girardcarillo@lal.in2p3.fr

#include <limits>
#include <string>
#include <iostream>

#include <TTree.h>
#include <TFile.h>
#include <TObject.h>
#include <TH1.h>
#include <TH2.h>

#include "/home/girardcarillo/Workdir/SNPlot/RootDisplay.h"

using namespace std;

void Subtract_bdf(const char* file_data_bdf,const char* file_bdf){

  TFile *theDataFile = new TFile(file_data_bdf,"READ");
  TH1F *henergy_spectrum_data_bdf = new TH1F ("energy_spectrum_data_bdf","", 150,0,4);
  henergy_spectrum_data_bdf = (TH1F*)theDataFile->Get("Total_energy_spectrum");

  TFile *theBdfFile = new TFile(file_bdf,"READ");
  TH1F *henergy_spectrum_bdf = new TH1F ("energy_spectrum_bdf","", 150,0,4);
  henergy_spectrum_bdf = (TH1F*)theBdfFile->Get("Total_energy_spectrum_bdf");

  // Define the Canvas
  TCanvas *c = new TCanvas("c", "canvas",10,10,2000,1000);
  gStyle->SetOptStat(0);
  // Upper plot will be in pad1
  TPad *pad1 = new TPad("pad1", "pad1", 0, 0.29, 1, 1.0);
  pad1->SetBottomMargin(0.02); // Upper and lower plot are joined
  pad1->SetGridx();         // Vertical grid
  pad1->Draw();             // Draw the upper pad: pad1
  pad1->cd();               // pad1 becomes the current pad

  THStack *hs = new THStack("hs","");
  hs->Add(henergy_spectrum_data_bdf) ;
  henergy_spectrum_data_bdf->SetLineColor(kMagenta+2) ;
  hs->Add(henergy_spectrum_bdf) ;
  henergy_spectrum_bdf->SetLineColor(kOrange+7) ;
  hs->Draw("nostack");
  hs->GetYaxis()->SetTitle("#Counts") ;
  hs->GetYaxis()->SetTitleSize(0.048);

  // config_histo1D(henergy_spectrum_data_bdf,"","","#counts",2,1,kGreen+2);
  // henergy_spectrum_data_bdf->SetTitle("Total energy spectrum (w and w/o ^{60}Co source)");
  // config_histo1D(henergy_spectrum_bdf,"SAME","","#counts",2,1,kOrange+7);
  // henergy_spectrum_data_bdf->GetXaxis()->SetLabelSize(0);
  // henergy_spectrum_data_bdf->SetStats(0);



  // c11->BuildLegend(0.82,0.66,0.89,0.89) ;
  // c11->SaveAs("plots/ex_ampl_spect.pdf") ;


  auto legend = new TLegend(0.69,0.72,0.89,0.87);
  legend->AddEntry(henergy_spectrum_data_bdf,"With ^{60}Co source","l");
  legend->AddEntry(henergy_spectrum_bdf,"Background run","l");
  legend->Draw();

  // lower plot will be in pad2
  c->cd();          // Go back to the main canvas before defining pad2
  TPad *pad2 = new TPad("pad2", "pad2", 0, 0.005, 1, 0.303);
  pad2->SetBottomMargin(0.4);
  pad2->SetGridx(); // vertical grid
  pad2->Draw();
  pad2->cd();       // pad2 becomes the current pad

  // // Do not draw the Y axis label on the upper plot and redraw a small
  // // axis instead, in order to avoid the first label (0) to be clipped.
  // henergy_spectrum_data_bdf->GetYaxis()->SetLabelSize(0.);
  // TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
  // axis->SetLabelFont(52); // Absolute font size in pixel (precision 3)
  // axis->SetLabelSize(52);
  // axis->Draw();

  // Define the ratio plot
  TH1F *hRatio = (TH1F*)henergy_spectrum_data_bdf->Clone("hRatio");
  hRatio->Sumw2();
  hRatio->SetStats(0);      // No statistics on lower plot
  hRatio->Divide(henergy_spectrum_bdf);
  TLine *line = new TLine(0,1,4,1);
  config_histo1D(hRatio,"E","Total energy (MeV)","S/B",2,1,1);
  hRatio->SetMarkerStyle(8) ;
  hRatio->SetMarkerSize(0.5) ;
  hRatio->SetMarkerColor(MultiPlotColors(0)) ;

  hRatio->GetXaxis()->SetTitleSize(0.12);
  hRatio->GetYaxis()->SetTitleSize(0.12);
  hRatio->GetYaxis()->SetTitleOffset(0.4);
  hRatio->GetXaxis()->SetLabelSize(0.09);
  hRatio->GetYaxis()->SetLabelSize(0.09);
  hRatio->SetStats(0);
  hRatio->SetTitle("");
line->Draw("SAME");

  c->SaveAs("bdf.pdf");
}
