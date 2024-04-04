#ifndef MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_DEFS_H_
#define MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_DEFS_H_

#define BUFFER_SIZE 1000

typedef enum CoreBUnit {
  kCoreBUnitBytes = 0,
  kCoreBUnitKiloBytes = 1,
  kCoreBUnitMegaBytes = 2,
  kCoreBUnitGigaBytes = 3
} CoreBUnit;

typedef enum CoreError {
  kCoreErrorSuccess,
  kCoreErrorAllocFail,
  kCoreErrorCannotOpenProcFile,
  kCoreErrorCannotReadProcFile,
  kCoreErrorCurlInitFail
} CoreError;

__attribute__((unused)) static const char* kCoreErrorsStr[] = {
    "success",
    "allocation fail",
    "cannot open proc file",
    "proc file read failed",
    "curl not initialized"
};

#endif // MONITORINGSYSTEM_SRC_AGENTS_CORE_INCLUDE_DEFS_H_

