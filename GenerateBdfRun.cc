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

const int row_tot_number=13 ;
const int column_tot_number=20 ;

void GenerateBdf(string type, string run, string filename, double energy_cut_min=0,double energy_cut_max=0, bool enable_drawing = 0, bool enable_root_files = 0) ;
bool CutRun (string type,string run, int row0, int column0, int row1, int column1) ;
bool select_PM(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row,int *hit) ;
bool select_cross(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row) ;
bool select_far(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row,int hit) ;
bool select_damaged(vector<int> *col,vector<int> *row) ;

void GenerateBdfRun(string type, string run, string filename, double energy_cut_min=0,double energy_cut_max=0, bool enable_drawing = 0, bool enable_root_files = 0){

  GenerateBdf(type, run, filename, energy_cut_min, energy_cut_max, enable_drawing, enable_root_files) ;

}



void GenerateBdf(string type, string run, string filename, double energy_cut_min=0,double energy_cut_max=0, bool enable_drawing = 0, bool enable_root_files = 0){

  int selected_column = -1 ;
  int selected_row = -1 ;

  if (run == "186") {
    if (type == "data") {
      selected_column = 0 ;
      selected_row = 6 ;
    }
    else if (type == "bkg") {
      selected_column = 19 ;
      selected_row = 6 ;
    }
  }

  else if (run == "182") {
    if (type == "data") {
      selected_column = 19 ;
      selected_row = 6 ;
    }
    else if (type == "bkg") {
      selected_column = 0 ;
      selected_row = 6 ;
    }
  }

  else if (run == "it_gauche") {
    if (type == "data") {
      selected_column = 0 ;
      selected_row = 6 ;
    }
    else if (type == "bkg") {
      selected_column = 19 ;
      selected_row = 6 ;
    }
  }

  else {
    cout << "From function GenerateBdf: bad run id" << endl ;
  }

  //data file
  TFile *DataFile = new TFile(filename.c_str(),"READ") ;
  TTree *theTree = nullptr ;
  DataFile->GetObject("T",theTree) ;

  if (DataFile->IsOpen()) {
    cout << "File " << filename << " opened sucessfully" << endl ;
  }

  theTree = (TTree*)DataFile->Get("DataCut") ;

  theTree->SetBranchAddress("calo_row",&calo_row) ;
  theTree->SetBranchAddress("calo_column",&calo_column) ;
  theTree->SetBranchAddress("calo_time",&calo_time) ;
  theTree->SetBranchAddress("calo_energy",&calo_energy) ;

  cout << theTree->GetEntries() << " entries"<< endl ;

  TH2D *h2counts = new TH2D ("counts","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2coincidence = new TH2D ("coincidence","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2energy = new TH2D ("energy","", 25, 0.6, 2.5, 25, 0.6, 2.5) ;
  TH1F *henergy_spectrum_near = new TH1F("energy spectrum near","",50, 0, 4) ;
  TH1F *henergy_spectrum_far = new TH1F("energy spectrum far","",50, 0, 4) ;
  TH1F *henergy_spectrum = new TH1F("Total_energy_spectrum","",100, 0, 4) ;
  TH1F *henergy_spectrum_Emin = new TH1F("Energy_spectrum_Emin","",100, 0, 4) ;
  TH1F *henergy_spectrum_Emax = new TH1F("Energy_spectrum_Emax","",100, 0, 4) ;


  double counts[column_tot_number][row_tot_number] ;
  double coincidence[column_tot_number][row_tot_number] ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j < row_tot_number ; ++j) {
      counts[i][j]=0 ;
      coincidence[i][j]=0 ;
    }
  }

  int counter_event = 0 ;
  int counter_loop = 0 ;
  int counter_far = 0 ;
  int counter_near = 0 ;

  for (Long64_t i=0 ;i<theTree->GetEntries() ;i++) {
    counter_loop++ ;
    theTree->GetEntry(i) ;
    if (i%100000==0) cout << "event " << i << endl ;

    if (isnan(calo_energy->at(0)) || isnan(calo_energy->at(1))){
      continue ;
    }

    else {

      if (CutRun(type,run,calo_row->at(0),calo_column->at(0),calo_row->at(1),calo_column->at(1))) {

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

        bool flag_energy_inf = 1 ;
        if (Emin < energy_cut_min || Emax < energy_cut_max) {
          flag_energy_inf=0 ;
        }

        bool flag_damaged = select_damaged(calo_column,calo_row) ;


        // Sélectionner une croix autour de l'OM de référence [selected_column:selected_row]

        if (flag_damaged) {
          if (flag_energy_inf) {

            //tableau coïncidences


            for (int j = 0 ; j < calo_row->size() ; ++j) {
              //counting number of events for each PM
              counts[calo_column->at(j)][calo_row->at(j)]++ ;
            }


            int hit=-1 ;
            bool selected_PMT = select_PM(calo_column,calo_row,selected_column,selected_row,&hit) ;
            if (selected_PMT) {
              //counting number of events in coincidence with reference OM, for each OM

              bool flag_square = select_cross(calo_column,calo_row,selected_column,selected_row) ;
              bool flag_far = select_far(calo_column,calo_row,selected_column,selected_row,abs(hit-1)) ;

              //if (!flag_square) {
                coincidence[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))]++ ;

                counter_event++ ;
                henergy_spectrum_Emin->Fill(Emin) ;
                henergy_spectrum_Emax->Fill(Emax) ;
                h2energy->Fill(Emin,Emax) ;

                henergy_spectrum->Fill(calo_energy->at(hit)) ;
                henergy_spectrum->Fill(calo_energy->at(abs(hit-1))) ;


                // // test 04/05/20 : comparer les 4 OMs les plus proches de l'OM de ref avec les autres
                // // plus loin
                // if (flag_square) {
                //   counter_near++;
                //   henergy_spectrum_near->Fill(calo_energy->at(hit)) ;
                //   henergy_spectrum_near->Fill(calo_energy->at(abs(hit-1))) ;
                // }
                // if (flag_far) {
                //   counter_far++;
                //   henergy_spectrum_far->Fill(calo_energy->at(hit)) ;
                //   henergy_spectrum_far->Fill(calo_energy->at(abs(hit-1))) ;
                // }
                //
              }

            }

          //}

          // if (i > 1e2) {
          //   cout << "\033[1;31mwarning break at \033[0m" << i << endl ;
          //   break ;
          // }

        }
      }
    }
  }

  cout << counter_event << " selected on " << counter_loop << endl ;
  cout << "counter far = " << counter_far << " counter_near = " << counter_near << endl ;

  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j< row_tot_number ; ++j) {
      if (run == "it_droite") {
        // to construct simus_it_droite: it_gauche->reverse->it_droite
        h2counts->SetBinContent(19-i+2,j+2,counts[i][j]) ;
        h2coincidence->SetBinContent(19-i+2,j+2,coincidence[i][j]) ;
      }
      else {
        h2counts->SetBinContent(i+2,j+2,counts[i][j]) ;
        h2coincidence->SetBinContent(i+2,j+2,coincidence[i][j]) ;
      }

    }
  }

  // // norm simus
  // henergy_spectrum_far->Scale(0.35) ;
  // henergy_spectrum_near->Scale(0.35) ;
  // TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;
  // config_histo1D(henergy_spectrum_far,"HIST","Energy (MeV)","",2,1,2) ;
  // config_histo1D(henergy_spectrum_near,"HISTSAME","Energy (MeV)","",2,1,1) ;
  // c->BuildLegend();


  // ///Drawing


  string title ;

  if (enable_drawing) {

    TCanvas *c = new TCanvas("c","c",10,10,2000,1000) ;
    config_histo2D(h2counts, "Number of events in each PMT", "Column","Row","COLZTEXT") ;
    title = "plots_"+type+"/counts_run"+run+".pdf" ;
    c->SaveAs(title.c_str()) ;

    TCanvas *c0 = new TCanvas("c0","c0",10,10,2000,1000) ;
    config_histo2D(h2coincidence, "Number of events in each PMT", "Column","Row","COLZTEXT") ;
    title = "plots_"+type+"/coincidence_run"+run+".pdf" ;
    c0->SaveAs(title.c_str()) ;

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
    title = "plots_"+type+"/energy_spectrum_emin_emax_run"+run+".pdf" ;
    c1->SaveAs(title.c_str()) ;

    TCanvas *c2 = new TCanvas("c2","c2",10,10,2000,1000) ;
    title = "plots_"+type+"/Emin_Emax_run"+run+".pdf" ;
    config_histo2D(h2energy, "E_{max} vs E_{min}", "E_{min}","E_{max}","COLZ") ;
    c2->SaveAs(title.c_str()) ;


  }

  if (enable_root_files) {

    config_histo1D(henergy_spectrum,"","Energy (MeV)","#counts",2,1,1) ;
    title  = "DetectorEfficiency/histograms/"+type+"/energy_spectrum_run"+run+".root" ;
    henergy_spectrum->SaveAs(title.c_str()) ;

    config_histo1D(henergy_spectrum_Emin,"","Energy (MeV)","#counts",2,1,1) ;
    title  = "DetectorEfficiency/histograms/"+type+"/energy_spectrum_Emin_run"+run+".root" ;
    henergy_spectrum_Emin->SaveAs(title.c_str()) ;

    config_histo1D(henergy_spectrum_Emax,"","Energy (MeV)","#counts",2,1,1) ;
    title  = "DetectorEfficiency/histograms/"+type+"/energy_spectrum_Emax_run"+run+".root" ;
    henergy_spectrum_Emax->SaveAs(title.c_str()) ;

    title = "coincidence_"+type+"_run"+run+".root" ;
    h2coincidence->SaveAs(title.c_str()) ;

  }

  theTree->ResetBranchAddresses() ;

}


