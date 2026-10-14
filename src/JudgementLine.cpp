#include "Visuals.hpp"
#include "TimingManager.hpp"
#include "main.hpp"
#include "JudgementBeam.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mesh.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/LayerMask.hpp"
#include "UnityEngine/Rendering/ShadowCastingMode.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Quaternion.hpp"

namespace RhythmSaber {
namespace {
UnityEngine::GameObject* root{};
UnityEngine::Mesh* mesh{};
UnityEngine::Material* material{};
ArrayW<UnityEngine::Vector3> beamVertices{};
double lastBeamTime{-1};
}
bool CreateJudgementLine() {
  using namespace UnityEngine;
  DestroyJudgementLine();
  const auto& s=Current().settings;
  // UI/Default supplies vertex alpha without a lighting dependency or postprocess.
  auto shader=Shader::Find("UI/Default");
  if(!shader) { Logger.error("UI/Default shader not found"); return false; }
  root=GameObject::New_ctor("RhythmSaberJudgementLine");
  int layer=LayerMask::NameToLayer("Environment"); if(layer>=0) root->set_layer(layer);
  // Follow the game origin (including AssignPlayerToTrack), never the HMD.
  if(Current().playerSpace) root->get_transform()->SetParent(Current().playerSpace,false);
  root->get_transform()->set_localPosition({0,0,s.lineDistance});
  root->get_transform()->set_localRotation(Quaternion::get_identity());
  material=Material::New_ctor(shader); material->set_mainTexture(Texture2D::get_whiteTexture());
  mesh=Mesh::New_ctor();
  constexpr int columns=41,rows=7;
  beamVertices=ArrayW<Vector3>(columns*rows);
  ArrayW<Color> colors(columns*rows); ArrayW<Vector2> uv(columns*rows); ArrayW<int> triangles((columns-1)*(rows-1)*6);
  // A single continuous white energy beam: bright core, smooth alpha falloff,
  // tiny travelling ripples. No posts, lower rails, collider or bloom pass.
  for(int x=0;x<columns;++x) for(int y=0;y<rows;++y) {
    const int a=x*rows+y;
    const auto p=BeamVertex(x,y,s.lineWidth,s.lineHeight,s.lineThickness,0);
    beamVertices[a]={p.x,p.y,0};
    colors[a]={s.red*s.brightness,s.green*s.brightness,s.blue*s.brightness,p.alpha*s.alpha};
    uv[a]={.5f,.5f};
    if(x<columns-1 && y<rows-1) {
      const int t=(x*(rows-1)+y)*6;
      const int indices[]{a,a+rows+1,a+rows,a,a+1,a+rows+1};
      for(int j=0;j<6;++j) triangles[t+j]=indices[j];
    }
  }
  mesh->set_vertices(beamVertices); mesh->set_colors(colors); mesh->set_uv(uv); mesh->set_triangles(triangles); mesh->RecalculateBounds();
  root->AddComponent<MeshFilter*>()->set_sharedMesh(mesh);
  auto renderer=root->AddComponent<MeshRenderer*>(); renderer->set_sharedMaterial(material);
  renderer->set_shadowCastingMode(Rendering::ShadowCastingMode::Off); renderer->set_receiveShadows(false);
  root->SetActive(s.showLine);
  lastBeamTime=-1;
  return true;
}
void DestroyJudgementLine() {
  using namespace UnityEngine;
  if(root && Object::op_Implicit_bool(root)) Object::Destroy(root);
  if(mesh && Object::op_Implicit_bool(mesh)) Object::Destroy(mesh);
  if(material && Object::op_Implicit_bool(material)) Object::Destroy(material);
  root=nullptr; mesh=nullptr; material=nullptr; beamVertices={};
}
void UpdateJudgementLine(double t) {
  if(!mesh || !root || !Current().settings.showLine || (t>=lastBeamTime && t-lastBeamTime<.05)) return;
  lastBeamTime=t;
  const auto& s=Current().settings;
  for(int x=0;x<41;++x) for(int y=0;y<7;++y) {
    const auto p=BeamVertex(x,y,s.lineWidth,s.lineHeight,s.lineThickness,t);
    beamVertices[x*7+y]={p.x,p.y,0};
  }
  mesh->set_vertices(beamVertices);
}
double SignedPlaneDistance(UnityEngine::Vector3 pos) {
  if(!root) return pos.z-Current().settings.lineDistance;
  auto transform=root->get_transform();
  // Local Z=0 is the rendered plane even under rotated/non-uniformly scaled
  // player-track parents. Quaternion forward alone would ignore parent shear.
  return transform->InverseTransformPoint(pos).z;
}
void SetVisualsPaused(bool paused) {
  if(root && UnityEngine::Object::op_Implicit_bool(root)) root->SetActive(!paused && Current().settings.showLine);
}
}
