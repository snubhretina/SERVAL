import socket
import os
from _thread import *
import time, csv, os
from pathlib import Path
from api import threaded
import threading
from model.model import SERVALModel
from utils.get_config import get_config
from logger.logging import logger

def start_server(num, ip, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((ip, port)) 
    server_socket.listen()
    
    logger.info(f'{num} Server setting finish.')

    client_sockets = []
    try:
        while True:
            logger.info('Waiting for connection...')
            client_socket, addr = server_socket.accept()
            client_sockets.append(client_socket)
            start_new_thread(threaded, (client_socket, addr, serval_model, path_cfg, error_config, logger))
            
            logger.info("Cur clients : %d" % len(client_sockets))

    except Exception as e :
        print ('error : ',e)

    finally:
        server_socket.close()

def main():
    logger.info('--- Server Starting ---')
    config_path = '/mnt/hdd2/jooyoung/23_SERVAL_GUI/220315_serval/serval_test_code/config.yaml'
    path_cfg, server_cfg, file_name_cfg, model_cfg = get_config(config_path)
    
    logger.info('Model Loading...')
    serval_model = SERVALModel(model_cfg)    

    logger.info('Server Setting...')

    HOST = server_cfg['host']
    PORT = server_cfg['port']

    os.makedirs(path_cfg['serval_root_dir'], exist_ok = True)

    start_new_thread(start_server, (1, HOST, PORT))
    start_new_thread(start_server, (2, HOST, PORT + 1000))

    logger.info('--- Server End ---')

if __name__ == '__main__':
    main()
