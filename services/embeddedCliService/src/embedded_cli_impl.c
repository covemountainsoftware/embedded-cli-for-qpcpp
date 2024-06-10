//include the embedded-cli with implementation flag,
//which pulls the actual CLI implementation into this source
//code file. Moved to a dedicated "C" file to facilitate
//memory leak detection with cpputest.
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"