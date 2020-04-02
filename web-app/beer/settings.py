INSTALLED_APPS = (
    'chartjs',
    'django.contrib.staticfiles'
)
STATIC_URL = '/static/'
STATIC_ROOT = join(data_dir, "static")
TEMPLATE_CONTEXT_PROCESSORS = [
    'django.core.context_processors.static',
]
