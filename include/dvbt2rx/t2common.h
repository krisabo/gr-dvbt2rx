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


#ifndef INCLUDED_DVBT2RX_T2COMMON_H
#define INCLUDED_DVBT2RX_T2COMMON_H

#include <dvbt2rx/api.h>

namespace gr {
  namespace dvbt2rx {

    /*!
     * \brief <+description+>
     *
     */
    struct DVBT2RX_API t2common
    {

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


        typedef enum{
            CP_1_128    = 0,
            CP_1_32     = 1,
            CP_1_16     = 2,
            CP_19_256   = 3,
            CP_1_8      = 4,
            CP_19_128   = 5,
            CP_1_4      = 6,
        }cp_interval;

        static const int p1_s2_1_t2base_fft_size[8];
        static const int p1_s2_1_t2lite_fft_size[8];

        static const int p1_s2_1_t2base_valid_gi[8][7];
        static const int p1_s2_1_t2lite_valid_gi[8][7];


        static const int p1_active_carriers[384];
        static const unsigned char s1_modulation_patterns[8][8];
        static const unsigned char s2_modulation_patterns[16][32];





    };

  } // namespace dvbt2rx
} // namespace gr

#endif /* INCLUDED_DVBT2RX_T2COMMON_H */

