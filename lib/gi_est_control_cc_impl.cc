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
        gi_est_control_cc::make(boost::shared_ptr <gr::blocks::delay> delay,
                                boost::shared_ptr <gr::blocks::moving_average_cc> moving_avg) {
            return gnuradio::get_initial_sptr
                    (new gi_est_control_cc_impl(delay, moving_avg));
        }

        /*
         * The private constructor
         */
        gi_est_control_cc_impl::gi_est_control_cc_impl(boost::shared_ptr <gr::blocks::delay> delay,
                                                       boost::shared_ptr <gr::blocks::moving_average_cc> moving_avg)
                : gr::block("gi_est_control_cc",
                            gr::io_signature::make(1, 1, sizeof(gr_complex)),
                            gr::io_signature::make(1, 1, sizeof(gr_complex))),
                  d_delay(delay),
                  d_moving_avg(moving_avg)
        {
            d_s2 = -1;
            d_s2_1 = -1;
            d_s1 = -1;
            d_fftlen = -1;
            d_corr_window = -1;
            d_finished = false;
            d_busy = false;

            message_port_register_in(pmt::mp("cfg_in"));
            set_msg_handler(pmt::mp("cfg_in"), boost::bind(&gi_est_control_cc_impl::msg_cfg_in, this, _1));
            message_port_register_in(pmt::mp("cfg_out"));


            //needed because of delayed signal
            set_min_output_buffer(100000);
        }

        /*
         * Our virtual destructor.
         */
        gi_est_control_cc_impl::~gi_est_control_cc_impl() {
        }


        void
        gi_est_control_cc_impl::msg_cfg_in(pmt::pmt_t msg){

        }

        void
        gi_est_control_cc_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
            ninput_items_required[0] = noutput_items;
        }

        int
        gi_est_control_cc_impl::general_work(int noutput_items,
                                             gr_vector_int &ninput_items,
                                             gr_vector_const_void_star &input_items,
                                             gr_vector_void_star &output_items) {
            const gr_complex *in = (const gr_complex *) input_items[0];
            gr_complex *out = (gr_complex *) output_items[0];
            int nin = ninput_items[0];

            //consume everything if finished, but no output
            if(d_finished) {
                consume_each(nin);
                return 0;
            }

            std::vector<tag_t> v;
            get_tags_in_window(v, 0, 0, noutput_items, pmt::mp("p1_start"));

            if(d_busy){
                memcpy(out, in, nin*sizeof(gr_complex));
                consume_each(nin);
                return nin;
            }


            if (v.size() ) {
                pmt::pmt_t dict = v[0].value;
                d_s2 = (int) pmt::to_long(pmt::dict_ref(dict, pmt::mp("S2"), pmt::from_long(-1)));
                d_s1 = (int) pmt::to_long(pmt::dict_ref(dict, pmt::mp("S1"), pmt::from_long(-1)));
                d_s2_1 = d_s2 >> 1;

                switch(d_s1){
                    case T2BASE_SISO:
                    case T2BASE_MISO:
                        d_fftlen = p1_s2_1_t2base_fft_size[d_s2_1];
                        d_corr_window = p1_s2_1_t2base_cp_corr_window[d_s2_1];
                        break;
                    case T2LITE_SISO:
                    case T2LITE_MISO:
                        d_fftlen = p1_s2_1_t2lite_fft_size[d_s2_1];
                        d_corr_window = p1_s2_1_t2lite_cp_corr_window[d_s2_1];
                        break;
                    default:
                        printf("GI-est: skip non T2 part\n");
                        d_fftlen = 0;
                        break;
                }

                if(d_fftlen != 0){
                    printf("GI-est, received new p1 signal\n");

                    (*d_delay).set_dly(d_fftlen);
                    (*d_moving_avg).set_length(d_corr_window);
                    (*d_moving_avg).set_scale(gr_complex(1./d_corr_window,0));

                    d_busy = true;
                    consume_each(0);
                    return 0;
                }
            }


            consume_each(nin);
            return 0;
        }


        const int gi_est_control_cc_impl::p1_s2_1_t2base_fft_size[8] = {2048, 8192, 4096, 1024, 16384, 32768, 8192, 32768};
        const int gi_est_control_cc_impl::p1_s2_1_t2lite_fft_size[8] = {2048, 8192, 4096, 16384, 16384, 0, 8192, 0};

        //TODO: values
        const int gi_est_control_cc_impl::p1_s2_1_t2base_cp_corr_window[8] = {0, 0, 0, 0, 0, 0, 0, 2432};
        const int gi_est_control_cc_impl::p1_s2_1_t2lite_cp_corr_window[8] = {0, 0, 0, 0, 0, 0, 0, 0};


        const int gi_est_control_cc_impl::p1_s2_1_t2base_valid_gi[8][7]={0};
        const int gi_est_control_cc_impl::p1_s2_1_t2lite_valid_gi[8][7]={0};

    } /* namespace dvbt2rx */
} /* namespace gr */

