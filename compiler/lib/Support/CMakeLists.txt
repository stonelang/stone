set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  Option
 )
set(Stone_support_sources
  InputFile.cpp
  Statistics.cpp
  Options.cpp

	LINK_LIBS 
	StoneBasic

  DEPENDS
  StoneOptions

)
add_Stone_library(StoneSupport
	${Stone_support_sources}
)

if (TARGET obj.StoneSupport)
  add_library(StoneSupport_tablegen ALIAS obj.StoneSupport)
elseif (NOT LLVM_LINK_LLVM_DYLIB)
  add_library(StoneSupport_tablegen ALIAS StoneSupport)
else()
  # Build a version of the support library that does not link against
  # libLLVM-*.so, to be used by Stone-tblgen. This is so Stone-tblgen doesn't
  # link against libLLVMSupport twice (once statically and once via
  # libLLVM-*.so).
  add_llvm_library(StoneSupport_tablegen
    BUILDTREE_ONLY STATIC DISABLE_LLVM_LINK_LLVM_DYLIB
    ${Stone_support_sources})
endif()



