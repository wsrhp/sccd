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
// $Id: B4dEventAction.cc 75604 2013-11-04 13:17:26Z gcosmo $
//
/// \file B4dEventAction.cc
/// \brief Implementation of the B4dEventAction class

#include "B4dEventAction.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dEventAction::B4dEventAction()
    : G4UserEventAction(),
    PixelEdepHCID(-1),eventID(-1)
{
    G4String pixelnamepre="pixel_";
    G4int i,j;
    for(i=0;i<nofpixelineachrow;i++)   //为像素点命名
    {
        for(j=0;j<nofpixelineachcolumn;j++)
        {
            pixelname[i][j]=pixelnamepre+num2str(i)+"_"+num2str(j);
        }
    }
    Yno=(G4int) (nofpixelineachcolumn-1)/2;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4dEventAction::~B4dEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4THitsMap<G4double>* //这句就是声明了一个形参为G4double类型的函数指针
//这里这样写，是函数声明，可以没有形参，只要有形参类型就可以！
B4dEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
//const 限定符，它把一个对象转换成一个常量，用const的函数称为常成员函数，教材P141
{
    //GetHC的返回值转换成G4THitMap<G4double>
    //static_cast 强制类型转换
    //static_cast < new_type > ( expression )
    G4THitsMap<G4double>* hitsCollection
            = static_cast<G4THitsMap<G4double>*>(
                event->GetHCofThisEvent()->GetHC(hcID));

    if ( ! hitsCollection ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << hcID;
        G4Exception("B4dEventAction::GetHitsCollection()",
                    "MyCode0003", FatalException, msg);
    }

    return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double B4dEventAction::GetSum(G4THitsMap<G4double>* hitsMap) const
{
    G4double sumValue = 0;
    std::map<G4int, G4double*>::iterator it;
    //STL模板库中的容器：映射(map) 由{键，值}对组成的集合，以某种作用于键对上的谓词排列 <map>
    for ( it = hitsMap->GetMap()->begin(); it != hitsMap->GetMap()->end(); it++) {
        sumValue += *(it->second);
    }
    return sumValue;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dEventAction::PrintEventStatistics(
        G4double absoEdep, G4double absoTrackLength) const
{
    // Print event statistics
    //
    G4cout
            << "   Absorber: total energy: "
            << std::setw(7) << G4BestUnit(absoEdep, "Energy")
            << "       total track length: "
            << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
            << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dEventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    PixelEdepHCID=-1;
    eventID=-1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4dEventAction::EndOfEventAction(const G4Event* event)
{  
    // get analysis manager
    // Get hist collections IDs
    if ( PixelEdepHCID == -1 && eventID==-1) {
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        eventID = event->GetEventID();
        G4int i,j;
        for(i=0;i<nofpixelineachrow;i++)   //每次循环都生成一个像素，并设置为灵敏探测器
        {
            for(j=0;j<nofpixelineachcolumn;j++)
            {
                PixelEdepHCID  = G4SDManager::GetSDMpointer()->GetCollectionID( pixelname[i][j]+"/Edep");

                G4THitsMap<G4double>* hitsCollection
                        = static_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(PixelEdepHCID));
                if(hitsCollection->GetMap()->empty()==0)
                {
                    G4double PixelEdep =0;
                    std::map<G4int, G4double*>::iterator it;
                    for ( it = hitsCollection->GetMap()->begin(); it != hitsCollection->GetMap()->end(); it++) {
                        PixelEdep += *(it->second);
                        //PixelEdep += *it;
                    }

                    analysisManager->FillNtupleDColumn(0,eventID);
                    analysisManager->FillNtupleDColumn(1,i);
                    analysisManager->FillNtupleDColumn(2, j-Yno);
                    analysisManager->FillNtupleDColumn(3,PixelEdep*1000);
                    analysisManager->AddNtupleRow();

                }
            }
        }
    }
    //print per event (modulo n)
     G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
        G4cout << "---> End of event: " << eventID << G4endl;
    }
}  
G4String B4dEventAction::num2str(G4int i)
{
    char strtemp[9];
    sprintf(strtemp,"%d",i);
    return strtemp;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
