"""Data models and JSON persistence for benchmark runs."""

import json
import os
from dataclasses import dataclass, field, asdict
from datetime import datetime
from pathlib import Path


@dataclass
class BenchmarkSample:
    screen: int
    config_label: str
    sample_idx: int
    fps: int
    cpu_used: int


@dataclass
class BenchmarkRun:
    name: str
    timestamp: str = ""
    total_configs: int = 0
    samples: list = field(default_factory=list)

    def __post_init__(self):
        if not self.timestamp:
            self.timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    def add_sample(self, screen, config_label, sample_idx, fps, cpu_used):
        self.samples.append(BenchmarkSample(
            screen=screen,
            config_label=config_label,
            sample_idx=sample_idx,
            fps=fps,
            cpu_used=cpu_used,
        ))

    def get_screen_data(self, screen_id):
        """Return {config_label: [samples]} for a given screen."""
        result = {}
        for s in self.samples:
            if s.screen == screen_id:
                result.setdefault(s.config_label, []).append(s)
        return result

    def to_dict(self):
        d = asdict(self)
        return d

    @classmethod
    def from_dict(cls, d):
        run = cls(
            name=d["name"],
            timestamp=d.get("timestamp", ""),
            total_configs=d.get("total_configs", 0),
        )
        for s in d.get("samples", []):
            run.samples.append(BenchmarkSample(**s))
        return run


class DataStore:
    def __init__(self, data_dir="benchmark_data"):
        self.data_dir = Path(data_dir)
        self.data_dir.mkdir(exist_ok=True)
        self.runs = []
        self.run_files = {}  # id(run) -> Path
        self._load_all()

    def _load_all(self):
        """Load all existing JSON result files."""
        for f in sorted(self.data_dir.glob("*.json")):
            try:
                with open(f, "r") as fh:
                    data = json.load(fh)
                run = BenchmarkRun.from_dict(data)
                self.runs.append(run)
                self.run_files[id(run)] = f
            except (json.JSONDecodeError, KeyError):
                pass

    def save_run(self, run):
        """Save a run to JSON and add to the store."""
        safe_name = "".join(c if c.isalnum() or c in "-_ " else "_" for c in run.name)
        filename = f"{safe_name}_{run.timestamp}.json"
        filepath = self.data_dir / filename
        with open(filepath, "w") as fh:
            json.dump(run.to_dict(), fh, indent=2)
        if run not in self.runs:
            self.runs.append(run)
        self.run_files[id(run)] = filepath
        return filepath

    def delete_run(self, run):
        """Delete a run from the store and remove its JSON file."""
        filepath = self.run_files.pop(id(run), None)
        if filepath and filepath.exists():
            filepath.unlink()
        if run in self.runs:
            self.runs.remove(run)

    def get_all_config_labels(self, screen_id):
        """Get ordered unique config labels across all runs for a screen."""
        labels = []
        seen = set()
        for run in self.runs:
            for s in run.samples:
                if s.screen == screen_id and s.config_label not in seen:
                    labels.append(s.config_label)
                    seen.add(s.config_label)
        return labels
