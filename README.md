PDS2PNG
=======
pds2png converts color images stored in NASA's Planetary Data System format from .RED/.GRN/.BLU files to PNG files. It can for instance be used to convert raw true color images captured by the viking landers.o

Compiling
---------
Requires libpng.
Compile with either spank, make or 

```no-highlight
  gcc pds2png.c -o pds2png `pkg-config --libs --cflags libpng` -std=c99
```

Usage
-----
pds2png [RED file] [BLU file] [GRN file] [png output]

Example Images
--------------
Some example images converted with PDS2PNG
![Image1](https://github.com/noname22/pds2png/wiki/images/11I106.jpg)
![image2](https://github.com/noname22/pds2png/wiki/images/12B069.jpg)

More images can be found at NASA's Viking Lander EDR Image Archive

http://pds-geosciences.wustl.edu/missions/vlander/images.html
