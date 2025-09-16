import yaml
from pathlib import Path

def get_config(config_path):
    config = yaml.load(open(config_path, 'r'), Loader=yaml.FullLoader)
    root_path = Path(config['path']['serval_log_root_path'])

    path_cfg = {'serval_root_dir': root_path,
                'server_log_dir': root_path / config['path']['server_log_folder'],
                'error_report_dir': root_path / config['path']['error_report_folder'],
                'result_dir': root_path / config['path']['result'],

                'root_result_folder': config['path']['root_result_folder'],
                'fundus_img_folder': config['path']['fundus_img_folder'],
                'init_av_mask_folder': config['path']['init_av_mask_folder'],
                'result_folder': config['path']['result_folder'],

                'disc_info_folder': config['path']['disc_info_folder'],
                'elapsed_time_folder': config['path']['elapsed_time_folder'],
                'mask_folder': config['path']['mask_folder'],
                'metric_folder': config['path']['metric_folder'],
                'ROI_mask_folder': config['path']['ROI_mask_folder']
                }
    
    error_config = config['error']

    server_cfg = config['server']
    file_name_cfg = config['file']
    model_cfg = config['model']


    return path_cfg, server_cfg, file_name_cfg, model_cfg, error_config