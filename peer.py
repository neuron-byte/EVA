import socket


def ping(signal,expected_response_len):
    try:
        signal_bytes = (signal).to_bytes(1,byteorder="little")
        SOCK_UDP.sendto(signal_bytes,EVA_CON)
        try:
            return SOCK_UDP.recvfrom(expected_response_len)
        
        except socket.timeout:
            if not sucess_connection:
                print("Erro02 - Timeout: A unidade EVA demorou muito para responder o ping de conexão")
                SOCK_UDP.close()
                exit()
                
            if receving_frame:
                print("Erro04 - Timeout: A unidade EVA demorou muito para responder e um frame foi perdido")
                lost_packages+=1

            return None
    except:
        print("Erro03 - Falha ao conectar ao EVA")
        SOCK_UDP.close()
        exit()

if __name__ == "__main__":
    EVA_CON = ("192.168.0.104",12345)
    ESP_MAX_BYTES = 1460
    SOCK_UDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    SOCK_UDP.settimeout(1000)
    lost_packages = 0
    num_packages = 0
    receving_frame = False
    sucess_connection = False
    frame = []
    ping(0,1)
    print("Conexão com EVA estabelecida")
    sucess_connection = True

    while True:
        if not receving_frame: 
            eva_response = ping(1,ESP_MAX_BYTES)
            if not eva_response:
                continue
            num_packages = int.from_bytes(eva_response[0], byteorder="little")
            receving_frame = True
        
        for _ in range(num_packages):
            print(_)
            eva_response = ping(2,ESP_MAX_BYTES)
            
            if not eva_response:
                frame = []
                break
            frame.append(eva_response[0])

        receving_frame = False
        #converter frame para algum formato?