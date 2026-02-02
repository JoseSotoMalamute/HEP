#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h" // Necesario para manejar colores avanzados

void plot_result() {
    // 1. Configurar archivo de salida ROOT
    TFile* rootFile = new TFile("Edep_Electron_150.root", "RECREATE");

    // 2. Crear el histograma
    TH1F* hist = new TH1F("Edep", "Edep Electron 150 GeV;Energy [MeV];Entries", 100, 0., 10.);

    std::string fileName = "EnergyDeposition_h1_Edep.csv";
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cout << "ERROR: No se encuentra " << fileName << std::endl;
        return;
    }

    std::string line;
    int currentBin = 0;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line.find("entries") != std::string::npos) {
            continue;
        }

        std::stringstream ss(line);
        std::string segment;
        
        if (std::getline(ss, segment, ',')) {
            try {
                double counts = std::stod(segment);
                hist->SetBinContent(currentBin, counts);
                currentBin++;
            } catch (...) {
                continue;
            }
        }
    }
    
    file.close();

    // --- SECCIÓN DE ESTILO ---
    // Definimos un color verde pastel (R=144, G=238, B=144) con 50% de transparencia (0.5)
    // El método GetColorTransparent devuelve un índice de color que ROOT entiende.
    Short_t colorIndex = TColor::GetColorTransparent(kGreen - 9, 0.5);
    
    hist->SetFillColor(colorIndex); // Asigna el color verde pastel
    hist->SetFillStyle(1001);        // 1001 es el código para relleno sólido
    hist->SetLineColor(kGreen + 2);  // Un borde verde un poco más oscuro para que resalte
    hist->SetLineWidth(2);           // Grosor del borde
    // -------------------------

    hist->Write();
    
    // Opcional: Si quieres verlo al ejecutar, necesitarías un TCanvas
    // TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);
    // hist->Draw("HIST");

    std::cout << "Histograma generado con éxito y estilo aplicado." << std::endl;

    delete rootFile;
}
