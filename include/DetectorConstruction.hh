/* ------- GAMMA ATTENUATION SIMULATION -------
Autor: @isabelnieto900, @PoPPop21
Fecha: Octubre 2025
-----------------------------------------------
*/
#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh" // <-- Importante: incluir para G4Material*

class DetectorMessenger; // Forward declaration
class G4LogicalVolume;   // Forward declaration

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume *Construct();
    virtual void ConstructSDandField();

    // Métodos para cambiar propiedades dinámicamente
    void SetMaterialType(const G4String &material);
    void SetThickness(G4double thick);

    // Getters para que otras clases accedan a las propiedades
    const G4String &GetMaterialName() const { return materialType; }
    G4double GetThickness() const { return thickness; }
    const G4Material *GetCurrentMaterial() const { return currentMaterial; } // <-- El método que faltaba

private:
    G4Material *DefineMaterials();

    DetectorMessenger *messenger;
    G4String materialType;
    G4double thickness;
    G4Material *currentMaterial; // <-- El miembro que faltaba
    G4LogicalVolume *logicDet;   // Puntero al volumen lógico del detector
};

#endif