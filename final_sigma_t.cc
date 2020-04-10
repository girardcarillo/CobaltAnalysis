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

const int row_tot_number=13;
const int column_tot_number=20;

void final_sigma_t(const char* file1,const char* file2){
  TFile *theDataFile = new TFile(file1,"READ");

  TH2D *h2sigmaData = new TH2D ("sigma data","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1);
  h2sigmaData = (TH2D*)theDataFile->Get("sigma");

  TFile *theSimusFile = new TFile(file2,"READ");
  TH2D *h2sigmaSimus = new TH2D ("sigma simus","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1);
  h2sigmaSimus = (TH2D*)theSimusFile->Get("sigma");

  TH2D *h2sigma = new TH2D ("sigma","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1);

  if (!(h2sigmaData)||!(h2sigmaSimus)) cout << "histogram does not exist!" << endl;

  double sigma=0;
  for (int i=0; i < h2sigmaData->GetNbinsX(); ++i) {
    for (int j=0; j < h2sigmaData->GetNbinsY(); ++j) {
      if (h2sigmaData->GetBinContent(i,j)!=0&&h2sigmaSimus->GetBinContent(i,j)!=0) {
        sigma=sqrt(pow(h2sigmaData->GetBinContent(i,j),2)-pow(h2sigmaSimus->GetBinContent(i,j),2));
      }
      else {
        sigma=0;
      }
      if (sigma) {
        h2sigma->SetBinContent(i,j,sigma);
      }
    }
  }

  TCanvas *c1 = new TCanvas("c1","c1",200,10,700,500);

  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat("1.2f");
  c1->Divide(2,2);
  c1->cd(0);
  h2sigmaData->Draw();
  c1->cd(1);
  h2sigmaSimus->Draw();
  c1->cd(2);
  config_histo2D(h2sigma, "sigma", "Column","Row","COLZTEXT");c1->SaveAs("plots_data/sigma_PMT.pdf");
  h2sigma->Draw("COLZTEXT");


}
