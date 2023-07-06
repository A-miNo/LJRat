def str_to_c_str(str_to_be_conv):
    return (str_to_be_conv.encode() + b'\x00')