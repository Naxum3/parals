import cv2
import multiprocessing
import time
import argparse
import logging

logging.basicConfig(level=logging.INFO, filename="log/sensor_log.log", filemode="w")


class Sensor:
    def get(self):
        raise NotImplementedError('Subclasses must implement method get()')


class SensorCam(Sensor):
    def __init__(self, camera_name: str, resolution: tuple):
        if camera_name.isdigit():
            camera_name = int(camera_name)
        self.camera_name = camera_name
        self.resolution = resolution
        self.capture = cv2.VideoCapture(self.camera_name)
        if not self.capture.isOpened():
            logging.error("Problem when opening the cam, maybe isn't connect")
            exit()
        self.capture.set(cv2.CAP_PROP_FRAME_WIDTH, self.resolution[0])
        self.capture.set(cv2.CAP_PROP_FRAME_HEIGHT, self.resolution[1])

    def get(self):
        ret, frame = self.capture.read()
        if not ret:
            logging.error("Problem getting an image from the camera, it may have been disabled")
            exit()
        return frame

    def __del__(self):
        self.capture.release()


class SensorX(Sensor):
    def __init__(self, delay: float):
        self._delay = delay
        self._data = 0

    def get(self) -> int:
        time.sleep(self._delay)
        self._data += 1
        return self._data


class WindowImage:
    def __init__(self, display_rate: int):
        self.display_rate = display_rate
        self._running = True

    def show(self, img, datas=[]):
        for i, data in enumerate(datas):
            cv2.putText(img, f'{i + 1}: {data}', (20, (i + 1) * 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        cv2.imshow('Sensors window', img)
        cv2.waitKey(self.display_rate)

    def __del__(self):
        cv2.destroyAllWindows()


def sensor_process(sensor, queue):
    while True:
        queue.put(sensor.get())


def main(camera_name, res, camera_delay):
    sensor_cam = SensorCam(camera_name, res)

    sensor1 = SensorX(0.1)
    sensor2 = SensorX(0.5)
    sensor3 = SensorX(1)

    queue_sensor1 = multiprocessing.Queue()
    queue_sensor2 = multiprocessing.Queue()
    queue_sensor3 = multiprocessing.Queue()

    process1 = multiprocessing.Process(target=sensor_process, args=(sensor1, queue_sensor1))
    process2 = multiprocessing.Process(target=sensor_process, args=(sensor2, queue_sensor2))
    process3 = multiprocessing.Process(target=sensor_process, args=(sensor3, queue_sensor3))

    process1.start()
    process2.start()
    process3.start()

    window = WindowImage(camera_delay)

    data_sensor1 = 0
    data_sensor2 = 0
    data_sensor3 = 0
    try:
        while True:
            frame = sensor_cam.get()

            if not queue_sensor1.empty():
                data_sensor1 = queue_sensor1.get()
            if not queue_sensor2.empty():
                data_sensor2 = queue_sensor2.get()
            if not queue_sensor3.empty():
                data_sensor3 = queue_sensor3.get()

            window.show(frame, [data_sensor1, data_sensor2, data_sensor3])

            key = cv2.waitKey(1)
            if key == ord('q'):
                break

    finally:
        process1.terminate()
        process2.terminate()
        process3.terminate()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('camera_name', type=str, help='camera name')
    parser.add_argument('res', type=int, nargs=2, help='resolution of image, 2 ARGUMENTS, format: w h')
    parser.add_argument('camera_delay', type=int, help='camera delay')
    args = parser.parse_args()
    main(args.camera_name, args.res, args.camera_delay)