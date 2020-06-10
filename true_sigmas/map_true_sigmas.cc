// map_true_sigmas.C
// Author: Cloé Girard-Carillo <girardcarillo@lal.in2p3.fr>

// This macro aims to create a map of true sigmas from Cobalt analysis
// root 'map_true_sigmas.cc("true_sigmas.txt")' -b -q

#include <iostream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <TROOT.h>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TMathText.h>

#include "/home/girardcarillo/Workdir/SNPlot/RootDisplay.h"

using namespace std;

//----------------------------//

const int row_tot_number=13 ;
const int column_tot_number=20 ;

void map_true_sigmas (string filename) {


  TTree *tree = new TTree("ntuple","sigmas") ;

  tree->ReadFile(filename.c_str(),"col/I:row/I:sigma/F") ;

  Int_t col,row ;
  Float_t sigma ;
  tree->SetBranchAddress("col",&col) ;
  tree->SetBranchAddress("row",&row) ;
  tree->SetBranchAddress("sigma",&sigma) ;

  double sigmas[column_tot_number][row_tot_number] ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {
      sigmas[i][j] = 0 ;
    }
  }

  TH2D *h2counts = new TH2D ("counts","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2sigma = new TH2D ("sigmas","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH1F *hsigma = new TH1F ("sigmas","", 25, 0.3, 1) ;


  for (int i=0; i<tree->GetEntries(); i++){
    tree->GetEntry(i);
    h2counts->SetBinContent(col+2,row+2,h2counts->GetBinContent(col+2,row+2)+1) ;
    h2sigma->SetBinContent(col+2,row+2,h2sigma->GetBinContent(col+2,row+2)+sigma) ;
  }

  for (int i = 0; i < column_tot_number; ++i) {
    for (int j = 0; j < row_tot_number; ++j) {
      if (h2sigma->GetBinContent(i+2,j+2) != 0) {
        h2sigma->SetBinContent(i+2,j+2,h2sigma->GetBinContent(i+2,j+2)/h2counts->GetBinContent(i+2,j+2)) ;
        hsigma->Fill(h2sigma->GetBinContent(i+2,j+2)) ;
      }
    }
  }


  TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;
  gStyle->SetPaintTextFormat("1.3f") ;
  gStyle->SetPalette(20,python_viridis) ;

  config_histo2D(h2counts, "", "Column","Row","COLZTEXT") ; c->SaveAs("plots/map_counts.pdf") ;
  config_histo2D(h2sigma, "", "Column","Row","COLZTEXT") ; c->SaveAs("plots/map_sigmas.pdf") ;
  config_histo1D(hsigma,"","Sigma (ns)","",2,1,1) ; c->SaveAs("plots/sigmas.pdf") ;

  gStyle->SetOptStat(1) ;
}//end macro
