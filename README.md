# Backlight Dimmer

This is a small utility i use to linearize my backlight.

Generally PWM based backlights have the problem of strongly non-linear perceived brightness, unless this is already being comprensated for by your operating system. This utility assumes that the eye will respond logarithmically to the true light intensity, and "predistorts" the output signal using an exponential to achieve linear brightness scaling.

For this, we fit an exponential between 0 and 1 (later mapped to 0 and `max_brightness` respectively) with a scaling factor `alpha`, where x is the perceived brightness between 0 and 1 (0 to 100%):

```
y = f(x) = (exp(alpha * x) - 1) / (exp(alpha) - 1)
```

For the resulting `f(x) -> f(0) = 0` and `f(1) = 1` for all `alpha`, where `alpha` is a factor determining "how exponential" the curve appears. Values of around 3-4 for `alpha` usually work out well.

Because this is a monotonic function, we can easily calculate an inverse and use that to determine the current perceived brightness from the current pwm percentage:

```
x = g(y) = log(y * (exp(alpha) - 1) + 1) / alpha
```

Finally this results in the following simple algorithm implemented in this tool:

* Read current and max backlight pwm values (`brightness` and `max_brightness` for amdgpu backlights respectively)
* Apply inverse `g(x)` to determine current perceived brightness
* Add percentage to linear brightness scale
* Apply `f(x)` to get back into the non-linear domain
* Scale real brightness \in [0;1] to [0;`max_brightness] and update backlight.

# Installation / Usage

Adjust the compile-time constants to suit your system. Then run the Makefile and store the binary in a location that's referenced by your path.


To increase the brightness of your backlight by "1%", run:

```
$ ./set_brightness +1
```

Conversely, to decrease the brightness by "5%":

```
$ set_brightness -5
```
