/* -*- c++ -*- */

#define DVBT2RX_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "dvbt2rx_swig_doc.i"

%{
#include "dvbt2rx/p1_demod.h"
#include "dvbt2rx/gi_est_decider.h"
#include "dvbt2rx/gi_est_control_cc.h"
%}

%include "dvbt2rx/p1_demod.h"
GR_SWIG_BLOCK_MAGIC2(dvbt2rx, p1_demod);
%include "dvbt2rx/gi_est_decider.h"
GR_SWIG_BLOCK_MAGIC2(dvbt2rx, gi_est_decider);
%include "dvbt2rx/gi_est_control_cc.h"
GR_SWIG_BLOCK_MAGIC2(dvbt2rx, gi_est_control_cc);

