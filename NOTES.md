# Miles' Notes

- If moving around / adding / removing ugens in config.mk or h/ugens.h make
  sure to `make clean` then `make install`; this wipes out the ugens.h

- Make sure to build against soundpipe with the type SPFLOAT defined as
  double, NOT float and additionally build soundpipe with USE_DOUBLE=1.
  In past testing reading a table past around 18s or so produces artifacts
  without this.
