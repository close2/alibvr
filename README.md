INCOMPLETE
==========


Please come back in a few days / weeks / months.


Missing:
========

* Makefiles do not work
* Examples do not work
* There is no documentation currently

Goal of this library:
=====================

This is library for the *atmega 328*.  This is the IC used in arduinos.

Every functionality of the atmega is exposed through a class with static
functions.  This library will never create objects!

Pins are assigned using `typedef`s instead of macros.

Using C++ templates it should be possible to have very few / none C-macros.
Also C++ templates are all "resolved" during compile time.  This means that
the produced code is very compact.


Usage:
======

## ports.h

