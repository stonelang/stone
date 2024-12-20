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

# Drop stone-tablegen-targets from LLVM_COMMON_DEPENDS.
# so that we could use StoneSupport within stone-tblgen and other stone
# component.
list(REMOVE_ITEM LLVM_COMMON_DEPENDS stone-tablegen-targets)


set(stone_support_sources
  InputFile.cpp
  Statistics.cpp
  Options.cpp

	LINK_LIBS 
	StoneBasic

  DEPENDS
  StoneOptions

)
add_stone_library(StoneSupport ${stone_support_sources})

if (TARGET obj.StoneSupport)
  add_library(StoneSupport_tablegen ALIAS obj.StoneSupport)
elseif (NOT LLVM_LINK_LLVM_DYLIB)
  add_library(StoneSupport_tablegen ALIAS StoneSupport)
else()
  # Build a version of the support library that does not link against
  # libLLVM-*.so, to be used by stone-tblgen. This is so stone-tblgen doesn't
  # link against libLLVMSupport twice (once statically and once via
  # libLLVM-*.so).
  add_llvm_library(StoneSupport_tablegen
    BUILDTREE_ONLY STATIC DISABLE_LLVM_LINK_LLVM_DYLIB
    ${stone_support_sources})
endif()



set(LLVM_COMMON_DEPENDS_OLD ${LLVM_COMMON_DEPENDS})

# Drop stone-tablegen-targets from LLVM_COMMON_DEPENDS.
# so that we could use StoneSupport within stone-tblgen and other stone
# component.
list(REMOVE_ITEM LLVM_COMMON_DEPENDS stone-tablegen-targets)

set(LLVM_LINK_COMPONENTS
  Support
  )

set(stone_support_sources
  RISCVVIntrinsicUtils.cpp
  )

add_stone_library(StoneSupport ${stone_support_sources})

if (TARGET obj.StoneSupport)
  add_library(StoneSupport_tablegen ALIAS obj.StoneSupport)
elseif (NOT LLVM_LINK_LLVM_DYLIB)
  add_library(StoneSupport_tablegen ALIAS StoneSupport)
else()
  # Build a version of the support library that does not link against
  # libLLVM-*.so, to be used by stone-tblgen. This is so stone-tblgen doesn't
  # link against libLLVMSupport twice (once statically and once via
  # libLLVM-*.so).
  add_llvm_library(StoneSupport_tablegen
    BUILDTREE_ONLY STATIC DISABLE_LLVM_LINK_LLVM_DYLIB
    ${stone_support_sources})
endif()





