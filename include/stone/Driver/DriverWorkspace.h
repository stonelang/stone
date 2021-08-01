#ifndef STONE_DRIVER_DRIVERWORKSPACE_H
#define STONE_DRIVER_DRIVERWORKSPACE_H

namespace stone {
class Driver;

class DriverWorkspace {
  Driver &driver;

public:
  DriverWorkspace(Driver &driver);
  ~DriverWorkspace();

public:
  void BuildJobs();

private:
  /// Build jobs for multiple compiles -- each job gets one source file
  void BuildJobsForMultipleCompileKind();

  /// Build jobs for a single compile -- the compile jobs has multiple files.
  void BuildJobsForSingleCompileKind();

  /// Builds the compile jobs
  void BuildCompileJobs();

  /// Builds the  link job
  void BuildLinkJob();

  /// Build compile only jobs
  void BuildBackendJob();

  /// Build compile only jobs
  void BuildAssembleJob();
};

} // namespace stone
#endif
