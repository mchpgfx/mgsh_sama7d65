"""Constants for the benchmark host application."""

BAUD_RATE = 115200
BENCH_PREFIX = "[BENCH,"

SCREEN_NAMES = {
    1: "Counter",
    2: "Motion Rects",
    3: "Image Render",
}

# 15-color palette for overlaying multiple runs
RUN_COLORS = [
    "#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd",
    "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf",
    "#aec7e8", "#ffbb78", "#98df8a", "#ff9896", "#c5b0d5",
]

HISTORICAL_ALPHA = 0.5
CURRENT_ALPHA = 1.0
