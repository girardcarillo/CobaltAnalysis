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

double distance_OM(int col1,int row1,int col2,int row2) ;

void merge(string side,const char* file_data,const char* file_bdf){

  int selected_column = 0 ;
  int selected_row = 6 ;

  TFile *theDataFile = new TFile(file_data,"READ") ;
  TH2F *h2data = new TH2F ("h2data","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2data = (TH2F*)theDataFile->Get("counts") ;

  TFile *theBdfFile = new TFile(file_bdf,"READ") ;
  TH2F *h2bdf = new TH2F ("h2bdf","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2bdf = (TH2F*)theBdfFile->Get("counts") ;

  TProfile *hdata = new TProfile ("hdata","", 50, 0, 20, 0, 1e6) ;
  TProfile *hbdf = new TProfile ("hbdf","", 50, 0, 20, 0, 1e6) ;

  double bdf = 0. ;
  double data = 0. ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {

      bdf = h2bdf->GetBinContent(i+2,j+2) ;
      data = h2data->GetBinContent(i+2,j+2) ;
      if (distance_OM(i,j,selected_column,selected_row) < 10) {
        hbdf->Fill(distance_OM(19-i,j,selected_column,selected_row),bdf) ;
        hdata->Fill(distance_OM(i,j,selected_column,selected_row),data) ;
      }

    }
  }

  gStyle->SetOptStat(0) ;

  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;

  config_profile(hdata,"","Distance from source (a.u.)","#Counts","",2,MultiPlotColors(10)) ;
  hdata->GetYaxis()->SetTitleOffset(1) ;
  config_profile(hbdf,"","Distance from source (a.u.)","#Counts","SAME",2,MultiPlotColors(9)) ;
  hdata->SetMarkerStyle(8) ;
  hbdf->SetMarkerStyle(8) ;
  hdata->SetMinimum(0.1) ;
  c->SetLogy() ;
  TLine* line = new TLine(10,0,10,1.2e4) ;
  line->SetLineStyle(2) ;
  line->Draw("SAME") ;

  TLegend* l = new TLegend(0.51,0.83,0.99,0.97) ;
  l->AddEntry(hdata,"Source side","lep");
  l->AddEntry(hbdf,"Opposite source side","lep");
  l->Draw() ;


  c->SaveAs("plots_data/merge_data_bdf.eps") ;


}


double distance_OM(int col1,int row1,int col2,int row2){

  return sqrt(pow((col2-col1),2)+pow((row2-row1),2)) ;

}
