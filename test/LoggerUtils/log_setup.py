import logging
import os

def get_logger(moduleName, logLevel=logging.DEBUG):

    moduleName = os.path.basename(moduleName)
    moduleName = moduleName.replace('.pyc', '').replace('.py', '')
    log_format = '%(asctime)s [%(name)s:%(lineno)d]: %(levelname)-5s: %(message)s'
    logging.basicConfig(format=log_format, level=logLevel)
    loggerObj = logging.getLogger(moduleName)
    return loggerObj
