from peer import eva_connection

MOVE_FORWARD:int = 6
MOVE_BACK:int = 7
TURN_RIGHT:int = 8 
TURN_LEFT:int = 9
STOP:int = 10

def main():
    eva_con:eva_connection = eva_connection("192.168.4.1", 12345)
    eva_con.connect() 
    
    packets:list[bytes] = None
    
    while True:
        escolha:str = str(input("direção: ")) 
        
        match escolha:
            case "w":
                eva_con.send_signal(MOVE_FORWARD)
            case "s":
                eva_con.send_signal(MOVE_BACK)
            case "d":
                eva_con.send_signal(TURN_RIGHT)
            case "a":
                eva_con.send_signal(TURN_LEFT)
            case "q":
                eva_con.send_signal(STOP)
            case "e":
                packets = eva_con.get_packets()
            case "f":
                break
            case _:
                print("Comando errado")
    
        if packets: print(packets[0])
            
if __name__ == "__main__":
    main()
    
