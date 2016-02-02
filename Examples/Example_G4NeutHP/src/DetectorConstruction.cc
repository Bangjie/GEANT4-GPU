#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

DetectorConstruction::DetectorConstruction()
	:G4VUserDetectorConstruction(), fPBox(0), fLBox(0), fMaterial(0),
	 fDetectorMessenger(0){ //initialization list for DetectorConstruction constructor
	fBoxSide = 1*m;
	DefineMaterials();
	//SetMaterial("Uranium");
	fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction(){
	delete fDetectorMessenger;
}

void DetectorConstruction::DefineMaterials() {
	G4String  name, symbol;
	G4double density, fractionmass , abundance;
	G4int ncomponents, natoms, iz , a,  n;
	G4NistManager* man = G4NistManager::Instance();
  
  // specific element name for thermal neutronHP
  // (see G4NeutronHPThermalScatteringNames.cc)
  G4Element* H  = new G4Element("TS_H_of_Water" ,"H" , 1., 1.0079*g/mole);
  G4Element* O  = new G4Element("Oxygen"        ,"O" , 8., 16.00*g/mole);

  G4Material* H2O =
  new G4Material("Water_ts", 1.000*g/cm3, ncomponents=2,
                         kStateLiquid, 593*kelvin, 150*bar);
  H2O->AddElement(H, natoms=2);
  H2O->AddElement(O, natoms=1);
  H2O->GetIonisation()->SetMeanExcitationEnergy(78.0*eV);

  // graphite
  G4Isotope* C12 = new G4Isotope("C12", 6, 12);
  G4Element* elC = new G4Element("TS_C_of_Graphite","C", ncomponents=1);
  elC->AddIsotope(C12, 100.*perCent);
  G4Material* graphite =
  new G4Material("graphite", 2.27*g/cm3, ncomponents=1,
                         kStateSolid, 293*kelvin, 1*atmosphere);
  graphite->AddElement(elC, natoms=1);

 ///G4cout << *(G4Material::GetMaterialTable()) << G4endl;

	G4Material* Uranium_Nist;
	G4Material* Uranium_a;
	G4Material* Uranium_b;

	Uranium_Nist =man->FindOrBuildMaterial("G4_U");
	G4Isotope* U4 = new G4Isotope("U234",92,234,234.04*g/mole);
	G4Isotope* U5 = new G4Isotope("U235",92,235,235.04*g/mole);
	G4Isotope* U8 = new G4Isotope("U238",92,238,238.05*g/mole);

	G4Element*  Uela=new G4Element(name="Uranium_element_a",symbol="Uela",ncomponents=3);
	Uela->AddIsotope(U4,abundance=0.01 *perCent);
	Uela->AddIsotope(U5,abundance=0.72 *perCent);
	Uela->AddIsotope(U8,abundance=99.27*perCent);

	G4Element*  Uelb=new G4Element(name="Uranium_element_b",symbol="Uelb",ncomponents=3);
	Uelb->AddIsotope(U4,abundance=0.01 *perCent);
	Uelb->AddIsotope(U4,abundance=99.27 *perCent);
	Uelb->AddIsotope(U4,abundance=0.72 *perCent);

	Uranium_a=new G4Material("Uranium_a",18.95*g/cm3,1);
	Uranium_a->AddElement(Uela,fractionmass=1.0);

	Uranium_b=new G4Material("Uranium_b",18.95*g/cm3,1);
	Uranium_b->AddElement(Uelb,fractionmass=1.0);

	G4cout<<" Uranium_a is  "<<G4endl<<    Uranium_a   <<G4endl;
	G4cout<<" Uranium_b is  "<<G4endl<<    Uranium_b   <<G4endl;
	G4cout<<" Uranium_Nist is  "<<G4endl<<    Uranium_Nist   <<G4endl;
}

G4Material* DetectorConstruction::MaterialWithSingleIsotope(G4String name, G4String symbol, G4double density, G4int Z, G4int A){ //define a material from an isoptope

}

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes(){
	//cleanup old geometry
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
}

void DetectorConstruction::SetMaterial(G4String materialChoice){
	// search the material by its name
  G4Material* pttoMaterial =
     G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);   
  
  if (pttoMaterial) { 
    if(fMaterial != pttoMaterial) {
      fMaterial = pttoMaterial;
      if(fLBox) { fLBox->SetMaterial(pttoMaterial); }
      G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetMaterial : "
           << materialChoice << " not found" << G4endl;
  } 
}

void DetectorConstruction::SetSize(G4double value){

}



