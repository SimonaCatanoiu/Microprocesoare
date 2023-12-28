from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit
from PySide6.QtGui import QIcon, QPalette, QColor, QFont
from PySide6.QtCore import Qt,QThread,Signal
from serial_controller import find_and_connect_serial_port,send_ledTrigger_to_serial,send_data_to_serial,read_data_from_serial,read_adc_value
import pyqtgraph as pg
import time

class SerialReceiverThread(QThread):
    received_signal = Signal(str)

    def __init__(self, serial_conn):
        super().__init__()
        self.serial_conn = serial_conn

    def run(self):
        while True:
            output = read_data_from_serial(self.serial_conn)
            self.received_signal.emit(output)

class ADCThread(QThread):
    adc_value_received = Signal(int)

    def __init__(self, serial_conn):
        super().__init__()
        self.serial_conn = serial_conn

    def run(self):
        while True:
            adc_value = read_adc_value()
            self.adc_value_received.emit(adc_value)
            time.sleep(1)


class MainWindow(QMainWindow):
    promotie: str = "2023-2024"
    team: list[str] = [
        "Catanoiu Simona",
        "Dumitrascu Andreea",
    ]
    seconds = []
    light_value = []

    def __init__(self):
        super().__init__()
        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))

        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member,1)
        team_box_layout.addWidget(second_member,1)
        team_box.setLayout(team_box_layout)

        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("LED Change")
        button2 = QPushButton("Control 2")
        button3 = QPushButton("Send")
        button1.clicked.connect(self.led_change_event)
        button3.clicked.connect(self.send_input)
        button2.clicked.connect(self.button2_action)
        self.line_edit = QLineEdit()
        self.line_edit.setAlignment(Qt.AlignmentFlag.AlignBottom)
        line_edit_label = QLabel("Input:", parent=self.line_edit)
        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(button1,1)
        control_panel_box_layout.addWidget(button2,1)

        control_panel_box_layout.addStretch()
        control_panel_box_layout.addWidget(line_edit_label)
        control_panel_box_layout.addWidget(self.line_edit, 1)
        control_panel_box_layout.addWidget(button3,1)

        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box,5)

        self.plot_widget = pg.PlotWidget()
        self.plot_widget.plot(self.seconds, self.light_value)

        secondary_layout.addWidget(self.plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 4)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        debug_box = QGroupBox("Debug")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit, 1)
        debug_box.setLayout(debug_box_layout)

        primary_layout.addWidget(debug_box, 1)

        widget = QWidget()
        widget.setLayout(primary_layout)
        
        self.setCentralWidget(widget)

        self.init_logic()

    def init_logic(self):
        # Initialize serial connection
        self.serial_conn = find_and_connect_serial_port()

        # Initialize and start the serial thread to receive data from microcontroller
        self.serial_thread = SerialReceiverThread(self.serial_conn)
        self.serial_thread.received_signal.connect(self.receive_output)
        self.serial_thread.start()

        self.adc_thread = ADCThread(self.serial_conn)
        self.adc_thread.adc_value_received.connect(self.update_plot)
        self.adc_thread.start()

    def send_input(self):
        input = self.line_edit.text()
        self.line_edit.clear()
        self.text_edit.insertPlainText(f"INPUT: {input}\n")
        send_data_to_serial(self.serial_conn,input)

    def receive_output(self, output):
        self.text_edit.insertPlainText(f"OUTPUT: {output}\n")

    def led_change_event(self):
        send_ledTrigger_to_serial(self.serial_conn,17)
        print(f"Sented GPIO change led sequence flag")

    def button2_action(self):
        send_ledTrigger_to_serial(self.serial_conn,18)

    def update_plot(self, adc_value):
        plot_value = adc_value
        if plot_value != -1:
            self.light_value.append(plot_value)
            self.seconds.append(len(self.light_value))

            if plot_value > 3000:
                color = (255, 0, 0)
            elif plot_value > 1000:
                color = (0, 255, 0) 
            else:
                color = (255, 255, 0)
            
            if len(self.seconds) > 1:
                x_start, x_end = self.seconds[-2], self.seconds[-1]
                y_start, y_end = self.light_value[-2], self.light_value[-1]
                line_segment = pg.PlotCurveItem(x=[x_start, x_end], y=[y_start, y_end], pen=pg.mkPen(color=color, width=2))
                self.plot_widget.addItem(line_segment)
                
            self.plot_widget.getPlotItem().getViewBox().setYRange(0, 6000)

            if len(self.seconds) > 60:
                self.light_value.clear()
                self.seconds.clear()
                self.plot_widget.clear()