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
// $Id: B4PrimaryGeneratorAction.cc 75215 2013-10-29 16:07:06Z gcosmo $
// 
/// \file B4PrimaryGeneratorAction.cc
/// \brief Implementation of the B4PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "B4PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
//#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
//#include "G4ParticleTable.hh"
//#include "G4ParticleDefinition.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4PrimaryGeneratorAction::B4PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0)
{
    InitializeGPS();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4PrimaryGeneratorAction::~B4PrimaryGeneratorAction()
{
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // This function is called at the begining of event
    // Set gun position
   // fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 30.0*um));
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

void B4PrimaryGeneratorAction::InitializeGPS()
{
    fParticleGun = new G4GeneralParticleSource();
    // In order to avoid dependence of PrimaryGeneratorAction
    // on DetectorConstruction class we get world volume
    // from G4LogicalVolumeStore
    //
    worldZHalfLength = 0;
    G4LogicalVolume* worlLV
            = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
    G4Box* worldBox = 0;
    if ( worlLV) worldBox = dynamic_cast< G4Box*>(worlLV->GetSolid());
    if ( worldBox ) {
        worldZHalfLength = worldBox->GetZHalfLength();
    }
    else  {
        G4ExceptionDescription msg;
        msg << "World volume of box not found." << G4endl;
        msg << "Perhaps you have changed geometry." << G4endl;
        msg << "The gun will be place in the center.";
        G4Exception("B4PrimaryGeneratorAction::GeneratePrimaries()",
                    "MyCode0002", JustWarning, msg);
    }

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* pion = particleTable->FindParticle("e-");
    fParticleGun->GetCurrentSource()->SetParticleDefinition(pion);
    // set energy distribution
    G4SPSEneDistribution *eneDist = fParticleGun->GetCurrentSource()->GetEneDist() ;
    eneDist->SetEnergyDisType("Mono"); // or gauss
    eneDist->SetMonoEnergy(70.0*keV);
    // set position distribution

    G4double z0 = 80.0*um;
    G4double x0 = 0.0*um;
    G4double y0 = 0.0*um;
    G4SPSPosDistribution *posDist = fParticleGun->GetCurrentSource()->GetPosDist();
    posDist->SetPosDisType("Beam");  // or Point,Plane,Volume,Beam
    posDist->SetCentreCoords(G4ThreeVector(x0,y0,z0));
    posDist->SetBeamSigmaInX(0.1*um);
    posDist->SetBeamSigmaInY(0.1*um);
    // set angular distribution
    G4SPSAngDistribution *angDist = fParticleGun->GetCurrentSource()->GetAngDist();
    angDist->SetParticleMomentumDirection( G4ThreeVector(0., 0., -1.) );
    //angDist->SetAngDistType("beam2d");
    //angDist->SetBeamSigmaInAngX(0.0*mrad);
    //angDist->SetBeamSigmaInAngY(0.0*mrad);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

