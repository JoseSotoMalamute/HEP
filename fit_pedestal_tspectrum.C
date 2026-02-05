// ============================================================
// fit_sphe_generalized_variable.C
// ============================================================

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TString.h>
#include <TMath.h>
#include <TPaveText.h> 
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

// ==========================================
// CONFIGURACIÓN GENERAL
// ==========================================
const int N_PEAKS = 3; 

// --- Función de Procesamiento por Canal ---
void procesar_canal(const char* archivo, const char* canal, const char* label, TCanvas* c, int ipad, TPaveText* pt)
{
    TFile* f = TFile::Open(archivo);
    if (!f || f->IsZombie()) return;
    TTree* t = (TTree*)f->Get("tree");
    if (!t) { f->Close(); return; }

    TString hname = Form("h_%s", canal);
    if (gROOT->FindObject(hname)) delete gROOT->FindObject(hname);

    // 1. RANGO DINÁMICO (Se ajusta solo según N_PEAKS)
    double estimated_max_x = N_PEAKS * 0.35 + 0.5; 
    
    TH1F* h = new TH1F(hname, 
        Form("Single Photo-Electron %s;fInt Integrated Charge [ns * V];Entries", label), 
        250, -0.5, estimated_max_x); 
        
    t->Draw(Form("%s.fInt>>%s", canal, hname.Data()), "", "goff");
    h->SetDirectory(0);
    f->Close();

    if (h->GetEntries() == 0) return;

    c->cd(ipad);
    gPad->Clear(); gPad->SetTicks();
    h->Draw("hist");

    // 2. Detección y Ganancia
    TSpectrum spec(40);
    int nfound = spec.Search(h, 2.0, "", 0.01); // Threshold ... 2.0 y 0.005
    if (nfound < 2) return;

    double* xpos = spec.GetPositionX();
    std::vector<double> found_peaks;
    for(int i=0; i<nfound; ++i) found_peaks.push_back(xpos[i]);
    std::sort(found_peaks.begin(), found_peaks.end());

    double mu_p0 = found_peaks[0];
    if (mu_p0 < -0.1 && found_peaks.size() > 1) mu_p0 = found_peaks[1];

    // ganancia dinamica
    double gain_ref = 0.0;
    std::vector<double> valid_peaks;
    valid_peaks.push_back(mu_p0);
    
    double current_mu = mu_p0;
    // Solo buscamos hasta N_PEAKS o 5, lo que sea menor
    int peaks_to_check = (N_PEAKS < 5) ? N_PEAKS : 5; 
    
    for(int k=0; k < peaks_to_check; ++k) { 
        double best_p = -999;
        for(double p : found_peaks) {
            if(p > current_mu + 0.1 && p < current_mu + 1.) { // distancia entre peaks  0.1 y 0.4
                best_p = p;
                break;
            }
        }
        if(best_p > -900) {
            valid_peaks.push_back(best_p);
            current_mu = best_p;
        } else {
            break; 
        }
    }

    if (valid_peaks.size() >= 2) {
        int n = valid_peaks.size() - 1;
        gain_ref = (valid_peaks.back() - mu_p0) / n;
    } else {
        gain_ref = 0.2;
    }

    // 3. Sigma Base
    double mu_p1_est = mu_p0 + gain_ref;
    TF1* f_temp = new TF1("ftemp", "gaus", mu_p1_est - 0.1, mu_p1_est + 0.1);
    f_temp->SetParameters(h->GetBinContent(h->FindBin(mu_p1_est)), mu_p1_est, 0.05);
    h->Fit(f_temp, "RQ0");
    double base_spe_sig = f_temp->GetParameter(2);
    double base_ped_sig = 0.04; 
    delete f_temp;

    // 4. GENERACIÓN DINÁMICA DE LA FÓRMULA
    TString fit_formula = "";
    for(int i=0; i < N_PEAKS; ++i) {
        if (i > 0) fit_formula += " + ";
        fit_formula += Form("gaus(%d)", i*3);
    }
    
    double min_fit = mu_p0 - 2.5 * base_ped_sig;
    double max_fit = mu_p0 + (N_PEAKS + 0.8) * gain_ref; 

    TF1* f_total = new TF1(Form("f_final_%s", canal), fit_formula, min_fit, max_fit);
    
    f_total->SetLineColor(kRed);
    f_total->SetLineWidth(2);
    f_total->SetNpx(1000);

    // 5. CONFIGURACIÓN DE PARÁMETROS
    for (int i = 0; i < N_PEAKS; ++i) {
        int idx = i * 3;
        double theoretical_mu = mu_p0 + i * gain_ref;
        double theoretical_sig;
        
        if (i == 0) theoretical_sig = base_ped_sig;
        else        theoretical_sig = base_spe_sig * TMath::Sqrt(i);

        // A. Amplitud
        int bin_theo = h->FindBin(theoretical_mu);
        double amp_val = 0;
        for(int k=-2; k<=2; ++k) {
            double v = h->GetBinContent(bin_theo + k);
            if(v > amp_val) amp_val = v;
        }
        if (amp_val < 0.5) amp_val = 1.0; 

        f_total->SetParameter(idx, amp_val);
        f_total->SetParLimits(idx, 0, amp_val * 5.0); 

        // B. Posición
        f_total->SetParameter(idx + 1, theoretical_mu);
        f_total->SetParLimits(idx + 1, theoretical_mu - 0.04, theoretical_mu + 0.05); // -0.04 y 0.04

        // C. Ancho
        f_total->SetParameter(idx + 2, theoretical_sig);
        
        double tolerance = 0.85;  // 65%
        double min_sig = theoretical_sig * (1.0 - tolerance);
        double max_sig_tolerance = theoretical_sig * (1.0 + tolerance);
        double max_sig_physics = gain_ref * 0.45;
        double final_max_sig = TMath::Min(max_sig_tolerance, max_sig_physics);

        if (i == 0) f_total->SetParLimits(idx + 2, 0.01, 0.09); 
        else        f_total->SetParLimits(idx + 2, min_sig, final_max_sig);
    }

    h->Fit(f_total, "RMQL+");

    // DIBUJO
    for (int i = 0; i < N_PEAKS; ++i) {
        TF1* c_draw = new TF1(Form("c%d", i), "gaus", min_fit, max_fit);
        c_draw->SetParameters(
            f_total->GetParameter(i*3),
            f_total->GetParameter(i*3+1),
            f_total->GetParameter(i*3+2)
        );
        c_draw->SetLineColor(kGray);
        c_draw->SetLineStyle(1);
        c_draw->Draw("same");
    }
    f_total->Draw("same");

    // REPORTE
    pt->AddText(Form("--- %s (%s) ---", label, canal));
    pt->AddText(Form("Gain: %.4f", gain_ref));
    pt->AddText("   Peak |    Mu    |  Sigma  ");
    pt->AddText("   --------------------------");

    for(int i=0; i < N_PEAKS; ++i) {
        double mu_val  = f_total->GetParameter(i*3+1);
        double sig_val = f_total->GetParameter(i*3+2);
        pt->AddText(Form("   P%d   |  %.4f  |  %.4f", i, mu_val, sig_val));
    }
    pt->AddText(" "); 
    std::cout << "--- " << label << " Procesado (" << N_PEAKS << " peaks) ---" << std::endl;
}

