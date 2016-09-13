#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: P1 detection correlator test
# Author: Kristian Maier
# Description: Correlator as presented in Implementation Guide
# Generated: Wed Sep 14 01:06:44 2016
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import numpy as np
import sip
import sys


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "P1 detection correlator test")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("P1 detection correlator test")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.T_A = T_A = 1024
        self.samp_rate = samp_rate = 8e6
        self.noise_amplitude = noise_amplitude = 0
        self.eps = eps = 0
        self.T_R = T_R = T_A
        self.T_C = T_C = 542
        self.T_B = T_B = 482

        ##################################################
        # Blocks
        ##################################################
        self._noise_amplitude_tool_bar = Qt.QToolBar(self)
        self._noise_amplitude_tool_bar.addWidget(Qt.QLabel("noise_amplitude"+": "))
        self._noise_amplitude_line_edit = Qt.QLineEdit(str(self.noise_amplitude))
        self._noise_amplitude_tool_bar.addWidget(self._noise_amplitude_line_edit)
        self._noise_amplitude_line_edit.returnPressed.connect(
        	lambda: self.set_noise_amplitude(eng_notation.str_to_num(str(self._noise_amplitude_line_edit.text().toAscii()))))
        self.top_layout.addWidget(self._noise_amplitude_tool_bar)
        self._eps_tool_bar = Qt.QToolBar(self)
        self._eps_tool_bar.addWidget(Qt.QLabel('freq_offset'+": "))
        self._eps_line_edit = Qt.QLineEdit(str(self.eps))
        self._eps_tool_bar.addWidget(self._eps_line_edit)
        self._eps_line_edit.returnPressed.connect(
        	lambda: self.set_eps(eng_notation.str_to_num(str(self._eps_line_edit.text().toAscii()))))
        self.top_layout.addWidget(self._eps_tool_bar)
        self.qtgui_time_sink_x_0 = qtgui.time_sink_f(
        	8000, #size
        	samp_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0.set_y_axis(0, 400000)
        
        self.qtgui_time_sink_x_0.set_y_label('Amplitude', "")
        
        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, qtgui.TRIG_SLOPE_POS, 100000, 400e-6, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0.enable_control_panel(True)
        
        if not True:
          self.qtgui_time_sink_x_0.disable_legend()
        
        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_win)
        self.qtgui_sink_x_0 = qtgui.sink_c(
        	1024, #fftsize
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	samp_rate, #bw
        	"", #name
        	True, #plotfreq
        	True, #plotwaterfall
        	True, #plottime
        	True, #plotconst
        )
        self.qtgui_sink_x_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_win = sip.wrapinstance(self.qtgui_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_sink_x_0_win)
        
        self.qtgui_sink_x_0.enable_rf_freq(False)
        
        
          
        self.qtgui_number_sink_1 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_NONE,
            1
        )
        self.qtgui_number_sink_1.set_update_time(0.10)
        self.qtgui_number_sink_1.set_title('')
        
        labels = ['eps', '', '', '', '',
                  '', '', '', '', '']
        units = ['', '', '', '', '',
                 '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_1.set_min(i, -20)
            self.qtgui_number_sink_1.set_max(i, 20)
            self.qtgui_number_sink_1.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_1.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_1.set_label(i, labels[i])
            self.qtgui_number_sink_1.set_unit(i, units[i])
            self.qtgui_number_sink_1.set_factor(i, factor[i])
        
        self.qtgui_number_sink_1.enable_autoscale(False)
        self._qtgui_number_sink_1_win = sip.wrapinstance(self.qtgui_number_sink_1.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_1_win)
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_NONE,
            1
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title('')
        
        labels = ["SNR", '', '', '', '',
                  '', '', '', '', '']
        units = ['', '', '', '', '',
                 '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_0.set_min(i, -1)
            self.qtgui_number_sink_0.set_max(i, 1)
            self.qtgui_number_sink_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0.set_label(i, labels[i])
            self.qtgui_number_sink_0.set_unit(i, units[i])
            self.qtgui_number_sink_0.set_factor(i, factor[i])
        
        self.qtgui_number_sink_0.enable_autoscale(False)
        self._qtgui_number_sink_0_win = sip.wrapinstance(self.qtgui_number_sink_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_0_win)
        self.fir_filter_xxx_0_1_0 = filter.fir_filter_fff(1, (np.ones(T_R)))
        self.fir_filter_xxx_0_1_0.declare_sample_delay(0)
        self.fir_filter_xxx_0_1 = filter.fir_filter_fff(1, (np.ones(T_R)))
        self.fir_filter_xxx_0_1.declare_sample_delay(0)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_nlog10_ff_0 = blocks.nlog10_ff(1, 1, 0)
        self.blocks_multiply_xx_0_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vff((1/(2*3.14), ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff((10, ))
        self.blocks_multiply_conjugate_cc_0_0 = blocks.multiply_conjugate_cc(1)
        self.blocks_multiply_conjugate_cc_0 = blocks.multiply_conjugate_cc(1)
        self.blocks_moving_average_xx_0_0 = blocks.moving_average_cc(1024, 1, 4000)
        self.blocks_moving_average_xx_0 = blocks.moving_average_cc(1024, 1, 4000)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/maier/workspace/gr-dvbt2rx/binary/signal.bin', True)
        self.blocks_divide_xx_0 = blocks.divide_ff(1)
        self.blocks_delay_0_0_0 = blocks.delay(gr.sizeof_gr_complex*1, T_B)
        self.blocks_delay_0_0 = blocks.delay(gr.sizeof_gr_complex*1, T_C)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_gr_complex*1, T_A)
        self.blocks_complex_to_mag_squared_0_0 = blocks.complex_to_mag_squared(1)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self.blocks_complex_to_arg_0 = blocks.complex_to_arg(1)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_sig_source_x_0_0 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, eps, 1, 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, -samp_rate/T_A, 1, 0)
        self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, noise_amplitude, 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 1))    
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_complex_to_mag_squared_0, 0))    
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0_0, 1))    
        self.connect((self.analog_sig_source_x_0_0, 0), (self.blocks_multiply_xx_0_0_0, 0))    
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_throttle_0, 0))    
        self.connect((self.blocks_complex_to_arg_0, 0), (self.blocks_multiply_const_vxx_1, 0))    
        self.connect((self.blocks_complex_to_mag_0, 0), (self.qtgui_time_sink_x_0, 0))    
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.fir_filter_xxx_0_1, 0))    
        self.connect((self.blocks_complex_to_mag_squared_0_0, 0), (self.fir_filter_xxx_0_1_0, 0))    
        self.connect((self.blocks_delay_0, 0), (self.blocks_multiply_xx_0, 1))    
        self.connect((self.blocks_delay_0_0, 0), (self.blocks_multiply_conjugate_cc_0, 1))    
        self.connect((self.blocks_delay_0_0_0, 0), (self.blocks_multiply_conjugate_cc_0_0, 1))    
        self.connect((self.blocks_divide_xx_0, 0), (self.blocks_nlog10_ff_0, 0))    
        self.connect((self.blocks_file_source_0, 0), (self.blocks_multiply_xx_0_0_0, 1))    
        self.connect((self.blocks_moving_average_xx_0, 0), (self.blocks_delay_0, 0))    
        self.connect((self.blocks_moving_average_xx_0_0, 0), (self.blocks_multiply_xx_0, 0))    
        self.connect((self.blocks_multiply_conjugate_cc_0, 0), (self.blocks_moving_average_xx_0, 0))    
        self.connect((self.blocks_multiply_conjugate_cc_0_0, 0), (self.blocks_moving_average_xx_0_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.qtgui_number_sink_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.qtgui_number_sink_1, 0))    
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_complex_to_arg_0, 0))    
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_complex_to_mag_0, 0))    
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.blocks_delay_0_0, 0))    
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.blocks_multiply_conjugate_cc_0_0, 0))    
        self.connect((self.blocks_multiply_xx_0_0_0, 0), (self.blocks_add_xx_0, 0))    
        self.connect((self.blocks_multiply_xx_0_0_0, 0), (self.blocks_complex_to_mag_squared_0_0, 0))    
        self.connect((self.blocks_nlog10_ff_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.blocks_delay_0_0_0, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.blocks_multiply_conjugate_cc_0, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.blocks_multiply_xx_0_0, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.qtgui_sink_x_0, 0))    
        self.connect((self.fir_filter_xxx_0_1, 0), (self.blocks_divide_xx_0, 1))    
        self.connect((self.fir_filter_xxx_0_1_0, 0), (self.blocks_divide_xx_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_T_A(self):
        return self.T_A

    def set_T_A(self, T_A):
        self.T_A = T_A
        self.set_T_R(self.T_A)
        self.blocks_delay_0.set_dly(self.T_A)
        self.analog_sig_source_x_0.set_frequency(-self.samp_rate/self.T_A)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)
        self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.analog_sig_source_x_0_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_0.set_frequency(-self.samp_rate/self.T_A)

    def get_noise_amplitude(self):
        return self.noise_amplitude

    def set_noise_amplitude(self, noise_amplitude):
        self.noise_amplitude = noise_amplitude
        Qt.QMetaObject.invokeMethod(self._noise_amplitude_line_edit, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.noise_amplitude)))
        self.analog_noise_source_x_0.set_amplitude(self.noise_amplitude)

    def get_eps(self):
        return self.eps

    def set_eps(self, eps):
        self.eps = eps
        Qt.QMetaObject.invokeMethod(self._eps_line_edit, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.eps)))
        self.analog_sig_source_x_0_0.set_frequency(self.eps)

    def get_T_R(self):
        return self.T_R

    def set_T_R(self, T_R):
        self.T_R = T_R
        self.fir_filter_xxx_0_1_0.set_taps((np.ones(self.T_R)))
        self.fir_filter_xxx_0_1.set_taps((np.ones(self.T_R)))

    def get_T_C(self):
        return self.T_C

    def set_T_C(self, T_C):
        self.T_C = T_C
        self.blocks_delay_0_0.set_dly(self.T_C)

    def get_T_B(self):
        return self.T_B

    def set_T_B(self, T_B):
        self.T_B = T_B
        self.blocks_delay_0_0_0.set_dly(self.T_B)


def main(top_block_cls=top_block, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
