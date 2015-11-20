//// Make this appear first!说明中要求把G4Timer.hh放在头文件列表中的第一个位置
#include "G4Timer.hh"

#include "B4Analysis.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"
#include "G4HCofThisEvent.hh"
#include "G4ProductionCuts.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include <iomanip>


Analysis* Analysis::singleton = 0;
//控制Analysis起作用的一个静态变量
//Analysis是一个单例类

Analysis* Analysis::GetInstance()
{
    //GetInstance被其他函数调用时就实际是调用一个Analysis的对象analysis.
    if ( singleton == 0 )
    {
        static Analysis analysis;
        singleton = &analysis;
    }
    return singleton;
}

Analysis::Analysis()
{
    //一定注意啦！要对各个成员数据进行初始化！！！！

    percentOfEvent=1;//必须将其初始化为1
    numOfEventnow=0;
    numOfEventtotal=0;
}

Analysis::~Analysis()
{}


void Analysis::BeginOfRun(const G4Run* aRun )
{

    timestart = time(NULL); //获取run开始时间
   //struct tm* Tmstart = localtime(&timestart);
   //tmstart = *Tmstart;
    tmstart = *(localtime(&timestart));
    strftime(timebuf,80,"%Y-%m-%d %H:%M:%S",&tmstart);  //将时间以2013-08-16 21:59:23的形式输出
    G4cout<<"Starting Run at   " <<timebuf<<G4endl;
    G4cout<<"Starting Run: "<<aRun->GetRunID()<<G4endl;
    G4cout<<"The Number of Event to be processed is :  "<<aRun->GetNumberOfEventToBeProcessed()<<G4endl;
    numOfEventtotal=aRun->GetNumberOfEventToBeProcessed();
    if(numOfEventtotal == 0) { return; }



    //创建保存数据的CSV文件
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    G4cout << "G4AnalysisManager Using " << analysisManager->GetType() << G4endl;
    analysisManager->SetVerboseLevel(1);
    analysisManager->CreateNtuple("B4", "Edep and TrackL");
    analysisManager->CreateNtupleDColumn("EventID");
    analysisManager->CreateNtupleDColumn("PixelX");
    analysisManager->CreateNtupleDColumn("PixelY");
    analysisManager->CreateNtupleDColumn("EneDep");
    analysisManager->FinishNtuple();
    // Open an output file
    //
    G4String fileName = "B4";
    analysisManager->OpenFile(fileName);

}
void Analysis::EndOfRun(const G4Run* /*aRun*/)
{
    // Writing and closing the ROOT file
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
    delete analysisManager;

    time_t timeend;
    struct tm* tmend;
    timeend = time(NULL); //获取run开始时间
    tmend=localtime(&timeend);
    strftime(timebuf,80,"%Y-%m-%d %H:%M:%S",tmend);  //将时间以2013-08-16 21:59:23的形式输出
    G4cout<<" Run end at   " <<timebuf<<G4endl;
}

void Analysis::BeginOfEvent(const G4Event* )
{}

void Analysis::EndOfEvent(const G4Event* )
{
    //print per event (modulo n)
     G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
        G4cout << "---> End of event: " << eventID << G4endl;
    }
    numOfEventnow+=1;
    //输出运行时间
    if (numOfEventnow==numOfEventtotal*percentOfEvent/100)
    {
        if(percentOfEvent==1)
        {
            ReportRunTime(percentOfEvent);//第一次输出运行时间
            percentOfEvent=10;
        }
        else
        {
            ReportRunTime(percentOfEvent);//输出运行时间及剩余时间

            percentOfEvent+=20;
        }
    }

}

G4String Analysis::num2str(G4int i)
{
    char strtemp[9];
    sprintf(strtemp,"%d",i);
    return strtemp;
}

void Analysis::ReportRunTime(G4int percent)
{
    //利用C++中的时间函数得到程序运行至某percent of events 的时间和剩余时间
    if(percent<=0 &&percent>=100)
    {
        G4cout<<"Error in Analysis::ReportRunTime(): illegal input"<<G4endl;
        return;
    }
    G4cout<<"Geant4 has processed "<<percent<<" % events."<<G4endl;
    G4double timecost,timeneed;
    G4double npercent=(G4double) percent;
    G4int dayneed,hourneed,minneed,secneed;

    time_t timenow;
    struct tm *tmnow;
    timenow = time(NULL); //获取当前日历时间
    tmnow=localtime(&timenow);
    strftime(timebuf,80,"%Y-%m-%d %H:%M:%S",&tmstart);  //将时间以2013-08-16 21:59:23的形式输出
    G4cout<<"Starting Run at   " <<timebuf<<G4endl;
    strftime(timebuf,80,"%Y-%m-%d %H:%M:%S",tmnow);  //将时间以2013-08-16 21:59:23的形式输出
    G4cout<<"Time Now is  " <<timebuf<<G4endl;
    //求出时间差。
    timecost=difftime(timenow,timestart);
    timeneed=(100-npercent)/npercent*timecost;
    dayneed=(G4int)(timeneed/(24*3600));
    hourneed=(G4int)((timeneed-dayneed*(24*3600))/3600);
    minneed=(G4int)((timeneed-dayneed*(24*3600)-hourneed*3600)/60);
    secneed=(G4int)(timeneed-dayneed*(24*3600)-hourneed*3600-minneed*60);
    G4cout<<"Simulation need more:  "<<dayneed<<" day "<<hourneed<<" hour "
         <<minneed<<" min "<<secneed<<" sec "<<G4endl;
    //预计结束时间
    int ttemp=0;
    ttemp= tmnow->tm_sec+secneed;
    if(ttemp>60){
        minneed+=1;
        tmnow->tm_sec=ttemp-60;
    }
    else{
        tmnow->tm_sec+=secneed;
    }
    ttemp=  tmnow->tm_min+minneed;
    if(ttemp>60){
        hourneed+=1;
        tmnow->tm_min=ttemp-60;
    }
    else{
        tmnow->tm_min+=minneed;
    }
    ttemp=   tmnow->tm_hour+hourneed;
    if(ttemp>24){
        dayneed+=1;
        tmnow->tm_hour=ttemp-60;
    }
    else{
        tmnow->tm_hour+=hourneed;
    }
    tmnow->tm_mday+=dayneed;
    strftime(timebuf,80,"%Y-%m-%d %H:%M:%S",tmnow);
    G4cout<<"Simulation should end at "<<timebuf<<G4endl;
}

