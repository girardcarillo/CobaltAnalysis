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

void RootAnalyzer(string filename/*,string correctedTimesFilename*/,int selected_column,int selected_row,double energy_cut_min=0,double energy_cut_max=0, bool enable_drawing = 0, double smearing_energy = 0){
  cout.precision(dbl::max_digits10) ;
  // TGaxis::SetMaxDigits(2) ;

  // HV applied during run (official text file converted in a root readable version with 'ReadFile.py')
  TTree *tree_HV = new TTree("ntuple_HV","corrected times from reflecto") ;

  tree_HV->ReadFile("HV.txt","wall_HV/I:col_HV/I:row_HV/I:HV/F") ;

  Int_t wall_HV,col_HV,row_HV ;
  Float_t HV ;
  tree_HV->SetBranchAddress("wall_HV",&wall_HV) ;
  tree_HV->SetBranchAddress("col_HV",&col_HV) ;
  tree_HV->SetBranchAddress("row_HV",&row_HV) ;
  tree_HV->SetBranchAddress("HV",&HV) ;

  // // corrected times from reflecto files
  // TTree *tree_correctedTimes = new TTree("ntuple_correctedTimes","corrected times from reflecto") ;

  // tree_correctedTimes->ReadFile(correctedTimesFilename.c_str(),"wall/I:col/I:row/I:CT/F") ;

  // Int_t wall,col,row ;
  // Float_t CT ;
  // tree_correctedTimes->SetBranchAddress("wall",&wall) ;
  // tree_correctedTimes->SetBranchAddress("col",&col) ;
  // tree_correctedTimes->SetBranchAddress("row",&row) ;
  // tree_correctedTimes->SetBranchAddress("CT",&CT) ;

  // sigma and energy files
  ofstream SigmaFile;
  ofstream EnergyFile;
  SigmaFile.open (Form("sigma_files/sigma_%d_%d.txt",selected_column,selected_row));
  EnergyFile.open (Form("energy_files/energy_%d_%d.txt",selected_column,selected_row));

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

  TH2D *h2energy_spectrum_one_PM = new TH2D ("energy_one_PM","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2counts = new TH2D ("counts","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2coincidence = new TH2D ("coincidence","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2sigmaError = new TH2D ("sigma_error","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2rms = new TH2D ("rms","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2chi2 = new TH2D ("chi2_ndf","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2sigma = new TH2D ("sigma","", column_tot_number+2, -1, column_tot_number+1, row_tot_number+2, -1, row_tot_number+1) ;
  TH2D *h2energy = new TH2D ("energy","", 100, energy_cut_min, 4, 100, energy_cut_min, 4) ;
  TH2D *hdeltat = new TH2D ("delta t vs OM","", 290, 0, 289, 1000, -20, 20) ;
  TH2D *hthreshold = new TH2D ("threshold","", 1000, 0, 3, 1000, 0, 500) ;

  TProfile *hsigma_stat = new TProfile ("sigma vs stat","", 100, 0, 600, 0, 2) ;
  TProfile *hsigma_distance = new TProfile ("sigma vs distance","", 100, 0, 10, 0, 2) ;
  TProfile *hsigma_energy_max  = new TProfile("sigma_Emax","", 100, 0.5, 1.5, 0, 4);
  TProfile *hsigma_energy_min  = new TProfile("sigma_Emin","", 100, 0.5, 1.5, 0, 4);
  TProfile *hsigma_energy  = new TProfile("sigma_E","", 50, 1.6, 2.8, 0, 4);
  TProfile *hsigma_HV = new TProfile("sigma vs HV","", 100, 1000, 2000, 0, 2) ;

  TH1F *hsigma_OM = new TH1F ("sigma vs OM","", 250, 0, 249) ;

  TH1F *htime = new TH1F("Delta_time","",150, -20, 20) ;
  TH1F *henergy_spectrum = new TH1F("Total_energy_spectrum","",100, 0, 4) ;
  TH1F *henergy_spectrum_Emin = new TH1F("Energy_spectrum_Emin","",100, 0, 4) ;
  TH1F *henergy_spectrum_Emax = new TH1F("Energy_spectrum_Emax","",100, 0, 4) ;
  TH1F *hsigma = new TH1F("Fitted sigmas","",50, 0, 2) ;

  TH1F *hcoincidence[column_tot_number][row_tot_number] ;
  // for (int i=0 ;i<column_tot_number ;i++) {
  //   for (int j=0 ;j<row_tot_number ;j++) {
  //     hcoincidence[i][j] = new TH1F(Form("coicidences between OMs [%d:%d]&[%d:%d] ",selected_column,selected_row,i,j),Form("coicidences between OMs [%d:%d]&[%d:%d]",selected_column,selected_row,i,j),100,-20,20) ;
  //   }
  // }


  vector <vector <vector <double> > > delta_times(column_tot_number) ;
  double counts[column_tot_number][row_tot_number] ;
  double calo_coincidence[column_tot_number][row_tot_number] ;
  double one_PM_mean_energies[column_tot_number][row_tot_number] ;
  double min_energy[column_tot_number][row_tot_number] ;
  double max_energy[column_tot_number][row_tot_number] ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    delta_times[i]=vector<vector <double> >(row_tot_number) ;
    for (int j = 0 ; j < row_tot_number ; ++j) {
      counts[i][j]=0 ;
      calo_coincidence[i][j]=0 ;
      one_PM_mean_energies[i][j]=0 ;
      min_energy[i][j]=0 ;
      max_energy[i][j]=0 ;
    }
  }

  int counter_event = 0 ;
  int counter_loop = 0 ;

  TRandom *grandom1 = new TRandom(1234);

  for (Long64_t i=0 ;i<theTree->GetEntries() ;i++) {
    counter_loop++ ;
    theTree->GetEntry(i) ;
    if (i%100000==0) cout << "event " << i << endl ;

    if (isnan(calo_energy->at(0)) || isnan(calo_energy->at(1))){
      continue ;
    }

    else {

      // // Calcul de energy avec charge (faire avec constantes de calib de Axel)
      // bool flag_energy = 1 ;
      // for (int i_energy = 0 ; i_energy < calo_energy->size() ; ++i_energy) {
      //   if (calo_energy->at(i_energy)<energy_cut) {
      //     flag_energy=0 ;
      //   }
      // }

      //Fill avec Emin/Emax
      double Emin = 0., Emax = 0. ;
      double t_Emin = 0., t_Emax = 0. ;
      double Delta_t = 0. ;

      if (smearing_energy != 0) {
        calo_energy->at(0) = calo_energy->at(0)+grandom1->Gaus(0,smearing_energy) ;
        calo_energy->at(1) = calo_energy->at(1)+grandom1->Gaus(0,smearing_energy) ;
      }
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


      // // Sélectionner un carré autour de l'OM de référence [selected_column:selected_row]
      // bool flag_square=0 ;
      // if (calo_column->at(0)>selected_column-3&&calo_column->at(0)<selected_column+3) {
      //   if (calo_row->at(0)>selected_row-3&&calo_row->at(0)<selected_row+3) {
      //     if (calo_column->at(1)>selected_column-3&&calo_column->at(1)<selected_column+3) {
      //       if (calo_row->at(1)>selected_row-3&&calo_row->at(1)<selected_row+3) {
      //         flag_square=1 ;
      //       }
      //     }
      //   }
      // }

      //if (flag_square) {
      if (flag_energy) {

        //delta t à corriger des longueurs de cables coax (cf reflecto)!!
        counter_event++ ;
        Delta_t = t_Emax - t_Emin ;
        htime->Fill(Delta_t) ;

        henergy_spectrum_Emin->Fill(Emin) ;
        henergy_spectrum_Emax->Fill(Emax) ;
        h2energy->Fill(Emin,Emax) ;

        for (int j = 0 ; j < calo_row->size() ; ++j) {
          //counting number of events for each PM
          counts[calo_column->at(j)][calo_row->at(j)]++ ;

          henergy_spectrum->Fill(calo_energy->at(j)) ;

        }


        //tableau coïncidences
        int hit=-1 ;
        bool selected_PMT = select_PM(calo_column,calo_row,selected_column,selected_row,&hit) ;
        if (selected_PMT) {

          // // test 17/04/2020 pour étudier efficacité du détecteur (comparaison simus/data dans le directory DetectorEfficiency)
          // if (calo_column->at(abs(hit-1))==9&&calo_row->at(abs(hit-1))==7) {
          //   henergy_spectrum->Fill(calo_energy->at(hit)) ;
          //   henergy_spectrum->Fill(calo_energy->at(abs(hit-1))) ;
          //   henergy_spectrum_Emin->Fill(Emin) ;
          //   henergy_spectrum_Emax->Fill(Emax) ;
          // }
          // //

          // //// test with corrected times from reflecto (10/01/20)
          // for (int i=0; i<tree_correctedTimes->GetEntries(); i++){
          //   tree_correctedTimes->GetEntry(i);
          //   // cout << "no corr: " << calo_time->at(0) << " " << calo_time->at(1) << endl ;
          //   if (col==calo_column->at(0)&&row==calo_row->at(0)) {
          //     calo_time->at(0) = calo_time->at(0) - CT ;
          //   }
          //   if (col==calo_column->at(1)&&row==calo_row->at(1)) {
          //     calo_time->at(1) = calo_time->at(1) - CT ;
          //   }
          //   // cout << "corr: " << calo_time->at(0) << " " << calo_time->at(1) << endl ;
          // }
          // ////


          double dt=calo_time->at(hit)-calo_time->at(abs(hit-1)) ;
          calo_coincidence[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))]++ ;
          delta_times[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))].push_back(dt) ;
          //hcoincidence[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))]->Fill(dt) ;
          one_PM_mean_energies[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))] += calo_energy->at(abs(hit-1)) ;

          min_energy[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))] += Emin ;
          max_energy[calo_column->at(abs(hit-1))][calo_row->at(abs(hit-1))] += Emax ;

          //test 09/01/2020
          // hdeltat->Fill(calo_number->at(abs(hit-1)),dt) ;
          //


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


  double mean = 0. ;
  double median = 0. ;
  double rms = 0. ;
  double inf = 0. ;
  double sup = 0. ;
  Long64_t size = 0. ;
  int bin  = 0 ;
  int nb = 5 ;

  int OM_counter = 0 ;
  for (int i = 0 ; i < column_tot_number ; ++i) {
    for (int j = 0 ; j< row_tot_number ; ++j) {
      OM_counter++ ;

      h2counts->SetBinContent(i+2,j+2,counts[i][j]) ;
      h2coincidence->SetBinContent(i+2,j+2,calo_coincidence[i][j]) ;


      // filling deltat t histogram with tab
      mean = TMath::Mean(delta_times[i][j].begin(),delta_times[i][j].end()) ;
      size = delta_times[i][j].size() ;
      rms =  TMath::RMS(delta_times[i][j].begin(), delta_times[i][j].end()) ;
      median = Median(delta_times[i][j]) ;
      inf = mean-nb*rms ;
      sup = mean+nb*rms ;

      if (delta_times[i][j].size()>5) {
        bin = size/2 ;
      }
      else {
        bin = 1;
      }

      hcoincidence[i][j] = new TH1F(Form("%d_%d",i,j),Form("%d_%d",i,j),bin,inf,sup) ;
      for (Long64_t k = 0; k < size; ++k) {
        if (fabs(delta_times[i][j].at(k)) < fabs(median)+2*rms) {
          hcoincidence[i][j]->Fill(delta_times[i][j].at(k)) ;
        }
      }

      one_PM_mean_energies[i][j] /= calo_coincidence[i][j] ;
      min_energy[i][j] /= calo_coincidence[i][j] ;
      max_energy[i][j] /= calo_coincidence[i][j] ;
      h2energy_spectrum_one_PM->SetBinContent(i+2,j+2,one_PM_mean_energies[i][j]) ;

      double fit_parameters[2] ;
      FitDeltat(hcoincidence[i][j],fit_parameters) ;


      if (calo_coincidence[i][j] == 0) {
        continue ;
      }

      else {

        if (fit_parameters[0]!=0) {// in function FitDeltat fit_parameters are set to 0 if not
                                   // passing chi2 conditions
          if (fit_parameters[0]<1.5) {
            if (fit_parameters[1]/fit_parameters[0]<0.25) {

              h2sigma->SetBinContent(i+2,j+2,fit_parameters[0]) ;
              hsigma->Fill(fit_parameters[0]) ;

              SigmaFile << i << " " << j << " " << calo_coincidence[i][j] << " " << fit_parameters[0] << endl ;
              EnergyFile << i << " " << j << " " << calo_coincidence[i][j] << " " << one_PM_mean_energies[i][j] << endl ;
              h2sigmaError->SetBinContent(i+2,j+2,fit_parameters[1]) ;

              double ratio_energy = 1./max_energy[i][j] + 1./min_energy[i][j] ;
              hsigma_OM->SetBinContent(OM_counter+1,fit_parameters[0]) ;
              hsigma_energy_min->Fill(min_energy[i][j],fit_parameters[0],1) ;
              hsigma_energy_max->Fill(max_energy[i][j],fit_parameters[0],1) ;
              hsigma_energy->Fill(ratio_energy,pow(fit_parameters[0],2),1) ;
              hsigma_stat->Fill(calo_coincidence[i][j],fit_parameters[0]) ;
              hsigma_distance->Fill(distance_OM(selected_column,selected_row,i,j),fit_parameters[0]) ;

              //// try to look at HV influence on results 23/01/20
              for (int k=0; k<tree_HV->GetEntries(); k++){
                tree_HV->GetEntry(k);
                if (col_HV==i&&row_HV==j) {
                  hsigma_HV->Fill(HV,fit_parameters[0]) ;
                }
              }
              ////
            }
          }
        }
      }
    }
  }
  //  }




  // ///Drawing

  if (enable_drawing) {

    // gStyle->SetOptStat(1110) ;
    // gStyle->SetOptFit(1111) ;
    // TCanvas *c1 = new TCanvas("c1","c1",10,10,2000,1000) ;
    // for (int i = 0 ; i < column_tot_number ; ++i) {
    //   for (int j = 0 ; j < row_tot_number ; ++j) {
    // // for (int i = selected_column-2 ; i < selected_column+3 ; ++i) {
    // //   for (int j = selected_row-2 ; j < selected_row+3 ; ++j) {

    //     if (hcoincidence[i][j]) {
    //       TString n = TString::Format("coinc%d_%d", i,j) ;
    //       hcoincidence[i][j]->Draw() ;
    //       hcoincidence[i][j]->GetXaxis()->SetTitle("Delta t") ;
    //       c1->SaveAs("fit_plots/"+n+".png") ;
    //     }

    //   }
    // }


    // gStyle->SetOptStat(0) ;
    TCanvas *c2 = new TCanvas("c2","c2",10,10,2000,1000) ;
    c2->Divide(2,2) ;
    gStyle->SetPaintTextFormat("1.2f") ;
    c2->cd(1) ;
    config_histo2D(h2sigma, "sigma t (ns)", "Column","Row","COLZTEXT") ; //h2sigma->SaveAs("plots_data/sigma.root") ;
    c2->cd(2) ;
    config_histo2D(h2sigmaError, "sigma error", "Column","Row","COLZTEXT") ;//c2->SaveAs("plots_data/sigmaError.pdf") ;
    c2->cd(3) ;
    config_histo2D(h2coincidence,Form("Coincidences with OM [%d,%d]",selected_column,selected_row),"column","row","COLZTEXT") ;//c2->SaveAs("plots_data/coincidence.pdf") ;
    c2->SaveAs("plots_data/4_plots.pdf") ;

    TCanvas *c3 = new TCanvas("c3","c3",10,10,2000,1000) ;
    config_histo2D(h2energy, "E_{max} vs E_{min}", "E_{min}","E_{max}","COLZ") ;c3->SaveAs("plots_data/Emin_Emax.pdf") ;
    config_histo2D(h2counts, "Number of events in each PMT", "Column","Row","COLZ") ;c3->SaveAs("plots_data/counts.pdf") ;
    config_histo2D(h2energy_spectrum_one_PM,Form("Mean energie for each PM in coincidence with OM [%d:%d]", selected_column, selected_row), "Column","Row","COLZTEXT") ;c3->SaveAs("plots_data/energy_one_PM.pdf") ;


    TCanvas *c4 = new TCanvas("c4","c4",10,10,2000,1000) ;
    c4->Divide(2,4) ;
    c4->cd(1) ;
    gStyle->SetOptStat(1) ;
    config_histo1D(hsigma,"","Sigma (ns)","",2,1,1) ;
    c4->cd(2) ;
    // config_histo2D(hdeltat, "Delta t by channel", "OM number","Delta t","COLZ") ;
    c4->cd(3) ;
    config_histo1D(hsigma_OM,"P","OM number","Sigma (ns)",1,1,1) ; hsigma_OM->SetMarkerStyle(8) ; hsigma_OM->SetMarkerSize(0.5) ;
    hsigma_OM->SetMaximum(2);
    c4->cd(4) ;
    hsigma_energy_min->Draw() ; hsigma_energy_min->SetXTitle("E_{min}") ; hsigma_energy_min->SetYTitle("sigma (ns)") ;
    c4->cd(5) ;
    hsigma_energy_max->Draw() ; hsigma_energy_max->SetXTitle("E_{max}") ; hsigma_energy_max->SetYTitle("sigma (ns)") ;
    c4->cd(6) ;
    hsigma_stat->Draw() ; hsigma_stat->SetXTitle("number of events") ; hsigma_stat->SetYTitle("sigma (ns)") ;
    c4->cd(7) ;
    hsigma_distance->Draw() ; hsigma_distance->SetXTitle("distance with reference OM (unit of width OM)") ; hsigma_distance->SetYTitle("sigma (ns)") ;
    c4->cd(8) ;
    hsigma_HV->Draw() ; hsigma_HV->SetXTitle("HV applied") ; hsigma_HV->SetYTitle("sigma (ns)") ;

    c4->SaveAs("plots_data/6_plots.pdf") ;

    TCanvas *c5 = new TCanvas("c5","c5",10,10,2000,1000) ;
    gStyle->SetOptStat(1110) ;
    //Emin vs Emax
    config_histo1D(henergy_spectrum_Emin,"","Energy (MeV)","#counts",2,1,46) ;henergy_spectrum_Emin->SetFillColorAlpha(46,0.5) ;
    config_histo1D(henergy_spectrum_Emax,"SAME","Energy (MeV)","#counts",2,1,30) ;henergy_spectrum_Emax->SetFillColorAlpha(30,0.5) ;
    config_histo1D(henergy_spectrum,"SAME","Energy (MeV)","#counts",2,1,1) ;
    henergy_spectrum_Emin->GetYaxis()->SetRangeUser(0,henergy_spectrum->GetMaximum()+henergy_spectrum->GetMaximum()*0.1) ;
    SetPaveStat(henergy_spectrum_Emin) ;
    gStyle->SetOptStat("e") ;
    gStyle->SetLegendBorderSize(0) ;
    c5->BuildLegend(0.63,0.60,0.87,0.81) ;
    c5->SaveAs("plots_data/energy_spectrum_emin_emax.pdf") ;


  }

  string s_smearing_energy = to_string(smearing_energy) ;
  config_histo1D(henergy_spectrum,"SAME","Energy (MeV)","#counts",2,1,1) ;
  string title = "DetectorEfficiency/histograms/energy_spectrum_"+s_smearing_energy+".root" ;
  henergy_spectrum->SaveAs(title.c_str()) ;

  title = "DetectorEfficiency/histograms/energy_spectrum_Emin_"+s_smearing_energy+".root" ;
  config_histo1D(henergy_spectrum_Emin,"SAME","Energy (MeV)","#counts",2,1,1) ;
  henergy_spectrum_Emin->SaveAs(title.c_str()) ;

  title = "DetectorEfficiency/histograms/energy_spectrum_Emax_"+s_smearing_energy+".root" ;
  config_histo1D(henergy_spectrum_Emax,"SAME","Energy (MeV)","#counts",2,1,1) ;
  henergy_spectrum_Emax->SaveAs(title.c_str()) ;

  theTree->ResetBranchAddresses() ;
  SigmaFile.close() ;
  EnergyFile.close() ;
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

