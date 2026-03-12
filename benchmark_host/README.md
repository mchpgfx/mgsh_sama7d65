# SAMA7D65 Benchmark Host

Python Tk application for automated graphics benchmark testing on the SAMA7D65 Curiosity board. Triggers a firmware-side benchmark sweep over UART, captures FPS and CPU data in real time, plots results, and persists runs as JSON for comparison.

## Requirements

- Python 3.10+
- SAMA7D65 Curiosity board with benchmark firmware flashed

Install dependencies:

```
pip install -r requirements.txt
```

## Important: Reflash Before Each Test

The firmware benchmark state machine must be freshly initialized before starting a test. **Reflash the board before each benchmark run** to ensure clean state. Without reflashing, the UART command receiver may not respond reliably.

## Quick Start

1. **Flash the firmware** to the board using MPLAB X
2. **Run the app**:
   ```
   cd benchmark_host
   python benchmark_app.py
   ```
3. **Connect**: Select the COM port from the dropdown, click **Connect**
4. **Ping**: Click **Ping** to verify the board responds (`[BENCH,PONG]` should appear in the Benchmark Protocol log)
5. **Name your test**: Enter a descriptive name (e.g. "Baseline", "After GPU fix", "v2.1")
6. **Start**: Click **Start** — the app auto-retries until the board acknowledges
7. **Wait**: The benchmark cycles through 28 configurations (~3 minutes). Progress bar and status update in real time
8. **Done**: Results are saved to `benchmark_data/` as JSON and plotted automatically

## UI Overview

```
[Port][Refresh][Connect] | [Ping] [Test name][Start][Stop] | [Demo][Export SVG][Manage Tests] [Progress] Status
+---------------------------+---------------------------+
|   Counter - FPS           |   Counter - CPU %         |
+---------------------------+---------------------------+
|   Motion Rects - FPS      |   Motion Rects - CPU %    |
+---------------------------+---------------------------+
|   Image Render - FPS      |   Image Render - CPU %    |
+---------------------------+---------------------------+
| Board Output              | Benchmark Protocol        |
+---------------------------+---------------------------+
```

### Buttons

| Button | Description |
|--------|-------------|
| **Refresh** | Rescan available COM ports |
| **Connect/Disconnect** | Open or close the serial connection (115200 baud) |
| **Ping** | Send `BENCH_PING` to verify board communication (auto-retries 5x) |
| **Start** | Send `BENCH_START` to begin the benchmark sweep (auto-retries 10x) |
| **Stop** | Send `BENCH_STOP` to abort a running benchmark |
| **Demo** | Run a simulated benchmark with random data (no board needed) |
| **Export SVG** | Save the current plot as a vector SVG file |
| **Manage Tests** | View, select, and delete saved test runs |

### Serial Logs

- **Board Output** (left): Raw UART output from the board (Task_Usage stats, etc.)
- **Benchmark Protocol** (right): `[BENCH,...]` messages and sent commands (prefixed `>>`)

## Benchmark Configurations

The firmware cycles through 28 configurations across 3 screens:

**Screen 1 — Counter** (5 configs): Font sizes 1–5

**Screen 2 — Motion Rects** (9 configs): Combinations of rectangle count (1, 3, 5, 10) and size (40, 100, 200, fullscreen)

**Screen 3 — Image Render** (14 configs): Image format (PNG8888, JPG24, RAW565, RAWRLE565) and size (40x40, 100x100, 200x200, 480x270)

Each configuration runs for 5 seconds with 1 second settle time. Data is sampled at 1 Hz.

## Comparing Runs

- Each completed test is saved as `benchmark_data/<name>_<timestamp>.json`
- On startup, all saved JSON files are loaded and plotted
- Multiple runs are shown as grouped bars side by side with distinct colors
- The current/latest run is at full opacity; historical runs are semi-transparent
- A shared legend at the top identifies each run by name and color

## UART Protocol

**Host to Board** (newline-terminated):
- `BENCH_START\n` — Start benchmark sweep
- `BENCH_STOP\n` — Abort benchmark
- `BENCH_PING\n` — Heartbeat check

**Board to Host** (`[BENCH,...]` prefix):
- `[BENCH,PONG]` — Reply to ping
- `[BENCH,START,<total>]` — Sweep begins
- `[BENCH,CFG,<screen>,<label>,<dwell>]` — New config active
- `[BENCH,DATA,<screen>,<label>,<idx>,<fps>,<cpu>]` — 1/sec sample
- `[BENCH,DONE]` — Sweep complete
- `[BENCH,ABORTED]` — Stopped early

## Troubleshooting

- **No response to Ping/Start**: Reflash the firmware and try again. The UART receiver can miss commands due to overrun errors during heavy printf output.
- **Sporadic Ping responses**: This is expected — the app auto-retries. The firmware clears UART overrun errors each poll cycle but some commands may still be lost.
- **Plots look blurry**: Use **Export SVG** for crisp vector output at any zoom level.
- **UI overflows on launch**: Resize the window once — it will reflow to fit.
