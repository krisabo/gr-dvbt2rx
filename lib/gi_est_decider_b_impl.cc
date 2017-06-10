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
 * peak
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "gi_est_decider_b_impl.h"

namespace gr {
  namespace dvbt2rx {

    gi_est_decider_b::sptr
    gi_est_decider_b::make()
    {
      return gnuradio::get_initial_sptr
        (new gi_est_decider_b_impl());
    }

    /*
     * The private constructor
     */
    gi_est_decider_b_impl::gi_est_decider_b_impl()
      : gr::sync_block("gi_est_decider_b",
              gr::io_signature::make(2, 2, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0))
    {
        message_port_register_out(pmt::mp("gi_est"));
    }

    /*
     * Our virtual destructor.
     */
    gi_est_decider_b_impl::~gi_est_decider_b_impl()
    {
    }

    int
    gi_est_decider_b_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dvbt2rx */
} /* namespace gr */

