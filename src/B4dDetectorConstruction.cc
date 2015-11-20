//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4dDetectorConstruction.cc 77601 2013-11-26 17:08:44Z gcosmo $
//
/// \file B4dDetectorConstruction.cc
/// \brief Implementation of the B4dDetectorConstruction class

#include "B4dDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"
#include "G4SDChargedFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4dDetectorConstruction::fMagFieldMessenger = 0; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dDetectorConstruction::B4dDetectorConstruction()
    : G4VUserDetectorConstruction(),
      fCheckOverlaps(true)
{
    G4String pixelnamepre="pixel_";
    G4int i,j;
    for(i=0;i<nofpixelineachrow;i++)   //为每个像素命名
    {
        for(j=0;j<nofpixelineachcolumn;j++)
        {
            pixelname[i][j]=pixelnamepre+num2str(i)+"_"+num2str(j);
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dDetectorConstruction::~B4dDetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4dDetectorConstruction::Construct()
{
    // Define materials
    DefineMaterials();

    // Define volumes
    return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dDetectorConstruction::DefineMaterials()
{ 
    // Lead material defined using NIST Manager
    G4NistManager* nistManager = G4NistManager::Instance();
    nistManager->FindOrBuildMaterial("G4_Au");
    nistManager->FindOrBuildMaterial("G4_Si");
    nistManager->FindOrBuildMaterial("G4_Be");
    nistManager->FindOrBuildMaterial("G4_Ag");
    nistManager->FindOrBuildMaterial("G4_Al");
    nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    // Liquid argon material
    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons;
    G4double density;
    new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
    // The argon by NIST Manager is a gas with a different density

    // Vacuum
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                   kStateGas, 2.73*kelvin, 3.e-18*pascal);

    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4dDetectorConstruction::DefineVolumes()
{
    // Geometry parameters
    //    G4double pixelSensiXY=240.0*um;//灵敏体的尺寸
    //    G4double pixelSensiThick=240.0*um;//灵敏体的厚度
    G4double pixelX=20.0*um;   //像素的大小
    G4double pixelY=20.0*um;   //像素的大小
    G4double pixelZ=50.0*um;   //像素的厚度
    G4double gap=0.1*um;   //像素间的间距

    //    const  G4int nofpixelineachcolumn=sccdpara->getpixelcolumn();//每列的像素个数,像素在Y方向上的个数
    //    const G4int nofpixelineachrow=sccdpara->getpixelrow();//每行的像素个数，

    G4double arrayX=pixelX*nofpixelineachrow+(nofpixelineachrow-1)*gap; //感光面元的尺寸
    G4double arrayY=pixelY*nofpixelineachcolumn+(nofpixelineachcolumn-1)*gap;
    G4double arrayZ=pixelZ;

    //银层的尺寸
    G4double AgLayerX=arrayX;
    G4double AgLayerY=arrayY;
    G4double AgLayerZ=10.0*um;
    G4double AgLayerPlaceZ=-(AgLayerZ+arrayZ)/2;//银层放置的位置

    G4double worldX = 2 * arrayX;
    G4double worldY = 2 * arrayY;
    G4double worldZ  = 5 *(arrayZ+AgLayerZ);

    // Get materials

    G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
    G4Material* worldMaterial = G4Material::GetMaterial("Galactic");
    G4Material* sensitiveMaterial = G4Material::GetMaterial("G4_Si");
    G4Material* AgLayerMaterial = G4Material::GetMaterial("G4_Ag");
    //  G4Material* gapMaterial = G4Material::GetMaterial("Galactic");

    if ( ! defaultMaterial || ! sensitiveMaterial  ) {
        G4ExceptionDescription msg;
        msg << "Cannot retrieve materials already defined.";
        G4Exception("B4DetectorConstruction::DefineVolumes()",
                    "MyCode0001", FatalException, msg);
    }

    //
    // World
    //
    G4VSolid* worldS
            = new G4Box("World",           // its name
                        worldX/2, worldY/2, worldZ/2); // its size

    G4LogicalVolume* worldLV
            = new G4LogicalVolume(
                worldS,           // its solid
                worldMaterial,  // its material
                "World");         // its name

    G4VPhysicalVolume* worldPV
            = new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(),  // at (0,0,0)
                worldLV,          // its logical volume
                "World",          // its name
                0,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps

    //
    // Array
    //
    G4VSolid* arrayS
            = new G4Box("Array",     // its name
                        arrayX/2, arrayY/2, arrayZ/2); // its size

    G4LogicalVolume* arrayLV
            = new G4LogicalVolume(
                arrayS,    // its solid
                defaultMaterial, // its material
                "Array");  // its name

    new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(),  // at (0,0,0)
                arrayLV,          // its logical volume
                "Array",    // its name
                worldLV,          // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps

    //
    // Pixel
    //
    G4VSolid* pixelS
            = new G4Box("Pixel",            // its name
                        pixelX/2, pixelY/2, pixelZ/2); // its size
    G4LogicalVolume* pixelLV[nofpixelineachrow][nofpixelineachcolumn];//多功能探测器
    G4int i,j;
    G4double pixelx[nofpixelineachrow], pixely[nofpixelineachcolumn],pixelz;
    pixelx[0]= -arrayX/2+pixelX/2;//左下角的像素为第一个[0][0]
    pixely[0]=  -arrayY/2+pixelY/2;
    pixelz=0.;
    for(j=1;j<nofpixelineachcolumn;j++)
    {
        pixely[j]=pixely[0]+j*(gap+pixelY);
    }
    for(i=1;i<nofpixelineachrow;i++)
    {

        pixelx[i]=pixelx[0]+i*(gap+pixelX);
    }

    for(i=0;i<nofpixelineachrow;i++)   //每次循环都生成一个像素，并设置为灵敏探测器
    {
        for(j=0;j<nofpixelineachcolumn;j++)
        {

            pixelLV[i][j]= new G4LogicalVolume(
                        pixelS,        // its solid
                        sensitiveMaterial, // its material
                        pixelname[i][j]);          // its name

            new G4PVPlacement(
                        0,                // no rotation
                        G4ThreeVector( pixelx[i],  pixely[j], pixelz), //  its position
                        pixelLV[i][j],            // its logical volume
                        pixelname[i][j],            // its name
                        arrayLV,          // its mother  volume
                        false,            // no boolean operation
                        0,                // copy number
                        fCheckOverlaps);  // checking overlaps
            //            pixelLV[i][j]->SetVisAttributes(simpleBoxVisAtt);
        }

    }

    G4VSolid* AgLayerS
            = new G4Box("AgLayerS",     // its name
                        AgLayerX/2, AgLayerY/2, AgLayerZ/2); // its size

    G4LogicalVolume* AgLayerLV
            = new G4LogicalVolume(
                AgLayerS,    // its solid
                AgLayerMaterial, // its material
                "AgLayerL");  // its name

    new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(0.,0.,AgLayerPlaceZ),  // at (0,0,0)
                AgLayerLV,          // its logical volume
                "AgLayer",    // its name
                worldLV,          // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                fCheckOverlaps);  // checking overlaps
    //
    //         G4Colour yellow (1.0, 1.0, 0.0) ; // yellow
    //         G4Colour white (1.0, 1.0, 1.0) ; // white
//    G4Colour gray1 (0.5, 0.5, 0.5) ; // gray
    //         G4Colour black (0.0, 0.0, 0.0) ; // black
    //         G4Colour red (1.0, 0.0, 0.0) ; // red
    //         G4Colour green (0.0, 1.0, 0.0) ; // green
    //         G4Colour blue (0.0, 0.0, 1.0) ; // blue
    G4Colour cyan (0.0, 1.0, 1.0) ; // cyan
    //         G4Colour magenta (1.0, 0.0, 1.0) ; // magenta
    G4Colour colour4 (1.0, 1.0, 1.0,0) ;//透明的
    // Visualization attributes
    //
    worldLV->SetVisAttributes (new G4VisAttributes(colour4));
//    arrayLV -> SetVisAttributes(new G4VisAttributes(gray1));
    AgLayerLV -> SetVisAttributes(new G4VisAttributes(cyan));
    //
    // print parameters
    //
    G4cout << "\n------------------------------------------------------------"
           << "\n---> The SensitiveArray is  " <<nofpixelineachrow<<" * " <<nofpixelineachcolumn << " piexels ( "
           << G4BestUnit(pixelX,"Length") << " * "<< G4BestUnit(pixelY,"Length") << " * "<< G4BestUnit(pixelZ, "Length")
           << " )of  " << sensitiveMaterial->GetName()<<AgLayerMaterial->GetName() << G4endl;
    G4cout << "\n------------------------------------------------------------"<<G4endl;

    //
    // Always return the physical World
    //
    return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dDetectorConstruction::ConstructSDandField()
{
    //    const  G4int nofpixelineachcolumn=11;//每列的像素个数,像素在X方向上的个数
    //    const G4int nofpixelineachrow=11;//每行的像素个数，
    //    const  G4int nofpixelineachcolumn=sccdpara->getpixelcolumn();//每列的像素个数,像素在Y方向上的个数
    //    const G4int nofpixelineachrow=sccdpara->getpixelrow();//每行的像素个数，

    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
    G4MultiFunctionalDetector* pixeldetctor[nofpixelineachrow][nofpixelineachcolumn];//多功能探测器
    G4VPrimitiveScorer* primitive;
    //
    // Scorers
    //
    G4int i,j;
    for(i=0;i<nofpixelineachrow;i++)   //每次循环都生成一个像素，并设置为灵敏探测器
    {
        for(j=0;j<nofpixelineachcolumn;j++)
        {
            pixeldetctor[i][j] = new G4MultiFunctionalDetector(pixelname[i][j]);
            primitive = new G4PSEnergyDeposit("Edep");
            pixeldetctor[i][j] ->RegisterPrimitive(primitive);
            SetSensitiveDetector(pixelname[i][j], pixeldetctor[i][j] );
        }
    }

    //
    // Magnetic field
    //
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue = G4ThreeVector();
    fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
    fMagFieldMessenger->SetVerboseLevel(1);

    // Register the field messenger for deleting
    G4AutoDelete::Register(fMagFieldMessenger);
}

G4String B4dDetectorConstruction::num2str(G4int i)
{
    char strtemp[9];
    sprintf(strtemp,"%d",i);
    return strtemp;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
