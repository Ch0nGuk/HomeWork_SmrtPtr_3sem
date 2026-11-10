import csv
import math
import statistics
import sys
from pathlib import Path
from xml.sax.saxutils import escape


SERIES = [
    ("raw", "Сырой new/delete", "#2563eb"),
    ("shrd", "ShrdPtr", "#ea580c"),
    ("shared_make", "std::make_shared", "#16a34a"),
]


def read_run(path):
    with open(path, newline="") as file:
        return {
            int(row["N"]): {key: float(row[key]) for key, _, _ in SERIES}
            for row in csv.DictReader(file)
        }


def label_for_n(n):
    if n >= 1_000_000:
        return f"{n // 1_000_000} млн"
    return f"{n // 1_000} тыс"


def main():
    if len(sys.argv) < 2:
        raise SystemExit("Использование: python3 tests/plot_load.py run1.csv [run2.csv ...]")

    runs = [read_run(path) for path in sys.argv[1:]]
    sizes = sorted(runs[0])
    if any(sorted(run) != sizes for run in runs):
        raise ValueError("Наборы значений N в файлах различаются")

    rows = []
    for n in sizes:
        values = {key: statistics.median(run[n][key] for run in runs) for key, _, _ in SERIES}
        rows.append((n, values))

    output = Path("docs")
    output.mkdir(exist_ok=True)
    with (output / "benchmark.csv").open("w", newline="") as file:
        writer = csv.writer(file, lineterminator="\n")
        writer.writerow(["N"] + [key for key, _, _ in SERIES])
        for n, values in rows:
            writer.writerow([n] + [f"{values[key]:.9g}" for key, _, _ in SERIES])

    points = [(n, values) for n, values in rows if n >= 1000]
    if not points:
        raise ValueError("Для графика нужны измерения начиная с N = 1000")

    left, right, top, bottom = 90, 930, 125, 465
    max_ns = max(values[key] * 1_000_000 / n for n, values in points for key, _, _ in SERIES)
    y_max = max(5, math.ceil(max_ns / 5) * 5)

    def x(n):
        return left + (math.log10(n) - math.log10(points[0][0])) / (math.log10(points[-1][0]) - math.log10(points[0][0])) * (right - left)

    def y(ns):
        return bottom - ns / y_max * (bottom - top)

    svg = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="960" height="550" viewBox="0 0 960 550">',
        '<title>Сравнение времени работы указателей</title>',
        '<desc>Медиана трёх запусков. Время одной итерации в наносекундах для разных N.</desc>',
        '<rect width="960" height="550" fill="#ffffff"/>',
        '<g font-family="Lato, Arial, sans-serif" fill="#17212f">',
        '<text x="90" y="42" font-size="25" font-weight="700">Сравнение указателей</text>',
        f'<text x="90" y="65" font-size="14" fill="#5a6878">Медиана {len(runs)} запусков · создание, чтение и удаление одного объекта</text>',
    ]

    for legend_x, (_, label, color) in zip((90, 360, 600), SERIES):
        svg.append(f'<line x1="{legend_x}" y1="91" x2="{legend_x + 26}" y2="91" stroke="{color}" stroke-width="3"/>')
        svg.append(f'<text x="{legend_x + 34}" y="96" font-size="14">{escape(label)}</text>')

    for tick in range(0, y_max + 1, 5):
        tick_y = y(tick)
        svg.append(f'<line x1="{left}" y1="{tick_y:.1f}" x2="{right}" y2="{tick_y:.1f}" stroke="#e1e7ee"/>')
        svg.append(f'<text x="{left - 12}" y="{tick_y + 5:.1f}" text-anchor="end" font-size="13" fill="#5a6878">{tick}</text>')

    for n, _ in points:
        tick_x = x(n)
        svg.append(f'<line x1="{tick_x:.1f}" y1="{top}" x2="{tick_x:.1f}" y2="{bottom}" stroke="#f0f3f6"/>')
        svg.append(f'<text x="{tick_x:.1f}" y="{bottom + 23}" text-anchor="middle" font-size="13" fill="#5a6878">{label_for_n(n)}</text>')

    for key, _, color in SERIES:
        coords = [(x(n), y(values[key] * 1_000_000 / n)) for n, values in points]
        path = " ".join(f"{px:.1f},{py:.1f}" for px, py in coords)
        svg.append(f'<polyline points="{path}" fill="none" stroke="{color}" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>')
        for px, py in coords:
            svg.append(f'<circle cx="{px:.1f}" cy="{py:.1f}" r="4" fill="{color}"/>')

    svg.extend([
        f'<line x1="{left}" y1="{bottom}" x2="{right}" y2="{bottom}" stroke="#657387"/>',
        f'<line x1="{left}" y1="{top}" x2="{left}" y2="{bottom}" stroke="#657387"/>',
        '<text x="510" y="522" text-anchor="middle" font-size="15">Число итераций N (логарифмическая шкала)</text>',
        '<text x="25" y="300" text-anchor="middle" transform="rotate(-90 25 300)" font-size="15">Время на итерацию, нс</text>',
        '</g></svg>',
    ])
    (output / "benchmark.svg").write_text("\n".join(svg) + "\n", encoding="utf-8")
    print("Созданы docs/benchmark.csv и docs/benchmark.svg")


if __name__ == "__main__":
    main()
