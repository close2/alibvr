# INCOMPLETE

Adc, ports access and irqs are starting to become somehow stable.

The API is still not fixed and very little has been actually tested.

Please come back in a few days / weeks / months.


# Goal of this library:

This is library for the *atmega 328*.  This is the IC used in arduinos.

Instead of defining pins and settings with C-macros, everything is set
using C/C++ code.  This is mostly possible through template arguments.

For instance pin definitions are passed around as template arguments.

This class _never_ creates objects!  Everything is static and resolved
during compile time, making the generated code very compact and
efficient.  *This is not a typical `C++` library!*

Ony very few features are implemented using C-macros.

I still hope that the described type-stack in
[b.atch.se](http://b.atch.se/) will at some point replace the
`#include` C-macro tricks, which are currently necessary to register
irq handlers.

This library is only tested with `gcc`!


# Documentation:

This README shows some simple examples.  Only parts of the
functionality of `alibvr` are covered.

Every subsystem has its own README_*.  I've tried to show the complete
functionality in those READMEs.

If you still need more detailed information use the
[doxygen](http://close2.github.io/alibvr/doxygen/html/) documentation.

I find doxygen documentations hard to read and hope that most use cases
are covered through examples in the READMEs.


# Subsystems:

[Ports](README_PORTS.md)

[Analog to digital conversion](README_ADC.md)


# Examples:

Examples may be found in my other github-repository:
[alibvr-examples](https://www.github.com/close2/alibvr-examples)

