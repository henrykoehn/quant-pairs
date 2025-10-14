import os
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import PercentFormatter

script_dir = os.path.dirname(os.path.abspath(__file__))

# ✅ Read the file your C++ exporter writes
trades_path = os.path.join(script_dir, "wwwroot", "trades.csv")
df = pd.read_csv(trades_path, parse_dates=["Date"]).set_index("Date")

plt.figure(figsize=(12, 6))
plt.plot(df.index, df["CumulativePNL"], label="Cumulative P&L")

buy  = df[df["Signal"] == 1]
sell = df[df["Signal"] == -1]
plt.scatter(buy.index,  buy["CumulativePNL"],  marker="^", label="Long Entry")
plt.scatter(sell.index, sell["CumulativePNL"], marker="v", label="Short Entry")

# Optional: show axis in percent to match the banner intuition
plt.gca().yaxis.set_major_formatter(PercentFormatter(1.0))
plt.ylabel("Cumulative Return")
plt.title("Strategy Cumulative P&L and Trades")
plt.legend()
plt.tight_layout()

# ✅ Save inside wwwroot so the site can serve it
outdir = os.path.join(script_dir, "wwwroot", "images")
os.makedirs(outdir, exist_ok=True)
plt.savefig(os.path.join(outdir, "strategy_performance.png"), dpi=150)
