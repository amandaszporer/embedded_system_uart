import serial
import threading
import queue

# =================== CONFIG ===================
SERIAL_PORT = 'COM5'  # Replace with your port
BAUD_RATE = 9600

# =================== GLOBALS ===================
lock = threading.Lock()
command_queue = queue.Queue()

# =================== THREAD: READ ===================
def read_from_serial(ser):
    while True:
        if ser.in_waiting:
            with lock:
                data = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
                handle_received_data(data)

# =================== DATA HANDLER ===================
def handle_received_data(data):
    for char in data:
        if char == '9':
            command_queue.put('get_strings')
        if char == '7':
            print("\n[MENU]")
            display_menu()
        elif char == '3':
            print("\n[Input Time Delay]")
            command_queue.put('delay_input')
        elif char == 'F':
            print(" ")
        else:
            print(f"{char}", end='', flush=True)

# =================== MENU DISPLAY ===================
def display_menu():
    print("""
================ MENU ================
1. Count up on LCD
2. Circular tone series
3. Get delay time X[ms]
4. Display potentiometer value
5. Send 'I love my Negev'
6. Clear LCD
7. Show menu
8. Sleep
9. Print Strings to LCD
=======================================
""")

# =================== SEND FUNCTIONS ===================
def send_command(ser, cmd):
    with lock:
        ser.write(cmd.encode('utf-8'))

def send_delay_input(ser):
    delay = input("")
    with lock:
        ser.write((delay + '\n').encode('utf-8'))

def send_strings_input(ser):
    string = input("Enter string (max 32 char): ")
    while not command_queue.empty():
        command_queue.get()

    if string in range(1,9):
        send_command(ser, string)
    else:
        with lock:
            ser.write((string + '\n').encode('utf-8'))
            command_queue.put('get_strings')

# =================== THREAD: USER INPUT ===================
def user_input_thread(ser):
    while True:
        try:
            # Wait briefly for a special command (max 0.1s)
            cmd = command_queue.get(timeout=0.1)
            if cmd == 'delay_input':
                send_delay_input(ser)
            elif cmd == 'get_strings':
                send_strings_input(ser)
            elif cmd == 'exit':
                break
        except queue.Empty:
            user_input = input("Select option (1-9) or 'exit': ").strip()
            if user_input.lower() == 'exit':
                command_queue.put('exit')
                break
            elif user_input in [str(i) for i in range(1, 10)]:
                send_command(ser, user_input)
            else:
                print("Invalid input.")

# =================== MAIN ===================
def main():
    global ser
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
        display_menu()
        # Start reader thread
        thread = threading.Thread(target=read_from_serial, args=(ser,), daemon=True)
        thread.start()

        # Start user input thread
        user_thread = threading.Thread(target=user_input_thread, args=(ser,))
        user_thread.start()

        user_thread.join()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        if 'ser' in globals() and ser.is_open:
            ser.close()
        print("Serial port closed. Exiting...")

# =================== ENTRY ===================
if __name__ == '__main__':
    main()
