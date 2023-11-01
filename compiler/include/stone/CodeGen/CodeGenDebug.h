#ifndef STONE_GEN_IRCODEGDEBUG_H
#define STONE_GEN_IRCODEGDEBUG_H

#include "stone/AST/Module.h"
#include "stone/Basic/LLVM.h"
#include "stone/CodeGen/CodeGenContext.h"

#include "llvm/IR/PassManager.h"

#include <memory>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
class SrcLoc;
class Clang;

namespace codegen {
class CodeGenModule;

class CodeGenDebug final {
  CodeGenContext &cgc;

  SrcLoc curLoc;

public:
  CodeGenDebug(CodeGenContext &cgc, CodeGenModule &cgm, Clang &clang,
               llvm::StringRef mainOutputFilenameForDebug,
               llvm::StringRef privateDiscriminator);

  ~CodeGenDebug();

public:
  /// Helper functions for getOrCreateType.
  /// @{
  /// Currently the checksum of an interface includes the number of
  /// ivars and property accessors.
  // llvm::DIType *CreateType(const BuiltinType *Ty);
  // llvm::DIType *CreateType(const ComplexType *Ty);
  // llvm::DIType *CreateType(const AutoType *Ty);
  // llvm::DIType *CreateType(const BitIntType *Ty);

  // llvm::DIType *CreateQualifiedType(QualType Ty, llvm::DIFile *Fg);
  // llvm::DIType *CreateQualifiedType(const FunctionProtoType *Ty,
  //                                   llvm::DIFile *Fg);

  // llvm::DIType *CreateType(const AliasType *Ty, llvm::DIFile *Fg);
  // llvm::DIType *CreateType(const TemplateSpecializationType *Ty,
  //                          llvm::DIFile *Fg);

  // llvm::DIType *CreateType(const PointerType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const BlockPointerType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const FunctionType *Ty, llvm::DIFile *F);
  // /// Get structure or union type.
  // llvm::DIType *CreateType(const NominalType *Tyg);
  // llvm::DIType *CreateTypeDefinition(const NominalType *Ty);
  // llvm::DICompositeType *CreateLimitedType(const NominalType *Ty);
  // void CollectContainingType(const CXXRecordDecl *RD,
  //                            llvm::DICompositeType *CT);
  // /// Get Objective-C interface type.
  // llvm::DIType *CreateType(const ObjCInterfaceType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateTypeDefinition(const ObjCInterfaceType *Ty,
  //                                    llvm::DIFile *F);

  // llvm::DIType *CreateType(const VectorType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const ConstantMatrixType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const ArrayType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const LValueReferenceType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const RValueReferenceType *Ty, llvm::DIFile
  // *Unit); llvm::DIType *CreateType(const MemberPointerType *Ty, llvm::DIFile
  // *F); llvm::DIType *CreateType(const AtomicType *Ty, llvm::DIFile *F);
  // llvm::DIType *CreateType(const PipeType *Ty, llvm::DIFile *F);
  // /// Get enumeration type.
  // llvm::DIType *CreateEnumType(const EnumType *Ty);
  // llvm::DIType *CreateTypeDefinition(const EnumType *Ty);

  /// Look up the completed type for a self pointer in the TypeCache and
  /// create a copy of it with the ObjectPointer and Artificial flags
  /// set. If the type is not cached, a new one is created. This should
  /// never happen though, since creating a type for the implicit self
  /// argument implies that we already parsed the interface definition
  /// and the ivar declarations in the implementation.
  // llvm::DIType *CreateSelfType(const QualType &QualTy, llvm::DIType *Ty);
  /// @}

  /// Get the type from the cache or return null type if it doesn't
  /// exist.
  // llvm::DIType *GetTypeOrNull(const QualType);
  /// Return the debug type for a C++ method.
  /// \arg CXXMethodDecl is of FunctionType. This function type is
  /// not updated to include implicit \c this pointer. Use this routine
  /// to get a method type which includes \c this pointer.
  // llvm::DISubroutineType *GetOrCreateMethodType(const MethodDecl *method,
  // llvm::DIFile *F, bool decl);
  // llvm::DISubroutineType *
  // GetOrCreateInstanceMethodType(QualType
  // thisPtr, const
  // FunctionProtoType *Func,
  //                               llvm::DIFile *Unit, bool decl);
  // llvm::DISubroutineType *
  // getOrCreateFunctionType(const
  // Decl *D, QualType FnType,
  // llvm::DIFile *F);
  // /// \return debug info
  // descriptor for vtable.
  // llvm::DIType
  // *getOrCreateVTablePtrType(llvm::DIFile
  // *F);

  // /// \return namespace
  // descriptor for the given
  // namespace decl.
  // llvm::DINamespace
  // *getOrCreateNamespace(const
  // NamespaceDecl *N);
  // llvm::DIType
  // *CreatePointerLikeType(llvm::dwarf::Tag
  // Tag, const Type *Ty,
  //                                     QualType PointeeTy, llvm::DIFile *F);
  // llvm::DIType
  // *getOrCreateStructPtrType(StringRef
  // Name, llvm::DIType
  // *&Cache);

  /// A helper function to create
  /// a subprogram for a single
  /// member function GlobalDecl.
  // llvm::DISubprogram
  // *CreateMemberFunction(const
  // MethodDecl *method,
  // llvm::DIFile *F,
  // llvm::DIType *nominalType);

  /// A helper function to collect
  /// debug info for C++ member
  /// functions. This is used while
  /// creating debug info entry for a
  /// Record.
  // void CollectMemberFunctions(const
  // CXXRecordDecl *Decl, llvm::DIFile
  // *F,
  //                                SmallVectorImpl<llvm::Metadata
  //                                *>
  //                                &E,
  //                                llvm::DIType
  //                                *T);

public:
  /// Update the current source location. If \arg loc is invalid it is
  /// ignored.
  void SetLoc(SrcLoc loc);

  /// Return the current source location. This does not necessarily correspond
  /// to the IRBuilder's current DebugLoc.
  SrcLoc GetLoc() const { return curLoc; }
};

} // namespace codegen
} // namespace stone

#endif
