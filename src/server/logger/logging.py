import logging

# Logger 객체 생성
logger = logging.getLogger('SERVAL_log')
logger.setLevel(logging.DEBUG)

# 콘솔 핸들러 설정
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.DEBUG)

# 파일 핸들러 설정
file_handler = logging.FileHandler('SERVAl_server.log')
file_handler.setLevel(logging.DEBUG)

# 로그 포맷 설정
formatter = logging.Formatter('%(asctime)s : %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
console_handler.setFormatter(formatter)
file_handler.setFormatter(formatter)


# 클라이언트 로거 객체 생성
logger.addHandler(console_handler)
logger.addHandler(file_handler)