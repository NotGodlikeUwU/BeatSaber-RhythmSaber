using System;
using RhythmSaber;
class Program
{
 static int tests;
 static void Check(bool ok,string name) { tests++; if(!ok) throw new Exception(name); }
 static void Near(double actual,double expected,string name) => Check(Math.Abs(actual-expected)<1e-5,name);
 static void Main()
 {
  var c=new PluginConfig(); var w=c.Windows();var p=c.Points();
  Check(c.Valid(),"defaults valid"); Near(c.LineDistance,1.35,"distance");Near(c.LineHeight,.8,"height");Near(c.LineWidth,2.8,"width");Near(c.LineOpacity,.75,"opacity");Near(c.TextSize,1.6,"size");
  var snapshot=c.Snapshot();c.GoodPoints=60;Check(snapshot.GoodPoints==70,"frozen session"); c=new PluginConfig();
  Check(TimingScoring.Judge(0,w,p,115).Score==115,"perfect");
  foreach(int sign in new[]{-1,1}) {
   Check(TimingScoring.Judge(sign*30,w,p,115).Grade==Grade.Great,"30ms great");
   Check(TimingScoring.Judge(sign*30,w,p,115).Score==100,"great points");
   Check(TimingScoring.Judge(sign*45,w,p,115).Score==70,"good points");
   Check(TimingScoring.Judge(sign*70,w,p,115).Grade==Grade.Bad,"strict bad");
   Check(TimingScoring.Judge(sign*85,w,p,115).Grade==Grade.Poor,"strict poor");
   Check(TimingScoring.Judge(sign*101,w,p,115).Grade==Grade.Miss,"strict miss");
   for(int i=0;i<5;i++) {
    Check((int)TimingScoring.Judge(sign*w[i],w,p,115).Grade==i,"inclusive boundary");
    Check((int)TimingScoring.Judge(sign*(w[i]+.001),w,p,115).Grade==i+1,"outside boundary");
   }
  }
  for(int max=1;max<=115;max++) {
   Check(TimingScoring.Judge(0,w,p,max).Score==max,"arc/chain maximum");
   Check(TimingScoring.Judge(30,w,p,max).Score<=max,"scaled great <= max");
   Check(TimingScoring.Judge(45,w,p,max).Score==(int)Math.Round(70*max/115.0,MidpointRounding.AwayFromZero),"scaled good");
  }
  p[2]=116;Check(!TimingScoring.Valid(w,p),"invalid points");p=c.Points();w[2]=w[1];Check(!TimingScoring.Valid(w,p),"equal windows");w=c.Windows();w[0]=double.NaN;Check(!TimingScoring.Valid(w,p),"nan windows");w=c.Windows();
  Check(TimingScoring.Judge(double.NaN,w,p,115).Score==0,"invalid delta");Check(TimingScoring.Judge(0,w,p,116).Score==0,"unsupported max");
  Near(TimingScoring.Offset(10.03,10,1,0),30,"late");Near(TimingScoring.Offset(9.97,10,1,0),-30,"early");
  Near(TimingScoring.Offset(10.03,10,1.5,0),20,"speed normalization");Near(TimingScoring.Offset(10.03,10,1,10),20,"calibration");
  Check(double.IsNaN(TimingScoring.Offset(1,1,0,0)),"pause invalid speed");
  Near(TimingScoring.Crossing(10,1,2,10,-10,1.35),9.865,"line geometry");
  Near(TimingScoring.Crossing(10,2,4,20,-20,1.35),9.865,"jump distance invariance at fixed speed");
  Check(double.IsNaN(TimingScoring.Crossing(10,1,2,1,1,0)),"no z motion");
  foreach(double hz in new[]{72.0,90,120}) {
   double dt=1/hz;var t=new TrajectoryTiming();t.Sample(10-dt,dt*10);t.Sample(10+dt,-dt*10);
   Check(t.Expected(10+dt,.5,out var time,out var est),"observed crossing");Near(time,10,"crossing interpolation");Check(!est,"confirmed");
   t=new TrajectoryTiming();t.Sample(10-dt,1+dt*10);t.Sample(10,1);
   Check(t.Expected(10,.5,out time,out est),"future crossing");Near(time,10.1,"prediction");Check(est,"prediction flagged");
  }
  var trajectory=new TrajectoryTiming();trajectory.Sample(1,1);trajectory.Sample(1.01,-5);Check(!trajectory.Expected(1.01,.5,out _,out _),"teleport not crossing");
  trajectory=new TrajectoryTiming();trajectory.Sample(1,.1);trajectory.Sample(1.5,-.1);Check(!trajectory.Expected(1.5,.5,out _,out _),"long gap reset");
  trajectory=new TrajectoryTiming();trajectory.Sample(10,1);trajectory.Sample(10.01,.9);trajectory.Sample(5,1);Check(!trajectory.Expected(5,.5,out _,out _),"seek reset");
  trajectory=new TrajectoryTiming();trajectory.Sample(10,1);trajectory.Sample(10.01,.9);trajectory.Sample(10.01,.9);Check(trajectory.Expected(10.01,.5,out _,out _),"duplicate clock retains velocity");
  trajectory=new TrajectoryTiming();trajectory.Sample(10,1);trajectory.Sample(10.01,.9);Check(!trajectory.Expected(10.1,.5,out _,out _),"stale sample");
  foreach(float t in new[]{0f,.1f,.5f,.75f})Near(FeedbackPresentation.JudgementOpacity(t),1,"judgement fully readable during hold");
  Near(FeedbackPresentation.JudgementOpacity(.875f),.5,"final smooth fade");
  Near(FeedbackPresentation.JudgementOpacity(1f),0,"effect finishes invisible");
  Near(FeedbackPresentation.JudgementOpacity(2f),0,"late callback remains invisible");
  Near(FeedbackPresentation.FailureFontSize(1.6f),2.16,"bad cut/miss half-size");
  Console.WriteLine($"PASS: {tests} timing/config/trajectory/feedback assertions.");
 }
}
