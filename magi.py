from peer import eva_connection
import numpy as np
import cv2

def rgb565_to_rgb(rgb565_bytes, *, width:int, height:int):
    rgb_values = [[(0, 0, 0) for _ in range(width)] for _ in range(height)]
    row = 0
    col = 0
    for i in range(0, len(rgb565_bytes), 2):
        # Extracting the 16-bit RGB565 value from the byte array
        rgb565 = (rgb565_bytes[i] << 8) | rgb565_bytes[i + 1]
        
        # Extracting individual components
        r = (rgb565 >> 11) & 0x1F
        g = (rgb565 >> 5) & 0x3F
        b = rgb565 & 0x1F
        
        # Scaling components to 8-bit
        r = (r * 255) // 31
        g = (g * 255) // 63
        b = (b * 255) // 31    
    
        rgb_values[row][col] = r, g, b
        
        col += 1
        if col == width:
            col = 0
            row += 1
        if row == 120:
            break
    return rgb_values

def convert_rgb_array_to_numpy(rgb_array):
    # Converte a matriz 2D em um array numpy
    return np.array(rgb_array, dtype=np.uint8)

def main():
    eva_con = eva_connection("192.168.4.1", 12345)
    eva_con.connect()    
    # while True:
        
    #     frame_in_packets = eva_con.get_packets()
    
    #     frame_in_bytes = b''.join(frame_in_packets)
    #     rgb_array = convert_rgb565_to_array(frame_in_bytes, 160,120)
    #     # rgb_array = rgb565_to_rgb(frame_in_bytes)
    #     cv2.imshow("Imagem", cv2.resize(np_arr, (160*2, 120*2)))
    #     cv2.waitKey(0)
    #     cv2.destroyAllWindows()
    #     break
        
    while True:
        data_packets = eva_con.get_packets()
        if data_packets:
            data_bytes = b''.join(data_packets)
            img_buffer = rgb565_to_rgb(data_bytes, width=320, height=240)
            np_arr = convert_rgb_array_to_numpy(img_buffer)

            cv2.imshow('ESP32 Camera', cv2.resize(np_arr, (160*2, 120*2)))

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

if __name__ == "__main__":
    main()
    
