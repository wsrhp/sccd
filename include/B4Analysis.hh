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
// $Id: B4Analysis.hh 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file B4Analysis.hh
/// \brief Selection of the analysis technology

#ifndef B4Analysis_h
#define B4Analysis_h 1

//#include "g4root.hh"
//#include "g4xml.hh"
#include "g4csv.hh"
#include "globals.hh"
#include "G4Material.hh"

#include "G4Step.hh"
#include "G4Run.hh"
#include "G4THitsMap.hh"
#include <time.h>
class G4Material;
class G4Run;
class G4Event;
class G4ParticleDefinition;
class Analysis
{
public:
    //好方法，通过定义一个静态的analysis对象，让整个程序的分析都在这个对象的调用下实现！
    //妈妈再也不用担心局部变量的问题啦！
    static Analysis* GetInstance();
    ~Analysis();
    Analysis();
    void BeginOfEvent(const G4Event* anEvent);
    void EndOfEvent(const G4Event* anEvent);
    void BeginOfRun(const G4Run* aRun);
    void EndOfRun(const G4Run* aRun);
    void ReportRunTime(G4int npercent);


private:
    //注意！！！！将分析类的构造函数定义为私有，就不能再在类外构造对象了
    // 只能在类内部构造对象。这就是singleton的设计模式
//    Analysis();
    static Analysis* singleton;

    // methods
    G4THitsMap<G4double>* GetHitsCollection(G4int hcID,
                                            const G4Event* event) const;
    G4double GetSum(G4THitsMap<G4double>* hitsMap) const;
    void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength) const;
    G4String num2str(G4int i);

    // data members
    G4int  PixelEdepHCID;
    static const  G4int  nofpixelineachcolumn=11;
    static const  G4int  nofpixelineachrow=11;
    G4String pixelname[nofpixelineachrow][nofpixelineachcolumn];
    G4int Yno;
    G4int eventID;


    time_t timestart;
    struct tm tmstart;
    char timebuf[80];

    G4int numOfEventnow;
    G4int numOfEventtotal;
    G4int percentOfEvent;//目前运行了多少百分比的event，以%为单位


};


#endif



