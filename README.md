# INCOMPLETE

Adc, ports access and irqs are starting to become somehow stable.

The API is still not fixed and very little has been actually tested.

Please come back in a few days / weeks / months.


## Goal of this library:

This is library for the *atmega 328*.  This is the IC used in arduinos.

Every functionality of the atmega is exposed through a class with static
functions.  This library will never create objects!

Pins are assigned using `typedef`s instead of macros.

Using C++ templates it should be possible to have very few / no C-macros.
Also C++ templates are all "resolved" during compile time.  This means that
the produced code is very compact.


## Documentation:

This README shows some simple examples.  Only parts of the
functionality of this library are covered.

Every subsystem has its own README_*.  I've tried to show the complete
functionality in those READMEs.

If you still need more detailed information use the
[doxygen](http://close2.github.io/alibvr/doxygen/html/) documentation.

I find doxygen documentations hard to read and hope that most use cases
are covered through examples in the READMEs.


## Subsystems:

[Ports](README_PORTS.md)

[Analog to digital conversion](README_ADC.md)


## Examples:

Examples may be found in my other github-repository:
[alibvr-examples](https://www.github.com/close2/alibvr-examples)

