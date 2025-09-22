import time

class Stopwatch:
  def __init__(self):
    self.start = time.time_ns()
    self.mid = time.time_ns()
  
  def lap(self):
    ret = time.time_ns() - self.mid
    self.mid = time.time_ns()
    return ret / 1e9
    
  def time(self):
    return (time.time_ns() - self.start) / 1e9
  
  def time_lap(self):
    return (time.time_ns() - self.mid) / 1e9
  
  def reset(self):
    self.start = time.time_ns()
    self.mid = time.time_ns()
  
  def sleep(self, sec):
    time.sleep(max(0, sec))