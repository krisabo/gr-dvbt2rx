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


#ifndef INCLUDED_DVBT2RX_GI_EST_CONTROL_CC_H
#define INCLUDED_DVBT2RX_GI_EST_CONTROL_CC_H

#include <dvbt2rx/api.h>
#include <gnuradio/block.h>

#include <gnuradio/blocks/delay.h>
#include <gnuradio/blocks/moving_average_cc.h>

namespace gr {
  namespace dvbt2rx {

    /*!
     * \brief <+description of block+>
     * \ingroup dvbt2rx
     *
     */
    class DVBT2RX_API gi_est_control_cc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<gi_est_control_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dvbt2rx::gi_est_control_cc.
       *
       * To avoid accidental use of raw pointers, dvbt2rx::gi_est_control_cc's
       * constructor is in a private implementation
       * class. dvbt2rx::gi_est_control_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(boost::shared_ptr<gr::blocks::delay> delay,
                       boost::shared_ptr<gr::blocks::moving_average_cc> moving_avg);
    };

  } // namespace dvbt2rx
} // namespace gr

#endif /* INCLUDED_DVBT2RX_GI_EST_CONTROL_CC_H */

