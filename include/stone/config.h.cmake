/* This generated file is for internal use. Do not include it from headers. */

#ifdef STONE_CONFIG_H
#error config.h can only be included once
#else
#define STONE_CONFIG_H

/* Bug report URL. */
#define BUG_REPORT_URL "${BUG_REPORT_URL}"

/* Default to -fPIE and -pie on Linux. */
#cmakedefine01 STONE_DEFAULT_PIE_ON_LINUX

/* Default linker to use. */
#define STONE_DEFAULT_LINKER "${STONE_DEFAULT_LINKER}"

/* Default C++ stdlib to use. */
#define STONE_DEFAULT_STDLIB "${STONE_DEFAULT_STDLIB}"

/* Default runtime library to use. */
#define STONE_DEFAULT_RTLIB "${STONE_DEFAULT_RTLIB}"

/* Default unwind library to use. */
#define STONE_DEFAULT_UNWINDLIB "${STONE_DEFAULT_UNWINDLIB}"

/* Default objcopy to use */
#define STONE_DEFAULT_OBJCOPY "${STONE_DEFAULT_OBJCOPY}"

/* Default OpenMP runtime used by -fopenmp. */
#define STONE_DEFAULT_OPENMP_RUNTIME "${STONE_DEFAULT_OPENMP_RUNTIME}"

/* Default architecture for SystemZ. */
#define STONE_SYSTEMZ_DEFAULT_ARCH "${STONE_SYSTEMZ_DEFAULT_ARCH}"

/* Multilib basename for libdir. */
#define STONE_INSTALL_LIBDIR_BASENAME "${STONE_INSTALL_LIBDIR_BASENAME}"

/* Relative directory for resource files */
#define STONE_RESOURCE_DIR "${STONE_RESOURCE_DIR}"

/* Directories clang will search for headers */
#define C_INCLUDE_DIRS "${C_INCLUDE_DIRS}"

/* Directories clang will search for configuration files */
#cmakedefine STONE_CONFIG_FILE_SYSTEM_DIR "${STONE_CONFIG_FILE_SYSTEM_DIR}"
#cmakedefine STONE_CONFIG_FILE_USER_DIR "${STONE_CONFIG_FILE_USER_DIR}"

/* Default <path> to all compiler invocations for --sysroot=<path>. */
#define DEFAULT_SYSROOT "${DEFAULT_SYSROOT}"

/* Directory where gcc is installed. */
#define GCC_INSTALL_PREFIX "${GCC_INSTALL_PREFIX}"

/* Define if we have libxml2 */
#cmakedefine STONE_HAVE_LIBXML ${STONE_HAVE_LIBXML}

/* Define if we have sys/resource.h (rlimits) */
#cmakedefine STONE_HAVE_RLIMITS ${STONE_HAVE_RLIMITS}

/* Define if we have dlfcn.h */
#cmakedefine STONE_HAVE_DLFCN_H ${STONE_HAVE_DLFCN_H}

/* Define if dladdr() is available on this platform. */
#cmakedefine STONE_HAVE_DLADDR ${STONE_HAVE_DLADDR}

/* Linker version detected at compile time. */
#cmakedefine HOST_LINK_VERSION "${HOST_LINK_VERSION}"

/* pass --build-id to ld */
#cmakedefine ENABLE_LINKER_BUILD_ID

/* enable x86 relax relocations by default */
#cmakedefine01 ENABLE_X86_RELAX_RELOCATIONS

/* Enable IEEE binary128 as default long double format on PowerPC Linux. */
#cmakedefine01 PPC_LINUX_DEFAULT_IEEELONGDOUBLE

/* Enable each functionality of modules */
#cmakedefine01 STONE_ENABLE_ARCMT
#cmakedefine01 STONE_ENABLE_OBJC_REWRITER
#cmakedefine01 STONE_ENABLE_STATIC_ANALYZER

/* Spawn a new process stone-compile.exe for the frontend tool invocation, when necessary */
#cmakedefine01 STONE_SPAWN_FRONTEND

/* Whether CIR is built into Stone */
#cmakedefine01 STONE_ENABLE_CIR

#endif
