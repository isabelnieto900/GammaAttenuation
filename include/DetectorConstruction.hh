/* ------- GAMMA ATTENUATION SIMULATION -------
Autor: @isabelnieto900, @PoPPop21
Fecha: Octubre 2025
-----------------------------------------------
*/
#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"
#include"G4RunManager.hh" 
#include "globals.hh"

class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {
public: 
    DetectorConstruction();
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct();

    // --- Métodos para cambiar parámetros ---
    void SetMaterialType(const G4String& material); // Cambiar material
    void SetThickness(G4double thickness); // Cambiar espesor

    G4String GetMaterial() const { return materialType; } // Tipo de material
    G4double GetThickness() const { return thickness; } // Espesor del material
    
private:
    G4Material* DefineMaterials(); // Definición de materiales

    G4String materialType; // Tipo de material
    G4double thickness; // Espesor del material
    DetectorMessenger* messenger;


};

#endif // DETECTORCONSTRUCTION_HH