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

  TFile *theDataFile_simus = new TFile("root_files/counts/simus/counts_data.root","READ") ;
  TH2F *h2data_simus = new TH2F ("h2data_simus","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  h2data_simus = (TH2F*)theDataFile_simus->Get("counts") ;

  TProfile *hratio = new TProfile ("hratio","", 15, 0, 10, 0, 30) ;
  TProfile *hdata = new TProfile ("hdata","", 20, 0, 20, 0, 1e6) ;
  TProfile *hdata_simus = new TProfile ("hdata_simus","", 20, 0, 20, 0, 1e6) ;
  TProfile *hbdf = new TProfile ("hbdf","", 20, 0, 20, 0, 1e6) ;
  TProfile *hbdf_simus = new TProfile ("hbdf_simus","", 20, 0, 20, 0, 1e6) ;

  double bdf = 0. ;
  double bdf_simus = 0. ;
  double data = 0. ;
  double data_simus = 0. ;
  double number_events_bdf = 0 ;
  double number_events_bdf_simus = 0 ;
  double number_events_data = 0 ;
  double number_events_data_simus = 0 ;

  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {
      if (distance_OM(i,j,source_column,source_row) < 10) {

        number_events_bdf += h2bdf->GetBinContent(i+2,j+2) ;
        number_events_bdf_simus += h2bdf_simus->GetBinContent(i+2,j+2) ;
        number_events_data += h2data->GetBinContent(i+2,j+2) ;
        number_events_data_simus += h2data_simus->GetBinContent(i+2,j+2) ;
        bdf = h2bdf->GetBinContent(i+2,j+2) ;
        bdf_simus = h2bdf_simus->GetBinContent(i+2,j+2) ;
        data = h2data->GetBinContent(i+2,j+2) ;
        data_simus = h2data_simus->GetBinContent(i+2,j+2) ;


        hbdf->Fill(distance_OM(19-i,j,source_column,source_row),bdf) ;
        hbdf_simus->Fill(distance_OM(19-i,j,source_column,source_row),bdf_simus) ;
        hdata->Fill(distance_OM(i,j,source_column,source_row),data) ;
        hdata_simus->Fill(distance_OM(i,j,source_column,source_row),data_simus) ;


        hratio->Fill(distance_OM(i,j,source_column,source_row),h2data->GetBinContent(i+2,j+2)/h2bdf->GetBinContent(i+2,j+2)) ;
      }
      if (distance_OM(i,j,source_column,source_row) > 10) {
        hratio->Fill(distance_OM(19-i,j,source_column,source_row),h2data->GetBinContent(i+2,j+2)/h2bdf->GetBinContent(i+2,j+2)) ;
      }
    }
  }

  cout << "far data = " <<  number_events_bdf << " close data = " <<  number_events_data << " far simus = " << number_events_bdf_simus << " close simus = " << number_events_data_simus << endl ;

  gStyle->SetOptStat(0) ;

  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;

  config_profile(hdata,"","d^{ source} (OM units)","#Counts","",1,MultiPlotColors(2)) ;
  hdata->GetYaxis()->SetTitleOffset(1) ;
  config_profile(hbdf,"","d^{ source} (OM units)","#Counts","SAME",1,MultiPlotColors(2)) ;
  hdata->SetMarkerSize(1.5) ;
  hdata->SetMarkerStyle(20) ;
  hbdf->SetMarkerSize(1.5) ;
  hbdf->SetMarkerStyle(20) ;
  hdata->SetMinimum(0.1) ;
  hdata->SetMaximum(1e4) ;
  c->SetLogy() ;

  TLine* line = new TLine(10,0,10,1e4) ;
  line->SetLineStyle(2) ;
  line->Draw("SAME") ;

  TLegend* l = new TLegend(0.123,0.163,0.485,0.341) ;
  l->SetBorderSize(0) ;
  l->SetTextSize(0.04);


  config_profile(hbdf_simus,"","d^{ source} (OM units)","#Counts","SAME",1,MultiPlotColors(0)) ;
  hbdf_simus->SetMarkerStyle(20) ;
  hbdf_simus->SetMarkerSize(1.5) ;

  config_profile(hdata_simus,"","d^{ source} (OM units)","#Counts","SAME",1,MultiPlotColors(0)) ;
  hdata_simus->SetMarkerStyle(20) ;
  hdata_simus->SetMarkerSize(1.5) ;

  TPaveText *pt = new TPaveText(0.6334412,11000.58,9.655592,38113.11,"br");

  pt->SetBorderSize(0);
  pt->SetTextSize(0.04);
  pt->SetLineColor(1);
  pt->SetLineStyle(1);
  pt->SetLineWidth(1);
  pt->SetFillColor(0);
  pt->SetFillStyle(1001);

  pt->AddText("Events close to Cobalt source");
  pt->Draw("same") ;
  pt = new TPaveText(10.3174,11000.58,19.33955,38113.11,"br");

  pt->SetBorderSize(0);
  pt->SetTextSize(0.04);
  pt->SetLineColor(1);
  pt->SetLineStyle(1);
  pt->SetLineWidth(1);
  pt->SetFillColor(0);
  pt->SetFillStyle(1001);

  pt->AddText("Events far from Cobalt source");
  pt->Draw("same") ;

  l->AddEntry(hdata_simus,"Simulations","lep");
  l->AddEntry(hdata,"Real data","lep");
  l->SetTextSize(0.04);


  l->Draw() ;


  c->SaveAs("plots_data/merge_data_bdf.eps") ;

  TCanvas *c1 = new TCanvas("c1","c1",10,10,2000,1000) ;
  config_profile(hratio,"","d^{ source} (OM units)","S/B","hist",1,MultiPlotColors(1)) ;
  hratio->SetFillColorAlpha(MultiPlotColors(1),0.1) ;
  c1->SaveAs("plots_data/ratio_distance.pdf") ;
}


double distance_OM(int col1,int row1,int col2,int row2){

  return sqrt(pow((col2-col1),2)+pow((row2-row1),2)) ;

}