void FitDeltat(TH1F* histo, double fit_parameters[2]){

  double range_neg = histo->GetMean()-histo->GetStdDev()*5 ;
  double range_pos = histo->GetMean()+histo->GetStdDev()*5 ;
  TF1 *f1 = new TF1("f1","gaus",range_neg,range_pos) ;

  // f1->SetParameter(1,histo->GetMean()) ;
  // f1->SetParameter(2,histo->GetRMS()) ;

  histo->Fit("f1","QR") ;


  bool fitting = 0 ;
  if (f1->GetChisquare()/f1->GetNDF()<4&&f1->GetChisquare()/f1->GetNDF()>0.01) {
    fitting = 1 ;
  }
  else {
    // cout << "histogram " << histo->GetName() << ": fit parameters not passing chi2 condition" << endl ;
    fitting = 0 ;
  }


  if (fitting) {

    fit_parameters[0] = f1->GetParameter(2) ;
    fit_parameters[1] = f1->GetParError(2) ;

  }
  else {
    for (int i = 0; i < 2; ++i) {
      fit_parameters[i] = 0 ;
    }
  }

  delete f1 ;

}


double distance_OM(int col1,int row1,int col2,int row2){

  return sqrt(pow((col2-col1),2)+pow((row2-row1),2)) ;

}


double Median(vector<double> vect) {
  size_t size = vect.size();

  if (size == 0) {
    return 0;  // Undefined, really.
  }

  else {
    sort(vect.begin(), vect.end());
    if (size % 2 == 0) {
      return (vect[size / 2 - 1] + vect[size / 2]) / 2;
    }
    else {
      return vect[size / 2];
    }
  }

}
