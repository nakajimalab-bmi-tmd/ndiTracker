import ndiTracker

aurora = ndiTracker.aurora()
aurora.auto_connect()
sensors = aurora.configure_sensors()
aurora.start()

try:
    while True:
        aurora.update()
        for sensor in sensors:
            data = aurora.get(sensor)
            if data[0]:
                print(data[1])
except KeyboardInterrupt:
    pass

aurora.stop()
