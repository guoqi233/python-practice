from .base import BaseCeleryTask
from celery import current_app


@current_app.task(base=BaseCeleryTask, ignore_result=True)
def demo_func(a, b):
    print("{0} + {1} = {2}".format(a, b, a+b))
