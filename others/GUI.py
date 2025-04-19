import tkinter as tk
import ttkbootstrap as ttk
import random
# import serial 
import socket
import math 

class App(ttk.Window):
    def __init__(self, title, size):
        super().__init__(themename='journal')
        self.title(title)
        self.geometry(f'{size[0]}x{size[1]}')
        self.minsize(size[0], size[1])

        # widgets
        self.menu = Menu(self)
        self.heatmap = Heatmap(self)
        self.heatmap.place_forget()

        # run
        self.mainloop()

class Menu(ttk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.place(x=0, y=0, relwidth=0.3, relheight=1)

        self.parent = parent  

        self.tx_vars = [tk.IntVar() for _ in range(8)]
        self.rx_vars = [tk.IntVar() for _ in range(8)]

        self.create_widgets()

        self.heatmap = Heatmap(self)
        self.heatmap.place_forget()  # Hide heatmap initially

        self.esp_ip = '192.168.4.1'  # Replace with your ESP32's IP
        self.esp_port = 80        # Same port as used by ESP32
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(3)  # Optional: Set a timeout (e.g., 3 seconds)

        try:
            self.sock.connect((self.esp_ip, self.esp_port))
            print("Connected to ESP32")
        except socket.error as e:
            print(f"Socket connection failed: {e}")



    def create_widgets(self):
        # Buttons
        menu_button01 = ttk.Button(self, text='Reset', bootstyle='danger', command=self.hide_heatmap)
        menu_button02 = ttk.Button(self, text='Show DI Map', bootstyle='success', command=self.show_heatmap)


        menu_button1 = ttk.Button(self, text='Transmitter', bootstyle='danger-outline-round')
        menu_button2 = ttk.Button(self, text='Receiver', bootstyle='success-outline-round')
        start_button = ttk.Button(self, text='Start', bootstyle='dark', command=lambda: [self.send_config(), self.hide_heatmap()])

        # Layout
        self.columnconfigure((0, 1, 2), weight=1, uniform='a')
        for i in range(11):
            self.rowconfigure(i, weight=1, uniform='a')

        menu_button01.grid(row=0, column=0, padx=4, pady=4, sticky='nsew')
        menu_button02.grid(row=0, column=2, padx=4, pady=4, sticky='nsew')    

        menu_button1.grid(row=1, column=0, padx=4, pady=4, sticky='nsew')
        menu_button2.grid(row=1, column=2, padx=4, pady=4, sticky='nsew')

        # Create toggle rows
        for i in range(8):
            frame = ttk.Frame(self)
            frame.grid(row=i+2, column=0, columnspan=3, sticky='nsew')
            tx = ttk.Checkbutton(frame, text=f'T {i+1}', variable=self.tx_vars[i], bootstyle='info')
            rx = ttk.Checkbutton(frame, text=f'R {i+1}', variable=self.rx_vars[i])
            tx.pack(side='left', expand=True)
            rx.pack(side='left', expand=True)

        # Start button at the bottom
        start_button.grid(row=10, column=0, columnspan=3, sticky='nsew', padx=4, pady=4)

    def send_config(self):
        # Create Txxxxxxxx and Ryyyyyyyy strings
        tx_bits = ''.join(str(var.get()) for var in reversed(self.tx_vars))
        rx_bits = ''.join(str(var.get()) for var in reversed(self.rx_vars))
        command = f'{tx_bits}{rx_bits} '

        # UART transmission (replace print with actual UART write)
        print(f"Sending: {command}\n")
        # self.serial_port.write(command.encode())  # Uncomment for real UART
        try:
            self.sock.sendall(command.encode())
            print("Sent to ESP32:", command)
        except socket.error as e:
            print(f"Failed to send: {e}")


    def hide_heatmap(self):
        self.parent.heatmap.place_forget()

    def show_heatmap(self):
        self.parent.heatmap.place(relx=0.3, rely=0, relwidth=0.7, relheight=1)



class Heatmap(ttk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.place(relx=0.3, rely=0, relwidth=0.7, relheight=1)

        self.canvas = tk.Canvas(self, bg='white')
        self.canvas.pack(fill='both', expand=True)
        self.canvas.bind('<Configure>', self.draw_heatmap)  

    def draw_heatmap(self, event=None):
        self.canvas.delete('all')

        rows, cols = 100, 100
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()

        outer_padding = 20
        drawable_width = width - 2 * outer_padding
        drawable_height = height - 2 * outer_padding

        cell_w = drawable_width / cols
        cell_h = drawable_height / rows

        # Get heatmap values from di_algo
        heatmap_values = self.di_algo(rows, cols)

        for i in range(rows):
            for j in range(cols):
                x0 = outer_padding + j * cell_w
                y0 = outer_padding + i * cell_h
                x1 = x0 + cell_w
                y1 = y0 + cell_h

                value = heatmap_values[i][j]
                color = self.get_color(value)
                self.canvas.create_rectangle(x0, y0, x1, y1, fill=color, outline=color)

    def di_algo(self, rows=10, cols=10):
        """Generates a heatmap with a smaller red core and quicker transition to green."""
        heatmap_data = []

        center_i = rows / 2
        center_j = cols / 2
        max_dist = math.sqrt(center_i**2 + center_j**2)

        for i in range(rows):
            row = []
            for j in range(cols):
                dist = math.sqrt((i - center_i)**2 + (j - center_j)**2)
                norm_dist = dist / max_dist

                # Apply exponent to steepen the transition (e.g., 2 = sharper fade)
                value = norm_dist**0.5
                value = min(max(value, 0), 1)
                row.append(value)
            heatmap_data.append(row)
        return heatmap_data

    def get_color(self, value):
        """Map a float [0,1] to a color. 0 = red, 1 = green, gradient in between."""
        r = int(255 * (1 - value))  # Red decreases as value increases
        g = int(255 * value)        # Green increases as value increases
        b = 0                       # No blue component
        return f'#{r:02x}{g:02x}{b:02x}'

App('TUE-08: ECOSYNC8x, a Structural Health Monitoring System', (1800, 1200))
