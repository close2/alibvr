# kdevelop hack

The "Problem" window in kdevelop 5 shows an error when using
`_SFR_MEM_ADDR`.


kdevelop 5 uses llvm for code parsing.

Unfortunately llvm doesn't know that the avr pointer size is 16bits and
kdevelop will show the llvm error that the cast from pointer to uint16_t
would be lossy.

I have added kdevelop_hack/ as include directory in kdevelop which will
redefine _SFR_MEM_ADDR.


My Makefiles don't use any kdevelop settings.


Here is my kdevelop 5 config:

```
[Buildset]
BuildItems=@Variant(\x00\x00\x00\t\x00\x00\x00\x00\x01\x00\x00\x00\x0b\x00\x00\x00\x00\x01\x00\x00\x00\x0c\x00a\x00l\x00i\x00b\x00v\x00r)

[CustomDefinesAndIncludes][ProjectPath0]
Path=.
parseAmbiguousAsCPP=true
parserArguments=-ferror-limit=100 -fspell-checking -Wdocumentation -Wno-documentation-deprecated-sync -Wunused-parameter -Wunreachable-code -Wall -std=c++11
parserArgumentsC=-ferror-limit=100 -fspell-checking -Wdocumentation -Wno-documentation-deprecated-sync -Wunused-parameter -Wunreachable-code -Wall -std=c99

[CustomDefinesAndIncludes][ProjectPath0][Compiler]
Name=Clang

[CustomDefinesAndIncludes][ProjectPath0][Defines]
FAKE_ADDR=32
__AVR_ATmega48P__=

[CustomDefinesAndIncludes][ProjectPath0][Includes]
1=/home/cl/projects/alibvr/kdevelop_hack
2=/usr/avr/include/
3=/home/cl/projects/alibvr/src

[Defines And Includes][Compiler]
Name=GCC
Path=gcc
Type=GCC

[MakeBuilder]
Default Make Environment Profile=

[Project]
VersionControlSupport=kdevgit
```
