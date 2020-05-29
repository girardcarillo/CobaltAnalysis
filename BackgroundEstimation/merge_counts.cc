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

void merge_counts(const char* file_data,const char* file_bdf){

  int source_column = 0 ;
  int source_row = 6 ;

  TFile *theDataFile = new TFile(file_data,"READ") ;
  TH2F *h2data = new TH2F ("h2data","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2data = (TH2F*)theDataFile->Get("counts") ;

  TFile *theBdfFile = new TFile(file_bdf,"READ") ;
  TH2F *h2bdf = new TH2F ("h2bdf","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2bdf = (TH2F*)theBdfFile->Get("counts") ;

  TFile *theBdfFile_simus = new TFile("root_files/counts/simus/counts_bkg.root","READ") ;
  TH2F *h2bdf_simus = new TH2F ("h2bdf_simus","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2bdf_simus = (TH2F*)theBdfFile_simus->Get("counts") ;


  TProfile *hdata = new TProfile ("hdata","", 20, 0, 20, 0, 1e6) ;
  TProfile *hbdf = new TProfile ("hbdf","", 20, 0, 20, 0, 1e6) ;
  TProfile *hbdf_simus = new TProfile ("hbdf","", 20, 0, 20, 0, 1e6) ;

  double bdf = 0. ;
  double bdf_simus = 0. ;
  double data = 0. ;
  double number_events_bdf = 0 ;
  double number_events_bdf_simus = 0 ;
  double number_events_data = 0 ;

  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {
      if (distance_OM(i,j,source_column,source_row) < 10) {

        number_events_bdf += h2bdf->GetBinContent(i+2,j+2) ;
        number_events_bdf_simus += h2bdf_simus->GetBinContent(i+2,j+2) ;
        number_events_data += h2data->GetBinContent(i+2,j+2) ;
        bdf = h2bdf->GetBinContent(i+2,j+2) ;
        bdf_simus = h2bdf_simus->GetBinContent(i+2,j+2) ;
        data = h2data->GetBinContent(i+2,j+2) ;
        hbdf->Fill(distance_OM(19-i,j,source_column,source_row),bdf) ;
        hbdf_simus->Fill(distance_OM(19-i,j,source_column,source_row),bdf_simus) ;
        hdata->Fill(distance_OM(i,j,source_column,source_row),data) ;

      }
    }
  }

  cout << "bdf simus = " <<  number_events_bdf << " bdf data = " << number_events_bdf_simus << " data = " << number_events_data << endl ;

  gStyle->SetOptStat(0) ;

  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;

  config_profile(hdata,"","Distance from source (a.u.)","#Counts","",2,MultiPlotColors(10)) ;
  hdata->GetYaxis()->SetTitleOffset(1) ;
  config_profile(hbdf,"","Distance from source (a.u.)","#Counts","SAME",2,MultiPlotColors(9)) ;
  config_profile(hbdf_simus,"","Distance from source (a.u.)","#Counts","SAME",2,MultiPlotColors(0)) ;
  hbdf_simus->SetMarkerStyle(8) ;
  hdata->SetMarkerStyle(8) ;
  hbdf->SetMarkerStyle(8) ;
  hdata->SetMinimum(0.1) ;
  hdata->SetMaximum(1e4) ;
  c->SetLogy() ;
  TLine* line = new TLine(10,0,10,1.2e4) ;
  line->SetLineStyle(2) ;
  line->Draw("SAME") ;

  TLegend* l = new TLegend(0.501,0.768,0.897,0.895) ;
  l->SetBorderSize(0) ;
  l->AddEntry(hdata,"Real data: events close to Cobalt source","lep");
  l->AddEntry(hbdf,"Real data: events far from Cobalt source","lep");
  l->AddEntry(hbdf_simus,"Simulated data: events far from Cobalt source","lep");
  l->Draw() ;


  c->SaveAs("plots_data/merge_data_bdf.eps") ;


}


double distance_OM(int col1,int row1,int col2,int row2){

  return sqrt(pow((col2-col1),2)+pow((row2-row1),2)) ;

}
