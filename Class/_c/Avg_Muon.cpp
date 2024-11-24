#include "../classe.hpp"
#include <cstdio>
#include <string>

// ****************************************************************
// This macro is used to create an average muon waveform and save it
// in a binary file, setting print=true, once you are happy with the
// selection cuts. The idea is to select the muon waveforms on an
// f_prompt basis and remove the ones that have light in coincidence.
//
// Parameters you can tune:
// - int_low, int_prompt, int_up <-> integration limits
// - f_prompt (integral[int_low;int_prompt]/integral[int_low;int_up])
//      We select waveforms with f_prompt lower than this value.
// - sat_low <-> discard wfs with samples below this level
// - amp_low, amp_up <-> the max element in the [int_low;int_prompt]
//      range must fall in [amp_low;amp_up]
// - bsl <-> accept only wfs with sample whithin [-bsl;+bsl] in the
//      pre trigger [0;prepulse_ticks]
// - rms <-> the selected wfs can differ from the average only by random
//      fluctuation, we must ensure there are no pulses in the tail of
//      our signal.
// ****************************************************************

////////////////////////////////////////////////////////
/////// HARD CODE //////////////////////////////////////

std::string muon_files_path = "/Users/federico/PhD/PDE/Muon_files/";


////////////////////////////////////////////////////////

//*********************************************
//-----------------------------------------------------------------
//------- Macro ---------------------------------------------------
void cla::Avg_Muon(){
//*********************************************
  std::vector<double> avg_mu;
  std::vector<std::vector<double>> sel_wf, mu_wf;

  read();
  
  TH1D* h_prompt = AllFpromptHisto(wfs, int_low, int_up, int_prompt);

  SelPDE_WF(wfs, sel_wf, prepulse_ticks, int_prompt,
            sat_low, amp_low, amp_up, bsl, rms);  

  TH2D* h2_fprompt_charge = BuildChargeFpromptHisto(sel_wf, mu_wf, int_low, int_up,
      int_prompt, f_prompt);
 
  cout << "\nMuon canidates: " << mu_wf.size() << endl;
  avgWF(mu_wf, avg_mu);

  if(print==true) {
    string outfile_name;
    cout << "Name of the muon file (without .dat)" << endl;
    cin >> outfile_name;
    outfile_name = muon_files_path+outfile_name;
    VecDouble_in_Binary(outfile_name, avg_mu);
    print = false;
  }

  double ymin, ymax;
  min_max_element(mu_wf, ymin, ymax);

  if (plot == true){
    TH2D* h2_mu_pers = new TH2D("h2_mu_pers", Form("%s;%s;%s", "Persistence", "Ticks", "ADC Counts"),
        memorydepth/2, 0., memorydepth, 
        120, ymin, ymax);
    
    for (int i=0; i< mu_wf.size(); i++) 
      for (int j=0; j<memorydepth; j=j+2) h2_mu_pers->Fill(j, mu_wf[i][j]);

    TH1D* h_muon_prompt = h2_fprompt_charge->ProjectionY("h_muon_prompt");
    
    TCanvas *c_mu_pers = new TCanvas("c_mu_pers","c_mu_pers",0,0,500,450);  
    c_mu_pers->cd();
    h2_mu_pers->Draw("COLZ");
    c_mu_pers->SetLogz();
    c_mu_pers->Modified(); c_mu_pers->Update();


    TCanvas *c_fprompt_charge = new TCanvas("c_fprompt_charge","c_fprompt_charge",500,0,500,450);
    c_fprompt_charge->cd();
    h2_fprompt_charge->Draw();
    c_fprompt_charge->Modified(); c_fprompt_charge->Update();


    TGraph *g_mu_avg = new TGraph(avg_mu.size(), &avg_mu[0]);
    //g_mu_avg->GetXaxis()->SetTitle("Time [#mus]");
    g_mu_avg->GetYaxis()->SetTitle("ADC counts");
    g_mu_avg->SetLineColor(2);
    TCanvas *c_mu_avg = new TCanvas("c_mu_avg","c_mu_avg",0,550,500,450);
    c_mu_avg->cd();
    g_mu_avg->Draw();
    c_mu_avg->Modified(); c_mu_avg->Update();

    TCanvas *c_prompt = new TCanvas("c_prompt","c_prompt",500,550,500,450);
    c_prompt->cd();
    h_prompt->SetLineColor(kRed);
    h_prompt->Draw();
    h_muon_prompt->Draw("SAME");
    c_prompt->Modified(); c_prompt->Update();
  }
}
