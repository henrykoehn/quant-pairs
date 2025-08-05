import os
import pandas as pd
import matplotlib.pyplot as plt

# 1) Load and parse dates
script_dir = os.path.dirname(os.path.abspath(__file__))
trades_path = os.path.join(os.path.dirname(script_dir), "docs", "trades.csv")
df = pd.read_csv(trades_path, parse_dates=["Date"])

# 2) Set Date as index for nicer plotting
df.set_index("Date", inplace=True)

plt.figure(figsize=(12, 6))

# Plot cumulative P&L
plt.plot(df.index, df["CumulativePNL"], label="Cumulative P&L")

# Scatter trades
buy = df[df["Signal"] == 1]
sell = df[df["Signal"] == -1]
plt.scatter(buy.index,
            buy["CumulativePNL"],
            color="green",
            marker="^",
            label="Long Entry")

plt.scatter(sell.index,
            sell["CumulativePNL"],
            color="red",
            marker="v",
            label="Short Entry")

# 3) Reduce date labels to ~10 ticks
total = len(df)
skip = max(1, total // 10)
ticks = df.index[::skip]
plt.xticks(ticks, [d.strftime("%Y-%m-%d") for d in ticks], rotation=45)

plt.title("Strategy Cumulative P&L and Trades")
plt.ylabel("Cumulative Return")
plt.legend()
plt.tight_layout()

# 4) Save only—do NOT call plt.show()
outdir = os.path.join(os.path.dirname(script_dir), "QuantDashboard", "wwwroot", "images")
os.makedirs(outdir, exist_ok=True)
plt.savefig(os.path.join(outdir, "strategy_performance.png"), dpi=150)
