Noise
-----
Simple example showing the Noise::noise() features. 

The Noise::noise() function returns a simplex noise value for the given 
input value. Make sure that the input value is in a range between `0.0f` and `1.0`.
You can use higher values but the noise will be repeated and denser.
The returned value will be in a range between `-1.0f` and `1.0f`. Note that due
to the nature of simplex noise the returned values will be more between `-0.6f` and `0.6f`

The Noise::unoise() function returns a values in the range between `0.0` and `1.0f`. 
Note that due to the nature of simplex noise the returned values will be more between `0.2f` and `0.8f`

