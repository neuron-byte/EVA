import socket
import errno

ESP_MAX_BYTES = 1460
HANDSHAKE = 0

class eva_connection:
    def __init__(self, eva_ip:str, eva_port:int):
        self.EVA_CON = eva_ip, eva_port
        
        self.SOCK_UDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.SOCK_UDP.settimeout(20)
        
        self.lost_packages = 0
        self.num_packages = 0
        
        self.receving_frame = False
        self.sucess_connection = False
        
    
    def connect(self):
        if self.sucess_connection == True: return

        self.sucess_connection = True if self.ping(HANDSHAKE, 1)[0] == HANDSHAKE else False
        self.SOCK_UDP.settimeout(1)
        return self.sucess_connection
    
    def ping(self, signal:int, expected_response_len:int):
        try:
            signal_bytes = signal.to_bytes(1,byteorder="little")
            self.SOCK_UDP.sendto(signal_bytes, self.EVA_CON)
            return self.SOCK_UDP.recvfrom(expected_response_len)
            
        except socket.timeout:
            if not self.sucess_connection:
                print("Erro02 - Timeout: A unidade EVA demorou muito para responder o ping de conexão")
                self.SOCK_UDP.close()
                exit()
                
            if self.receving_frame:
                print("Erro04 - Timeout: A unidade EVA demorou muito para responder e um frame foi perdido")
                self.lost_packages += 1

            return None
        
        except socket.error as err:
            if err == errno.EAGAIN or err == errno.EWOULDBLOCK:
                print("No data available")
            else:
                print(err)
                print("Erro03 - Falha ao conectar ao EVA")
                self.SOCK_UDP.close()    
                exit()
                
        except KeyboardInterrupt as err:
            print("Encerrando conexão")
            self.SOCK_UDP.close()
            exit()
        
    def convert_to_gray(self, frame_bytes):
        grayscale_bytes_pixels = b''
        for num_pixels in range(int(frame_bytes.__len__()/2)):
            
            pixelRGB565 = int.from_bytes(frame_bytes[(num_pixels*2):(num_pixels*2+2)])
            red = (((pixelRGB565 & 0xF800) >> 11)*255)//31
            green = (((pixelRGB565 & 0x07E0) >> 5)*255)//63
            blue = ((pixelRGB565 & 0x001F)*255)//31
            pixel_grayscale = int((0.299*red)+(0.587*green)+(0.114*blue))
            grayscale_bytes_pixels+=(pixel_grayscale.to_bytes(1,byteorder='little'))
        
        return grayscale_bytes_pixels
    
    def get_frame(self):
        frame_bytes = b''
        
        if not self.receving_frame: 
            eva_response = self.ping(1,ESP_MAX_BYTES)
            if not eva_response:
                print("Não ta respondendo")
                return None
            self.num_packages = int.from_bytes(eva_response[0], byteorder="little")
            print("NUMERO DE PKGS: "+str(self.num_packages))
            self.receving_frame = True

        
        for counter in range(self.num_packages):
            print(f"{counter+1}/{self.num_packages}")
            eva_response = self.ping(2,ESP_MAX_BYTES)
            if not eva_response:
                frame_bytes = b''
                break

            if counter == 26:
                frame_bytes+= eva_response[0][0:440]
                continue
                
            frame_bytes += eva_response[0]
        
        self.receving_frame = False
        
        # frame_bytes = self.convert_to_gray(frame_bytes)
        return frame_bytes

def main():
    print("Iniciando conexão")
    eva_con = eva_connection("192.168.4.1", 12345)
    eva_con.connect()
    print("Conexão com EVA estabelecida")
    eva_con.ping(1, 1)
    
    while True:
        eva_con.get_frame()

if __name__ == "__main__":
    main()