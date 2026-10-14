#pragma once
#include <string>
namespace RhythmSaber::SubmissionGuard {
std::string Conflict();
bool SetRequested(bool enabled);
void Sync();
void HoldForLevel();
void NewLevel();
bool Held();
}
