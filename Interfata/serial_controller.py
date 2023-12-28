import serial
import serial.tools.list_ports

baud_rate=9600
timeout=1
plot_value = -1

def find_open_serial_port():
    #parcurge toate porturile seriale (/dev/ttyS*) dev/ttyS5<=>COM5
    my_ports = serial.tools.list_ports.comports()
    for port in my_ports:
        try:
            serial_conn = serial.Serial(port.device, baud_rate,bytesize=8, timeout=1)
            serial_conn.close() 
            if(port.description!="ttyS0"):
                return port.device
        except serial.SerialException as e:
            print(f"ERROR:{e}")
            pass
    return None

def connect_to_serial_port(port):
    global serial_conn
    try:
        if(port==None):
            print(f'Error: no serial port found')
            return
        serial_conn = serial.Serial(port, baud_rate,bytesize=8,timeout=1)
        if serial_conn:
            print("Serial port oppened.")
            return serial_conn
        else:
            print(f"Failed to open serial port")
            return None
    except serial.SerialException as e:
        print(f"Error: {e}")
        return None
    
def find_and_connect_serial_port():
    port = find_open_serial_port()
    serial_conn=connect_to_serial_port(port)
    return serial_conn

def read_adc_value():
    global plot_value
    ret_value = plot_value
    return ret_value
def read_data_from_serial(serial_conn):
    global plot_value
    try:
        if serial_conn is not None and serial_conn.is_open:
            while True:
                try:
                    data_raw = serial_conn.readline()
                    print(data_raw)
                    if(data_raw!=b'' and data_raw!=b'\r'):
                       if b'\x19' in data_raw:
                        #Daca apare valoarea x12 inseamna ca am trimisa valoarea de plotat de la ADC
                        start_index = data_raw.index(b'\x19')
                        end_index = data_raw.index(b'\x19', start_index + 1)
                        #Extrag valoarea de la ADC
                        adc_value = int(data_raw[start_index + 1:end_index].decode('ascii'))
                        plot_value= adc_value
                        #Pot sa am si un mesaj sau input in afara de valoarea de ADC -> parsez
                        data_raw = data_raw[:start_index] + data_raw[end_index + 1:]
                        data = data_raw.decode('ascii')
                       if data:
                        print(f"Received: {data}")
                        return data
                except Exception as e:
                    print(f"Error: {str(e)}")
                    print("Non ASCII data")
    except serial.SerialException as e:
        print(f"Error reading from serial port: {e}")

def send_data_to_serial(serial_conn,data):
    try:
        if serial_conn is not None and serial_conn.is_open:
            serial_conn.write(data.encode('utf-8'))
            print(f"Sent: {data}")
    except serial.SerialException as e:
        print(f"Error writing to serial port: {e}")

def send_ledTrigger_to_serial(serial_conn,int_data):
    try:
        if serial_conn is not None and serial_conn.is_open:
            serial_conn.write(int_data.to_bytes(1,byteorder='little'))
    except serial.SerialException as e:
        print(f"Error writing to serial port: {e}")
    