"""Serial connection and background read thread."""

import threading
import serial
import serial.tools.list_ports

from config import BAUD_RATE, BENCH_PREFIX


class SerialHandler:
    def __init__(self, on_bench_line=None, on_raw_line=None):
        self.port = None
        self.ser = None
        self._thread = None
        self._running = False
        self.on_bench_line = on_bench_line
        self.on_raw_line = on_raw_line

    @staticmethod
    def list_ports():
        """Return list of available COM port names."""
        return [p.device for p in serial.tools.list_ports.comports()]

    def connect(self, port):
        """Open serial connection."""
        self.disconnect()
        self.port = port
        self.ser = serial.Serial(port, BAUD_RATE, timeout=0.1)
        self._running = True
        self._thread = threading.Thread(target=self._read_loop, daemon=True)
        self._thread.start()

    def disconnect(self):
        """Close serial connection."""
        self._running = False
        if self._thread and self._thread.is_alive():
            self._thread.join(timeout=2)
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.ser = None
        self._thread = None

    @property
    def is_connected(self):
        return self.ser is not None and self.ser.is_open

    def send(self, text):
        """Send a string (appends newline)."""
        if self.is_connected:
            self.ser.write((text + "\n").encode("ascii"))

    def _read_loop(self):
        """Background thread: read lines and dispatch."""
        while self._running and self.ser and self.ser.is_open:
            try:
                line = self.ser.readline()
                if not line:
                    continue
                text = line.decode("ascii", errors="replace").strip()
                if not text:
                    continue
                if text.startswith(BENCH_PREFIX):
                    if self.on_bench_line:
                        self.on_bench_line(text)
                else:
                    if self.on_raw_line:
                        self.on_raw_line(text)
            except serial.SerialException:
                self._running = False
                break
            except Exception:
                pass
