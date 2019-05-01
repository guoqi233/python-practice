from app import create_app
from celery import current_app as celery

app = create_app()


@app.shell_context_processor
def shell_context():
    return {
        "app": app,
        "celery": celery
    }


if __name__ == "__main__":
    app.run()
