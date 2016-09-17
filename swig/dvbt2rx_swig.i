/* -*- c++ -*- */

#define DVBT2RX_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "dvbt2rx_swig_doc.i"

%{
#include "dvbt2rx/p1_demod.h"
%}



%include "dvbt2rx/p1_demod.h"
GR_SWIG_BLOCK_MAGIC2(dvbt2rx, p1_demod);
