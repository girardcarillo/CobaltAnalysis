// Author Cloé Girard-Carillo girardcarillo@lal.in2p3.fr

#include <limits>
#include <string>
#include <iostream>
#include <fstream>

#include <TTree.h>
#include <TProfile.h>
#include <TFile.h>
#include <TObject.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>

#include "/home/girardcarillo/Workdir/SNPlot/RootDisplay.h"
#include "/home/girardcarillo/Workdir/SNPlot/EventTree.h"

using namespace std ;

typedef numeric_limits<double> dbl ;

const int row_tot_number=13 ;
const int column_tot_number=20 ;

void FitDeltat(TH1F* histo, double *fit_parameters) ;
bool select_PM(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row,int *hit) ;
double distance_OM(int col1,int row1,int col2,int row2) ;
double Median(vector<double> vect) ;

void GenerateBdfRun(int selected_column,int selected_row,double energy_cut_min=0,double energy_cut_max=0, bool enable_drawing = 0, bool enable_root_files = 0){

  string filename = "Cut_root_files/data/60Co_runs/Cut_2e_run184_Calib.root" ;

  //data file
  TFile *DataFile = new TFile(filename.c_str(),"READ") ;
  TTree *theTree = nullptr ;
  DataFile->GetObject("T",theTree) ;

  if (DataFile->IsOpen()) {
    cout << "File " << filename << " opened sucessfully" << endl ;
  }

  theTree = (TTree*)DataFile->Get("DataCut") ;

  theTree->SetBranchAddress("trigger_id",&trigger_id) ;
  theTree->SetBranchAddress("calo_row",&calo_row) ;
  theTree->SetBranchAddress("calo_column",&calo_column) ;
  theTree->SetBranchAddress("calo_id",&calo_id) ;
  theTree->SetBranchAddress("calo_module",&calo_module) ;
  theTree->SetBranchAddress("channel_raw_tdc", &channel_raw_tdc) ;
  theTree->SetBranchAddress("calo_time",&calo_time) ;
  theTree->SetBranchAddress("calo_energy",&calo_energy) ;
  theTree->SetBranchAddress("calo_peak",&calo_peak) ;
  theTree->SetBranchAddress("calo_number",&calo_number) ;
  theTree->SetBranchAddress("cut_multiplicity", &cut_multiplicity) ;

  cout << theTree->GetEntries() << " entries"<< endl ;

  TH2D *h2counts = new TH2D ("counts","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH1F *henergy_spectrum = new TH1F("Total_energy_spectrum","",100, 0, 4) ;
  TH1F *henergy_spectrum_Emin = new TH1F("Energy_spectrum_Emin","",100, 0, 4) ;
  TH1F *henergy_spectrum_Emax = new TH1F("Energy_spectrum_Emax","",100, 0, 4) ;


  double counts[column_tot_number][row_tot_number] ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {
      counts[i][j]=0 ;
    }
  }

  int counter_event = 0 ;
  int counter_loop = 0 ;


  for (Long64_t i=0 ;i<theTree->GetEntries() ;i++) {
    counter_loop++ ;
    theTree->GetEntry(i) ;
    if (i%100000==0) cout << "event " << i << endl ;

    if (isnan(calo_energy->at(0)) || isnan(calo_energy->at(1))){
      continue ;
    }

    else {

      if ( calo_row->at(0) >= calo_column->at(0)-3) {
        if (calo_row->at(1) >= calo_column->at(1)-3) {


          //Fill avec Emin/Emax
          double Emin = 0., Emax = 0. ;
          double t_Emin = 0., t_Emax = 0. ;

          if (calo_energy->at(0) < calo_energy->at(1)) {
            Emin = calo_energy->at(0) ; Emax = calo_energy->at(1) ;
            t_Emin = calo_time->at(0) ; t_Emax = calo_time->at(1) ;
          }
          else if (calo_energy->at(0)>calo_energy->at(1)) {
            Emax=calo_energy->at(0) ; Emin=calo_energy->at(1) ;
            t_Emax=calo_time->at(0) ; t_Emin=calo_time->at(1) ;
          }

          // test cut energy different pour Emin et Emax 18/12/19
          bool flag_energy = 1 ;
          for (int i_energy = 0 ; i_energy < calo_energy->size() ; ++i_energy) {
            if (Emin < energy_cut_min || Emax < energy_cut_max) {
              flag_energy=0 ;
            }
          }

          if (flag_energy) {

            //delta t à corriger des longueurs de cables coax (cf reflecto)!!
            counter_event++ ;

            henergy_spectrum_Emin->Fill(Emin) ;
            henergy_spectrum_Emax->Fill(Emax) ;

            for (int j = 0 ; j < calo_row->size() ; ++j) {
              //counting number of events for each PM
              counts[calo_column->at(j)][calo_row->at(j)]++ ;

              henergy_spectrum->Fill(calo_energy->at(j)) ;

            }


            //tableau coïncidences
            int hit=-1 ;
            bool selected_PMT = select_PM(calo_column,calo_row,selected_column,selected_row,&hit) ;
            if (selected_PMT) {



            }
          }
        }
      }
      // if (i > 1e6) {
      //   cout << "\033[1;31mwarning break at \033[0m" << i << endl ;
      //   break ;
      // }

    }
  }
  //  }

  cout << counter_event << " selected on " << counter_loop << endl ;


  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j< row_tot_number ; ++j) {

      h2counts->SetBinContent(i+2,j+2,counts[i][j]) ;


    }
  }

    // ///Drawing

  if (enable_drawing) {

    TCanvas *c0 = new TCanvas("c0","c0",10,10,2000,1000) ;
    config_histo2D(h2counts, "Number of events in each PMT", "Column","Row","COLZ") ;
    c0->SaveAs("plots_bkg/counts.pdf") ;


    TCanvas *c1 = new TCanvas("c1","c1",10,10,2000,1000) ;
    c1->SetLeftMargin(0.1788223);
    c1->SetRightMargin(0.02160994);
    c1->SetTopMargin(0.01807229);
    c1->SetBottomMargin(0.1817269);

    THStack *hs = new THStack("hs","");
    gStyle->SetOptStat(1110) ;
    gStyle->SetLegendBorderSize(0) ;

    hs->Add(henergy_spectrum_Emin,"HIST") ;
    henergy_spectrum_Emin->SetFillColorAlpha(46,0.5) ;
    henergy_spectrum_Emin->SetLineColor(46) ;
    hs->Add(henergy_spectrum_Emax,"HIST") ;
    henergy_spectrum_Emax->SetFillColorAlpha(30,0.5) ;
    henergy_spectrum_Emax->SetLineColor(30) ;
    hs->Add(henergy_spectrum,"HIST") ;
    henergy_spectrum->SetLineColor(1) ;
    hs->Draw("nostack");
    hs->GetXaxis()->SetTitle("Energy (MeV)") ;
    hs->GetYaxis()->SetTitle("#Counts") ;
    hs->GetYaxis()->SetTitle("#Counts") ;
    hs->GetXaxis()->SetLabelSize(0.06);
    hs->GetYaxis()->SetLabelSize(0.06);
    hs->GetYaxis()->SetTitleSize(0.06);
    hs->GetXaxis()->SetTitleSize(0.06);
    hs->GetXaxis()->SetTitleOffset(0.9);
    hs->GetYaxis()->SetTitleOffset(0.9);
    hs->GetXaxis()->SetRangeUser(0.71,4) ;

    // henergy_spectrum_Emin->GetYaxis()->SetRangeUser(0,henergy_spectrum->GetMaximum()+henergy_spectrum->GetMaximum()*0.1) ;
    SetPaveStat(henergy_spectrum_Emin) ;
    henergy_spectrum_Emin->SetMinimum(0.01) ;

    c1->SetLogy() ;
    c1->BuildLegend(0.617,0.775,0.977,0.98) ;
    c1->SaveAs("plots_bkg/energy_spectrum_emin_emax.pdf") ;


  }

  if (enable_root_files) {

    config_histo1D(henergy_spectrum,"","Energy (MeV)","#counts",2,1,1) ;
    henergy_spectrum->SaveAs("DetectorEfficiency/histograms/data/bkg/energy_spectrum.root") ;

    config_histo1D(henergy_spectrum_Emin,"","Energy (MeV)","#counts",2,1,1) ;
    henergy_spectrum_Emin->SaveAs("DetectorEfficiency/histograms/data/bkg/energy_spectrum_Emin.root") ;

    config_histo1D(henergy_spectrum_Emax,"","Energy (MeV)","#counts",2,1,1) ;
    henergy_spectrum_Emax->SaveAs("DetectorEfficiency/histograms/data/bkg/energy_spectrum_Emax.root") ;

  }



  theTree->ResetBranchAddresses() ;
}

bool select_PM(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row,int *hit){
  bool flag_test=0 ;
  for (int i = 0 ; i < vect_row->size() ; ++i) {
    if (vect_column->at(i)==selected_column) {
      if (vect_row->at(i)==selected_row) {
        *hit=i ;
        flag_test=1 ;
      }
    }
  }
  return flag_test ;
}
