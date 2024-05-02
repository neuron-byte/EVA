import threading
from peer import eva_connection
import numpy as np
import cv2
import tkinter as tk

MOVE_FORWARD:int = 6
MOVE_BACK:int = 7
TURN_RIGHT:int = 8 
TURN_LEFT:int = 9
STOP:int = 10


eva_con = eva_connection("192.168.4.1", 12345)
eva_con.connect() 

def render_image():
    data_packets = eva_con.get_packets()
    if data_packets:
        data_bytes = b''.join(data_packets)
        data_array = np.frombuffer(data_bytes, np.uint8)
        img_decoded = cv2.imdecode(data_array, cv2.IMREAD_COLOR)
        if img_decoded is not None:
            cv2.imshow('ESP32CAM Output', img_decoded)
        else:
            print("Não foi possível carregar o frame.")
            
    if cv2.waitKey(1) & 0xFF == ord('q'):
        cv2.destroyWindow();

class Controle(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)
        self.master = master
        self.master.title("Controle")
        self.create_widgets()
        self.bind_keys()
    
        self.thread_count = 0

    def create_widgets(self):
        self.btn_up = tk.Button(self.master, text="↑", command=self.up, width=10, height=5, bg="lightblue")
        self.btn_up.grid(row=0, column=1, padx=5, pady=5)

        self.btn_left = tk.Button(self.master, text="←", command=self.left, width=10, height=5, bg="lightblue")
        self.btn_left.grid(row=1, column=0, padx=5, pady=5)

        self.btn_right = tk.Button(self.master, text="→", command=self.right, width=10, height=5, bg="lightblue")
        self.btn_right.grid(row=1, column=2, padx=5, pady=5)

        self.btn_down = tk.Button(self.master, text="↓", command=self.down, width=10, height=5, bg="lightblue")
        self.btn_down.grid(row=2, column=1, padx=5, pady=5)
        
        self.space = tk.Label(self.master, width=10, height=5)
        self.space.grid(column=3, row=1, padx=5, pady=5)
        
        self.btn_img = tk.Button(self.master, text="GET IMAGE", command=self.get_img, width=15, height=5, bg="lightblue")
        self.btn_img.grid(column=4, row=1,padx=5, pady=15)
        
        self.btn_stream = tk.Button(self.master, text="START STREAMING", command=self.start_stream, width=15, height=5, bg="lightblue")
        self.btn_stream.grid(column=5, row=1,padx=5, pady=15)
        
        
    def bind_keys(self):
        self.master.bind('<KeyPress>', self.key_pressed)

    def key_pressed(self, event):
        if event.char == 'w' or event.char == 'W':
            self.up()
        if event.char == 's' or event.char == 'S':
            self.down()
        if event.char == 'a' or event.char == 'A':
            self.left()
        if event.char == 'd' or event.char == 'D':
            self.right()
        if event.char == 'f' or event.char == 'F':
            self.stream()
        if event.char == 'q' or event.char == 'Q':
            self.stop_stream()
        if event.char == ' ':
            self.get_img()

    def up(self):
        eva_con.send_signal(6)

    def down(self):
        eva_con.send_signal(MOVE_BACK)

    def left(self):
        eva_con.send_signal(TURN_LEFT)

    def right(self):
        eva_con.send_signal(TURN_RIGHT)
    
    def get_img(self):
        render_image()
        
    def start_stream(self):
        if self.thread_count != 1:
            self.t_stream = threading.Thread(target=self.stream)
            self.t_stream.start()
            self.thread_count = 1
    
    def stop_stream(self):
        if self.thread_count == 1:    
            self.t_stream.join()
            self.thread_count = 0
    
    def stream(self):
        while True:
            data_packets = eva_con.get_packets()
            if data_packets:
                data_bytes = b''.join(data_packets)
                data_array = np.frombuffer(data_bytes, np.uint8)
                img_decoded = cv2.imdecode(data_array, cv2.IMREAD_COLOR)
                if img_decoded is not None:
                    cv2.imshow('EVA 00 CAM', img_decoded)
                else:
                    print("Não foi possível carregar o frame.")
                    
            if cv2.waitKey(1) & 0xFF == ord('q'):
                cv2.destroyAllWindows()
                self.thread_count = 0
                break

root = tk.Tk()
root.resizable = False
app = Controle(master=root)
app.mainloop()