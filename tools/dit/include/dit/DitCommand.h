


namespace dit {
/*
Initialize project
mkdir MyProject
cd MyProject
dit -add-project MyProject  # Creates MyProject.dit

Add a package to the project
dit -add-package MyPackage --project MyProject

Add a module to the package
dit -add-module MyModule --package MyPackage

Add a file to the module
dit -add-file MyFile.stone --module MyModule

dit -add-file MyFile.stone --project MyProject --package MyPackage --module MyModule

cd MyProject/MyPackage/MyModule
dit -add-file MyFile.stone

dit -start-project Project1 -add-package Package1 -add-module Module1 -add-file File1.stone

*/

enum class DitCommandType {
    None = 0,
    Init,
    StartProject,
    AddProject, 
    AddPackage,
    AddModule,
    AddFile,
    RemoveProject, 
    RemovePackage,
    RemoveModule,
    RemoveFile,
    Publish,
    Sync,
    Build,
}

class DitCommand {
    DitCommandType commandType;
public:
    virtual ~DitCommand() = default;
    virtual void Execute() = 0; // Pure virtual method to enforce implementation

public:
    virtual llvm::StringRef GetName() = 0; // Pure virtual method to enforce implementation
    virtual llvm::StringRef GetKind() = 0; // Pure virtual method to enforce implementation
public:
    DitCommand(DitCommandType command) : commandType(commandType){}
 };

class AddProjectCommand : public DitCommand {
    std::string projectName;

public:
    AddProjectCommand(const std::string& name) : projectName(name) {}

    void Execute() override {
        std::cout << "Adding project: " << projectName << "\n";
        // Logic for adding a project
    }
};

class AddPackageCommand : public DitCommand {
    std::string packageName;

public:
    AddPackageCommand(const std::string& name) : packageName(name) {}

    void Execute() override {
        std::cout << "Adding package: " << packageName << "\n";
        // Logic for adding a package
    }
};

class AddModuleCommand : public DitCommand {
    std::string moduleName;
    std::string packageName;

public:
    AddModuleCommand(const std::string& module, const std::string& package)
        : moduleName(module), packageName(package) {}

    void Execute() override {
        std::cout << "Adding module: " << moduleName << " to package: " << packageName << "\n";
        // Logic for adding a module
    }
};

class AddFileCommand : public DitCommand {
    std::string fileName;
    std::string moduleName;

    std::string fileName;
    std::string moduleName;
    std::string packageName;
    std::string projectName;



public:
    AddFileCommand(const std::string& file, const std::string& module)
        : fileName(file), moduleName(module) {}

    void Execute() override {
        std::cout << "Adding file: " << fileName << " to module: " << moduleName << "\n";
        // Logic for adding a file
    }
};

class RemoveProjectCommand : public DitCommand {
    std::string projectName;

public:
    RemoveProjectCommand(const std::string& name) : projectName(name) {}

    void Execute() override {
        std::cout << "Removing project: " << projectName << "\n";
        // Logic for removing a project
    }
};

class RemovePackageCommand : public DitCommand {
    std::string packageName;

public:
    RemovePackageCommand(const std::string& name) : packageName(name) {}

    void Execute() override {
        std::cout << "Removing package: " << packageName << "\n";
        // Logic for removing a package
    }
};

class RemoveModuleCommand : public DitCommand {
    std::string moduleName;
    std::string packageName;

public:
    RemoveModuleCommand(const std::string& module, const std::string& package)
        : moduleName(module), packageName(package) {}

    void Execute() override {
        std::cout << "Removing module: " << moduleName << " from package: " << packageName << "\n";
        // Logic for removing a module
    }
};

class RemoveFileCommand : public DitCommand {
    std::string fileName;
    std::string moduleName;

public:
    RemoveFileCommand(const std::string& file, const std::string& module)
        : fileName(file), moduleName(module) {}

    void Execute() override {
        std::cout << "Removing file: " << fileName << " from module: " << moduleName << "\n";
        // Logic for removing a file
    }
};

class PublishCommand : public DitCommand {
public:
    void Execute() override {
        std::cout << "Publishing project...\n";
        // Logic for publishing a project
    }
};

class SyncCommand : public DitCommand {
public:
    void Execute() override {
        std::cout << "Synchronizing project...\n";
        // Logic for syncing project dependencies
    }
};

class BuildCommand : public DitCommand {
public:
    void Execute() override {
        std::cout << "Building project...\n";
        // Logic for building the project
    }
};


}
#endif 
 