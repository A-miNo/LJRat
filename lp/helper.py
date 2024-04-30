"""
Module that adds helper functions that can be utilized throughout modules and LJRat LP
"""

from datetime import datetime

def get_timestamp_str():
    current_time = datetime.datetime.now()
    timestamp = current_time.timestamp()
    date_time = datetime.datetime.fromtimestamp(timestamp)
    str_date_time = date_time.strftime("%d-%m-%Y-%H-%M-%S")
    return str_date_time

def str_to_c_str(str_to_be_conv):
    return (str_to_be_conv.encode() + b'\x00')

def filetime_to_dt(filetime):
    dt = datetime.utcfromtimestamp(WindowsTickToUnixSeconds(filetime))
    return dt.strftime("%m/%d/%Y %H:%M %p")

def WindowsTickToUnixSeconds(windowsTicks):
    WINDOWS_TICK_INTERVAL  = 100e-9 # 100 nanoseconds intervals
    SEC_TO_UNIX_EPOCH = (datetime(1970, 1, 1) - datetime(1601, 1, 1)).total_seconds()
    return windowsTicks * WINDOWS_TICK_INTERVAL - SEC_TO_UNIX_EPOCH

def sizeof_fmt(num):
    for unit in ("", "KB", "MB", "GB", "TB"):
        if abs(num) < 1024.0:
            return f"{num:3.1f}{unit}"
        num /= 1024.0