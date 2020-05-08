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

void MeanComparison(const char* file_data,const char* file_simus){

  TFile *theDataFile = new TFile(file_data,"READ") ;
  TH2F *h2data = new TH2F ("h2data","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2data = (TH2F*)theDataFile->Get("h2tmp") ;

  TFile *theSimusFile = new TFile(file_simus,"READ") ;
  TH2F *h2simus = new TH2F ("h2simus","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2simus = (TH2F*)theSimusFile->Get("h2tmp") ;

  TH1F *hcomparison = new TH1F("comparison1d","",20, -20, 20) ;
  TH1F *hdata = new TH1F("hdata","",20, -20, 20) ;
  TH1F *hsimus = new TH1F("hsimus","",50, -10, 5) ;

  double simus = 0. ;
  double data = 0. ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {
      simus = h2simus->GetBinContent(i+2,j+2) ;
      data = h2data->GetBinContent(i+2,j+2) ;
      if (simus != 0) {
        hsimus->Fill(simus) ;
        if (data != 0) {
          hdata->Fill(data) ;
          hcomparison->Fill(data-simus) ;
        }
      }

    }
  }

  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;
  c->Divide(2,2) ;
  gStyle->SetPaintTextFormat("1.2f") ;
  c->cd(1);
  hcomparison->Draw() ;
  c->cd(2) ;
  hdata->Draw() ;
  c->cd(3) ;
  hsimus->Draw() ;
  c->cd(4) ;
  h2simus->Draw("COLZTEXT") ;

}
