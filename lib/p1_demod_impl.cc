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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "p1_demod_impl.h"

namespace gr {
  namespace dvbt2rx {

    p1_demod::sptr
    p1_demod::make()
    {
      return gnuradio::get_initial_sptr
        (new p1_demod_impl());
    }

    /*
     * The private constructor
     */
    p1_demod_impl::p1_demod_impl()
      : gr::sync_block("p1_demod",
              gr::io_signature::make3(3, 3, sizeof(char),sizeof(gr_complex),sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
    	//make sure that enough samples are in the buffer
    	set_history(fft_len);

    	//initialize FFT
        d_in_fftw = (gr_complex*) fftwf_malloc(fft_len * sizeof(fftwf_complex));
        d_out_fftw = (gr_complex*) fftwf_malloc(fft_len * sizeof(fftwf_complex));
        d_fftw_plan = fftwf_plan_dft_1d(fft_len, reinterpret_cast<fftwf_complex*>(d_in_fftw),
                				reinterpret_cast<fftwf_complex*> (d_out_fftw), FFTW_FORWARD, FFTW_ESTIMATE);

        //variable used by volk
        int alig = volk_get_alignment();
        d_vec_tmp1_f = (float*) volk_malloc(2 * fft_len * sizeof(float), alig);
        d_vec_tmp0_f = (float*) volk_malloc(2 * fft_len * sizeof(float), alig);
//        d_energy_distribution = (float*) volk_malloc(fft_len * sizeof(float), alig);
//
//        //build energy distribution vector
//        memset(d_energy_distribution, 0, fft_len*sizeof(d_energy_distribution[0]));
//        for(int i=0; i<cnt_act_carriers; i++){
//        	d_energy_distribution[p1_active_carriers[i]] = 1;
//        }

    }

    /*
     * Our virtual destructor.
     */
    p1_demod_impl::~p1_demod_impl()
    {
        fftwf_destroy_plan(d_fftw_plan);
        fftwf_free(d_in_fftw);
        fftwf_free(d_out_fftw);
        volk_free(d_vec_tmp1_f);
        volk_free(d_vec_tmp0_f);
    }

    int
    p1_demod_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
    	const char *in_sync = (const char *) input_items[0];
    	const gr_complex *in_phase = (const gr_complex *) input_items[1];
    	const gr_complex *in_data = (const gr_complex *) input_items[2];
    	gr_complex *out = (gr_complex *) output_items[0];


    	memcpy(out, in_data, noutput_items*sizeof(gr_complex));


    	for(int i=0; i<noutput_items; i++){
    		if(in_sync[i]){

    			//correct fractional frequency offset
    			float ffo = std::arg(in_phase[i]);
    			gr_complex phase_inc = std::polar(1.0F, -1.0F/fft_len*ffo);
    			volk_32fc_s32fc_rotatorpuppet_32fc(d_in_fftw, in_data+i, phase_inc, fft_len);

    			//to freq domain
    			fftwf_execute(d_fftw_plan);

    			int ifo;
    			cds_correlation(&ifo, d_out_fftw);

    			printf("cfo = ifo(%i) + ffo(%f) = %f\n", ifo, ffo/3.141/2, ifo+ffo);

    			//s=demod(in_data+i, ffo)


    			add_item_tag(0,nitems_written(0)+i, pmt::mp("p1_start"), pmt::mp("change_me"));

    		}
    	}

      return noutput_items;
    }

