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

        //variables used by volk
        int alig = volk_get_alignment();
        d_vec_tmp2_f = (float*) volk_malloc(2 * fft_len * sizeof(float), alig);
        d_vec_tmp1_f = (float*) volk_malloc(2 * fft_len * sizeof(float), alig);
        d_vec_tmp0_f = (float*) volk_malloc(2 * fft_len * sizeof(float), alig);

        init_p1_scramble_seq();

    }


    p1_demod_impl::~p1_demod_impl()
    {
        fftwf_destroy_plan(d_fftw_plan);
        fftwf_free(d_in_fftw);
        fftwf_free(d_out_fftw);
        volk_free(d_vec_tmp2_f);
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
    			correct_ffo(ffo, d_in_fftw, in_data+i);

    			//to freq domain
    			fftwf_execute(d_fftw_plan);
    			fftshift(d_in_fftw, d_out_fftw);

    			detect_interferer_and_clip(d_out_fftw, d_vec_tmp2_f, d_in_fftw);

    			int ifo;
    			if(cds_correlation(&ifo, d_vec_tmp2_f)) {
        			correct_ifo(ifo, d_in_fftw, d_out_fftw);

        			int s1, s2;
        			if(demod(&s1, &s2, d_in_fftw)){
        				float cfo = ifo + ffo/M_PI/2;

            			printf("Detected P1 Pilot: cfo = ifo(%i) + ffo(%f) = %f, offset: %lu, s1: %i, s2: %i\n",
            					ifo, ffo/M_PI/2, cfo, nitems_written(0)+i, s1, s2);

            			// add stream tag
            			pmt::pmt_t value = pmt::make_dict();
            			value = pmt::dict_add(value, pmt::mp("cfo"), pmt::from_float(cfo));
            			value = pmt::dict_add(value, pmt::mp("S1"), pmt::mp(s1));
            			value = pmt::dict_add(value, pmt::mp("S2"), pmt::mp(s2));
            			add_item_tag(0,nitems_written(0)+i, pmt::mp("p1_start"), value);

        			}
    			}
    		}
    	}

      return noutput_items;
    }



    /*
     * Demods the P1 Symbol.
     *
     * Takes a 1024 input vector and demods the S1, S2 values.
     *
     * Returns true if successful.
     *
     */
    bool p1_demod_impl::demod(int* s1, int* s2, const gr_complex* in){

    	//TODO:? use volk for speed opt.

    	gr_complex symbols[cnt_act_carriers];
    	gr_complex dbpsk_demod[cnt_act_carriers];
       	float s1_soft_bits [s1_bit_length];
       	float s2_soft_bits [s2_bit_length];

       	float s1_corr_results[s1_pattern];
       	float s2_corr_results[s2_pattern];



    	//demap
    	for(int i=0; i<cnt_act_carriers; i++){
    		symbols[i] = in[p1_active_carriers[i] + 86];
    	}

    	//descramble
    	for(int i=0; i<cnt_act_carriers; i++){
    		symbols[i] *= p1_scramble_seq[i];
    	}

    	//DBPSK soft demod
    	//note: S1 and S2 sequence always starts with a 0 bit, so initial DBPSK symbol is defined
    	dbpsk_demod[0] = gr_complex(1,0);
    	for(int i=1; i<cnt_act_carriers; i++){
    		dbpsk_demod[i] = (symbols[i-1] * std::conj(symbols[i]));
    	}

    	//add the double sent part of the s1 symbol to improve reception
    	for(int i=0; i<s1_bit_length; i++){
    		dbpsk_demod[i] += dbpsk_demod[i+s1_bit_length+s2_bit_length];
    	}

    	// see 10.2.2.7.3 of implementation guide -> ? what are they doing ?
    	// using a other decoding scheme

    	//demod to soft bits
    	for(int i=0; i<s1_bit_length; i++){
    		s1_soft_bits[i] += std::real(dbpsk_demod[i]);
    	}
    	for(int i=0; i<s2_bit_length; i++){
    		s2_soft_bits[i] += std::real(dbpsk_demod[i+s1_bit_length]);
    	}

    	//correlate s1 with the original known pattern
    	for(int i=0; i<s1_pattern; i++){
    		s1_corr_results[i] = 0;
    		for(int b=0; b<s1_bit_length; b++){
    			unsigned char bit =  (s1_modulation_patterns[i][b/8]  >> (7-b%8) & 1);
    			if(bit){
    				s1_corr_results[i] -= s1_soft_bits[b];
    			}else{
    				s1_corr_results[i] += s1_soft_bits[b];
    			}

    		}
    	}

    	//correlate s2 with the original known pattern
    	for(int i=0; i<s2_pattern; i++){
    		s2_corr_results[i] = 0;
    		for(int b=0; b<s2_bit_length; b++){
    			unsigned char bit =  (s2_modulation_patterns[i][b/8]  >> (7-b%8) & 1);
    			if(bit){
    				s2_corr_results[i] -= s2_soft_bits[b];
    			}else{
    				s2_corr_results[i] += s2_soft_bits[b];
    			}

    		}
    	}

    	//look for max peaks
    	float max=0, second_max=0;
    	int max_index = 0, second_max_index=0;

    	max_and_second_max(&max, &max_index, &second_max, &second_max_index, s1_corr_results, s1_pattern);
    	if(max > 5 * second_max){
    		*s1 = max_index;
    	}else{
    		return false;
    	}

    	max_and_second_max(&max, &max_index, &second_max, &second_max_index, s2_corr_results, s2_pattern);
      	if(max > 5 * second_max){
        	*s2 = max_index;
        }else{
        	return false;
        }

    	return true;

    }

    /*
     * Searches for the max and second max element in arr, where points > 2 are the number elements
     */
    void p1_demod_impl::max_and_second_max(float* max, int* max_index, float* sec_max, int* sec_max_index, float* arr, int points){
    	//TODO: faster with volk?

    	if(arr[0]>arr[1]){
        	*max = arr[0];
    		*max_index=0;
        	*sec_max = arr[1];
    		*sec_max_index=1;
    	}else{
        	*max = arr[1];
    		*max_index=1;
        	*sec_max = arr[0];
    		*sec_max_index=0;
    	}

    	for(int i=2; i<points; i++){
    		if(arr[i]>*max){
    			*sec_max = *max;
    			*sec_max_index = *max_index;
    			*max=arr[i];
    			*max_index=i;
    		}
    	}

    }


    /*
     * Swaps the two halfs of the input vector
     */
    void p1_demod_impl::fftshift(gr_complex* out, const gr_complex* in){
    	memcpy(out, in+fft_len/2, fft_len/2*sizeof(in[0]));
    	memcpy(out+fft_len/2, in, fft_len/2*sizeof(in[0]));

    }


    /*
     * Swaps the two halfs of the input vector
     */
    void p1_demod_impl::fftshift(float* out, const float* in){
    	memcpy(out, in+fft_len/2, fft_len/2*sizeof(in[0]));
    	memcpy(out+fft_len/2, in, fft_len/2*sizeof(in[0]));

    }


    /*
     * copied from gr-dvbt2
     */
    void p1_demod_impl::init_p1_scramble_seq()
    {
        int sr = 0x4e46;
        for (int i = 0; i < 384; i++)
        {
            int b = ((sr) ^ (sr >> 1)) & 1;
            if (b == 0)
            {
            	p1_scramble_seq[i] = 1;
            }
            else
            {
            	p1_scramble_seq[i] = -1;
            }
            sr >>= 1;
            if(b) sr |= 0x4000;
        }
    }

    /*
     * Clips very high peaks in the power spectrum and in the corresponding complex vector.
     *
     * Returns the clipped complex and magnitude float vectors.
     */
    void p1_demod_impl::detect_interferer_and_clip(gr_complex* out_fc, float* out_mag,  const gr_complex* in){

    	memcpy(out_fc, in, fft_len*sizeof(in[0]));

    	float mean = 0;
    	float stddev = 0;

    	// d_vec_tmp1 = abs(p1_freq_domain)
    	volk_32fc_magnitude_squared_32f(out_mag, in, fft_len);

    	// TODO: good Algorithm? useful? values?
    	// 1. calculate mean and stddev
    	// 2. look for values above mean + 5*stddev
    	// 3. clip these values

    	volk_32f_stddev_and_mean_32f_x2(&stddev, &mean, out_mag, fft_len);

    	for(int i=0; i<fft_len; i++){
    		if(out_mag[i] > mean + 5 * stddev){
    			out_mag[i] = mean + 3 * stddev;
//    			out_fc[i] = out_fc[i] / std::abs(out_fc[i]) * mean;
//    			printf("strong interferer detected at freq: %i\n", i-fft_len/2);
    		}
    	}
    }


    /*
     * Estimates the position of the correlation peak in frequency domain and therefore the IFO.
     * Correlation is done with the energy distribution of the carriers and
     * the energy/abs of the p1 in freq domain.
     *
     * Returns true if a peak was found, false if the p1 signal is not detected.
     */
    bool p1_demod_impl::cds_correlation(int* ifo, const float* p1_magnitude){

    	// punish when there is energy at frequencies where it shouldnt
    	// therefore subtract the mean
     	float mean = 0;
    	volk_32f_accumulator_s32f(&mean, p1_magnitude, fft_len);
    	mean = mean/fft_len;

		for(int i=0; i<fft_len; i++){
			d_vec_tmp0_f[i] = p1_magnitude[i] - mean;
		}


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
    	}

    	//detect max peak
    	int max_index, second_max_index;
		float max, second_max;
       	max_and_second_max(&max, &max_index, &second_max, &second_max_index, d_vec_tmp1_f, fft_len);

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


    void p1_demod_impl::correct_ffo(float ffo, gr_complex* out, const gr_complex* in){
		gr_complex phase_inc = std::polar(1.0F, -1.0F/fft_len*ffo);
		volk_32fc_s32fc_rotatorpuppet_32fc(out, in, phase_inc, fft_len);
    }


    void p1_demod_impl::correct_ifo(int ifo, gr_complex* out, const gr_complex* in){
    	if(ifo==0)
    		memcpy(out, in, fft_len*sizeof(out[0]));

    	if(ifo>=0){
    		memcpy(out, in+ifo, (fft_len-ifo)*sizeof(out[0]));
    		memset(out+fft_len-ifo, 0, ifo*sizeof(out[0]) );
    	}
    	else{
    		memcpy(out-ifo, in, (fft_len+ifo)*sizeof(out[0]));
    		memset(out, 0, -ifo*sizeof(out[0]) );

    	}
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


