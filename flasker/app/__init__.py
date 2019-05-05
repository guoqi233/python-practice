from flask import Flask

__all__ = ["create_app", ]
app = Flask(__name__)


def create_app():
    app.config.from_pyfile("../config.py")
    init_ext()
    return app


def init_ext():
    from app.exts import celery_app
    celery_app.config_from_object(app.config.get_namespace("CELERY_"))
