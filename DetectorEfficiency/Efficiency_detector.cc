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
  TH1F *henergy_data = (TH1F*)theDataFile->Get("Total_energy_spectrum");

  TFile *theSimusFile = new TFile(file_simus,"READ");
  TH1F *henergy_simus = (TH1F*)theSimusFile->Get("Total_energy_spectrum");

  TFile *theBdfFile = new TFile("histograms/bkg/energy_spectrum_bkg.root","READ");
  TH1F *henergy_bdf = (TH1F*)theBdfFile->Get("Total_energy_spectrum");

  double scale = 3.5 ;
  henergy_simus->Scale(scale) ; // 7.2 pour 1e8 simus ; 0.72 pour 1e9 simus
  // henergy_simus->Scale(0.72) ; // 7.2 pour 1e8 simus ; 0.72 pour 1e9 simus

  // sans scale efficiency
  // henergy_data->SetMinimum(0.1);
  // RatioCanvas(henergy_data, henergy_simus, "Data", "Simulations", "Energy (MeV)", "Data/Simus", "plots/Eff.pdf") ;

  gStyle->SetOptStat(0) ;
  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;
  config_histo1D(henergy_data,"HIST","Energy (MeV)","",2,1,MultiPlotColors(0)) ;
  config_histo1D(henergy_simus,"HISTSAME","Energy (MeV)","",2,1,MultiPlotColors(1)) ;
  config_histo1D(henergy_bdf,"HISTSAME","Energy (MeV)","",2,1,MultiPlotColors(3)) ;
  c->SetLogy() ;
  auto legend = new TLegend(0.49,0.77,0.99,0.97);
  legend->AddEntry(henergy_data,Form("Data+bdf: entries %1.f",henergy_data->GetEntries()),"l");
  legend->AddEntry(henergy_simus,Form("Cobalt simulations: entries %1.f",henergy_simus->GetEntries()*scale),"l");
  legend->AddEntry(henergy_bdf,Form("Bdf: entries %1.f",henergy_bdf->GetEntries()),"l");
  legend->Draw();
  c->SaveAs("plots/Comparison_simus_data_bdf.pdf") ;

  // // Compare simus GenerateBackgroundRun
  // RatioCanvas(henergy_data, henergy_simus, "Cobalt simulations", "Topological cut", "Energy (MeV)", "Simus/Cut", "plots/Eff.pdf") ;


}
