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
// $Id: B4RunAction.cc 75215 2013-10-29 16:07:06Z gcosmo $
//
/// \file B4RunAction.cc
/// \brief Implementation of the B4RunAction class

#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction()
 : G4UserRunAction()
{ 
  // set printing event number per each event

    //这里是构造部分，是否就不能得到run的指针
//     G4double numOfEventtotal=  ->GetNumberOfEventToBeProcessed();
//     G4int numOfPrintProgess= numOfEventtotal/10;
//     G4cout<<"numOfPrintProgess"<<numOfPrintProgess<<"numOfEventtotal"<<numOfEventtotal<<G4endl;
//  G4RunManager::GetRunManager()->SetPrintProgress(5);



  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "G4AnalysisManager Using " << analysisManager->GetType() << G4endl;

  // Create directories 
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
//  analysisManager->SetFirstHistoId(1);

  // Book histograms, ntuple
  //
  
//  // Creating histograms
//  analysisManager->CreateH1("Edep","Edep in absorber", 100, 0., 800*MeV);
//   analysisManager->CreateH1("Leng","trackL in absorber", 100, 0., 1*m);

  // Creating ntuple
  //
  analysisManager->CreateNtuple("B4", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("EventID");
  analysisManager->CreateNtupleDColumn("PixelX");
  analysisManager->CreateNtupleDColumn("PixelY");
  analysisManager->CreateNtupleDColumn("EneDep");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
    G4int numOfEventtotal=  aRun->GetNumberOfEventToBeProcessed();
    G4int numOfPrintProgess= numOfEventtotal/10;
    G4RunManager::GetRunManager()->SetPrintProgress(numOfPrintProgess);

  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "B4";
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  //

//  if ( analysisManager->GetH1(1) ) {
//    G4cout << "\n ----> print histograms statistic ";
//    if(isMaster) {
//      G4cout << "for the entire run \n" << G4endl;
//    }
//    else {
//      G4cout << "for the local thread \n" << G4endl;
//    }
    
//    G4cout << " EAbs : mean = "
//       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
//       << " rms = "
//       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
//      }

  // save histograms & ntuple
  //
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
  analysisManager->CloseFile();
G4cout << " End! Enjoy! "<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
