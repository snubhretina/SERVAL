import os
import numpy as np
import cv2
import time
import datetime
from PIL import Image
import pandas as pd
import csv
from pathlib import Path
import json
import socket
import traceback

def write_user_log(current_time, account_name, addr, dataset_name, file_name, log_text, path_cfg):
    user_log = f"{current_time} {addr[0]}:{addr[1]} ({account_name}) : {dataset_name} / {file_name} : {log_text}"

    save_result_path = Path.joinpath(path_cfg['result_dir'], account_name, "log")
    os.makedirs(save_result_path, exist_ok = True)

    with open(Path.joinpath(save_result_path,"log_file.log"), 'a+') as f:
        f.write(user_log + "\n")
        f.close()

def clear_socket_buffer(sock):
    try:
        sock.settimeout(0.1)  # 비우기 위해 짧은 타임아웃 설정
        while True:
            # 버퍼에 남은 데이터를 모두 읽음
            data = sock.recv(4096)
            if not data:
                break

    except socket.timeout:
        pass  # 타임아웃 발생 시 버퍼 비우기 완료
    finally:
        sock.settimeout(None)  # 타임아웃 복원

def threaded(client_socket, client_sockets, addr, serval_model, path_cfg, error_config, logger):
    # logger.info('Connected by ' + addr[0] + ':' + str(addr[1]))
    while True:
        try:
            # -------- Receive Commands -------- #
            data_order, _ = client_socket.recvfrom(16)
            
            # -------- Extract Vessel Mask and Optic Disc -------- #
            if data_order.__str__().find("extract") > -1:
                try:
                    start_time = time.time()
                    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                    
                    # receive account name length #
                    account_name_len_byte, _ = client_socket.recvfrom(16)

                    # receive account name #
                    account_name_byte, _ = client_socket.recvfrom(int(account_name_len_byte))
                    account_name = account_name_byte.decode()

                    # receive dataset name length #
                    dataset_name_len_byte, _ = client_socket.recvfrom(16)

                    # receive dataset name #
                    dataset_name_byte, _ = client_socket.recvfrom(int(dataset_name_len_byte))
                    dataset_name = dataset_name_byte.decode()

                    # Get image size #
                    length, _ = client_socket.recvfrom(16)
                    fn_len, _ = client_socket.recvfrom(16)
                    client_socket.send(fn_len)
                    data, _ = client_socket.recvfrom(int(fn_len))
                    data = data.decode('ascii')

                    # Get image extension #
                    if data.find(".png") > -1:
                        filename = data.split(".png")[0]
                    elif data.find(".jpg") > -1:
                        filename = data.split(".jpg")[0]
                    
                    client_socket.send(length)
                    height, _ = client_socket.recvfrom(16)
                    width, _ = client_socket.recvfrom(16)
                    height = int(height)
                    width = int(width)
                    
                    write_user_log(current_time, account_name, addr, dataset_name, filename, "Init mask and disc.", path_cfg)
                    
                    all_data = bytearray()

                    while True:
                        stringData = client_socket.recv(4096)
                        if not stringData:
                            break
                        
                        all_data.extend(stringData)

                        if b'end' in stringData:
                            break

                    if all_data.endswith(b'end'):
                        all_data = all_data[:-3]
                    
                    received_data = np.frombuffer(all_data, dtype='uint8')

                    fundus_img = received_data[:int(length)].reshape([height,width,3])
                    fundus_img = cv2.cvtColor(fundus_img, cv2.COLOR_BGR2RGB)

                    input_fundus_img_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['fundus_img_folder'])
                    os.makedirs(input_fundus_img_path, exist_ok = True)

                    Image.fromarray(fundus_img).save(Path.joinpath(input_fundus_img_path, filename + ".png"))

                    # [fovea, disc], disc_length = find_disc_fovea_Faster_RCNN.find_disc_fovea(fundus_img)        

                    # fastrcnn 모델 돌아감 #
                    [fovea, disc], disc_length = serval_model.find_disc_fovea(fundus_img)        

                    # HVGN SSA 모델 돌아감 #
                    prob_map, classfication_img = serval_model.vessel_classification(fundus_img)            
                    
                    classfication_img = (classfication_img > 200).astype(np.uint8)*255

                    classfication_img_data = classfication_img.reshape(-1)
                    prob_map_data = prob_map.reshape(-1)

                    classfication_img_length = str(classfication_img_data.__len__())
                    prob_map_length = str(prob_map_data.__len__())

                    init_AV_mask_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['init_av_mask_folder'], account_name)
                    os.makedirs(init_AV_mask_path, exist_ok = True)

                    Image.fromarray(classfication_img).save(Path.joinpath(init_AV_mask_path, filename + "_AV_mask.png"))
                    Image.fromarray(prob_map).save(Path.joinpath(init_AV_mask_path, filename + "_prob_map.png"))


                    #send fovea location
                    try:
                        client_socket.send(bytes(str(fovea[0]).rjust(16), 'utf-8'))
                        client_socket.send(bytes(str(fovea[1]).rjust(16), 'utf-8'))
                    except:
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))

                    #send disc location
                    try:
                        client_socket.send(bytes(str(disc[0]).rjust(16), 'utf-8'))
                        client_socket.send(bytes(str(disc[1]).rjust(16), 'utf-8'))
                    except:
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))

                    #send classified a/v image
                    client_socket.send(bytes(classfication_img_length.rjust(16), 'utf-8'))
                    client_socket.send(classfication_img_data.tobytes())

                    #send prob image
                    client_socket.send(bytes(prob_map_length.rjust(16), 'utf-8'))
                    client_socket.send(prob_map_data.tobytes())

                    #send fovea location
                    try:
                        client_socket.send(bytes(str(fovea[0]).rjust(16), 'utf-8'))
                        client_socket.send(bytes(str(fovea[1]).rjust(16), 'utf-8'))
                    except:
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))

                    #send disc location
                    try:
                        client_socket.send(bytes(str(disc[0]).rjust(16), 'utf-8'))
                        client_socket.send(bytes(str(disc[1]).rjust(16), 'utf-8'))
                    except:
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))
                        client_socket.send(bytes(str('0').rjust(16), 'utf-8'))
                    
                    end_time = time.time()
                    inference_time = end_time - start_time
                    inference_time = str(datetime.timedelta(seconds=int(inference_time)))

                    inference_time_key = ['save_time', 'hour', 'minute', 'second']
                    inference_time_list = inference_time.split(':')
                    inference_time_dict = dict(zip(inference_time_key, [current_time] + inference_time_list))
                    inference_time_df = pd.DataFrame(inference_time_dict, index=[0])
                    
                    inference_time_csv_path = Path.joinpath(init_AV_mask_path, filename + "_inference_time.csv")

                    if os.path.exists(inference_time_csv_path):
                        inference_time_df.to_csv(inference_time_csv_path, mode='a', header=False, index=False)
                    else:
                        inference_time_df.to_csv(inference_time_csv_path, mode='w', header=True, index=False)

                except:
                    error_message = traceback.format_exc()
                    logger.info(addr[0] + ':' + str(addr[1]) + f' ({account_name}) : {error_message}')
                    break

            # ---------------------------------------------------- #

            # -------------- Receive Log Data -------------- #
            elif data_order.__str__().find('log') > -1:
                account_name_len_byte, _ = client_socket.recvfrom(16)
                account_name_byte, _ = client_socket.recvfrom(int(account_name_len_byte))

                # receive dataset name length #
                dataset_name_len_byte, _ = client_socket.recvfrom(16)

                # receive dataset name #
                dataset_name_byte, _ = client_socket.recvfrom(int(dataset_name_len_byte))
                dataset_name = dataset_name_byte.decode()

                log_len_byte, _ = client_socket.recvfrom(16)
                log_text_byte, _ = client_socket.recvfrom(int(log_len_byte))

                account_name = account_name_byte.decode()

                log_text = log_text_byte.decode()

                save_result_path = Path.joinpath(path_cfg['result_dir'], account_name, "log")
                # log_save_path = result_log_path + account_name

                log_text = log_text[:20] + f'{addr[0]}:{addr[1]} ({account_name}) : {dataset_name} / ' + log_text[20:]

                os.makedirs(save_result_path, exist_ok = True)

                with open(Path.joinpath(save_result_path,"log_file.log"), 'a+') as f:
                    f.write(log_text + "\n")
                    f.close()
            # ---------------------------------------------------- #
            
            # -------------- Receive Editing Result -------------- #
            elif data_order.__str__().find("result") > -1:
                try:
                    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                    # receive account name length #
                    account_name_len_byte, _ = client_socket.recvfrom(16)

                    # receive account name #
                    account_name_byte, _ = client_socket.recvfrom(int(account_name_len_byte))
                    account_name = account_name_byte.decode()
                    
                    # logger.info(addr[0] + ':' + str(addr[1]) + f' ({account_name}) : Save Result.')
                    # receive dataset name length #
                    dataset_name_len_byte, _ = client_socket.recvfrom(16)

                    # receive dataset name #
                    dataset_name_byte, _ = client_socket.recvfrom(int(dataset_name_len_byte))
                    dataset_name = dataset_name_byte.decode()

                    # receive image name length # 
                    fn_len, _ = client_socket.recvfrom(16)
                    # client_socket.send(fn_len)
                    
                    # receive image name # 
                    data, _ = client_socket.recvfrom(int(fn_len))
                    data = data.decode('ascii')

                    if data.find(".png") > -1:
                        filename = data.split(".png")[0]
                    elif data.find(".jpg") > -1:
                        filename = data.split(".jpg")[0]
                    else:
                        filename = ""
                    
                    write_user_log(current_time, account_name, addr, dataset_name, filename, "Save Result.", path_cfg)

                    # receive elapsed time length # 
                    elapsed_time_len_byte, _ = client_socket.recvfrom(16)

                    # receive elapsed time # 
                    elapsed_time_byte, _ = client_socket.recvfrom(int(elapsed_time_len_byte))
                    elapsed_time = elapsed_time_byte.decode()

                    # receive elipse data num (3 or 6) #
                    ellipse_data_nunm_byte, _ = client_socket.recvfrom(16)
                    ellipse_data_num = int(ellipse_data_nunm_byte)

                    # receive elipse data (3 or 6) #
                    # disc circle x, y, radius
                    # disc ellipse x, y, radius, width, height, angle
                    ellipse_data_list = [current_time]
                    for i in range(0, ellipse_data_num):
                        ellipse_data, _ = client_socket.recvfrom(16)
                        if str(ellipse_data).find("-") != -1:
                            ellipse_data = float(str(ellipse_data).split("-")[-1][:-1]) * (-1)
                        else:
                            ellipse_data = float(ellipse_data)

                        ellipse_data_list.append(ellipse_data)
                    
                    if ellipse_data_num == 3:
                        ellipse_data_list.extend([0, 0, 0])

                    # receive result image height, width, channel #
                    height_byte, _ = client_socket.recvfrom(16)
                    width_byte, _ = client_socket.recvfrom(16)
                    channel_byte, _ = client_socket.recvfrom(16)

                    height = int(height_byte)
                    width = int(width_byte)
                    channel = int(channel_byte)
                    data_len = height * width * channel
                    
                    # receive result image data #
                    length = height * width * channel
                    save_result_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['result_folder'], account_name, path_cfg['mask_folder'])

                    os.makedirs(save_result_path, exist_ok = True)

                    # ---- Receive ROI mask data ---- #
                    
                    count = 0
                    while True:
                        all_data = bytearray() 

                        while True:
                            stringData = client_socket.recv(4096)
                            if not stringData:
                                break
                            
                            all_data.extend(stringData)

                            if b'end' in stringData:
                                break

                        if all_data.endswith(b'end'):
                            all_data = all_data[:-3]

                        received_data = np.frombuffer(all_data, dtype='uint8')
                        received_data = received_data[:int(length)]

                        if int(length) == received_data.__len__():
                        # if 1 == 0:
                            mask_data = received_data.reshape([height,width,3])
                            client_socket.send(bytes(str(1), encoding='utf-8'))
                            break

                        else:
                            if count == error_config['error_limit']:
                                client_socket.send(bytes(str(2), encoding='utf-8'))
                                raise Exception('Error: Mask data length is not matched.')

                            else:
                                client_socket.send(bytes(str(0), encoding='utf-8'))
                                count += 1

                    mask_img = cv2.cvtColor(mask_data, cv2.COLOR_BGR2RGB)

                    mask_img_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['result_folder'], account_name, path_cfg['mask_folder'])
                    os.makedirs(mask_img_path, exist_ok = True)

                    Image.fromarray(mask_img).save(Path.joinpath(mask_img_path, filename + ".png"))
                    
                    # ---- Receive ROI mask data ---- #
                    all_data = bytearray()

                    while True:
                        stringData = client_socket.recv(4096)
                        if not stringData:
                            break
                        
                        all_data.extend(stringData)

                        if b'end' in stringData:
                            break

                    if all_data.endswith(b'end'):
                        all_data = all_data[:-3]
                    
                    received_data = np.frombuffer(all_data, dtype='uint8')

                    ROI_mask_data = received_data[:int(length)].reshape([height,width,3])
                    ROI_mask_img = cv2.cvtColor(ROI_mask_data, cv2.COLOR_BGR2RGB)

                    ROI_mask_img_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['result_folder'], account_name, path_cfg['ROI_mask_folder'])
                    os.makedirs(ROI_mask_img_path, exist_ok = True)

                    Image.fromarray(ROI_mask_img).save(Path.joinpath(ROI_mask_img_path, "ROI_" + filename + ".png"))

                    #---- receive result json file ----#
                    # json_file_length_byte, _ = client_socket.recvfrom(16)
                    # json_file_length = int(json_file_length_byte)

                    # json_total_data_byte = bytearray()
                    # while True:
                    #     stringData = client_socket.recv(4096)
                    #     if not stringData:
                    #         break
                        
                    #     json_total_data_byte.extend(stringData)

                    #     if b'end' in stringData:
                    #         break

                    # if json_total_data_byte.endswith(b'end'):
                    #     json_total_data_byte = json_total_data_byte[:-3]
                    
                    # json_total_data_byte = json_total_data_byte[:json_file_length]
                    # json_total_data = bytes(json_total_data_byte).decode('utf-8')
                    # json.loads(json_total_data)

                    # metric_json_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['result_folder'], account_name, path_cfg['metric_folder'])
                    # os.makedirs(metric_json_path, exist_ok = True)
                    
                    # received_json_data = json.loads(json_total_data)

                    # json_path = Path.joinpath(metric_json_path, filename + "_metric.json")
                    

                    # with open(json_path, 'w') as json_file:
                    #     json.dump(received_json_data, json_file, indent=4)
                    
                    disc_info_key = ['save_time', 'x', 'y', 'radius', 'width', 'height', 'angle']
                    
                    disc_info_dict = dict(zip(disc_info_key, ellipse_data_list))
                    disc_info_df = pd.DataFrame(disc_info_dict, index=[0])

                    disc_info_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['result_folder'], account_name, path_cfg['disc_info_folder'])
                    os.makedirs(disc_info_path, exist_ok = True)

                    disc_info_csv_path = Path.joinpath(disc_info_path, filename + "_ellipse_data.csv")
                    
                    if os.path.exists(disc_info_csv_path):
                        disc_info_df.to_csv(disc_info_csv_path, mode='a', header=False, index=False)
                    else:
                        disc_info_df.to_csv(disc_info_csv_path, mode='w', header=True, index=False)

                    elapsed_time_key = ['save_time', 'hour', 'minute', 'second']
                    elapsed_time_list = elapsed_time.split(':')
                    elapsed_time_dict = dict(zip(elapsed_time_key, [current_time] + elapsed_time_list))
                    elapsed_time_df = pd.DataFrame(elapsed_time_dict, index=[0])
                    
                    elapsed_time_path = Path.joinpath(path_cfg['result_dir'], account_name, path_cfg['root_result_folder'], dataset_name, path_cfg['result_folder'], account_name, path_cfg['elapsed_time_folder'])

                    os.makedirs(elapsed_time_path, exist_ok = True)
                    elapsed_time_csv_path = Path.joinpath(elapsed_time_path, filename + "_elapsed_time.csv")

                    if os.path.exists(elapsed_time_csv_path):
                        elapsed_time_df.to_csv(elapsed_time_csv_path, mode='a', header=False, index=False)
                    else:
                        elapsed_time_df.to_csv(elapsed_time_csv_path, mode='w', header=True, index=False) 

                except:
                    error_message = traceback.format_exc()
                    logger.info(addr[0] + ':' + str(addr[1]) + f' ({account_name}) : {error_message}')
            # ---------------------------------------------------- #

            # --------------- Receive Error Report --------------- #
            elif data_order.__str__().find('report') > -1:
                try:
                    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                    account_name_len_byte, _ = client_socket.recvfrom(16)
                    account_name_byte, _ = client_socket.recvfrom(int(account_name_len_byte))
                    account_name = account_name_byte.decode()

                    data_length, _ = client_socket.recvfrom(16)
                    client_socket.send(data_length)
                    if((data_length.decode('euc-kr') != '')):
                        print('message error {}'.format(data_length.decode('euc-kr')))

                    try:
                        data, _ = client_socket.recvfrom(int(data_length))
                        data = data.decode('euc-kr')
                    except: 
                        print('report break')
                        break

                    write_user_log(current_time, account_name, addr, "", "", "Error Report.", path_cfg)

                    error_report_dir= Path.joinpath(path_cfg['error_report_dir'], account_name)
                    # error_report_file_path = error_report_path + account_name + "/"
                    report_file_name = "error_report.csv"
                    os.makedirs(error_report_dir, exist_ok = True)

                    report_path = Path.joinpath(error_report_dir, report_file_name)

                    if(os.path.isfile(report_path)):
                        csv_report_file_open = open(report_path, 'a', encoding='utf-8-sig')
                        csv_report_file = csv.writer(csv_report_file_open)
                        csv_report_file.writerow([data])
                    else:
                        csv_report_file_open = open(report_path, 'w', encoding='utf-8-sig')
                        csv_report_file = csv.writer(csv_report_file_open)
                        csv_report_file.writerow([data])

                    csv_report_file_open.flush()
                
                except:
                    error_message = traceback.format_exc()
                    logger.info(addr[0] + ':' + str(addr[1]) + f' ({account_name}) : {error_message}')
            # ---------------------------------------------------- #

            else:
                try:
                    while True:
                        data_order, _ = client_socket.recvfrom(16)
                        print('>> Received from ', data_order.decode('euc-kr'))
                        break
                except:
                    break

        except ConnectionResetError as e:
            error_message = traceback.format_exc()
            logger.info(addr[0] + ':' + str(addr[1]) + f' ({account_name}) : {error_message}')
            logger.info(addr[0] + ':' + str(addr[1]) + f' ({account_name} : Disconnected')
            break
        
        #thread close
        try:
            clear_socket_buffer(client_socket)

        except:
            if client_socket in client_sockets :
                client_sockets.remove(client_socket)
                
        client_socket.close()
        break

    if client_socket in client_sockets :
        client_sockets.remove(client_socket)
        print('remove client list : ',len(client_sockets))