using System;
using System.Collections.Generic;
using System.Globalization;
using HarmonyLib;
using UnityEngine;
using TMPro;
using Zenject;

namespace RhythmSaber
{
    internal static class Visuals
    {
        public static Transform Plane => root != null ? root.transform : null;
        static GameObject root;
        static Mesh mesh;
        static Material beamMaterial;
        static Vector3[] vertices;
        static TextMeshPro banner, statistics;
        static FlyingTextSpawner failures;
        static float nextBeamTime;
        static int revision;
        static readonly Dictionary<FlyingScoreEffect, Style> styles = new Dictionary<FlyingScoreEffect, Style>(128);
        static readonly AccessTools.FieldRef<PlayerTransforms, Transform> origin = AccessTools.FieldRefAccess<PlayerTransforms, Transform>("_originParentTransform");
        static readonly AccessTools.FieldRef<FlyingScoreEffect, TextMeshPro> textField = AccessTools.FieldRefAccess<FlyingScoreEffect, TextMeshPro>("_text");
        static readonly AccessTools.FieldRef<FlyingScoreEffect, IReadonlyCutScoreBuffer> bufferField = AccessTools.FieldRefAccess<FlyingScoreEffect, IReadonlyCutScoreBuffer>("_cutScoreBuffer");
        static readonly AccessTools.FieldRef<FlyingScoreEffect, SpriteRenderer> indicatorField = AccessTools.FieldRefAccess<FlyingScoreEffect, SpriteRenderer>("_maxCutDistanceScoreIndicator");
        static readonly AccessTools.FieldRef<FlyingScoreEffect, Color> colorField = AccessTools.FieldRefAccess<FlyingScoreEffect, Color>("_color");
        static readonly AccessTools.FieldRef<FlyingScoreEffect, float> alphaField = AccessTools.FieldRefAccess<FlyingScoreEffect, float>("_colorAMultiplier");
        sealed class Style
        {
            public TextMeshPro Text;
            public float Size, Alpha, Progress;
            public bool Rich, Wrap, Indicator;
            public TextAlignmentOptions Alignment;
            public TextOverflowModes Overflow;
            public Color Color;
            public Judgement Judgement;
            public string Label;
        }
        static Color GradeColor(Grade grade)
        {
            switch(grade)
            {
                case Grade.Perfect: return new Color(1, .9f, .35f);
                case Grade.Great: return new Color(.2f, 1, .35f);
                case Grade.Good: return new Color(.2f, .85f, 1);
                case Grade.Bad: return new Color(1, .5f, .15f);
                default: return new Color(1, .2f, .2f);
            }
        }
        static string Label(Judgement j)
        {
            string offset = !TimingScoring.Finite(j.Delta) ? "NO CROSSING" : Session.Settings.ShowOffset ?
                (j.Estimated ? "~" : "") + j.Delta.ToString("+0;-0;0", CultureInfo.InvariantCulture) + " ms" :
                (j.Estimated ? "~" : "") + (j.Delta < -.5 ? "EARLY" : j.Delta > .5 ? "LATE" : "ON TIME");
            return "<b><size=110%>" + j.Grade.ToString().ToUpperInvariant() + "</size></b>\n<size=85%>" + j.Score +
                "</size>\n<size=55%>" + offset + "</size>";
        }
        public static void Attach(FlyingScoreEffect effect)
        {
            if (!Session.Active || !Session.Settings.ShowJudgements) return;
            var buffer = bufferField(effect);
            if (buffer == null || !Session.Find(buffer.noteCutInfo.noteData, out var judgement)) return;
            var text = textField(effect);
            var indicator = indicatorField(effect);
            styles[effect] = new Style { Text = text, Size = text.fontSize, Rich = text.richText,
                Wrap = text.enableWordWrapping, Alignment = text.alignment, Overflow = text.overflowMode,
                Indicator = indicator != null && indicator.enabled, Color = colorField(effect), Alpha = alphaField(effect),
                Judgement = judgement, Label = Label(judgement) };
            text.fontSize *= .6f * Session.Settings.TextSize * (judgement.Grade == Grade.Miss ? .5f : 1f);
            text.richText = true; text.enableWordWrapping = false;
            text.alignment = TextAlignmentOptions.Center; text.overflowMode = TextOverflowModes.Overflow;
            colorField(effect) = GradeColor(judgement.Grade); alphaField(effect) = 1;
            if (indicator != null) indicator.enabled = false;
            Refresh(effect);
        }
        public static void Refresh(FlyingScoreEffect effect)
        {
            if (!styles.TryGetValue(effect, out var style) || style.Text == null) return;
            style.Text.text = style.Label;
            Color c = GradeColor(style.Judgement.Grade);
            colorField(effect) = c; alphaField(effect) = 1;
            c.a = FeedbackPresentation.JudgementOpacity(style.Progress); style.Text.color = c;
        }
        public static void Update(FlyingScoreEffect effect, float progress)
        {
            if(styles.TryGetValue(effect, out var style)) style.Progress = progress;
            Refresh(effect);
        }
        public static void Restore(FlyingScoreEffect effect)
        {
            if (!styles.TryGetValue(effect, out var style)) return;
            if (style.Text != null)
            {
                style.Text.fontSize = style.Size; style.Text.richText = style.Rich;
                style.Text.enableWordWrapping = style.Wrap; style.Text.alignment = style.Alignment;
                style.Text.overflowMode = style.Overflow;
            }
            if (effect != null)
            {
                colorField(effect) = style.Color; alphaField(effect) = style.Alpha;
                var indicator = indicatorField(effect); if(indicator != null) indicator.enabled = style.Indicator;
            }
            styles.Remove(effect);
        }
        public static void Create(PlayerTransforms player, DiContainer container)
        {
            Destroy();
            root = new GameObject("RhythmSaberJudgementPlane");
            root.transform.SetParent(origin(player), false);
            root.transform.localPosition = new Vector3(0, 0, Session.Settings.LineDistance);
            root.transform.localRotation = Quaternion.identity;
            if (Session.Active && Session.Settings.ShowLine) CreateBeam();
            banner = CreateText("RhythmSaberStatus", new Vector3(0, .85f, 2.8f-Session.Settings.LineDistance), 2.8f);
            banner.text = Session.Active ? "RhythmSaber Active — Online Score Submission Disabled" :
                "RhythmSaber disabled\n" + Session.Reason;
            banner.color = Session.Active ? Color.white : new Color(1, .4f, .3f);
            if (Session.Active && Session.Settings.ShowStatistics)
                statistics = CreateText("RhythmSaberStatistics", new Vector3(0, 1.02f, 2.8f-Session.Settings.LineDistance), 3.5f);
            if (Session.Active && Session.Settings.ShowJudgements)
            {
                failures = container.InstantiateComponentOnNewGameObject<FlyingTextSpawner>("RhythmSaberFailures");
                Set(failures, "_duration", .9f); Set(failures, "_xSpread", .2f);
                Set(failures, "_targetYPos", 1.45f); Set(failures, "_targetZPos", 2.8f);
                Set(failures, "_color", new Color(1, .2f, .2f)); Set(failures, "_fontSize", FeedbackPresentation.FailureFontSize(Session.Settings.TextSize));
                Set(failures, "_shake", false);
            }
            revision = -1; nextBeamTime = float.NegativeInfinity;
        }
        static void Set(object target, string name, object value)
        {
            var f = AccessTools.Field(target.GetType(), name);
            if(f == null) throw new MissingFieldException(target.GetType().FullName, name);
            f.SetValue(target, value);
        }
        static TextMeshPro CreateText(string name, Vector3 position, float size)
        {
            var go = new GameObject(name); go.transform.SetParent(root.transform, false);
            go.transform.localPosition = position;
            go.transform.localScale = Vector3.one*.1f;
            int layer = LayerMask.NameToLayer("UI"); if(layer >= 0) go.layer = layer;
            var t = go.AddComponent<TextMeshPro>();
            var font = TMP_Settings.defaultFontAsset;
            if(font == null)
                foreach(var native in Resources.FindObjectsOfTypeAll<FlyingScoreEffect>())
                { var source = textField(native); if(source != null && source.font != null) { font = source.font; break; } }
            if(font != null) t.font = font;
            t.fontSize = size; t.alignment = TextAlignmentOptions.Center;
            t.enableWordWrapping = false; t.overflowMode = TextOverflowModes.Overflow;
            t.rectTransform.sizeDelta = new Vector2(25, 5); return t;
        }
        static void CreateBeam()
        {
            var go = new GameObject("RhythmSaberBeam"); go.transform.SetParent(root.transform, false);
            int layer = LayerMask.NameToLayer("Environment"); if(layer >= 0) go.layer = layer;
            mesh = new Mesh { name = "RhythmSaberBeamMesh" }; mesh.MarkDynamic();
            vertices = new Vector3[41*7]; var colors = new Color[vertices.Length]; var uv = new Vector2[vertices.Length];
            var tris = new int[40*6*6]; float[] alpha = { 0, .025f, .22f, 1, .22f, .025f, 0 };
            for (int col = 0; col <= 40; ++col)
                for(int row = 0; row < 7; ++row)
                {
                    int index = col*7+row; uv[index] = new Vector2(col/40f, row/6f);
                    colors[index] = new Color(Session.Settings.LineRed*Session.Settings.LineBrightness,
                        Session.Settings.LineGreen*Session.Settings.LineBrightness, Session.Settings.LineBlue*Session.Settings.LineBrightness,
                        alpha[row]*Session.Settings.LineOpacity);
                }
            int n = 0;
            for(int col = 0; col < 40; ++col) for(int row = 0; row < 6; ++row)
            { int a = col*7+row, b = a+7; tris[n++]=a; tris[n++]=a+1; tris[n++]=b; tris[n++]=b; tris[n++]=a+1; tris[n++]=b+1; }
            mesh.vertices = vertices; mesh.colors = colors; mesh.uv = uv; mesh.triangles = tris;
            var shader = Shader.Find("UI/Default");
            if(shader == null) throw new InvalidOperationException("UI/Default beam shader unavailable.");
            beamMaterial = new Material(shader) { name = "RhythmSaberWhiteBeam", mainTexture = Texture2D.whiteTexture, color = Color.white };
            go.AddComponent<MeshFilter>().sharedMesh = mesh;
            var renderer = go.AddComponent<MeshRenderer>(); renderer.sharedMaterial = beamMaterial;
            renderer.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off; renderer.receiveShadows = false;
            UpdateBeam(0);
        }
        static void UpdateBeam(float time)
        {
            for(int col = 0; col <= 40; ++col) for(int row = 0; row < 7; ++row)
            {
                float x = Session.Settings.LineWidth*(col/40f-.5f);
                float wave = .12f*Mathf.Sin(x*18-time*6)+.06f*Mathf.Sin(x*31+time*7);
                vertices[col*7+row] = new Vector3(x, Session.Settings.LineHeight+Session.Settings.LineThickness*(row-3+wave), 0);
            }
            mesh.vertices = vertices; mesh.RecalculateBounds();
        }
        public static void Tick()
        {
            if (!Session.Active || Session.Clock == null || root == null) return;
            float time = Session.Clock.songTime;
            if (mesh != null && (time >= nextBeamTime || time < nextBeamTime-.1f))
            { UpdateBeam(time); nextBeamTime = time+.05f; }
            if(statistics != null && revision != Session.Revision)
            {
                long total = 0; foreach(var c in Session.Counts) total += c;
                statistics.text = $"PERFECT {Session.Counts[0]}   GREAT {Session.Counts[1]}   GOOD {Session.Counts[2]}\n" +
                    $"BAD {Session.Counts[3]}   POOR {Session.Counts[4]}   MISS {Session.Counts[5]}\n" +
                    $"Mean {(Session.Measured > 0 ? Session.SumDelta/Session.Measured : 0):+0.0;-0.0;0.0} ms   Perfect {(total > 0 ? 100.0*Session.Counts[0]/total : 0):0.0}%";
                revision = Session.Revision;
            }
        }
        public static void Failure(NoteController note, string label)
        {
            if (failures != null && Session.Settings.ShowJudgements && note != null)
                failures.SpawnText(note.noteTransform.position, note.worldRotation, note.inverseWorldRotation, label);
        }
        public static void Destroy()
        {
            foreach(var effect in new List<FlyingScoreEffect>(styles.Keys)) Restore(effect);
            if(failures != null) UnityEngine.Object.Destroy(failures.gameObject);
            if(root != null) UnityEngine.Object.Destroy(root);
            if(mesh != null) UnityEngine.Object.Destroy(mesh);
            if(beamMaterial != null) UnityEngine.Object.Destroy(beamMaterial);
            root = null; mesh = null; beamMaterial = null; banner = statistics = null; failures = null;
        }
    }
}