bool CutRun (string type, string run, int row0, int column0, int row1, int column1) {

  bool cut = 0 ;
  if (type == "bkg") {
    if (run == "185" || run == "it_gauche_haut") {
      cut = (row0 <= column0-3 && row1 <= column1-3) ;
    }
    else if (run == "184" || run == "it_droite_bas") {
      cut = (row0 >= column0-2 && row1 >= column1-2) ;
    }
    else if (run == "186" || run == "it_gauche") {
      cut = (column0 > 9 && column1 > 9) ;
    }
    else if (run == "182") {
      cut = (column0 <= 9 && column1 <= 9) ;
    }
    else if (run == "it_droite") {
      cut = (column0 > 9 && column1 > 9) ;
    }
    else {
      cout << "From function CutRun: no cut config!!" << endl ;
    }
  }

  else if (type == "data") {
    if (run == "185" || run == "it_gauche_haut") {
      cut = (row0 >= column0-2 && row1 >= column1-2) ;
    }
    else if (run == "184" || run == "it_droite_bas") {
      cut = (row0 <= column0-3 && row1 <= column1-3) ;
    }
    else if (run == "186" || run == "it_gauche") {
      cut = (column0 <= 9 && column1 <= 9) ;
    }
    else if (run == "182") {
      cut = (column0 >= 9 && column1 >= 9) ;
    }
    else if( run == "it_droite") {
      cut = (column0 < 9 && column1 < 9) ;
    }
    else {
      cout << "From function CutRun: no cut config!!" << endl ;
    }
  }

  else {
    cout << "From function CutRun: no type config!!" << endl ;
  }

  return cut ;

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

bool select_cross(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row){
  bool flag=0 ;

  if (calo_column->at(0) == selected_column) {
    if (calo_row->at(0) == selected_row+1) {
      flag=1 ;
    }
  }
  if (calo_column->at(1) == selected_column) {
    if (calo_row->at(1) == selected_row+1) {
      flag=1 ;
    }
  }
  if (calo_column->at(0) == selected_column+1) {
    if (calo_row->at(0) == selected_row) {
      flag=1 ;
    }
  }
  if (calo_column->at(1) == selected_column+1) {
    if (calo_row->at(1) == selected_row) {
      flag=1 ;
    }
  }
  if (calo_column->at(0) == selected_column) {
    if (calo_row->at(0) == selected_row-1) {
      flag=1 ;
    }
  }
  if (calo_column->at(1) == selected_column) {
    if (calo_row->at(1) == selected_row-1) {
      flag=1 ;
    }
  }
  if (calo_column->at(0) == selected_column-1) {
    if (calo_row->at(0) == selected_row) {
      flag=1 ;
    }
  }
  if (calo_column->at(1) == selected_column-1) {
    if (calo_row->at(1) == selected_row) {
      flag=1 ;
    }
  }


  return flag ;

}

bool select_far(vector<int> *vect_column,vector<int> *vect_row,int selected_column,int selected_row, int hit){
  bool flag=0 ;

  if (calo_column->at(hit) > selected_column+5) {
    flag=1 ;
  }
  if (calo_column->at(hit) < selected_column-5) {
    flag=1 ;
  }
  if (calo_row->at(hit) > selected_row+5) {
    flag=1 ;
  }
  if (calo_row->at(hit) < selected_row-5) {
    flag=1 ;
  }

  return flag ;

}


bool select_damaged(vector<int> *col,vector<int> *row){

  bool flag = 1 ;

  for (int i = 0; i < col->size(); ++i) {
    if (col->at(i) == 2 && row->at(i) == 2) {
      flag = 0 ;
    }
    if (col->at(i) == 2 && row->at(i) == 7) {
      flag = 0 ;
    }
    if (col->at(i) == 14 && row->at(i) == 6) {
      flag = 0 ;
    }
  }

  return flag ;
}
