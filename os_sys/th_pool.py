import traceback
from threading import Thread, Lock
from queue import Queue, Empty
import time
import random


class WorkThread(Thread):
    def __init__(self, job_queue, manager, max_count=100):
        super(WorkThread, self).__init__()
        self.job_queue = job_queue
        self.manager = manager
        self.max_count = max_count
        self.cur_count = 0
        self.working = False
        self.stop = False

    def run(self):
        while self.cur_count < self.max_count:
            if self.stop:
                break
            try:
                self.working = True
                self.execute()
            except Exception:
                traceback.print_exc()
                pass
            finally:
                self.working = False
                self.cur_count += 1

        self.remove_th()

    def execute(self):
        try:
            job = self.job_queue.get(timeout=3)
        except Empty:
            return
        else:
            print("th {0} start work. get {1}".format(self.name, job))
            time.sleep(0.5 + random.uniform(0, 0.5))

    def remove_th(self):
        self.manager.remove_th(self)


class ManageThread(Thread):
    def __init__(self, pool):
        super(ManageThread, self).__init__()
        self.pool = pool
        self.lock = Lock()

    def run(self):
        while self.pool.running:
            while len(self.pool.worker) < self.pool.max_th_num:
                th = WorkThread(job_queue=self.pool.job_queue, manager=self)
                self.pool.worker.append(th)
                print("create new thread.")
                th.setDaemon(False)
                th.start()
            else:
                time.sleep(1)

    def remove_th(self, th):
        self.lock.acquire()
        if th in self.pool.worker:
            self.pool.worker.remove(th)
        self.lock.release()


class ThreadPool(object):
    def __init__(self, max_th_num=1):
        self.job_queue = Queue()
        self.running = True

        self.max_th_num = max_th_num

        self.worker = list()
        self.manager = ManageThread(self)

    def shutdown(self):
        self.running = False
        for th in self.worker:
            th.stop = True
        for th in self.worker:
            th.join()
        self.manager.join()

    def start(self):
        self.manager.setDaemon(False)
        self.manager.start()


def main():
    pool = ThreadPool(max_th_num=8)
    pool.start()
    time.sleep(0.1)
    for i in range(200):
        pool.job_queue.put(i)
    time.sleep(20)
    pool.shutdown()


if __name__ == "__main__":
    main()
