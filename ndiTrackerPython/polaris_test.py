import ndiTracker

polaris = ndiTracker.polaris()
polaris.auto_connect()
sensors = polaris.configure_active_trackers()
polaris.start()

try:
    while True:
        polaris.update()
        for sensor in sensors:
            data = polaris.get(sensor)
            if data[0]:
                print(data[1])
except KeyboardInterrupt:
    pass

polaris.stop()
