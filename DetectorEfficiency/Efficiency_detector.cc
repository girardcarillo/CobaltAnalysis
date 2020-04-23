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

void Efficiency_detector(const char* file_data_Emax,const char* file_data_Emin,const char* file_simus_Emax,const char* file_simus_Emin){

  TFile *theDataFileEmax = new TFile(file_data_Emax,"READ");
  TH1F *henergy_data_Emax = (TH1F*)theDataFileEmax->Get("Energy_spectrum_Emax");

  TFile *theDataFileEmin = new TFile(file_data_Emin,"READ");
  TH1F *henergy_data_Emin = (TH1F*)theDataFileEmin->Get("Energy_spectrum_Emin");

  TFile *theSimusFileEmax = new TFile(file_simus_Emax,"READ");
  TH1F *henergy_simus_Emax = (TH1F*)theSimusFileEmax->Get("Energy_spectrum_Emax");

  TFile *theSimusFileEmin = new TFile(file_simus_Emin,"READ");
  TH1F *henergy_simus_Emin = (TH1F*)theSimusFileEmin->Get("Energy_spectrum_Emin");

  henergy_simus_Emax->Scale(0.375) ; // 3.75 pour 1e8 simus ; 0.375 pour 1e9 simus
  henergy_simus_Emin->Scale(0.375) ;

  // avec scale efficiency:
  // henergy_data_Emax->Scale(henergy_simus_Emax->GetEntries()*3.75/henergy_data_Emax->GetEntries()) ;
  // henergy_data_Emin->Scale(henergy_simus_Emax->GetEntries()*3.75/henergy_data_Emax->GetEntries()) ;
  // RatioCanvas(henergy_data_Emax, henergy_simus_Emax, "Data/efficiency", "Simulations", "Energy (MeV)", "Data/Simus", "plots/Eff_Emax.pdf") ;
  // RatioCanvas(henergy_data_Emin, henergy_simus_Emin, "Data/efficiency", "Simulations", "Energy (MeV)", "Data/Simus", "plots/Eff_Emin.pdf") ;


  // // sans scale efficiency
  henergy_data_Emin->SetMinimum(0.1);
  RatioCanvas(henergy_data_Emax, henergy_simus_Emax, "Data", "Simulations", "Energy (MeV)", "Data/Simus", "plots/Eff_Emax.pdf") ;
  RatioCanvas(henergy_data_Emin, henergy_simus_Emin, "Data", "Simulations", "Energy (MeV)", "Data/Simus", "plots/Eff_Emin.pdf") ;


}
