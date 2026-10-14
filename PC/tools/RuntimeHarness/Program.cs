using System;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
class Program
{
 static int checks;
 static void Check(bool ok,string label) { if(!ok)throw new Exception(label); checks++; }
 static BindingFlags all = BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Static|BindingFlags.Instance;
 static object Get(object obj,string name) => obj.GetType().GetProperty(name,all).GetValue(obj);
 static void Field(object obj,string name,object value) => obj.GetType().GetField(name,all).SetValue(obj,value);
 static int Main(string[] args)
 {
  try {
   string game=args[0], pluginPath=Path.GetFullPath(args[1]);
   AppDomain.CurrentDomain.AssemblyResolve += (s,e)=>{
    string file=new AssemblyName(e.Name).Name+".dll";
    foreach(string dir in new[]{"Beat Saber_Data/Managed","Plugins","Libs","IPA/Libs"}) {
     string path=Path.Combine(game,dir,file);if(File.Exists(path))return Assembly.LoadFrom(path);
    } return null;
   };
   var harmonyAssembly=Assembly.LoadFrom(Path.Combine(game,"Libs/0Harmony.dll"));
   var harmonyType=harmonyAssembly.GetType("HarmonyLib.Harmony",true);
   var harmony=Activator.CreateInstance(harmonyType,new object[]{"RhythmSaber.PC.OfflineHarness"});
   var plugin=Assembly.LoadFrom(pluginPath);
   // CLR cannot prepare Unity ECall entry points. Exercise the pure managed
   // scoring getters with real Harmony; validate engine hooks with Cecil instead.
   foreach(string name in new[]{"RhythmSaber.TotalCutScoreHook","RhythmSaber.GoodCutScoreHook"}) {
    var processor=harmonyType.GetMethod("CreateClassProcessor",new[]{typeof(Type)}).Invoke(harmony,new object[]{plugin.GetType(name,true)});
    processor.GetType().GetMethod("Patch",Type.EmptyTypes).Invoke(processor,null);
    Check(true,"managed Harmony patch creation");
   }
   var main=Assembly.LoadFrom(Path.Combine(game,"Beat Saber_Data/Managed/Main.dll"));
   var beatmap=Assembly.LoadFrom(Path.Combine(game,"Beat Saber_Data/Managed/BeatmapCore.dll"));
   var models=Assembly.LoadFrom(Path.Combine(game,"Beat Saber_Data/Managed/DataModels.dll"));
   var gameplay=Assembly.LoadFrom(Path.Combine(game,"Beat Saber_Data/Managed/GameplayCore.dll"));
   Func<string,Type> gameType=(name)=>main.GetType(name)??beatmap.GetType(name)??models.GetType(name)??gameplay.GetType(name);
   var session=plugin.GetType("RhythmSaber.Session",true);
   var active=session.GetField("Active",all);active.SetValue(null,true);
   var records=session.GetField("results",all).GetValue(null);
   var add=records.GetType().GetMethod("Add");var clear=records.GetType().GetMethod("Clear");
   var scoring=plugin.GetType("RhythmSaber.TimingScoring",true);
   var judge=scoring.GetMethod("Judge");
   var noteType=gameType("NoteData");var enumType=noteType.GetNestedType("ScoringType",all);
   var infoType=gameType("NoteCutInfo");var bufferType=gameType("CutScoreBuffer");
   var elementType=gameType("GoodCutScoringElement");
   var scoreModel=gameType("ScoreModel");
   for(int type=1;type<=8;type++) {
    var note=FormatterServices.GetUninitializedObject(noteType);
    Field(note,"<scoringType>k__BackingField",Enum.ToObject(enumType,type));
    var info=Activator.CreateInstance(infoType);Field(info,"noteData",note);
    var definition=scoreModel.GetMethod("GetNoteScoreDefinition").Invoke(null,new[]{Enum.ToObject(enumType,type)});
    int max=(int)Get(definition,"maxCutScore");
    foreach(double delta in new[]{0.0,-30,30,-45,45,-70,70,-85,85,101}) {
     clear.Invoke(records,null);
     var j=judge.Invoke(null,new object[]{delta,new double[]{15,30,50,75,100},new[]{115,100,70,10,0},max,false});
     int expected=(int)j.GetType().GetField("Score").GetValue(j);
     add.Invoke(records,new[]{note,j});
     var buffer=FormatterServices.GetUninitializedObject(bufferType);
     Field(buffer,"_noteCutInfo",info);Field(buffer,"_noteScoreDefinition",definition);
     Field(buffer,"_beforeCutScore",70);Field(buffer,"_afterCutScore",30);Field(buffer,"_centerDistanceCutScore",15);
     var refreshArgs=new object[]{buffer,70,30,15};
     plugin.GetType("RhythmSaber.CutScoreHook").GetMethod("Prefix",all).Invoke(null,refreshArgs);
     Field(buffer,"_beforeCutScore",refreshArgs[1]);Field(buffer,"_afterCutScore",refreshArgs[2]);Field(buffer,"_centerDistanceCutScore",refreshArgs[3]);
     Check((int)Get(buffer,"beforeCutScore")==expected,"full before replacement");
     Check((int)Get(buffer,"afterCutScore")==0,"after cleared");
     Check((int)Get(buffer,"centerDistanceCutScore")==0,"center cleared");
     Check((int)Get(buffer,"cutScore")==expected,"fixed arc/chain contribution replaced");
     var element=FormatterServices.GetUninitializedObject(elementType);Field(element,"_cutScoreBuffer",buffer);
     Check((int)Get(element,"cutScore")==expected,"native scoring element timing total");
     active.SetValue(null,false);
     Field(buffer,"_beforeCutScore",70);Field(buffer,"_afterCutScore",30);Field(buffer,"_centerDistanceCutScore",15);
     int fixedScore=(int)definition.GetType().GetField("fixedCutScore").GetValue(definition);
     Check((int)Get(buffer,"cutScore")==115+fixedScore,"disabled native total restored");
     active.SetValue(null,true);
    }
   }
   active.SetValue(null,false);
   harmonyType.GetMethod("UnpatchSelf").Invoke(harmony,null);
   Console.WriteLine("PASS: "+checks+" real Harmony/native managed scoring checks (no Unity player).");
   return 0;
  }catch(Exception e){Console.Error.WriteLine(e);return 1;}
 }
}
