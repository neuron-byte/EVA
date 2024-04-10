from peer import eva_connection
import numpy as np
import cv2

def main():
    eva_con = eva_connection("192.168.4.1", 12345)
      
    while True:
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
            break

if __name__ == "__main__":
    main()
    
