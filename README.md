# nvm
## Abstract VN computer

An implementation of a specific abstract maschine described in
"Compilation of programming exercises with solutions" (V. Ćirić, 1978. University in Belgrade)

VN stands for "von-Neumann like".

Additional implementation of a (primitive) compiler-assembler-linker pipeline.

Useful for having an abstract target architecture for developing:

* assemblers, languages, parsers, lexers, linkers 
* compilers, interpreters
* a small-footprint general purpose virtual computer
* practical validators for exercises in machine code programming

## Compiling

You only actually need nvmas.c, nvmas.h and nvmaslib.c for the abstract machine.
By default, the C header assumes win x86 target, so change

* CCPLATFORM and
* CCARCH

macros to reflect your actual environment. The virtual machine part is written
for maximum portability so it should compile (and work) on both Win and \*Nix systems,
including macOS, thus gcc, MS cl or clang could all be used readily.

Note on Windows you need link against Winsock library, which is Ws2\_32.lib

There is a separate GNUstep branch in preparation, that uses the
GNUmakefile system, to make the project compatible with Xcode-like environments.

An Objective-C version is also in the making.

### Makefile

You can also compile the program using a BA system compatible with
classic **make** utility syntax. Be sure to adapt it to your
own environment. It is designed so that most of the variable
stuff is at the top of the file, to get that "effortless" feel.

## Compiler

As for the compiler-assembler-linker pipeline, it is implemented as
three *Tcl* scripts that read from *stdin* and write to *stdout*, so
they should be connected via the command pipe ("|"), and the output
of the chain redirected to a binary file.

The virtual machine reads binary executable code from stdin as well.

For convenience, a cmd "batch" script is included which will give you
the basic idea for how to pipe the scripts, and "run" the resulting
"executable".

### Compiler input

The compiler-assembler-linker pipeline expects a primitive "high-level"
source code input where each token is *delimited by space* (including brackets).

It will automatically assign addresses to variables, and expects
"identifiers" to be **single lowercase english alphabet letters.**

It also reads from a **hardcoded** source file, *srctest.vmt*.

Literal values need to be *assigned to variables* first.

There needs to be a **delimiter** between literal values and
*gist* of the code, in the form of a **32-bit 0 value**. You can
sneak it in by putting a redundant "assignment" such as "t = 0" after
declaring literal values.


