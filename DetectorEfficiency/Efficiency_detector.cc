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

void Efficiency_detector(const char* file_data,const char* file_simus){

  TFile *theDataFile = new TFile(file_data,"READ");
  TH1F *henergy_data = new TH1F ("energy_data","", 20,0.7,4);
  henergy_data = (TH1F*)theDataFile->Get("Total_energy_spectrum");

  TFile *theSimusFile = new TFile(file_simus,"READ");
  TH1F *henergy_simus = new TH1F ("energy_simus","", 20,0.7,4);
  henergy_simus = (TH1F*)theSimusFile->Get("Total_energy_spectrum");
  henergy_simus->Scale(0.375) ;

  // Define the Canvas
  TCanvas *c = new TCanvas("c", "canvas",10,10,2000,1000);
  gStyle->SetOptStat(0);
  // Upper plot will be in pad1
  TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
  pad1->SetTopMargin(0.0172117);
  pad1->SetBottomMargin(0.008892714);
  pad1->SetGridx();         // Vertical grid
  pad1->Draw();             // Draw the upper pad: pad1
  pad1->cd();               // pad1 becomes the current pad

  THStack *hs = new THStack("hs","");
  hs->Add(henergy_data) ;
  henergy_data->SetLineColor(kMagenta+2) ;
  hs->Add(henergy_simus,"HIST") ;
  henergy_simus->SetLineColor(kOrange+7) ;
  hs->Draw("nostack");
  hs->GetYaxis()->SetTitle("#Counts") ;
  hs->GetXaxis()->SetLabelSize(0);
  hs->GetYaxis()->SetTitleSize(0.06);
  hs->GetYaxis()->SetTitleOffset(0.7);
  hs->GetYaxis()->SetLabelSize(0.05);
  hs->GetXaxis()->SetRangeUser(0.71,3) ;
  pad1->SetLogy();

  auto legend = new TLegend(0.64,0.71,0.89,0.97);
  legend->AddEntry(henergy_data,"Data","l");
  legend->AddEntry(henergy_simus,"Simulations","l");
  legend->Draw();

  // lower plot will be in pad2
  c->cd();          // Go back to the main canvas before defining pad2
  TPad *pad2 = new TPad("pad2", "pad2", 0, 0.005, 1, 0.303);
  pad2->SetTopMargin(0.02287);
  pad2->SetBottomMargin(0.3235088);
  pad2->SetGridx(); // vertical grid
  pad2->Draw();
  pad2->cd();       // pad2 becomes the current pad
  pad2->SetLogy();

  // Define the ratio plot
  TH1F *hRatio = (TH1F*)henergy_data->Clone("hRatio");
  // hRatio->Sumw2();
  hRatio->SetStats(0);      // No statistics on lower plot
  hRatio->Divide(henergy_simus);
  TLine *line = new TLine(0.71,1,3,1);
  config_histo1D(hRatio,"HIST","Energy (MeV)","Data/Simu",2,1,1);
  hRatio->SetMarkerStyle(8) ;
  hRatio->SetMarkerSize(0.5) ;
  hRatio->SetMarkerColor(MultiPlotColors(0)) ;
  hRatio->GetXaxis()->SetRangeUser(0.71,3) ;

  hRatio->GetXaxis()->SetTitleSize(0.15);
  hRatio->GetYaxis()->SetTitleSize(0.15);
  hRatio->GetYaxis()->SetTitleOffset(0.3);
  hRatio->GetXaxis()->SetLabelSize(0.13);
  hRatio->GetYaxis()->SetLabelSize(0.13);
  hRatio->SetStats(0);
  hRatio->SetTitle("");
  line->Draw("SAME");

  c->SaveAs("simus.pdf");
}
