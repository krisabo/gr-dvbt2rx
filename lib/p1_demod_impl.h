/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DVBT2RX_P1_DEMOD_IMPL_H
#define INCLUDED_DVBT2RX_P1_DEMOD_IMPL_H

#include <dvbt2rx/p1_demod.h>
#include <volk/volk.h>
#include <fftw3.h>
#include <cmath>

namespace gr {
  namespace dvbt2rx {

    class p1_demod_impl : public p1_demod
    {
     private:
    	const static int fft_len = 1024;
    	const static int cnt_act_carriers = 384;

        gr_complex *d_in_fftw;
        gr_complex *d_out_fftw;
        fftwf_plan d_fftw_plan;

        float *d_vec_tmp1_f;
        float *d_vec_tmp0_f;
//        float *d_energy_distribution;


        const static int p1_active_carriers[384];
        const static unsigned char s1_modulation_patterns[8][8];
        const static unsigned char s2_modulation_patterns[16][32];

        bool cds_correlation(int* ifo,const gr_complex* p1_freq_domain);
        //static int void demod_p1();

     public:
      p1_demod_impl();
      ~p1_demod_impl();

      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

    };

  } // namespace dvbt2rx
} // namespace gr

#endif /* INCLUDED_DVBT2RX_P1_DEMOD_IMPL_H */

