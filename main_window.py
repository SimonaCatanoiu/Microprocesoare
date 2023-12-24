from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit
from PySide6.QtGui import QIcon, QPalette, QColor, QFont
from PySide6.QtCore import Qt,QThread,Signal
from serial_controller import find_and_connect_serial_port,send_ledTrigger_to_serial,send_data_to_serial,read_data_from_serial
import pyqtgraph as pg

class SerialReceiverThread(QThread):
    received_signal = Signal(str)

    def __init__(self, serial_conn):
        super().__init__()
        self.serial_conn = serial_conn

    def run(self):
        while True:
            output = read_data_from_serial(self.serial_conn)
            self.received_signal.emit(output)


class MainWindow(QMainWindow):
    promotie: str = "2023-2024"
    team: list[str] = [
        "Catanoiu Simona",
        "Dumitrascu Andreea",
    ]
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

        plot_widget = pg.PlotWidget()
        hour = [1,2,3,4,5,6,7,8,9,10]
        temperature = [30,32,34,32,33,31,29,32,35,45]

        plot_widget.plot(hour, temperature)

        secondary_layout.addWidget(plot_widget, 3)
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