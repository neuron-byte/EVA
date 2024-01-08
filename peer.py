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

def convertToGray(frame_bytes):
    grayscale_bytes_pixels = b''
    print(int(frame_bytes.__len__()/2))
    for num_pixels in range(int(frame_bytes.__len__()/2)):
        
        pixelRGB565 = int.from_bytes(frame_bytes[(num_pixels*2):(num_pixels*2+2)])
        red = (((pixelRGB565 & 0xF800) >> 11)*255)//31
        green = (((pixelRGB565 & 0x07E0) >> 5)*255)//63
        blue = ((pixelRGB565 & 0x001F)*255)//31
        pixel_grayscale = int((0.299*red)+(0.587*green)+(0.114*blue))
        grayscale_bytes_pixels+=(pixel_grayscale.to_bytes(1,byteorder='little'))
    
    return grayscale_bytes_pixels

if __name__ == "__main__":
    print("Oi")
    EVA_CON = ("192.168.0.100",12345)
    ESP_MAX_BYTES = 1460
    SOCK_UDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    SOCK_UDP.settimeout(1000)
    lost_packages = 0
    num_packages = 0
    receving_frame = False
    sucess_connection = False
    frame_bytes = b''
    ping(0,1)
    print("Conexão com EVA estabelecida")
    sucess_connection = True
    
    while True:
        if not receving_frame: 
            eva_response = ping(1,ESP_MAX_BYTES)
            if not eva_response:
                print("Não ta respondendo")
                continue
            num_packages = int.from_bytes(eva_response[0], byteorder="little")
            print("NUMERO DE PKGS: "+str(num_packages))
            receving_frame = True

        
        for counter in range(num_packages):
            print(f"{counter+1}/{num_packages}")
            eva_response = ping(2,ESP_MAX_BYTES)
            if not eva_response:
                frame_bytes = b''
                break

            if counter == 26:
                frame_bytes+= eva_response[0][0:440]
                continue
                
            frame_bytes += eva_response[0]
        
        frame_bytes = convertToGray(frame_bytes)
        receving_frame = False
        frame_bytes = b''