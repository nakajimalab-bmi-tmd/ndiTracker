import ndiTracker

aurora = ndiTracker.aurora()
aurora.auto_connect()
sensors = aurora.configure_sensors()
aurora.start()

try:
    aurora.update()
    data = aurora.get(sensors[0])
except KeyboardInterrupt:
    pass


aurora.stop()
