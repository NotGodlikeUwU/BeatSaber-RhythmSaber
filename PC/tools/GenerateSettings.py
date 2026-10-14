from pathlib import Path
import re
root = Path(__file__).resolve().parents[1]
properties = re.findall(r'public virtual (bool|float|int) (\w+) \{', (root/'src/PluginConfig.cs').read_text())
labels = {
 'Enabled':'Enable RhythmSaber','ShowLine':'Show Judgement Line','ShowJudgements':'Show Timing Judgements',
 'ShowOffset':'Show milliseconds (off: EARLY/LATE)','ShowStatistics':'Show Timing Statistics',
 'TextSize':'Flying judgement size','LineDistance':'Beam distance (m)','LineHeight':'Beam height (m)',
 'LineWidth':'Beam width (m)','LineThickness':'Line thickness (m)','LineBrightness':'Line brightness',
 'LineOpacity':'Line opacity','TimingOffset':'Timing offset (ms)','Diagnostics':'Diagnostic logging'}
ranges = {'LineDistance':(-.5,5,.05),'LineHeight':(.1,3,.05),'LineWidth':(.5,10,.1), 'TextSize':(.25,3,.05),
 'LineThickness':(.001,.1,.001),'LineBrightness':(0,3,.1),'LineOpacity':(0,1,.05),'TimingOffset':(-500,500,1)}
cs = ['using BeatSaberMarkupLanguage.Attributes;','namespace RhythmSaber {','public class SettingsView {',
 '[UIValue("status")] public string Status { get { if (Plugin.Failure.Length > 0) return Plugin.Failure; if (!Plugin.Config.Valid()) return "Invalid settings: windows must increase; points must not increase."; var reason = ScoreSubmissionGuard.Check(); return reason.Length > 0 ? reason : "Timing scoring is local only. Changes apply next song."; } }',
 '[UIComponent("status-label")] private TMPro.TextMeshProUGUI statusLabel = null;',
 '[UIAction("#post-parse")] public void RefreshStatus() { if (statusLabel != null) statusLabel.text = Status; }']
for typ, name in properties:
 cs += [f'[UIValue("{name}")] public {typ} {name} {{ get => Plugin.Config.{name}; set {{ Plugin.Config.{name} = value; Plugin.Config.Changed(); RefreshStatus(); }} }}']
cs += ['[UIAction("reset")] public void Reset() { Plugin.Config.CopyFrom(new PluginConfig()); Plugin.Config.Changed(); RefreshStatus(); }','}}']
(root/'src/SettingsView.cs').write_text('\n'.join(cs)+'\n')
groups = [ ('General', [n for _,n in properties if n in ['Enabled','ShowLine','ShowJudgements','ShowOffset','ShowStatistics','TextSize','Diagnostics']]),
 ('Beam', [n for _,n in properties if n.startswith('Line')]),
 ('Timing', [n for _,n in properties if n=='TimingOffset' or n.endswith('Window')]),
 ('Points', [n for _,n in properties if n.endswith('Points')]) ]
# BSML 1.12.5 TabTag has no LayoutElement. A parent VerticalLayoutGroup
# therefore collapses its height; pref-height on <tab> has no handler target.
# Match BSML's own tab examples: stretch the pages in a dedicated background
# rect, and anchor the selector above them. No layout group sizes these rects.
frame = 'horizontal-fit="Unconstrained" vertical-fit="Unconstrained"'
xml = [f'<bg id="settings-root" anchor-min="0 0" anchor-max="1 1" size-delta="0 0" anchored-position="0 0" {frame}>',
 '<text id="status-label" text="" font-size="3" word-wrapping="true" anchor-min="0 1" anchor-max="1 1" pivot="0.5 1" anchored-position="0 0" size-delta="0 5"/>',
 f'<tab-selector id="settings-tabs" tab-tag="tabs" anchor-min="0 1" anchor-max="1 1" pivot="0.5 1" anchored-position="0 -6" size-delta="0 7" child-expand-width="true" child-control-width="true" {frame}/>',
 f'<bg id="settings-pages" anchor-min="0 0" anchor-max="1 1" size-delta="0 -22" anchored-position="0 -3" {frame}>']
types = dict((n,t) for t,n in properties)
for group,names in groups:
 xml += [f'<tab tags="tabs" tab-name="{group}" anchor-min="0 0" anchor-max="1 1" size-delta="0 0" anchored-position="0 0" {frame}>','<scrollable-settings-container>']
 for name in names:
  label = labels.get(name,re.sub(r'(?<!^)([A-Z])',r' \1',name))
  if name.endswith('Window'): label += ' (ms)'
  if types[name]=='bool': xml += [f'<bool-setting text="{label}" value="{name}" apply-on-change="true"/>']
  else:
   lo,hi,step = ranges.get(name, (0,115,1) if name.endswith('Points') else (1,250,1) if name.endswith('Window') else (0,1,.05))
   xml += [f'<increment-setting text="{label}" value="{name}" min="{lo}" max="{hi}" increment="{step}" integer-only="{str(types[name]=="int").lower()}" apply-on-change="true"/>']
 xml += ['</scrollable-settings-container>','</tab>']
xml += ['</bg>', f'<button text="Reset defaults (reopen menu to refresh)" on-click="reset" anchor-min="0 0" anchor-max="1 0" pivot="0.5 0" anchored-position="0 1" size-delta="0 6" {frame}/>', '</bg>']
(root/'UI').mkdir(exist_ok=True)
(root/'UI/settings.bsml').write_text('\n'.join(xml)+'\n',encoding='utf-8')
