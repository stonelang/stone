
enum class JobKind {
  None,
  Compile,
  Link,
};
class Job {
  JobKind kind;
  llvm::TinyPtrVector<const file::File *> inputs;

public:
  Job(JobKind kind) : kind(kind) {}
};

class CompileJob : public Job {
public:
  CompileJob(const file::File *input) : Job(JobKind::Compile) {}
};

/// DepList
class MultiPuJob : public Job {
  llvm::TinyPtrVector<const Job *> deps;

public:
  FlexJob(JobKind kind, InputList inputs) : SoloJob(kind) {}
  FlexJob(JobKind kind, DepList inputs) : SoloJob(kind) {}

public:
  void AddDep(const Job *job) {}
};

class LinkJob : public FlexJob {

public:
  LinkJob() : FlexJob(JobKind::Link) {}
};

//-----------

enum class JobKind {
  None,
  Compile,
  Link,
};
class Job {
  JobKind kind;
  llvm::TinyPtrVector<const file::File *> inputs;

public:
  Job(JobKind kind) : kind(kind) {}
};

class CompileJob : public Job {
public:
  CompileJob(const file::File *input) : Job(JobKind::Compile) {}
};

/// DepList
class FlexJob : public Job {
  llvm::TinyPtrVector<const Job *> deps;

public:
  FlexJob(JobKind kind, InputList inputs) : SoloJob(kind) {}
  FlexJob(JobKind kind, DepList inputs) : SoloJob(kind) {}

public:
  void AddDep(const Job *job) {}
};

class LinkJob : public FlexJob {

public:
  LinkJob() : FlexJob(JobKind::Link) {}
};

int main() { return 0; }