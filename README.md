```
sudo apt-get install libjack-jackd2-dev debhelper
./build-deb.sh
sudo dpkg -i jack-sine_1.0.0-1_armhf.deb
```

```
Usage: jack_sine frequency amplitude [-n]

        frequency - a value between 0.0 - 20000.0
        amplitude - a value between 0.0 - 1.0
        n         - don't autoconnect to playback ports (optional)

The order of the arguments is important!

Version: 1.0.0
```
