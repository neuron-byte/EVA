import socket
import errno
from tqdm import tqdm

ESP_MAX_BYTES = 1460
HANDSHAKE:int = 1
DATA_REQUEST:int = 2
RECEIVING_DATA:int = 3
RESET_DATA:int = 4

class eva_connection:
    def __init__(self, eva_ip:str, eva_port:int) -> None:
        self.EVA_CON:socket._Address = eva_ip, eva_port
        
        self.sock:socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(10)
        
        self.lost_packages:int = 0
        self.num_packages:int = 0
        
        self.receving_packets:bool = False
        self.sucess_connection:bool = False
        
    
    def connect(self) -> bool:
        if self.sucess_connection == True: return

        self.sucess_connection = True if self.__ping(HANDSHAKE) else False
        self.sock.settimeout(2)
        return self.sucess_connection
    
    def __async_ping(self, signal:int):
        signal_bytes = signal.to_bytes()
        print(f"bytes do sinal a ser enviado: {signal_bytes}")
        self.sock.sendto(signal_bytes, self.EVA_CON)
        
    def __ping(self, signal:int, expected_response_len:int=1) -> tuple[bytes, any]:
        try:
            self.__async_ping(signal)
            return self.sock.recvfrom(expected_response_len)
            
        except socket.timeout as err:
            print(err)
            if not self.sucess_connection:
                print("Erro02 - Timeout: A unidade EVA demorou muito para responder o ping de conexão")
                self.sock.close()
                exit()
                
            if self.receving_packets:
                print("Erro04 - Timeout: A unidade EVA demorou muito para responder e um frame foi perdido")
                self.lost_packages += 1

            return None
        
        except socket.error as err:
            if err == errno.EAGAIN or err == errno.EWOULDBLOCK:
                print("No data available")
            else:
                print(err)
                print("Erro03 - Falha ao conectar ao EVA")
                self.sock.close()    
                exit()
                
        except KeyboardInterrupt as err:
            print("Encerrando conexão")
            self.sock.close()
            exit()
    
    def send_signal(self, signal:int) -> None:
        self.__async_ping(signal)
    
    def get_packets(self) -> list[bytes] | None:
        packets = list()
        
        packets_response = self.__ping(DATA_REQUEST, ESP_MAX_BYTES)
        if not packets_response:
            print("[warning] No response")
            return None
        print(packets_response)
        self.num_packages = int.from_bytes(packets_response[0], byteorder="little")
        print(f"NUMERO DE PKGS: {self.num_packages}")
        
        self.receving_packets = True

        for _ in tqdm(range(self.num_packages), desc='Receiving Packages', unit='package'):
            eva_response = self.__ping(RECEIVING_DATA, ESP_MAX_BYTES)
            if not eva_response:
                packets = list()
                break    
            packets.append(eva_response[0])
            #print(f"{counter+1}/{self.num_packages}")
            
        
        self.receving_packets = False
        return packets
    


def main():
    print("Iniciando conexão")
    eva_con = eva_connection("192.168.4.1", 12345)
    eva_con.connect()
    print("Conexão com EVA estabelecida")
    eva_con.get_packets()
    for _ in range(24):
        eva_con.get_packets()
        eva_con.cont = eva_con.cont+1
''' 
if __name__ == "__main__":
    main()
'''