    /*
     * Estimates the position of the correlation peak. Correlation is done with the energy
     * distribution of the carriers and the energy/abs of the p1 in freq domain.
     *
     * This is also an estimation of the carrier integer frequency offset.
     *
     * Returns true if a peak was found, false if the p1 signal is not detected
     */
    bool p1_demod_impl::cds_correlation(int* ifo, const gr_complex* p1_freq_domain){



    	// energy in rx symbol
    	// d_vec_tmp1 = abs(p1_freq_domain)^2
    	volk_32fc_magnitude_squared_32f(d_vec_tmp1_f, p1_freq_domain, fft_len);

    	// fftshift
    	// d_vec_tmp0 = [d_vec_tmp1[512:1023], d_vec_tmp1[0:511]]
    	memcpy(d_vec_tmp0_f, d_vec_tmp1_f+fft_len/2, fft_len/2 * sizeof(d_vec_tmp0_f[0]));
    	memcpy(d_vec_tmp0_f+fft_len/2, d_vec_tmp1_f, fft_len/2 * sizeof(d_vec_tmp0_f[0]));

    	// Algorithm: punish when there is energy at frequencies where it shouldnt
    	// and delete high CW interferer (see implementation guide)
		// 1. calculate mean and stddev
		// 2. look for values above mean + 3*stddev, TODO: value=3?
		// 3. clip these values
		// 4. calculate new mean
		// 5. subtract that mean

    	float mean = 0;
    	float stddev = 0;

    	volk_32f_stddev_and_mean_32f_x2(&stddev, &mean, d_vec_tmp0_f, fft_len);
    	//printf("mean = %f, stddev = %f\n", mean, stddev);
    	for(int i=0; i<fft_len; i++){
    		if(d_vec_tmp0_f[i] > mean + 5 * stddev){
    			d_vec_tmp0_f[i] = mean + 5 * stddev;
    			//printf("strong interferer detected at freq: %i\n", i-fft_len/2);
    		}
    	}

    	volk_32f_accumulator_s32f(&mean, d_vec_tmp0_f, fft_len);
    	mean = mean/fft_len;

    	//printf("mean = %f\n", mean);
		for(int i=0; i<fft_len; i++){
			d_vec_tmp0_f[i] -= mean;
		}

    	//printf("mean = %f, stddev = %f\n", mean, stddev);

    	//d_vec_tmp1 = 0
    	memset(d_vec_tmp1_f, 0, fft_len*sizeof(float));

    	//not all the range from -1024 to 1024 is used
    	//from i=-512 to 511 correlation
    	for(int i=0; i<fft_len; i++){
    		//iterate over active carriers
    		for(int k=0; k<cnt_act_carriers; k++){
    			int carrier = p1_active_carriers[k]+i+86-fft_len/2;	//86 because of offset (see standard)

    			if(carrier > 0 && carrier < fft_len)
    				d_vec_tmp1_f[i] += d_vec_tmp0_f[carrier];
    		}
		//if(i>500 && i<520)
		//	printf("corr[%i] = %f\n", i-fft_len/2, *(d_vec_tmp1_f+i));
    	}

    	//detect max peak
    	unsigned int max_index;
    	float max;
    	volk_32f_index_max_32u(&max_index, d_vec_tmp1_f, fft_len);
    	max = d_vec_tmp1_f[max_index];
    	d_vec_tmp1_f[max_index] = -3.4028e+38;

    	//detect second max peak
    	unsigned int second_max_index;
    	float second_max;
    	volk_32f_index_max_32u(&second_max_index, d_vec_tmp1_f, fft_len);
    	second_max = d_vec_tmp1_f[second_max_index];

    	//printf("max_index: %i, secondmax_index: %i\n", max_index, second_max_index);
    	//printf("max: %f, secondmax: %f\n\n", max, second_max);
    	//return ifo
    	*ifo = max_index-fft_len/2;

    	//TODO: value=2?
    	if(max > 2*second_max){
    		return true;
    	}
    	return false;
    }



    const int p1_demod_impl::p1_active_carriers[cnt_act_carriers] =
    {
        44, 45, 47, 51, 54, 59, 62, 64, 65, 66, 70, 75, 78, 80, 81, 82, 84, 85, 87, 88, 89, 90,
        94, 96, 97, 98, 102, 107, 110, 112, 113, 114, 116, 117, 119, 120, 121, 122, 124,
        125, 127, 131, 132, 133, 135, 136, 137, 138, 142, 144, 145, 146, 148, 149, 151,
        152, 153, 154, 158, 160, 161, 162, 166, 171,

        172, 173, 175, 179, 182, 187, 190, 192, 193, 194, 198, 203, 206, 208, 209, 210,
        212, 213, 215, 216, 217, 218, 222, 224, 225, 226, 230, 235, 238, 240, 241, 242,
        244, 245, 247, 248, 249, 250, 252, 253, 255, 259, 260, 261, 263, 264, 265, 266,
        270, 272, 273, 274, 276, 277, 279, 280, 281, 282, 286, 288, 289, 290, 294, 299,
        300, 301, 303, 307, 310, 315, 318, 320, 321, 322, 326, 331, 334, 336, 337, 338,
        340, 341, 343, 344, 345, 346, 350, 352, 353, 354, 358, 363, 364, 365, 367, 371,
        374, 379, 382, 384, 385, 386, 390, 395, 396, 397, 399, 403, 406, 411, 412, 413,
        415, 419, 420, 421, 423, 424, 425, 426, 428, 429, 431, 435, 438, 443, 446, 448,
        449, 450, 454, 459, 462, 464, 465, 466, 468, 469, 471, 472, 473, 474, 478, 480,
        481, 482, 486, 491, 494, 496, 497, 498, 500, 501, 503, 504, 505, 506, 508, 509,
        511, 515, 516, 517, 519, 520, 521, 522, 526, 528, 529, 530, 532, 533, 535, 536,
        537, 538, 542, 544, 545, 546, 550, 555, 558, 560, 561, 562, 564, 565, 567, 568,
        569, 570, 572, 573, 575, 579, 580, 581, 583, 584, 585, 586, 588, 589, 591, 595,
        598, 603, 604, 605, 607, 611, 612, 613, 615, 616, 617, 618, 622, 624, 625, 626,
        628, 629, 631, 632, 633, 634, 636, 637, 639, 643, 644, 645, 647, 648, 649, 650,
        654, 656, 657, 658, 660, 661, 663, 664, 665, 666, 670, 672, 673, 674, 678, 683,

        684, 689, 692, 696, 698, 699, 701, 702, 703, 704, 706, 707, 708,
        712, 714, 715, 717, 718, 719, 720, 722, 723, 725, 726, 727, 729,
        733, 734, 735, 736, 738, 739, 740, 744, 746, 747, 748, 753, 756,
        760, 762, 763, 765, 766, 767, 768, 770, 771, 772, 776, 778, 779,
        780, 785, 788, 792, 794, 795, 796, 801, 805, 806, 807, 809
    };

