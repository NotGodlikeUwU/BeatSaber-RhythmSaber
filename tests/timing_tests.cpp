#include "TimingScoring.hpp"
#include "GuardPolicy.hpp"
#include "MapCompatibility.hpp"
#include "JudgementBeam.hpp"
#include "JudgementText.hpp"
#include "TrajectoryTiming.hpp"
#include "Defaults.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iterator>
using namespace RhythmSaber;
int checks{};
void Check(bool condition,const char* name) { ++checks; if(!condition) { std::cerr<<"FAILED: "<<name<<'\n'; std::exit(1); } }
bool Near(double a,double b,double eps=1e-7) { return std::abs(a-b)<eps; }
int main(int argc,char** argv) {
  if(argc==3 && std::string_view(argv[1])=="--map") {
    std::ifstream input(argv[2],std::ios::binary);
    if(!input) { std::cerr<<"Cannot read map\n";return 2; }
    const std::string data{std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
    const auto conflict=MapConflict(data);
    std::cout<<(conflict.empty()?"PASS: serialized custom map accepted":conflict)<<" | observedMotion="<<MapNeedsObservedTiming(data)<<'\n';
    return conflict.empty()?0:1;
  }
  Windows w;
  Check(Judge(0,w).score==115,"exact perfect");
  for(double sign:{-1.,1.}) {
    Check(Judge(sign*30,w).grade==Grade::Great && Judge(sign*30,w).score==100,"30ms earns GREAT 100");
    Check(Judge(sign*65,w).grade==Grade::Bad && Judge(sign*65,w).score==10,"stricter 65ms");
    Check(Judge(sign*85,w).grade==Grade::Poor && Judge(sign*85,w).score==0,"85ms earns zero");
    for(unsigned i=0;i<5;++i) {
      Check(static_cast<unsigned>(Judge(sign*w.ms[i],w).grade)==i,"inclusive boundary");
      Check(static_cast<unsigned>(Judge(sign*(w.ms[i]+.001),w).grade)==i+1,"boundary successor");
    }
    Check(Judge(sign*151,w).score==0,"outside poor");
  }
  Check(!Windows{{20,20,70,110,150}}.Valid(),"equal windows");
  Check(!Windows{{40,20,70,110,150}}.Valid(),"reversed windows");
  Check(!Windows{{0,40,70,110,150}}.Valid(),"zero window");
  Check(Judge(std::numeric_limits<double>::quiet_NaN(),w).score==0,"invalid measurement");
  Check(Judge(0,Windows{{20,20,70,110,150}}).score==0,"invalid config yields zero");
  // At 10m/s, a plane 0.5m ahead is crossed 50ms before note.time.
  auto crossing=PlaneCrossing(10,1,2,10,-10,.5);
  Check(crossing && Near(*crossing,9.95),"line expected time");
  Check(Near(*OffsetMs(9.95,*crossing,1,0),0),"line and judgement agree");
  Check(Near(*OffsetMs(9.92,*crossing,1,0),-30),"negative early");
  Check(Near(*OffsetMs(9.98,*crossing,1,0),30),"positive late");
  // Doubling jump distance AND duration must not change speed or plane timing.
  Check(Near(*PlaneCrossing(10,2,4,20,-20,.5),*crossing),"changed jump duration");
  Check(Near(*PlaneCrossing(10,.5,1,10,-10,.5),9.975),"faster NJS");
  Check(Near(*PlaneCrossing(10,1,2,11,-9,.5),10.05),"jump coordinates already contain spatial offsets");
  Check(Near(*OffsetMs(10.06,10,2,0),30),"speed normalized real milliseconds");
  Check(Near(*OffsetMs(10.03,10,1,30),0),"calibration subtracts lateness");
  Check(!PlaneCrossing(10,1,2,10,10,.5),"zero velocity");
  Check(!PlaneCrossing(10,1,2,10,-10,20),"plane outside jump");
  Check(!OffsetMs(10,10,0,0),"paused/zero speed");
  const auto farther=PlaneCrossing(10,1,2,10,-10,1.5);
  Check(farther && Near(*farther,9.85),"platform edge plane 150ms before note time");
  Check(Near(*OffsetMs(9.85,*farther,1,0),0) && Judge(*OffsetMs(9.85,*farther,1,0),w).score==115,"farther visible fence is the perfect plane");
  Check(Near(MigrateLineDistance(.5f,0),1.5),"old default moves to platform edge");
  Check(Near(MigrateLineDistance(.8f,0),.8,1e-6),"custom distance preserved");
  Check(Near(MigrateLineDistance(.5f,1),.5),"migration only once");
  Check(BeamVertex(0,3,2.8f,1.35f,.025f,0).x<-1.39f && BeamVertex(40,3,2.8f,1.35f,.025f,0).x>1.39f,"one beam spans outer note centers");
  Check(Near(BeamVertex(20,3,2.8f,1.35f,.025f,0).alpha*.75f,.75),"beam peak opacity 75 percent");
  Check(BeamVertex(20,0,2.8f,1.35f,.025f,0).alpha==0 && BeamVertex(20,6,2.8f,1.35f,.025f,0).alpha==0,"beam soft transparent edges");
  // No integrated time: BPM and long-track timestamps do not accumulate drift.
  for(int hz:{72,90,120}) for(double noteTime:{1.,60.,600.,3600.}) {
    const auto expected=*PlaneCrossing(noteTime,1,2,10,-10,.5);
    const auto detected=std::ceil(expected*hz)/hz;
    Check(*OffsetMs(detected,expected,1,0)>=-1e-6 && *OffsetMs(detected,expected,1,0)<=1000./hz+1e-6,"frame callback error bounded by frame");
  }
  Statistics stats;
  stats.Add(Judge(-30,w));stats.Add(Judge(30,w));stats.Add(Judge(0,w));stats.AddMiss();
  Check(Near(stats.Mean(),0),"mean sign");
  Check(Near(stats.MeanAbsolute(),20),"mean absolute");
  Check(Near(stats.PerfectPercent(),25),"perfect denominator includes misses");
  Check(stats.counts[1]==2 && stats.counts[5]==1,"counts");
  Check(!ModConflict("BeatLeader","0.9.0","libbl.so").empty(),"BeatLeader conflict");
  Check(!ModConflict("scoresaber","3.4.1","libscoresaber.so").empty(),"ScoreSaber streaming conflict");
  Check(!ModConflict("UnknownId","1","/mods/libscoresaber.so").empty(),"ScoreSaber path alias");
  Check(!ModConflict("UnknownId","1","/mods/libbl.so").empty(),"BeatLeader path alias");
  Check(ModConflict("metacore","1.6.2","").empty(),"MetaCore known version");
  Check(ModConflict("metacore","1.6.3","").empty(),"MetaCore compatible version");
  Check(!ModConflict("metacore","2.0.4","").empty(),"MetaCore other ABI");
  Check(ModConflict("SongCore","1.1.26","").empty(),"normal dependency compatible");
  Check(ModConflict("AutoSaber","1.0.0","libAutoSaber.so").empty(),"map generator allowed");
  Check(ModConflict("replay","0.5.2","libreplay.so").empty(),"installed replay allowed");
  Check(ModConflict("NoodleExtensions","1","libnoodleextensions.so").empty(),"installed noodle allowed");
  Check(ModConflict("MappingExtensions","1","libmappingextensions.so").empty(),"installed mapping allowed");
  Check(ModConflict("Cinema","1","libcinema.so").empty(),"Cinema allowed");
  Check(ModConflict("Vivify","1","libvivify.so").empty(),"Vivify allowed");
  Check(ModConflict("Chroma","1","libchroma.so").empty(),"Chroma allowed");
  Check(ModConflict("MultiplayerCore","1","libmultiplayercore.so").empty(),"installed multiplayer plugin allowed in solo");
  Check(!PlaybackConflict("replay").empty(),"active replay disabler");
  Check(!PlaybackConflict("Replay").empty(),"legacy replay disabler case");
  Check(PlaybackConflict("rhythmsaber").empty(),"own submission guard is not replay");
  Check(PlaybackConflict("MappingExtensions").empty(),"generic offline disabler is not replay");
  Check(MapConflict(R"({"_version":"2.0.0","_notes":[{"_time":1,"_lineIndex":0}]})").empty(),"ordinary v2 map");
  Check(MapConflict(R"({"version":"3.3.0","colorNotes":[{"b":1,"x":0,"customData":{"color":[1,0,0]}}]})").empty(),"decorative note color allowed");
  Check(MapConflict(R"({"version":"4.0.0","colorNotesData":[{"x":0,"y":1}]})").empty(),"ordinary v4 data");
  Check(MapNeedsObservedTiming(R"({"_notes":[{"_customData":{"_position":[0,0,1]}}]})"),"custom coordinates observe real positions");
  Check(MapNeedsObservedTiming(R"({"colorNotes":[{"customData":{"animation":{"offsetPosition":[0,0,1]}}}]})"),"note animation uses observed timing");
  Check(MapNeedsObservedTiming(R"({"colorNotes":[{"customData":{"track":"moving-notes"}}]})"),"note tracks use observed timing");
  Check(MapConflict(R"({"colorNotesData":[{"customData":{"noteJumpMovementSpeed":20}}]})").empty(),"constant per-note NJS uses real jump parameters");
  Check(MapConflict(R"({"_notes":[{"_customData":{"_noteJumpMovementSpeed":16,"_noteJumpStartBeatOffset":-0.25}}]})").empty(),"v2 per-note jump parameters");
  Check(MapConflict(R"({"colorNotes":[{"customData":{"noteJumpMovementSpeed":null,"noteJumpStartBeatOffset":0}}]})").empty(),"null per-note NJS uses default");
  Check(MapNeedsObservedTiming(R"({"colorNotes":[{"customData":{"noteJumpMovementSpeed":0}}]})"),"nonstandard speed observed");
  Check(MapNeedsObservedTiming(R"({"colorNotes":[{"customData":{"noteJumpMovementSpeed":"animated"}}]})"),"nonconstant speed observed");
  Check(MapConflict(R"({"_notes":[{"_customData":{"_fake":true}}]})").empty(),"fake rules no longer disable entire map");
  Check(MapConflict(R"({"_notes":[{"_customData":{"_fake":false,"_interactable":true}}]})").empty(),"default cut flags allowed");
  Check(MapNeedsObservedTiming(R"({"customData":{"customEvents":[{"t":"AssignPlayerToTrack","d":{"track":"player"}}]}})"),"player-space animation observed");
  Check(MapConflict(R"({"customData":{"customEvents":[{"t":"AnimateTrack","d":{"track":"lights"}}]},"colorNotes":[]})").empty(),"environment-only tracks allowed");
  Check(!MapConflict("bad json").empty(),"invalid custom data refuses activation");
  Check(MapConflict("\xEF\xBB\xBF{\"_version\":\"2.0.0\",\"_notes\":[]}").empty(),"editor UTF8 BOM accepted");
  Check(MapConflict(R"({/* editor comment */ "colorNotes":[{"b":1,}],})").empty(),"ordinary editor JSON comments and trailing commas");
  Check(MapConflict(R"({"_notes":[{"_customData":{"_animation":{"_color":[[1,0,0,1,0]]}}}]})").empty(),"animated Chroma colors do not disable custom songs");
  Check(MapConflict(R"({"colorNotes":[{"customData":{"animation":{"color":"red"}}}]})").empty(),"v3 decorative animation");
  Check(MapNeedsObservedTiming(R"({"colorNotes":[{"customData":{"animation":{"time":[[0,0],[1,1]]}}}]})"),"animated time uses observed plane");
  for(int type=1;type<=8;++type) Check(TimingScoredType(type),"all native scored note types supported");
  Check(!TimingScoredType(0) && !TimingScoredType(-1),"NoScore/Ignore objects do not receive points");
  Check(Judge(0,w,85).score==85 && Judge(0,w,20).score==20,"special notes retain native max");
  Check(Judge(30,w,85).score==74 && Judge(30,w,20).score==17,"special note timing proportional");
  Check(Judge(85,w,85).score==0 && Judge(101,w,20).score==0,"no fixed bonus survives poor timing");
  Check(Judge(20,w).score==100 && Judge(40,w).score==70,"strict windows with requested GREAT/GOOD points");
  Check(Judge(40,w).grade==Grade::Good && Judge(40,w).score==70,"GOOD 70 default");
  const PointTable custom{{110,95,65,20,5}};
  Check(custom.Valid() && Judge(0,w,115,custom).score==110,"custom PERFECT points reach real score");
  Check(Judge(-25,w,115,custom).score==95 && Judge(25,w,115,custom).score==95,"custom GREAT symmetric");
  Check(Judge(45,w,115,custom).score==65 && Judge(65,w,115,custom).score==20 && Judge(85,w,115,custom).score==5,"custom GOOD BAD POOR points");
  Check(Judge(101,w,115,custom).score==0,"MISS remains zero independently of points");
  Check(Judge(25,w,20,custom).score==17,"custom points respect chain link maximum");
  Check(!PointTable{{116,100,70,10,0}}.Valid(),"115 hard cap");
  Check(!PointTable{{115,100,70,10,-1}}.Valid(),"negative points rejected");
  Check(!PointTable{{100,115,70,10,0}}.Valid(),"worse grade cannot earn more");
  Check(PointTable{{115,115,70,0,0}}.Valid(),"equal grade points allowed");
  Check(Judge(0,w,115,PointTable{{116,100,70,10,0}}).score==0,"invalid curve cannot award oversized score");
  Check(Defaults::Enabled && !Defaults::ShowStatistics,"screenshot enabled and statistics off defaults");
  Check(Near(Defaults::Distance,1.35,1e-6) && Near(Defaults::Height,.8,1e-6) && Near(Defaults::Thickness,.015,1e-6),"screenshot geometry defaults");
  Check(Near(Defaults::Width,2.8,1e-6) && Near(Defaults::TextSize,1.6,1e-6) && Near(Defaults::Opacity,.75),"screenshot width text size opacity defaults");
  Check(Near(Defaults::TextScale(1.6f),.96,1e-6) && Defaults::TextScale(.5f)<Defaults::TextScale(1.6f),"smaller text at existing 1.6 setting and adjustable size");
  Check(Near(Defaults::ReplaceOldDefault(1.5f,1.5f,1.35f),1.35,1e-6),"old standard distance migrates");
  Check(Near(Defaults::ReplaceOldDefault(1.1f,1.5f,1.35f),1.1,1e-6),"custom geometry preserved");
  Check(Judge(0,w,0).score==0 && Judge(0,w,116).score==0,"unknown maxima fail to zero");
  TrajectoryTiming linear;
  linear.Sample(1,.2);linear.Sample(1.01,.1);
  auto early=linear.Expected(1.01,.5);
  Check(early && early->estimated && Near(early->time,1.02),"early moving note predicts upcoming crossing");
  TrajectoryTiming scaled;scaled.Sample(1,.1);scaled.Sample(1.01,.05);
  Check(scaled.Expected(1.01,.5) && Near(scaled.Expected(1.01,.5)->time,early->time),"constant player-space scale preserves crossing time");
  linear.Sample(1.03,-.1);
  auto confirmed=linear.Expected(1.03,.5);
  Check(confirmed && !confirmed->estimated && Near(confirmed->time,1.02),"observed crossing interpolates between frames");
  Check(Near(*OffsetMs(1.03,confirmed->time,1,0),10),"observed lateness positive");
  linear.Sample(1.1,-.15);linear.Sample(1.11,-.05);
  auto returning=linear.Expected(1.11,.5);
  Check(returning && returning->estimated && Near(returning->time,1.115),"oscillating note chooses nearest upcoming crossing");
  linear.Sample(1.12,.05);
  Check(linear.Expected(1.12,.5) && !linear.Expected(1.12,.5)->estimated && Near(linear.Expected(1.12,.5)->time,1.115),"second real crossing replaces older passage");
  TrajectoryTiming reverse;reverse.Sample(1,-.2);reverse.Sample(1.01,-.1);
  Check(reverse.Expected(1.01,.5) && Near(reverse.Expected(1.01,.5)->time,1.02),"reverse-direction approach measured");
  TrajectoryTiming frozen;frozen.Sample(1,1);frozen.Sample(1.01,1);
  Check(!frozen.Expected(1.01,.5),"stationary distant note cannot invent perfect time");
  TrajectoryTiming teleported;teleported.Sample(1,100);teleported.Sample(1.01,-100);
  Check(!teleported.Expected(1.01,.5),"large teleport does not prove crossing");
  linear.Sample(.5,1);
  Check(!linear.Expected(.5,.5),"restart resets motion history");
  linear.Sample(1.5,-1);
  Check(!linear.Expected(1.5,.5),"long sample gap drops old crossing");
  Check(!MapNeedsObservedTiming(R"({"colorNotes":[{"customData":{"color":[1,0,0]}}]})"),"Chroma colors retain analytic timing");
  Check(MapConflict(R"({"colorNotes":[],"sliders":[{}],"burstSliders":[{}]})").empty(),"Arc/Chain map no blanket rejection");
  char display[256]; auto judgement=Judge(8,w); FormatJudgement(display,sizeof(display),judgement,true);
  Check(std::string_view(display).find("PERFECT")!=std::string_view::npos && std::string_view(display).find("115")!=std::string_view::npos && std::string_view(display).find("+8 ms")!=std::string_view::npos,"grade score and signed milliseconds together");
  judgement.estimated=true;FormatJudgement(display,sizeof(display),judgement,true);
  Check(std::string_view(display).find("~+8 ms")!=std::string_view::npos,"predicted timing explicitly marked");
  judgement=Judge(-30,w);FormatJudgement(display,sizeof(display),judgement,false);
  Check(std::string_view(display).find("EARLY")!=std::string_view::npos,"early label display mode");
  Statistics quality;judgement.estimated=true;quality.Add(judgement);quality.Add({Grade::Miss,0,NAN});
  Check(quality.estimatedCuts==1 && quality.unmeasurableCuts==1,"measurement quality recorded separately");
  GuardState guard;
  Check(!guard.Disabled(),"off native submit");guard.requested=true;
  Check(guard.Disabled(),"enabled before level");guard.held=true;guard.requested=false;
  Check(guard.Disabled(),"off during changed run keeps guard");
  Check(guard.Disabled(),"results keep guard");guard.NewLevel();
  Check(!guard.Disabled(),"next untouched level releases own guard");
  for(float progress:{0.f,.1f,.5f,.75f}) Check(Near(Defaults::JudgementOpacity(progress),1),"judgement opacity held at full");
  Check(Near(Defaults::JudgementOpacity(.875f),.5),"last quarter fades smoothly");
  Check(Near(Defaults::JudgementOpacity(1.f),0),"end of effect invisible");
  Check(Near(Defaults::JudgementOpacity(2.f),0),"late progress stays invisible");
  Check(Near(Defaults::FailureFontSize(1.6f),2.16,1e-6),"failure feedback half-sized");
  std::cout<<"PASS: "<<checks<<" timing, guard and feedback checks\n";
}
