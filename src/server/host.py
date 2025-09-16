import socket
import os
from _thread import *
import time, csv, os
from pathlib import Path
from api import threaded
from model.model import SERVALModel
import threading
from utils.get_config import get_config
from logger.logging import logger

def start_server_1(HOST, PORT, serval_model, path_cfg, error_config, logger):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen()

    client_sockets = []

    try:
        while True:
            print('Server1 Waiting for connection...')
            client_socket, addr = server_socket.accept()
            client_sockets.append(client_socket)
            start_new_thread(threaded, (client_socket, client_sockets, addr, serval_model, path_cfg, error_config, logger))
            
            print("Server1 Cur clients : %d" % len(client_sockets))

    except Exception as e :
        print ('error : ',e)

    finally:
        server_socket.close()

# def start_server_2(HOST, PORT, serval_model, path_cfg, logger):
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#     server_socket.bind((HOST, PORT))
#     server_socket.listen()

#     client_sockets = []

#     try:
#         while True:
#             print('Server2 Waiting for connection...')
#             client_socket, addr = server_socket.accept()
#             client_sockets.append(client_socket)
#             start_new_thread(threaded_for_send_result, (client_socket, addr, serval_model, path_cfg, client_sockets, logger))
            
#             logger.info("Server2 Cur clients : %d" % len(client_sockets))

#     except Exception as e :
#         print ('error : ',e)

#     finally:
#         server_socket.close()
        

def main():
    logger.info('--- Server Starting ---')
    config_path = './config.yaml'
    path_cfg, server_cfg, file_name_cfg, model_cfg, error_config = get_config(config_path)
    
    # 모델 로드 #
    logger.info('Model Loading...')
    serval_model = SERVALModel(model_cfg)    

    logger.info('Server Setting...')
    HOST = server_cfg['host']
    PORT = server_cfg['port']
    logger.info('Server setting finish.')
    os.makedirs(path_cfg['serval_root_dir'], exist_ok = True)
    
    server1_thread = threading.Thread(target = start_server_1, args = (HOST, PORT, serval_model, path_cfg, error_config, logger))
    # server2_thread = threading.Thread(target = start_server_2, args = (HOST, 19990, serval_model, path_cfg, logger))

    server1_thread.start()
    # server2_thread.start()
    
    server1_thread.join()
    # server2_thread.join()


    logger.info('--- Server End ---')

if __name__ == '__main__':
    main()
