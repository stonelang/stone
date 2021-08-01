#include "stone/Driver/DriverWorkspace.h"
#include "stone/Driver/Driver.h"

using namespace stone;

DriverWorkspace::DriverWorkspace(Driver &driver) : driver(driver) {}

DriverWorkspace::~DriverWorkspace() {}

void DriverWorkspace::BuildJobs() {}

/// Build jobs for multiple compiles -- each job gets one source file
void DriverWorkspace::BuildJobsForMultipleCompileKind() {}

/// Build jobs for a single compile -- the compile jobs has multiple files.
void DriverWorkspace::BuildJobsForSingleCompileKind() {}

/// Builds the compile jobs
void DriverWorkspace::BuildCompileJobs() {}

/// Builds the  link job
void DriverWorkspace::BuildLinkJob() {}

/// Build compile only jobs
void DriverWorkspace::BuildBackendJob() {}

/// Build compile only jobs
void DriverWorkspace::BuildAssembleJob() {}