    const unsigned char p1_demod_impl::s1_modulation_patterns[8][8] =
    {
        {0x12, 0x47, 0x21, 0x74, 0x1D, 0x48, 0x2E, 0x7B},
        {0x47, 0x12, 0x74, 0x21, 0x48, 0x1D, 0x7B, 0x2E},
        {0x21, 0x74, 0x12, 0x47, 0x2E, 0x7B, 0x1D, 0x48},
        {0x74, 0x21, 0x47, 0x12, 0x7B, 0x2E, 0x48, 0x1D},
        {0x1D, 0x48, 0x2E, 0x7B, 0x12, 0x47, 0x21, 0x74},
        {0x48, 0x1D, 0x7B, 0x2E, 0x47, 0x12, 0x74, 0x21},
        {0x2E, 0x7B, 0x1D, 0x48, 0x21, 0x74, 0x12, 0x47},
        {0x7B, 0x2E, 0x48, 0x1D, 0x74, 0x21, 0x47, 0x12}
    };

    const unsigned char p1_demod_impl::s2_modulation_patterns[16][32] =
    {
        {0x12, 0x1D, 0x47, 0x48, 0x21, 0x2E, 0x74, 0x7B, 0x1D, 0x12, 0x48, 0x47, 0x2E, 0x21, 0x7B, 0x74,
         0x12, 0xE2, 0x47, 0xB7, 0x21, 0xD1, 0x74, 0x84, 0x1D, 0xED, 0x48, 0xB8, 0x2E, 0xDE, 0x7B, 0x8B},
        {0x47, 0x48, 0x12, 0x1D, 0x74, 0x7B, 0x21, 0x2E, 0x48, 0x47, 0x1D, 0x12, 0x7B, 0x74, 0x2E, 0x21,
         0x47, 0xB7, 0x12, 0xE2, 0x74, 0x84, 0x21, 0xD1, 0x48, 0xB8, 0x1D, 0xED, 0x7B, 0x8B, 0x2E, 0xDE},
        {0x21, 0x2E, 0x74, 0x7B, 0x12, 0x1D, 0x47, 0x48, 0x2E, 0x21, 0x7B, 0x74, 0x1D, 0x12, 0x48, 0x47,
         0x21, 0xD1, 0x74, 0x84, 0x12, 0xE2, 0x47, 0xB7, 0x2E, 0xDE, 0x7B, 0x8B, 0x1D, 0xED, 0x48, 0xB8},
        {0x74, 0x7B, 0x21, 0x2E, 0x47, 0x48, 0x12, 0x1D, 0x7B, 0x74, 0x2E, 0x21, 0x48, 0x47, 0x1D, 0x12,
         0x74, 0x84, 0x21, 0xD1, 0x47, 0xB7, 0x12, 0xE2, 0x7B, 0x8B, 0x2E, 0xDE, 0x48, 0xB8, 0x1D, 0xED},
        {0x1D, 0x12, 0x48, 0x47, 0x2E, 0x21, 0x7B, 0x74, 0x12, 0x1D, 0x47, 0x48, 0x21, 0x2E, 0x74, 0x7B,
         0x1D, 0xED, 0x48, 0xB8, 0x2E, 0xDE, 0x7B, 0x8B, 0x12, 0xE2, 0x47, 0xB7, 0x21, 0xD1, 0x74, 0x84},
        {0x48, 0x47, 0x1D, 0x12, 0x7B, 0x74, 0x2E, 0x21, 0x47, 0x48, 0x12, 0x1D, 0x74, 0x7B, 0x21, 0x2E,
         0x48, 0xB8, 0x1D, 0xED, 0x7B, 0x8B, 0x2E, 0xDE, 0x47, 0xB7, 0x12, 0xE2, 0x74, 0x84, 0x21, 0xD1},
        {0x2E, 0x21, 0x7B, 0x74, 0x1D, 0x12, 0x48, 0x47, 0x21, 0x2E, 0x74, 0x7B, 0x12, 0x1D, 0x47, 0x48,
         0x2E, 0xDE, 0x7B, 0x8B, 0x1D, 0xED, 0x48, 0xB8, 0x21, 0xD1, 0x74, 0x84, 0x12, 0xE2, 0x47, 0xB7},
        {0x7B, 0x74, 0x2E, 0x21, 0x48, 0x47, 0x1D, 0x12, 0x74, 0x7B, 0x21, 0x2E, 0x47, 0x48, 0x12, 0x1D,
         0x7B, 0x8B, 0x2E, 0xDE, 0x48, 0xB8, 0x1D, 0xED, 0x74, 0x84, 0x21, 0xD1, 0x47, 0xB7, 0x12, 0xE2},
        {0x12, 0xE2, 0x47, 0xB7, 0x21, 0xD1, 0x74, 0x84, 0x1D, 0xED, 0x48, 0xB8, 0x2E, 0xDE, 0x7B, 0x8B,
         0x12, 0x1D, 0x47, 0x48, 0x21, 0x2E, 0x74, 0x7B, 0x1D, 0x12, 0x48, 0x47, 0x2E, 0x21, 0x7B, 0x74},
        {0x47, 0xB7, 0x12, 0xE2, 0x74, 0x84, 0x21, 0xD1, 0x48, 0xB8, 0x1D, 0xED, 0x7B, 0x8B, 0x2E, 0xDE,
         0x47, 0x48, 0x12, 0x1D, 0x74, 0x7B, 0x21, 0x2E, 0x48, 0x47, 0x1D, 0x12, 0x7B, 0x74, 0x2E, 0x21},
        {0x21, 0xD1, 0x74, 0x84, 0x12, 0xE2, 0x47, 0xB7, 0x2E, 0xDE, 0x7B, 0x8B, 0x1D, 0xED, 0x48, 0xB8,
         0x21, 0x2E, 0x74, 0x7B, 0x12, 0x1D, 0x47, 0x48, 0x2E, 0x21, 0x7B, 0x74, 0x1D, 0x12, 0x48, 0x47},
        {0x74, 0x84, 0x21, 0xD1, 0x47, 0xB7, 0x12, 0xE2, 0x7B, 0x8B, 0x2E, 0xDE, 0x48, 0xB8, 0x1D, 0xED,
         0x74, 0x7B, 0x21, 0x2E, 0x47, 0x48, 0x12, 0x1D, 0x7B, 0x74, 0x2E, 0x21, 0x48, 0x47, 0x1D, 0x12},
        {0x1D, 0xED, 0x48, 0xB8, 0x2E, 0xDE, 0x7B, 0x8B, 0x12, 0xE2, 0x47, 0xB7, 0x21, 0xD1, 0x74, 0x84,
         0x1D, 0x12, 0x48, 0x47, 0x2E, 0x21, 0x7B, 0x74, 0x12, 0x1D, 0x47, 0x48, 0x21, 0x2E, 0x74, 0x7B},
        {0x48, 0xB8, 0x1D, 0xED, 0x7B, 0x8B, 0x2E, 0xDE, 0x47, 0xB7, 0x12, 0xE2, 0x74, 0x84, 0x21, 0xD1,
         0x48, 0x47, 0x1D, 0x12, 0x7B, 0x74, 0x2E, 0x21, 0x47, 0x48, 0x12, 0x1D, 0x74, 0x7B, 0x21, 0x2E},
        {0x2E, 0xDE, 0x7B, 0x8B, 0x1D, 0xED, 0x48, 0xB8, 0x21, 0xD1, 0x74, 0x84, 0x12, 0xE2, 0x47, 0xB7,
         0x2E, 0x21, 0x7B, 0x74, 0x1D, 0x12, 0x48, 0x47, 0x21, 0x2E, 0x74, 0x7B, 0x12, 0x1D, 0x47, 0x48},
        {0x7B, 0x8B, 0x2E, 0xDE, 0x48, 0xB8, 0x1D, 0xED, 0x74, 0x84, 0x21, 0xD1, 0x47, 0xB7, 0x12, 0xE2,
         0x7B, 0x74, 0x2E, 0x21, 0x48, 0x47, 0x1D, 0x12, 0x74, 0x7B, 0x21, 0x2E, 0x47, 0x48, 0x12, 0x1D}
    };

  } /* namespace dvbt2rx */
} /* namespace gr */

