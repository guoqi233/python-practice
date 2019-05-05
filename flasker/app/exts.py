from celery import Celery
from celery import platforms
platforms.C_FORCE_ROOT = True

celery_app = Celery()
