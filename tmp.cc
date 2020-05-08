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

const int row_tot_number=13 ;
const int column_tot_number=20 ;



void tmp(const char* file_data,const char* file_simus){

  TFile *theDataFile = new TFile(file_data,"READ");
  TH1F *htime_data = (TH1F*)theDataFile->Get("tmp");

  TFile *theSimusFile = new TFile(file_simus,"READ");
  TH1F *htime_simus = (TH1F*)theSimusFile->Get("tmp");

  double scale = 0.35 ;
  htime_simus->Scale(scale) ; // 7.2 pour 1e8 simus ; 0.72 pour 1e9 simus



  gStyle->SetOptStat(0) ;
  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;
  config_histo1D(htime_data,"HIST","","",2,1,MultiPlotColors(0)) ;

  TLegend* legend1 = new TLegend(0.556,0.641,0.894,0.877);
  legend1->SetBorderSize(0) ;

  TF1 *f1 = new TF1("f1","gaus",-10,2) ;
  htime_data->Fit("f1") ;
  f1->Draw("SAME") ;
  f1->SetLineColor(MultiPlotColors(1)) ;
  gStyle->SetOptFit(0) ;

  float chi2 = f1->GetChisquare() ;
  float ndf = f1->GetNDF() ;

  legend1->AddEntry(htime_data,"#Delta t distribution","l");
  legend1->AddEntry(/*gr_celerity->GetFunction("f1")*/f1,"Gaussian fit","l");
  legend1->AddEntry((TObject*)0,Form("#chi^{2}/ndf = %1.4f/%1.f",chi2,ndf),"");
  legend1->AddEntry((TObject*)0,Form("Mean = %1.3f #pm %1.3f",f1->GetParameter(1),f1->GetParError(1)),"");
  legend1->AddEntry((TObject*)0,Form("  #sigma     = %1.3f #pm %1.3f",f1->GetParameter(2),f1->GetParError(2)),"");

  legend1->Draw() ;

  c->SaveAs("tmp.eps") ;


}
