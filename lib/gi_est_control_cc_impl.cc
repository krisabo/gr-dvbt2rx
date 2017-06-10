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
                            boost::shared_ptr<gr::blocks::peak_detector2_fb> peak,
                            boost::shared_ptr<gr::blocks::moving_average_cc> moving_avg,
                            boost::shared_ptr<gi_est_decider_b> gi_decider)
    {
      return gnuradio::get_initial_sptr
        (new gi_est_control_cc_impl(delay, peak, moving_avg, gi_decider));
    }

    /*
     * The private constructor
     */
    gi_est_control_cc_impl::gi_est_control_cc_impl(boost::shared_ptr<gr::blocks::delay> delay,
                                           boost::shared_ptr<gr::blocks::peak_detector2_fb> peak,
                                           boost::shared_ptr<gr::blocks::moving_average_cc> moving_avg,
                                           boost::shared_ptr<gi_est_decider_b> gi_decider)
      : gr::sync_block("gi_est_control_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {}

    /*
     * Our virtual destructor.
     */
    gi_est_control_cc_impl::~gi_est_control_cc_impl()
    {
    }

    int
    gi_est_control_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        std::vector<tag_t> v;
        get_tags_in_window(v, 0, 0, noutput_items, pmt::mp("p1_start"));

        if(v.size()){
            pmt::pmt_t dict = v[0].value;
            int s2_tmp = (int)pmt::to_long(pmt::dict_ref(dict, pmt::mp("S2"), pmt::from_long(-1)));

            if(s2_tmp >= 0 && s2_tmp <= 15 && s2_tmp != d_s2){
                printf("GI-est, received new p1 signal\n");
                d_recv_p1 = true;
            }
        }



        return noutput_items;
    }

  } /* namespace dvbt2rx */
} /* namespace gr */

