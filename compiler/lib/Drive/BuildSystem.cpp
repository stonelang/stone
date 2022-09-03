#include "stone/Drive/BuildSystem.h"

#include "stone/Basic/Defer.h"

using namespace stone;

BuildSystem::BuildSystem(Driver &dirver) : driver(driver) {}

void BuildSystem::Initialize() {}

BuildSystem::~BuildSystem() {}

bool BuildSystem::IsDirty(const file::File &entry) {
  return true;
  // Load("stone.build")
  // Get BuildEntry().IsDirty();
}
void BuildSystem::Clean() {
  // Delelete stone.build
}

void BuildSystem::StartBuild() {}
void BuildSystem::StopBuild() {}
// class JobsBuilt final {
//   Compilation &compilation;

// public:
//   bool cacheJob;
//   Job *linkJob;
//   List<Job> cache;

// public:
//   JobsBuilt(const JobsBuilt &) = delete;
//   void operator=(const JobsBuilt &) = delete;

// public:
//   JobsBuilt(Compilation &compilation) : compilation(compilation) {}
//   ~JobsBuilt() { Finish(); }

// private:
//   void Finish();
// };
// void JobsBuilt::Finish() {

//   if (compilation.IsLinkable()) {
//     auto job = compilation.GetToolChain().CreateLinkJob(compilation);
//     compilation.GetCompilation().GetQueue().Push(job);
//     for (auto &j : cache) {
//       if (j.GetKind() == JobKind::Compile) {
//         job->AddDep(&j);
//       }
//     }
//   }
// }

// static Job *BuildCompileJob(Compilation &compilation, JobsBuilt &jb,
//                             file::File &input) {

//   switch (compilation.GetCompilationMode()) {}

//   auto job = compilation.GetToolChain().CreateCompileJob(compilation);
//   job->AddInput(input);
//   compilation.GetCompilation().GetQueue().Push(job);
//   jb.cacheJob = true;
//   return job;
// }
// static Job *BuildLinkJob(Compilation &compilation, JobsBuilt &jb,
//                          List<Job> deps) {

//   return nullptr;
// }

// static Job *BuildJob(Compilation &compilation, JobsBuilt &jb,
//                      file::File &input) {

//   switch (input.GetType()) {
//   case file::Type::Stone: {
//     return BuildCompileJob(compilation, jb, input);
//     break;
//   }
//   }
//   return nullptr;
// }

// void Compilation::BuildJobs() {

//   JobsBuilt jb(*this);

//   for (auto &input : GetDriverOptions().inputFiles) {

//     assert(input.GetType() == GetInputFileType() &&
//            "Incompatible input file types");

//     jb.cacheJob = false;
//     auto job = BuildJob(*this, jb, input);
//     if (job && jb.cacheJob) {
//       jb.cache.Add(job);
//     }
//   }
// }