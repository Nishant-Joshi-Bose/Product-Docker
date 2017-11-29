import logging

def get_logger(modulename):
    if '.' in modulename:
        new_list = modulename.split('.')
        modulename = new_list[len(new_list)-1]
        if 'py' in modulename:
            modulename = new_list[len(new_list)-2]
    if '/' in modulename:
        new_list = modulename.split('/')
        modulename = new_list[len(new_list)-1]
        if '.' in modulename:
            modulename = modulename.split('.')[0]
    log_format = '%(asctime)s [%(name)s:%(lineno)d]: %(levelname)-5s: %(message)s'
    logging.basicConfig( format=log_format, level=logging.DEBUG )
    return logging.getLogger(modulename)
