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
 
set(dit_sources
  Dit.cpp
   
	LINK_LIBS 
	 

  DEPENDS
   

)
add_stone_library(Dit ${dit_sources})


 