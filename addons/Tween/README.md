Tween
------

Implementation of the tween functions made available by [Robert Penner](http://www.robertpenner.com/easing/).
See `apps/examples/tween/` for all the implemented tween functions. You can walk through the different tween
functions by pressing 0,1,2,3,..,A,B,C,...,T.

The tween class is templated so if you implement the nessary mathematical operators (*, /, +, -), you can
tween any object you like.


````C
Tween<float> my_tween;
my_tween.set(0.0, 1000.0, 5000, 0); // tween from 0.0 to 1000 in 5000 millis

Tween<Color> my_tween;
Color from(0.0, 0.0, 1.0);
color to(1.0, 0.0, 0.0);
my_tween.set(from, to, 5000, 0); // tween from color `from` to color `to`
                     
````
