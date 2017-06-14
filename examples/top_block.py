#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Wed Jun 14 16:04:37 2017
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

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from t2_p1_detector import t2_p1_detector  # grc-generated hier_block
import dvbt2rx
import sip
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        qtgui.util.check_set_qss()
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
        self.samp_rate = samp_rate = 9142857.14286

        ##################################################
        # Blocks
        ##################################################
        self.blocks_moving_average = blocks.moving_average_cc(1, 1, 4000)
        self.blocks_delay = blocks.delay(gr.sizeof_gr_complex*1, 0)
        self.t2_p1_detector_0 = t2_p1_detector()
        self.qtgui_time_sink_x_0_0_0 = qtgui.time_sink_f(
        	1024000, #size
        	1, #samp_rate
        	"", #name
        	2 #number of inputs
        )
        self.qtgui_time_sink_x_0_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0_0_0.set_y_axis(0, 1)

        self.qtgui_time_sink_x_0_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0_0_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0_0_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, qtgui.TRIG_SLOPE_POS, 0.0, 50000, 0, "p1_start")
        self.qtgui_time_sink_x_0_0_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0_0_0.enable_control_panel(True)

        if not True:
          self.qtgui_time_sink_x_0_0_0.disable_legend()

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

        for i in xrange(2):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_0_0_win)
        self.dvbt2rx_gi_est_decider_0 = dvbt2rx.gi_est_decider(3, 16)
        self.dvbt2rx_gi_est_control_cc_0 = dvbt2rx.gi_est_control_cc(self.blocks_delay, self.blocks_moving_average)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_tag_gate_0_0 = blocks.tag_gate(gr.sizeof_gr_complex * 1, False)
        self.blocks_tag_gate_0 = blocks.tag_gate(gr.sizeof_gr_complex * 1, False)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_conjugate_0 = blocks.multiply_conjugate_cc(1)
        self.blocks_moving_average_1 = blocks.moving_average_ff(2**12, 1./2**24, 4000)
        (self.blocks_moving_average_1).set_min_output_buffer(500000)
        self.blocks_file_source_0_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/kmaier/workspace/gr-dvbt2rx/binary_testfiles/ard_multiplex_karlsruhe_rx_sr9142857.14286.iq', True)
        self.blocks_complex_to_mag_squared_1_0 = blocks.complex_to_mag_squared(1)
        self.blocks_complex_to_mag_squared_1 = blocks.complex_to_mag_squared(1)
        (self.blocks_complex_to_mag_squared_1).set_min_output_buffer(500000)
        self.analog_sig_source_x_1 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, samp_rate/1024*0, 1, 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_complex_to_mag_squared_1, 0), (self.dvbt2rx_gi_est_decider_0, 0))
        self.connect((self.blocks_complex_to_mag_squared_1, 0), (self.qtgui_time_sink_x_0_0_0, 0))
        self.connect((self.blocks_complex_to_mag_squared_1_0, 0), (self.blocks_moving_average_1, 0))
        self.connect((self.blocks_delay, 0), (self.blocks_tag_gate_0, 0))
        self.connect((self.blocks_file_source_0_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_moving_average, 0), (self.blocks_complex_to_mag_squared_1, 0))
        self.connect((self.blocks_moving_average_1, 0), (self.dvbt2rx_gi_est_decider_0, 1))
        self.connect((self.blocks_moving_average_1, 0), (self.qtgui_time_sink_x_0_0_0, 1))
        self.connect((self.blocks_multiply_conjugate_0, 0), (self.blocks_moving_average, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_tag_gate_0, 0), (self.blocks_multiply_conjugate_0, 1))
        self.connect((self.blocks_tag_gate_0_0, 0), (self.blocks_complex_to_mag_squared_1_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.t2_p1_detector_0, 0))
        self.connect((self.dvbt2rx_gi_est_control_cc_0, 0), (self.blocks_delay, 0))
        self.connect((self.dvbt2rx_gi_est_control_cc_0, 0), (self.blocks_multiply_conjugate_0, 0))
        self.connect((self.dvbt2rx_gi_est_control_cc_0, 0), (self.blocks_tag_gate_0_0, 0))
        self.connect((self.t2_p1_detector_0, 0), (self.dvbt2rx_gi_est_control_cc_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.analog_sig_source_x_1.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_1.set_frequency(self.samp_rate/1024*0)


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
