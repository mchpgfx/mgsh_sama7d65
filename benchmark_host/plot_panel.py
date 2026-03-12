"""Matplotlib Figure embedded in Tk with 3x2 subplot grid."""

import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from config import SCREEN_NAMES, RUN_COLORS, HISTORICAL_ALPHA, CURRENT_ALPHA


class PlotPanel:
    def __init__(self, parent):
        self.fig = Figure(figsize=(12, 8), dpi=96)
        self.fig.set_tight_layout(True)
        self.axes = []
        for i in range(3):
            ax_fps = self.fig.add_subplot(3, 2, i * 2 + 1)
            ax_cpu = self.fig.add_subplot(3, 2, i * 2 + 2)
            self.axes.append((ax_fps, ax_cpu))

        self.canvas = FigureCanvasTkAgg(self.fig, master=parent)
        widget = self.canvas.get_tk_widget()
        widget.pack(fill="both", expand=True)

    def update_plots(self, runs, highlight_run=None):
        """Redraw all 6 subplots with grouped bar charts."""
        # Always clear all axes first
        for ax_fps, ax_cpu in self.axes:
            ax_fps.clear()
            ax_cpu.clear()

        if self.fig.legends:
            for leg in self.fig.legends:
                leg.remove()

        if not runs:
            for screen_idx in range(3):
                screen_id = screen_idx + 1
                screen_name = SCREEN_NAMES.get(screen_id, f"Screen {screen_id}")
                self.axes[screen_idx][0].set_title(f"{screen_name} - FPS")
                self.axes[screen_idx][1].set_title(f"{screen_name} - CPU %")
            self.fig.set_tight_layout(True)
            self.canvas.draw()
            return

        for screen_idx in range(3):
            screen_id = screen_idx + 1
            ax_fps, ax_cpu = self.axes[screen_idx]

            # Collect all config labels in order across all runs
            all_labels = []
            seen = set()
            for run in runs:
                for label in run.get_screen_data(screen_id):
                    if label not in seen:
                        all_labels.append(label)
                        seen.add(label)

            if not all_labels:
                ax_fps.set_title(f"{SCREEN_NAMES.get(screen_id, f'Screen {screen_id}')} - FPS")
                ax_cpu.set_title(f"{SCREEN_NAMES.get(screen_id, f'Screen {screen_id}')} - CPU %")
                continue

            n_labels = len(all_labels)
            n_runs = len(runs)
            bar_width = 0.8 / max(n_runs, 1)
            x = np.arange(n_labels)

            for run_idx, run in enumerate(runs):
                screen_data = run.get_screen_data(screen_id)
                fps_vals = []
                cpu_vals = []
                for label in all_labels:
                    samples = screen_data.get(label, [])
                    if samples:
                        fps_vals.append(np.mean([s.fps for s in samples]))
                        cpu_vals.append(np.mean([s.cpu_used for s in samples]))
                    else:
                        fps_vals.append(0)
                        cpu_vals.append(0)

                color = RUN_COLORS[run_idx % len(RUN_COLORS)]
                alpha = CURRENT_ALPHA if run is highlight_run else HISTORICAL_ALPHA
                offset = (run_idx - n_runs / 2 + 0.5) * bar_width

                bars_fps = ax_fps.bar(x + offset, fps_vals, bar_width,
                                      label=run.name, color=color, alpha=alpha)
                bars_cpu = ax_cpu.bar(x + offset, cpu_vals, bar_width,
                                      label=run.name, color=color, alpha=alpha)

                for bar in bars_fps:
                    h = bar.get_height()
                    if h > 0:
                        ax_fps.text(bar.get_x() + bar.get_width() / 2, h,
                                    f"{h:.0f}", ha="center", va="bottom",
                                    fontsize=8, alpha=alpha)
                for bar in bars_cpu:
                    h = bar.get_height()
                    if h > 0:
                        ax_cpu.text(bar.get_x() + bar.get_width() / 2, h,
                                    f"{h:.0f}", ha="center", va="bottom",
                                    fontsize=8, alpha=alpha)

            screen_name = SCREEN_NAMES.get(screen_id, f"Screen {screen_id}")
            ax_fps.set_title(f"{screen_name} - FPS")
            ax_fps.set_xticks(x)
            ax_fps.set_xticklabels(all_labels, rotation=45, ha="right", fontsize=7)
            ax_fps.set_ylabel("FPS")

            ax_cpu.set_title(f"{screen_name} - CPU %")
            ax_cpu.set_xticks(x)
            ax_cpu.set_xticklabels(all_labels, rotation=45, ha="right", fontsize=7)
            ax_cpu.set_ylabel("CPU %")
            ax_cpu.set_ylim(0, 100)

        # Single shared legend at the top of the figure
        handles, labels = self.axes[0][0].get_legend_handles_labels()
        if handles:
            self.fig.legend(handles, labels, loc="upper center",
                            ncol=min(len(handles), 6), fontsize=8,
                            framealpha=0.9)

        self.fig.subplots_adjust(top=0.93, hspace=0.6, bottom=0.08)
        self.canvas.draw()
