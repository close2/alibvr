# Analog to digital conversion

The adc subsystem is abstracted through the `Adc` class.

Default settings (input, reference voltage and mode) may be set as
template arguments of the `Adc` class.

Calling the static method `init()` will set the subsystem.  The
`init()` method also provides template arguments, making it possible to
override the default settings.  Note that the order of those template
arguments differ!


## Doxygen documentation

The Adc subsystem is documented in the:
[namespace adc](http://close2.github.io/alibvr/doxygen/html/de/d26/namespaceadc.html)


## Settings

When doing adc you have to specify

* an input
* a reference voltage
* a mode


## Input selection

In addition to input pins (see `PIN_ADC*` `typedef`s in
[README_PORTS.md](README_PORTS.md]) `Input::Temperature`, `Input::V1_1`
and `Input::Gnd` are allowed inputs.


## Reference voltages

Possible references are defined in:
```C++
+++ADC_REFS+++
```


## ADC modes

Possible modes are defined in:
```C++
+++ADC_MODES+++
```

See the
[doxygen doc](http://close2.github.io/alibvr/doxygen/html/de/d26/namespaceadc.html#a8094fa55ea1a7729bb35c230163c0f8f)
for short descriptions of those enum values.


## Synchronous conversions

`adc_8bit()` or `adc_10bit()` by default start a conversion and
busywait for the conversion to finish.

```C++
+++ADC_SYNC+++
```

A non-zero template argument to `adc_*bit()` will put the cpu into idle
sleep, which reduces noise and improves conversion results.

You have to register an ADC-irq handler if you want to use noise
reduction.  (The default handler would reset the cpu and `adc_*bit()`
must enable irqs, so that the cpu leaves sleep mode after a
conversion.)

```C++
+++ADC_NOISE_RED1+++
// main { ...
+++ADC_NOISE_RED2[^,  ]+++
// ... }  // end of main
+++REGISTER_IRQS+++
```

Your code will not compile (with a semi-useful error) if you don't
register a handler.


## Background conversions

While the adc-subsystem performs a conversion the cpu is free to do
other stuff.

Everything mentioned in the previous synchronous section applies to
the background conversions as well.

To start a conversion simply call `start_adc_*bit()`.  To find out if
the conversion is still running, use `is_adc_finished()`.

`busy_wait_adc_finished()` will loop until `is_adc_finished()` returns
true.

`get_adc_*bit_result()` will return the result.

```C++
+++ADC_BACKGROUND+++
```


## Adc Tasks and Adc Irq handler

When a `Task` (except for the `nullptr`) is provided as template
argument to `Adc` then `init()` will enable the Adc irq and the
`Task` will be called every time an Adc irq is raised (i.e. every
time a conversion finishes).

`init()` will not change the global irq flag!  If you register a task,
don't forget to enable irqs.

The type of `Task` is `typedef void(* task)(const uint16_t&)`.  A
function, which takes a 16bit result as argument.  Even if you only use
8bit conversions compiler optimizations should generate (near) optimal
code.

The irq handler must be registered and all irq handlers enabled by
including the corresponding header files:

```C++
+++ADC_TASK1+++
// main { ...
+++ADC_TASK2[^,  ]+++
// ... }  // end of main
+++REGISTER_IRQS+++
```


## Switching between different inputs, reference voltages or modes

The Adc class itself allows you to specify input, reference voltage and
mode as template arguments.  It is however possible to override those
values when calling `init()`.

If you want to change the input, reference voltage or mode, simply call
`init()` with the new values as template arguments.

To make your intentions clearer in this case, you can use `Input::Unset`
as template argument for the Adc class, and only pass a real input when
calling `init()`.  This is however not necessary.  The generated code
will be either equal or very similar.

Using another Adc class with different template arguments is another
possibility.  Remember however, that the adc subsystem is shared.

Here one class (`typedef`) is used to sample two inputs.
```C++
+++ADC_2_INPUTS1+++
```

The same result using two classes:
```C++
+++ADC_2_INPUTS2+++
```

Remember, the subsystem is shared:
```C++
+++ADC_SHARED+++
```

It is possible to register multiple `Task`s, but _all_ `Task`s will
be executed instead of only the one where the conversion was started!
