#ifndef STONE_DRIVER_FILETPE_EXECUTION_H
#define STONE_DRIVER_FILETPE_EXECUTION_H

namespace stone {

class FileTypeExecution final {
public:
  FileType GetType();

public:
  // FileType GetDependency();
};

class StoneFileTypeExecution : public FileTypeExecution {
public:
};

} // namespace stone
#endif
