import datetime  

def get_timestamp_str():
    current_time = datetime.datetime.now()
    timestamp = current_time.timestamp()
    date_time = datetime.datetime.fromtimestamp(timestamp)
    str_date_time = date_time.strftime("%d-%m-%Y-%H-%M-%S")
    return str_date_time

def str_to_c_str(str_to_be_conv):
    return (str_to_be_conv.encode() + b'\x00')