// ============================================================
// MAIN
// ============================================================
void fit_pedestal_tspectrum(const char* archivo)
{
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    
    // Nombres Dinámicos
    TString strArchivo = archivo;
    Ssiz_t slashPos = strArchivo.Last('/');
    TString nombreBase = (slashPos != -1) ? strArchivo(slashPos + 1, strArchivo.Length()) : strArchivo;
    if (nombreBase.EndsWith(".root")) nombreBase.ReplaceAll(".root", "");
    
    TString outDir = "/Users/otymar/Documents/Physics/Tesis/SEP/nuevos/";
    
    TString nombrePlot   = outDir + nombreBase + "_Spectra.png";
    TString nombreReport = outDir + nombreBase + "_Report.pdf";

    TCanvas* c = new TCanvas("c_sphe", "Single Photo-Electron Spectra", 1200, 900);
    c->Divide(2,2);

    TCanvas* c_rep = new TCanvas("c_rep", "Report Values", 800, 1100);
    TPaveText* pt = new TPaveText(0.05, 0.05, 0.95, 0.95, "NDC");
    pt->SetTextAlign(12); 
    pt->SetTextFont(102); 
    pt->SetTextSize(0.025); 
    pt->SetFillColor(kWhite);
    
    pt->AddText(Form("=== REPORTE SPhe (%d Peaks) ===", N_PEAKS));
    pt->AddText(Form("Archivo: %s", nombreBase.Data()));
    pt->AddText(" ");

    std::cout << "\n=== Procesando: " << nombreBase << " con " << N_PEAKS << " Peaks ===\n";
    
    procesar_canal(archivo, "H0", "A1", c, 1, pt);
    procesar_canal(archivo, "H1", "B1", c, 2, pt);
    procesar_canal(archivo, "H2", "A2", c, 3, pt);
    procesar_canal(archivo, "H3", "B2", c, 4, pt);
    
    c->Update();

    c->SaveAs(nombrePlot);
    std::cout << "-> Graficos guardados en: " << nombrePlot << std::endl;

    c_rep->cd();
    pt->Draw();
    c_rep->SaveAs(nombreReport);
    std::cout << "-> Reporte guardado en:   " << nombreReport << std::endl;
}
