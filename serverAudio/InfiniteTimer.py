import threading

class InfiniteTimer():
    """
    A Thread that executes infinitely, used to reset to idle position
    """
    def __init__(self, t, hFunction, conn):
        self.t = t
        self.hFunction = hFunction
        self.conn = conn
        self.thread = threading.Timer(self.t, self.handle_function)

    def is_timer_on(self):
        if self.thread.is_alive() and not self.thread.finished.is_set():
            return True
        else:
            return False
        
    def handle_function(self):
        self.hFunction(self.conn)
        self.thread.cancel()
        
    def start(self, t = None,  hFunction = None):
        if t == None and hFunction == None:
            self.thread = threading.Timer(self.t, self.handle_function)
        elif t != None and hFunction != None:
            self.thread.cancel()
            self.t = t
            self.hFunction = hFunction
            self.thread = threading.Timer(self.t, self.handle_function)
        else:
            print("arguments not defined")
        self.thread.start()
        
    def cancel(self):
        self.thread.cancel()