from celery.app.task import Task
from app import create_app

flask_app = create_app()


class BaseCeleryTask(Task):
    def run(self, *args, **kwargs):
        """The body of the task executed by workers."""
        raise NotImplementedError('Tasks must define the run method.')

    def __call__(self, *args, **kwargs):
        with flask_app.app_context():
            return super(BaseCeleryTask, self).__call__(*args, **kwargs)
