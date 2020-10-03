# Miles' Notes

- If moving around / adding / removing ugens in config.mk or h/ugens.h make
  sure to `make clean` then `make install`; this wipes out the ugens.h

- Make sure to build against soundpipe with the type SPFLOAT defined as
  double, NOT float and additionally build soundpipe with USE_DOUBLE=1.
  In past testing reading a table past around 18s or so produces artifacts
  without this.

- Certain ugens such as buthp/butlp are built seemingly without antialiasing
  support. This can cause artifacts that sound basically like a bitcrusher
  effect (e.g. downsampling resolution) after some time using buthp.
  So really Sporth should always be run with SR=(2 * MAX loadfile SR).
  So if the maximum SR used in all loadfile(2)s is 44100, use SR for Sporth
  as 88200.
