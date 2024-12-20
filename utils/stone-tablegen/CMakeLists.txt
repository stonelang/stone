set(LLVM_LINK_COMPONENTS 
  Support
  Core
  Option
)

add_tablegen(stone-tablegen STONE
  DESTINATION "${STONE_TOOLS_INSTALL_DIR}"
  EXPORT Stone
  
  DiagnosticEmitter.cpp
  main.cpp
  )

target_link_libraries(stone-tablegen PRIVATE StoneSupport_tablegen)
