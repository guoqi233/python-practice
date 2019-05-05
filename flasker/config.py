from kombu import Exchange, Queue

DEBUG = True


exchange_demo = Exchange(name="exchange_demo", type="direct")

CELERY_BROKER_URL = 'amqp://{0}:{1}@localhost:5672/demo'.format("demo", "1")
CELERY_TIMEZONE = "Asia/Shanghai"
CELERY_TASK_QUEUES = [
    Queue(name="queue_demo",
          exchange=exchange_demo,
          routing_key="demo",
          message_ttl=10.0
          ),
]
CELERY_TASK_ROUTES = {
    'app.tasks.demo.*': {'queue': 'queue_demo', 'routing_key': 'demo'}
}
CELERY_IMPORTS = ("app.tasks", )
CELERY_WORKER_MAX_TASKS_PER_CHILD = 100
