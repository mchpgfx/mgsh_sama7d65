"""Main Tk application for SAMA7D65 benchmark automation."""

import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
import os
import sys
import random
import threading
import time

# Windows high-DPI awareness — must be called before Tk init
try:
    import ctypes
    ctypes.windll.shcore.SetProcessDpiAwareness(1)
except Exception:
    pass

# Ensure imports work when run from this directory
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from serial_handler import SerialHandler
from data_store import DataStore, BenchmarkRun
from plot_panel import PlotPanel


class BenchmarkApp:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("SAMA7D65 Benchmark Host")
        data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "benchmark_data")
        self.store = DataStore(data_dir)
        self.current_run = None
        self.bench_running = False
        self.total_configs = 0
        self.configs_done = 0

        self.serial = SerialHandler(
            on_bench_line=self._on_bench_line,
            on_raw_line=self._on_raw_line,
        )

        self._build_ui()
        self._refresh_ports()

        # Draw any historical data
        if self.store.runs:
            self.plot_panel.update_plots(self.store.runs)

        # Poll for thread-safe UI updates
        self._pending_bench_lines = []
        self._pending_raw_lines = []
        self._poll_pending()

        # Deferred resize: force re-layout after widgets are packed
        self.root.after(50, lambda: self.root.geometry("1280x900"))

    def _build_ui(self):
        # Control bar
        ctrl = ttk.Frame(self.root, padding=5)
        ctrl.pack(fill="x")

        ttk.Label(ctrl, text="Port:").pack(side="left")
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(ctrl, textvariable=self.port_var, width=12, state="readonly")
        self.port_combo.pack(side="left", padx=2)
        ttk.Button(ctrl, text="Refresh", command=self._refresh_ports).pack(side="left")
        self.connect_btn = ttk.Button(ctrl, text="Connect", command=self._toggle_connect)
        self.connect_btn.pack(side="left", padx=5)

        ttk.Separator(ctrl, orient="vertical").pack(side="left", fill="y", padx=8)

        ttk.Label(ctrl, text="Test:").pack(side="left")
        self.test_name_var = tk.StringVar(value="Baseline")
        ttk.Entry(ctrl, textvariable=self.test_name_var, width=16).pack(side="left", padx=2)

        self.ping_btn = ttk.Button(ctrl, text="Ping", command=self._ping, state="disabled")
        self.ping_btn.pack(side="left", padx=2)
        self.start_btn = ttk.Button(ctrl, text="Start", command=self._start_bench, state="disabled")
        self.start_btn.pack(side="left", padx=2)
        self.stop_btn = ttk.Button(ctrl, text="Stop", command=self._stop_bench, state="disabled")
        self.stop_btn.pack(side="left", padx=2)

        ttk.Separator(ctrl, orient="vertical").pack(side="left", fill="y", padx=8)
        self.demo_btn = ttk.Button(ctrl, text="Demo", command=self._start_demo)
        self.demo_btn.pack(side="left", padx=2)
        ttk.Button(ctrl, text="Export SVG", command=self._export_svg).pack(side="left", padx=2)
        ttk.Button(ctrl, text="Manage Tests", command=self._manage_tests).pack(side="left", padx=2)

        self.progress = ttk.Progressbar(ctrl, length=120, mode="determinate")
        self.progress.pack(side="left", padx=8)

        self.status_var = tk.StringVar(value="Disconnected")
        ttk.Label(ctrl, textvariable=self.status_var).pack(side="left", padx=5)

        # Main area: paned with plots on top, log on bottom
        pane = ttk.PanedWindow(self.root, orient="vertical")
        pane.pack(fill="both", expand=True, padx=5, pady=5)

        plot_frame = ttk.Frame(pane)
        pane.add(plot_frame, weight=4)

        self.plot_panel = PlotPanel(plot_frame)

        log_container = ttk.Frame(pane)
        pane.add(log_container, weight=1)
        log_container.columnconfigure(0, weight=1)
        log_container.columnconfigure(1, weight=1)
        log_container.rowconfigure(0, weight=1)

        raw_frame = ttk.LabelFrame(log_container, text="Board Output")
        raw_frame.grid(row=0, column=0, sticky="nsew", padx=(0, 2))
        self.raw_log = scrolledtext.ScrolledText(raw_frame, height=8, font=("Consolas", 9))
        self.raw_log.pack(fill="both", expand=True)

        bench_frame = ttk.LabelFrame(log_container, text="Benchmark Protocol")
        bench_frame.grid(row=0, column=1, sticky="nsew", padx=(2, 0))
        self.bench_log = scrolledtext.ScrolledText(bench_frame, height=8, font=("Consolas", 9))
        self.bench_log.pack(fill="both", expand=True)

    def _refresh_ports(self):
        ports = SerialHandler.list_ports()
        self.port_combo["values"] = ports
        if ports and not self.port_var.get():
            self.port_var.set(ports[0])

    def _toggle_connect(self):
        if self.serial.is_connected:
            self.serial.disconnect()
            self.connect_btn.config(text="Connect")
            self.start_btn.config(state="disabled")
            self.ping_btn.config(state="disabled")
            self.stop_btn.config(state="disabled")
            self.status_var.set("Disconnected")
        else:
            port = self.port_var.get()
            if not port:
                return
            try:
                self.serial.connect(port)
                self.connect_btn.config(text="Disconnect")
                self.start_btn.config(state="normal")
                self.ping_btn.config(state="normal")
                self.status_var.set(f"Connected: {port}")
            except Exception as e:
                self.status_var.set(f"Error: {e}")

    def _ping(self):
        if self.serial.is_connected:
            self._ping_retries = 0
            self._send_ping()

    def _send_ping(self):
        if self._ping_retries >= 5:
            self.status_var.set("Ping: no response after 5 attempts")
            return
        self.serial.send("BENCH_PING")
        self._pending_bench_lines.append(">> BENCH_PING")
        self._ping_retries += 1
        self.status_var.set(f"Ping sent... (attempt {self._ping_retries})")
        self.root.after(300, self._check_pong)

    def _check_pong(self):
        if self.status_var.get().startswith("Board alive"):
            return  # Got PONG
        self._send_ping()

    def _start_bench(self):
        if not self.serial.is_connected:
            return
        name = self.test_name_var.get().strip() or "Unnamed"
        self.current_run = BenchmarkRun(name=name)
        self.bench_running = False
        self.configs_done = 0
        self.total_configs = 0
        self.progress["value"] = 0
        self.start_btn.config(state="disabled")
        self.stop_btn.config(state="normal")
        self._bench_retries = 0
        self._send_bench_start()

    def _send_bench_start(self):
        """Send BENCH_START with retries until board responds."""
        if self.bench_running or self._bench_retries >= 10:
            return
        self.serial.send("BENCH_START")
        self._pending_bench_lines.append(">> BENCH_START")
        self._bench_retries += 1
        self.status_var.set(f"Waiting for board... (attempt {self._bench_retries})")
        # Retry after 500ms if no START response yet
        self.root.after(500, self._send_bench_start)

    def _on_bench_line(self, line):
        """Called from serial thread — queue for main thread."""
        self._pending_bench_lines.append(line)

    def _on_raw_line(self, line):
        """Called from serial thread — queue for main thread."""
        self._pending_raw_lines.append(line)

    def _poll_pending(self):
        """Process queued lines on the main thread."""
        needs_plot_update = False

        while self._pending_bench_lines:
            line = self._pending_bench_lines.pop(0)
            if not line.startswith(">>"):
                if self._process_bench_line(line):
                    needs_plot_update = True
            self._append_to_widget(self.bench_log, line)

        if needs_plot_update:
            all_runs = self.store.runs[:]
            if self.current_run and self.current_run not in all_runs:
                all_runs.append(self.current_run)
            self.plot_panel.update_plots(all_runs, highlight_run=self.current_run)

        while self._pending_raw_lines:
            line = self._pending_raw_lines.pop(0)
            self._append_to_widget(self.raw_log, line)

        self.root.after(100, self._poll_pending)

    def _process_bench_line(self, line):
        """Parse [BENCH,...] protocol lines. Returns True if plot needs update."""
        # Strip brackets: "[BENCH,XXX,...]" -> "BENCH,XXX,..."
        content = line.strip("[]")
        parts = content.split(",")
        if len(parts) < 2:
            return False

        msg_type = parts[1]

        if msg_type == "PONG":
            self.status_var.set("Board alive (PONG)")

        elif msg_type == "START":
            self.bench_running = True
            if len(parts) >= 3:
                self.total_configs = int(parts[2])
                if self.current_run:
                    self.current_run.total_configs = self.total_configs
            self.progress["maximum"] = self.total_configs
            self.progress["value"] = 0
            self.configs_done = 0
            self.status_var.set(f"Running: 0/{self.total_configs}")

        elif msg_type == "CFG":
            self.configs_done += 1
            if self.total_configs > 0:
                self.progress["value"] = self.configs_done
            cfg_label = parts[3] if len(parts) > 3 else "?"
            self.status_var.set(f"Config {self.configs_done}/{self.total_configs}: {cfg_label}")

        elif msg_type == "DATA":
            if self.current_run and len(parts) >= 7:
                screen = int(parts[2])
                config_label = parts[3]
                sample_idx = int(parts[4])
                fps_val = int(parts[5])
                cpu_val = int(parts[6])
                self.current_run.add_sample(screen, config_label, sample_idx, fps_val, cpu_val)
                return True

        elif msg_type == "DONE":
            self.bench_running = False
            self.start_btn.config(state="normal" if self.serial.is_connected else "disabled")
            self.stop_btn.config(state="disabled")
            self.demo_btn.config(state="normal")
            if self.current_run:
                filepath = self.store.save_run(self.current_run)
                self.status_var.set(f"Done! Saved: {os.path.basename(str(filepath))}")
                self.current_run = None
                return True

        elif msg_type == "ABORTED":
            self.bench_running = False
            self.start_btn.config(state="normal" if self.serial.is_connected else "disabled")
            self.stop_btn.config(state="disabled")
            self.demo_btn.config(state="normal")
            self.status_var.set("Aborted")
            self.current_run = None

        return False

    def _append_to_widget(self, widget, text):
        widget.insert("end", text + "\n")
        widget.see("end")
        line_count = int(widget.index("end-1c").split(".")[0])
        if line_count > 2000:
            widget.delete("1.0", f"{line_count - 1500}.0")

    def _start_demo(self):
        """Simulate a full benchmark run with random data."""
        self._demo_stop = False
        name = self.test_name_var.get().strip() or "Demo"
        self.current_run = BenchmarkRun(name=name)
        self.bench_running = True
        self.demo_btn.config(state="disabled")
        self.start_btn.config(state="disabled")
        self.stop_btn.config(state="normal")

        configs = [
            # Screen 1: FPS Counter
            (1, "size1", 5), (1, "size2", 5), (1, "size3", 5),
            (1, "size4", 5), (1, "size5", 5),
            # Screen 2: Motion Rects
            (2, "cnt1_sz40", 5), (2, "cnt1_sz100", 5), (2, "cnt1_sz200", 5),
            (2, "cnt1_full", 5), (2, "cnt3_sz40", 5), (2, "cnt3_sz100", 5),
            (2, "cnt5_sz40", 5), (2, "cnt5_sz100", 5), (2, "cnt10_sz40", 5),
            # Screen 3: Images
            (3, "png8888_40", 5), (3, "png8888_100", 5),
            (3, "jpg24_40", 5), (3, "jpg24_100", 5),
            (3, "jpg24_200", 5), (3, "jpg24_480", 5),
            (3, "raw565_40", 5), (3, "raw565_100", 5),
            (3, "raw565_200", 5), (3, "raw565_480", 5),
            (3, "rawrle565_40", 5), (3, "rawrle565_100", 5),
            (3, "rawrle565_200", 5), (3, "rawrle565_480", 5),
        ]

        def generate():
            self._on_bench_line(f"[BENCH,START,{len(configs)}]")
            for screen, label, dwell in configs:
                if self._demo_stop:
                    self._on_bench_line("[BENCH,ABORTED]")
                    return
                self._on_bench_line(f"[BENCH,CFG,{screen},{label},{dwell}]")
                # Base FPS varies by screen/config complexity
                base_fps = random.randint(15, 60)
                base_cpu = random.randint(30, 85)
                for s in range(dwell):
                    if self._demo_stop:
                        self._on_bench_line("[BENCH,ABORTED]")
                        return
                    fps_val = base_fps + random.randint(-3, 3)
                    cpu_val = min(100, max(0, base_cpu + random.randint(-5, 5)))
                    self._on_bench_line(
                        f"[BENCH,DATA,{screen},{label},{s},{fps_val},{cpu_val}]"
                    )
                    time.sleep(0.05)  # Fast demo: 50ms per sample
            self._on_bench_line("[BENCH,DONE]")

        threading.Thread(target=generate, daemon=True).start()

    def _stop_bench(self):
        if hasattr(self, '_demo_stop'):
            self._demo_stop = True
        if self.serial.is_connected:
            self.serial.send("BENCH_STOP")
            self._pending_bench_lines.append(">> BENCH_STOP")
        self.bench_running = False
        self.start_btn.config(state="normal" if self.serial.is_connected else "disabled")
        self.stop_btn.config(state="disabled")
        self.demo_btn.config(state="normal")
        self.status_var.set("Stopped")

    def _manage_tests(self):
        """Open a dialog to select and delete test runs."""
        if not self.store.runs:
            self.status_var.set("No saved tests to manage")
            return

        dlg = tk.Toplevel(self.root)
        dlg.title("Manage Tests")
        dlg.geometry("400x350")
        dlg.transient(self.root)
        dlg.grab_set()

        ttk.Label(dlg, text="Select tests to delete:", padding=5).pack(anchor="w")

        list_frame = ttk.Frame(dlg)
        list_frame.pack(fill="both", expand=True, padx=5)

        scrollbar = ttk.Scrollbar(list_frame)
        scrollbar.pack(side="right", fill="y")

        listbox = tk.Listbox(list_frame, selectmode="extended", font=("Consolas", 9),
                             yscrollcommand=scrollbar.set)
        listbox.pack(fill="both", expand=True)
        scrollbar.config(command=listbox.yview)

        run_map = {}
        for i, run in enumerate(self.store.runs):
            label = f"{run.name}  ({run.timestamp}, {len(run.samples)} samples)"
            listbox.insert("end", label)
            run_map[i] = run

        btn_frame = ttk.Frame(dlg, padding=5)
        btn_frame.pack(fill="x")

        def select_all():
            listbox.select_set(0, "end")

        def delete_selected():
            sel = listbox.curselection()
            if not sel:
                return
            count = len(sel)
            if not messagebox.askyesno(
                "Confirm Delete",
                f"Delete {count} test(s)? This removes the JSON files permanently.",
                parent=dlg,
            ):
                return
            # Delete in reverse order to keep indices valid
            for idx in reversed(sel):
                run = run_map[idx]
                self.store.delete_run(run)
                listbox.delete(idx)
            # Rebuild run_map
            run_map.clear()
            for i, run in enumerate(self.store.runs):
                run_map[i] = run
            # Refresh plots
            self.plot_panel.update_plots(self.store.runs)
            self.status_var.set(f"Deleted {count} test(s)")

        ttk.Button(btn_frame, text="Select All", command=select_all).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="Delete Selected", command=delete_selected).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="Close", command=dlg.destroy).pack(side="right", padx=2)

    def _export_svg(self):
        """Export the current plot to SVG."""
        path = filedialog.asksaveasfilename(
            defaultextension=".svg",
            filetypes=[("SVG files", "*.svg"), ("All files", "*.*")],
            initialfile="benchmark_plot.svg",
        )
        if path:
            self.plot_panel.fig.savefig(path, format="svg", bbox_inches="tight")
            self.status_var.set(f"Exported: {os.path.basename(path)}")

    def run(self):
        self.root.protocol("WM_DELETE_WINDOW", self._on_close)
        self.root.mainloop()

    def _on_close(self):
        self.serial.disconnect()
        self.root.destroy()


if __name__ == "__main__":
    app = BenchmarkApp()
    app.run()
