/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_DVBT2RX_GI_EST_CONTROL_CC_IMPL_H
#define INCLUDED_DVBT2RX_GI_EST_CONTROL_CC_IMPL_H

#include <dvbt2rx/gi_est_control_cc.h>
#include <dvbt2rx/t2common.h>

namespace gr {
  namespace dvbt2rx {

    class gi_est_control_cc_impl : public gi_est_control_cc
    {
     private:

        typedef enum{
            T2BASE_SISO = 0,
            T2BASE_MISO = 1,
            NONT2       = 2,
            T2LITE_SISO = 3,
            T2LITE_MISO = 4,
            resvd0      = 5,
            resvd1      = 6,
            resvd2      = 7,
        }p1_s1;


        int d_s2;
        int d_s1;
        int d_s2_1;
        int d_fftlen;
        int d_corr_window;

        bool d_busy;
        bool d_finished;

        static const int p1_s2_1_t2base_fft_size[8];
        static const int p1_s2_1_t2lite_fft_size[8];
        static const int p1_s2_1_t2base_cp_corr_window[8];
        static const int p1_s2_1_t2lite_cp_corr_window[8];

        static const int p1_s2_1_t2base_valid_gi[8][7];
        static const int p1_s2_1_t2lite_valid_gi[8][7];

        boost::shared_ptr<gr::blocks::delay> d_delay;
        boost::shared_ptr<gr::blocks::moving_average_cc> d_moving_avg;

        void msg_cfg_in(pmt::pmt_t msg);


     public:
      gi_est_control_cc_impl(boost::shared_ptr<gr::blocks::delay> delay,
                             boost::shared_ptr<gr::blocks::moving_average_cc> moving_avg);
      ~gi_est_control_cc_impl();

        void forecast (int noutput_items, gr_vector_int &ninput_items_required)  ;

      // Where all the action really happens
      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dvbt2rx
} // namespace gr

#endif /* INCLUDED_DVBT2RX_GI_EST_CONTROL_CC_IMPL_H */

