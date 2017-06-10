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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "gi_est_control_cc_impl.h"

namespace gr {
    namespace dvbt2rx {

        gi_est_control_cc::sptr
        gi_est_control_cc::make(boost::shared_ptr<gr::blocks::delay> delay,
                                boost::shared_ptr<gr::blocks::moving_average_cc> moving_avg,
                                boost::shared_ptr<gi_est_decider_b> gi_decider)
        {
            return gnuradio::get_initial_sptr
                    (new gi_est_control_cc_impl(delay, moving_avg, gi_decider));
        }

        /*
         * The private constructor
         */
        gi_est_control_cc_impl::gi_est_control_cc_impl(boost::shared_ptr<gr::blocks::delay> delay,
                                                       boost::shared_ptr<gr::blocks::moving_average_cc> moving_avg,
                                                       boost::shared_ptr<gi_est_decider_b> gi_decider)
                : gr::block("gi_est_control_cc",
                                 gr::io_signature::make(1, 1, sizeof(gr_complex)),
                                 gr::io_signature::make(1, 1, sizeof(gr_complex))),
                  d_delay(delay),
                  d_moving_avg(moving_avg),
                  d_gi_decider(gi_decider)
        {

        }

        /*
         * Our virtual destructor.
         */
        gi_est_control_cc_impl::~gi_est_control_cc_impl() {
        }


        void
        gi_est_control_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required) {
            ninput_items_required[0] = noutput_items;
        }

        int
        gi_est_control_cc_impl::general_work(int noutput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
        {
            const gr_complex *in = (const gr_complex *) input_items[0];
            gr_complex *out = (gr_complex *) output_items[0];

            if(d_finished)
                return 0;

            std::vector<tag_t> v;
            get_tags_in_window(v, 0, 0, noutput_items, pmt::mp("p1_start"));


            if (v.size() && d_recv_p1 == false) {
                pmt::pmt_t dict = v[0].value;
                int s2 = (int) pmt::to_long(pmt::dict_ref(dict, pmt::mp("S2"), pmt::from_long(-1)));
                int s1 = (int) pmt::to_long(pmt::dict_ref(dict, pmt::mp("S1"), pmt::from_long(-1)));
                int fft_len;

                switch(s1){
                    case t2common::T2BASE_SISO:
                    case t2common::T2BASE_MISO:
                        fft_len = t2common::p1_s2_1_t2base_fft_size[s2];
                        break;
                    case t2common::T2LITE_SISO:
                    case t2common::T2LITE_MISO:
                        fft_len = t2common::p1_s2_1_t2lite_fft_size[s2];
                        break;
                    default:
                        printf("GI-est: skip non T2 part\n");
                        fft_len = 0;
                        break;
                }

                if(fft_len != 0){
                    printf("GI-est, received new p1 signal\n");
                    d_recv_p1 = true;
                    (*d_delay).set_dly(fft_len);
                   // (*gi_est_decider_b).set_fftlen();
                    //TODO: dynamic moving average length?
                    (*d_moving_avg).set_length(fft_len * 1/8);
                    return 0;
                }
            }

            if(d_recv_p1){
                memcpy(out, in , noutput_items*sizeof(gr_complex));
                return noutput_items;
            }

            return 0;
        }

    } /* namespace dvbt2rx */
} /* namespace gr */

